#include "../../Pch.h"
#include "Camera.h"
#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "../../Object/Common/Transform.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderModel.h"
#include "../../DrawUI/SceneUI/PauseMenu.h"

// 静的メンバ変数の実体定義
float Camera::sharedSensitivity_ = 0.5f;

Camera::Camera(void)
    : pauseMenu_(nullptr)
    , mode_(MODE::NONE)
    , prevMode_(MODE::NONE)
    , targetPos_(Utility::VECTOR_ZERO)
    , cameraUp_({ 0.0f, 1.0f, 0.0f })
    , defaultPos_(Utility::VECTOR_ZERO)
    , prePushPos_(Utility::VECTOR_ZERO)
    , rotation_(Quaternion::Identity())
    , yaw_(DEFAULT_YAW)
    , pitch_(DEFAULT_VERTICAL)
    , shakeDir_(Utility::VECTOR_ZERO)
    , shakeStep_(0.0f)
    , sensitivity_(sharedSensitivity_)
    , screenCenterX_(Application::SCREEN_SIZE_X / 2)
    , screenCenterY_(Application::SCREEN_SIZE_Y / 2)
    , mouseDeltaX_(0)
    , mouseDeltaY_(0)
    , cameraOffset_(Utility::VECTOR_ZERO)
    , isLockonEnabled_(false)
    , isFollowFrozen_(false)
    , isPitchInverted_(false)
    , isYawInverted_(false)
    , frozenTargetPos_(Utility::VECTOR_ZERO)
    , frozenCameraPos_(Utility::VECTOR_ZERO)
    , initialYaw_(0.0f)
    , initialDistance_(0.0f)
    , initialHeightOffset_(0.0f)
    , keyRotateSpeed_(0.0f)
    , spotLightHandle_(-1)
    , distance_(DEFAULT_DISTANCE)
{
    // ポーズメニューのインスタンス取得
    pauseMenu_ = Application::GetInstance().GetPauseMenu();
}
 
void Camera::Initialize(void)
{
    // 各カメラモードに対応する更新関数をテーブルに登録
    beforeDrawFuncTable_[MODE::NONE] = [this] { SetBeforeDrawFollow(); };
    beforeDrawFuncTable_[MODE::FIXED_POINT] = [this] { SetBeforeDrawFixedPoint(); };
    beforeDrawFuncTable_[MODE::FREE] = [this] { SetBeforeDrawFree(); };
    beforeDrawFuncTable_[MODE::FOLLOW] = [this] { SetBeforeDrawFollow(); };
    beforeDrawFuncTable_[MODE::FOLLOW_SPRING] = [this] { SetBeforeDrawFollowSpring(); };
    beforeDrawFuncTable_[MODE::FOLLOW_PERSPECTIVE] = [this] { SetBeforeDrawFollowPerspective(); };
    beforeDrawFuncTable_[MODE::SHAKE] = [this] { SetBeforeDrawShake(); };
    beforeDrawFuncTable_[MODE::FREE_MOUSE] = [this] { SetBeforeDrawFreeMouse(); };
    beforeDrawFuncTable_[MODE::TPS_MOUSE] = [this] { SetBeforeDrawTPSMouse(); };
    beforeDrawFuncTable_[MODE::VERSATILITY_LOCKON] = [this] { SetBeforeDrawLockon(); };

    // 各種初期設定
    SetUpDefault();
    SetUpLighting();

    // 衝突判定の初期設定
    radius_ = COLLISION_RADIUS;
    InitCollider();

    // 衝突管理システムへの登録
    CollisionController::GetInstance().RegisterUnit(this);
}

void Camera::SetSensitivity(float sensitivity)
{
    sharedSensitivity_ = sensitivity;
}

void Camera::InitCollider(void)
{
    // カメラ周囲のめり込み防止用球体コライダを作成
    ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::CAMERA,
        &trans_, Utility::VECTOR_ZERO, COLLISION_RADIUS);

    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colSphere);
}

void Camera::OnCollisionStay(const CollisionInfo& info)
{
}

void Camera::CalcGravityPow(void)
{
}

void Camera::Collision(void)
{
}

void Camera::SetPitchInvert(bool inverted)
{
    isPitchInverted_ = inverted;
}

void Camera::SetYawInvert(bool inverted)
{
    isYawInverted_ = inverted;
}

void Camera::UpdateBeforeCollision(void)
{
    // 押し出し前の座標を保存
    prePos_ = trans_.pos;
}

void Camera::Update(void)
{
    UnitBase::Update();

    // 壁や床からの押し出し解決
    HandleCollisionPushback();

    // カメラ座標にスポットライトを同期
    if (spotLightHandle_ != -1)
    {
        SetLightPositionHandle(spotLightHandle_, trans_.pos);

        SetLightDirectionHandle(spotLightHandle_, rotation_.ToEuler());
    }
}

void Camera::SetBeforeDraw(void)
{
    // クリップ距離を設定する(SetDrawScreenでリセットされる)
    SetCameraNearFar(CLIP_NEAR, CLIP_FAR);

    // 現在のモードに対応する更新処理を実行
    if (beforeDrawFuncTable_.count(mode_))
    {
        beforeDrawFuncTable_[mode_]();
    }

    // カメラの設定(位置と注視点による制御)
    SetCameraPositionAndTargetAndUpVec(trans_.pos, targetPos_, cameraUp_);

    // DXライブラリのカメラとEffekseerのカメラを同期する
    Effekseer_Sync3DSetting();
}

void Camera::SetBeforeDrawFixedPoint(void)
{
}

void Camera::SetBeforeDrawFree(void)
{
    auto& ins = InputManager::GetInstance();

    // 移動操作
    ProcessMove();

    // 減速
    Decelerate(MOVE_DEC_POW);

    // 移動
    Move();
}

void Camera::SetBeforeDrawFollow(void)
{
    if (!followTarget_) return;

    // 追従対象の現在の座標と回転を取得
    const VECTOR& followPos = followTarget_->pos;
    const Quaternion& followRot = followTarget_->quaRot;

    // 対象の向きに合わせてカメラのオフセット位置を計算
    VECTOR relativePosToCamera = followRot.PosAxis(RELATIVE_F2C_POS_FOLLOW);
    trans_.pos = VAdd(followPos, relativePosToCamera);

    // カメラから見た注視点の位置を計算（対象の回転を考慮）
    VECTOR relativePosToTarget = followRot.PosAxis(RELATIVE_C2T_POS);
    targetPos_ = VAdd(trans_.pos, relativePosToTarget);

    // 対象の回転に基づいたカメラの上方向ベクトルを設定
    cameraUp_ = followRot.PosAxis(rotation_.GetUp());
}

void Camera::SetBeforeDrawFollowSpring(void)
{
    if (!followTarget_) return;

    auto& input = InputManager::GetInstance();

    // 特定のキー入力によってシェイク（揺れ）モードへ一時的に遷移
    if (input.IsTrgDown(KEY_INPUT_C))
    {
        prevMode_ = mode_;
        ChangeMode(MODE::SHAKE);
        return;
    }

    // スプリング物理のパラメータ設定（バネ定数と減衰係数）
    constexpr float SPRING_K = 50.0f;
    const     float DAMPING = 2.0f * sqrt(SPRING_K);
    const     float DELTA_TIME = SceneManager::GetInstance().GetDeltaTime();

    const VECTOR& followPos = followTarget_->pos;

    // このモードでは回転を固定（Identity）して追従位置を計算
    const Quaternion fixedRot = Quaternion::Identity();

    // 本来あるべき理想的なカメラ位置を算出
    VECTOR relativePosToCamera = fixedRot.PosAxis(RELATIVE_F2C_POS_FOLLOW);
    VECTOR idealPosition = VAdd(followPos, relativePosToCamera);

    // 現在位置と理想位置の差分（バネの伸び）を取得
    VECTOR diff = VSub(trans_.pos, idealPosition);

    // フックの法則（F = -kx）と速度による減衰（Damping）を計算
    VECTOR springForce = VScale(diff, -SPRING_K);
    VECTOR dampingForce = VScale(velocity_, DAMPING);
    VECTOR totalForce = VSub(springForce, dampingForce);

    // 加速度（力）を速度に加算
    velocity_ = VAdd(velocity_, VScale(totalForce, DELTA_TIME));

    // 速度を現在座標に加算して、遅延を伴う追従を実現
    trans_.pos = VAdd(trans_.pos, VScale(velocity_, DELTA_TIME));

    // 注視点および上方向ベクトルの更新
    VECTOR relativePosToTarget = fixedRot.PosAxis(RELATIVE_C2T_POS);
    targetPos_ = VAdd(trans_.pos, relativePosToTarget);

    cameraUp_ = fixedRot.PosAxis(rotation_.GetUp());
}

void Camera::SetBeforeDrawFollowPerspective(void)
{
    if (!followTarget_) return;

    // 追従対象（プレイヤー等）の座標と回転を取得
    const VECTOR& followPos = followTarget_->pos;
    const Quaternion& followRot = followTarget_->quaRot;

    // カメラ座標を対象の座標と完全に一致させる（一人称視点 / 対象の内部に配置）
    trans_.pos = followPos;

    // 対象の向きに合わせて、視線の先（注視点）を計算
    VECTOR relativePosToTarget = followRot.PosAxis(RELATIVE_C2T_POS_PERSPECTIVE);
    targetPos_ = VAdd(trans_.pos, relativePosToTarget);

    // 対象の回転に基づいたカメラの上方向ベクトルを設定
    cameraUp_ = followRot.PosAxis(rotation_.GetUp());
}

void Camera::SetBeforeDrawShake(void)
{
    const float deltaTime = SceneManager::GetInstance().GetDeltaTime();

    // 残り時間を減算し、終了判定
    shakeStep_ -= deltaTime;
    if (shakeStep_ <= 0.0f)
    {
        // 揺れ開始前の座標に戻して、元のカメラモード（スプリング追従）へ復帰
        trans_.pos = defaultPos_;
        ChangeMode(MODE::FOLLOW_SPRING);
        return;
    }

    // サイン関数を用いて振動値（-1.0 〜 1.0）を計算
    float sineValue = sinf(shakeStep_ * SHAKE_SPEED);

    // 正負の判定のみを行い、最大幅でカクカクと揺れる演出にする（必要に応じてsineValueをそのまま掛けてもOK）
    float shakeFactor = (sineValue >= 0.0f) ? 1.0f : -1.0f;

    // 指定された揺れ方向に振幅を掛けてオフセット座標を算出
    VECTOR offset = VScale(shakeDir_, shakeFactor * SHAKE_WIDTH);

    // 基準座標に揺れ分を加算して座標を更新
    trans_.pos = VAdd(defaultPos_, offset);
}

void Camera::SetBeforeDrawFreeMouse(void)
{
    auto& input = InputManager::GetInstance();

    // 現在のマウス座標を取得し、画面中央からの差分（デルタ）を算出
    Vector2 currentMousePos = input.GetMousePos();
    mouseDeltaX_ = static_cast<int>(currentMousePos.x) - screenCenterX_;
    mouseDeltaY_ = static_cast<int>(currentMousePos.y) - screenCenterY_;

    // 水平方向の回転（Yaw）計算
    float yawAmount = static_cast<float>(mouseDeltaX_) * sensitivity_;
    if (isYawInverted_) yawAmount *= -1.0f;
    Quaternion yawRot = Quaternion::AngleAxis(Utility::Deg2RadF(yawAmount), Utility::AXIS_Y);

    // 垂直方向の回転（Pitch）計算
    float pitchAmount = static_cast<float>(mouseDeltaY_) * sensitivity_;
    if (isPitchInverted_) pitchAmount *= -1.0f;
    Quaternion pitchRot = Quaternion::AngleAxis(Utility::Deg2RadF(pitchAmount), Utility::AXIS_X);

    // クォータニオンを合成してカメラの回転を更新（Yawを先に適用してワールド座標ベースの旋回にする）
    rotation_ = yawRot.Mult(rotation_);
    rotation_ = rotation_.Mult(pitchRot);

    // 回転後の向きに合わせて注視点を更新
    VECTOR relativePosToTarget = rotation_.PosAxis(RELATIVE_C2T_POS);
    targetPos_ = VAdd(trans_.pos, relativePosToTarget);

    // カメラの上方向ベクトルを更新
    cameraUp_ = rotation_.GetUp();

    // マウスカーソルを画面中央に固定し、無限に回転できるようにする
    SetMousePoint(screenCenterX_, screenCenterY_);
}

void Camera::SetBeforeDrawTPSMouse(void)
{
    //// ポーズメニュー表示中はマウス操作を無効化し、カーソルを表示
    //if (pauseMenu_)
    //{
    //    if (pauseMenu_->IsVisible()) return;
    //    SetMouseDispFlag(FALSE);
    //}
    //else
    //{
    //    pauseMenu_ = Application::GetInstance().GetPauseMenu();
    //}

    //if (!followTarget_) return;

    //auto& input = InputManager::GetInstance();
    //Vector2 mousePos = input.GetMousePos();

    //// マウスの移動量を取得
    //mouseDeltaX_ = static_cast<int>(mousePos.x) - screenCenterX_;
    //mouseDeltaY_ = static_cast<int>(mousePos.y) - screenCenterY_;

    //// マウス感度と反転設定を考慮して角度（度数法）を更新
    //float yawDelta = static_cast<float>(mouseDeltaX_) * sensitivity_;
    //if (isYawInverted_) yawDelta *= -1.0f;
    //yaw_ += yawDelta;

    //float pitchDelta = static_cast<float>(mouseDeltaY_) * sensitivity_;
    //if (!isPitchInverted_) pitchDelta *= -1.0f;
    //pitch_ += pitchDelta;

    //// 追従凍結（ポーズ中など）でもカメラの旋回だけは可能にする処理
    //if (isFollowFrozen_)
    //{
    //    yaw_ += keyRotateSpeed_;
    //}

    //// 垂直角度を一定範囲内に制限（カメラが一周して反転するのを防ぐ）
    //pitch_ = std::clamp(pitch_, PITCH_LIMIT_DOWN, PITCH_LIMIT_UP);

    //const VECTOR& basePos = followTarget_->pos;

    //// 球面座標系を用いて、角度から相対座標を算出
    //float radPitch = Utility::Deg2RadF(pitch_);
    //float radYaw = Utility::Deg2RadF(yaw_);

    //cameraOffset_.x = distance_ * cosf(radPitch) * sinf(radYaw);
    //cameraOffset_.y = distance_ * sinf(radPitch);
    //cameraOffset_.z = distance_ * cosf(radPitch) * cosf(radYaw);

    //// 追従対象の座標にオフセットを加算して理想的な位置を決定
    //VECTOR idealPos = VAdd(basePos, cameraOffset_);

    //// 地形へのめり込み防止処理
    //float groundY = GetGroundHeight(idealPos);
    //float minCameraHeight = groundY + COLLISION_RADIUS + MIN_CAMERA_WORLD_Y;

    //if (idealPos.y < minCameraHeight)
    //{
    //    idealPos.y = minCameraHeight;
    //}

    //// 近距離時に地面を突き抜けて下から見上げるのを防ぐ補正
    //constexpr float CLOSE_DIST_LIMIT = 100.0f;
    //constexpr float MIN_VISUAL_HEIGHT = 50.0f;
    //if (distance_ < CLOSE_DIST_LIMIT && idealPos.y < basePos.y + MIN_VISUAL_HEIGHT)
    //{
    //    idealPos.y = basePos.y + MIN_VISUAL_HEIGHT;
    //}

    //trans_.pos = idealPos;

    //// プレイヤーの足元ではなく少し上（腰から頭付近）を注視するようにオフセットを設定
    //constexpr float TARGET_Y_OFFSET = 100.0f;
    //targetPos_ = VGet(basePos.x, basePos.y + TARGET_Y_OFFSET, basePos.z);

    //// TPSモードでは基本的に上方向はワールド座標のY軸固定
    //cameraUp_ = Utility::DIR_UP;

    //// マウスカーソルの固定
    //SetMousePoint(screenCenterX_, screenCenterY_);

    // ポーズメニュー等のチェック
    if (pauseMenu_ && pauseMenu_->IsVisible()) return;
    if (!pauseMenu_) pauseMenu_ = Application::GetInstance().GetPauseMenu();
    if (!followTarget_) return;

    SetMouseDispFlag(FALSE);

    // --- 関数内定数 ---
    const float kStickSens = 2.0f;          // スティック感度
    const float kTargetY = 100.0f;          // 注視点の高さオフセット
    const float kCloseLimit = 100.0f;       // 地面補正が働く距離
    const float kMinVisualY = 50.0f;        // 距離が近い時の最低高度
    const float kEpsilon = 0.0001f;         // ゼロ除算防止

    auto& input = InputManager::GetInstance();
    float dt = SceneManager::GetInstance().GetDeltaTime();

    // 1. 入力値の取得（マウス差分 + スティック倒し量）
    Vector2 mousePos = input.GetMousePos();
    auto pad = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

    // X軸（ヨー）：マウス差分 ＋ スティック入力（感度と時間を考慮）
    float moveX = static_cast<float>(static_cast<int>(mousePos.x) - screenCenterX_) + (pad.AKeyRX / 1000.0f * kStickSens / dt);
    float moveY = static_cast<float>(static_cast<int>(mousePos.y) - screenCenterY_) + (pad.AKeyRY / 1000.0f * kStickSens / dt);

    // 2. 角度更新（反転設定の適用）
    yaw_ += moveX * sensitivity_ * (isYawInverted_ ? -1.0f : 1.0f);
    pitch_ += moveY * sensitivity_ * (isPitchInverted_ ? 1.0f : -1.0f);

    if (isFollowFrozen_) yaw_ += keyRotateSpeed_;
    pitch_ = std::clamp(pitch_, PITCH_LIMIT_DOWN, PITCH_LIMIT_UP);

    // 3. 座標計算（球面座標）
    float rp = Utility::Deg2RadF(pitch_), ry = Utility::Deg2RadF(yaw_);
    cameraOffset_ = VGet(distance_ * cosf(rp) * sinf(ry), distance_ * sinf(rp), distance_ * cosf(rp) * cosf(ry));

    const VECTOR& basePos = followTarget_->pos;
    VECTOR idealPos = VAdd(basePos, cameraOffset_);

    // 4. 地面めり込み防止
    float minH = GetGroundHeight(idealPos) + COLLISION_RADIUS + MIN_CAMERA_WORLD_Y;
    if (idealPos.y < minH) idealPos.y = minH;

    // 近距離時の高さ補正
    if (distance_ < kCloseLimit && idealPos.y < basePos.y + kMinVisualY) idealPos.y = basePos.y + kMinVisualY;

    // 最終座標と注視点の適用
    trans_.pos = idealPos;
    targetPos_ = VGet(basePos.x, basePos.y + kTargetY, basePos.z);
    cameraUp_ = Utility::DIR_UP;

    SetMousePoint(screenCenterX_, screenCenterY_);
}

void Camera::SetBeforeDrawLockon(void)
{
    if (!followTarget_) return;

    const VECTOR& playerPos = followTarget_->pos;
    auto& input = InputManager::GetInstance();

    // マウスが動かされたかどうかを判定
    bool isMouseMoved = (static_cast<int>(input.GetMousePos().x) != screenCenterX_ ||
        static_cast<int>(input.GetMousePos().y) != screenCenterY_);

    if (isLockonEnabled_ && lockonTarget_)
    {
        const VECTOR& enemyPos = lockonTarget_->pos;

        // プレイヤーの現在の回転を取得
        Quaternion playerRot = followTarget_->quaRot.Mult(followTarget_->quaRotLocal);

        // プレイヤーの背後方向にオフセットを計算
        constexpr VECTOR LOCKON_BACK_OFFSET = { 0.0f, 200.0f, -400.0f };
        VECTOR rotatedOffset = playerRot.PosAxis(LOCKON_BACK_OFFSET);
        VECTOR desiredPos = VAdd(playerPos, rotatedOffset);

        // プレイヤーとの相対位置に応じて、カメラ位置を左右に微調整
        float deltaX = trans_.pos.x - playerPos.x;
        if (deltaX > 50.0f)
        {
            desiredPos = VAdd(playerPos, VGet(150.0f, 250.0f, -300.0f));
        }
        else if (deltaX < -50.0f)
        {
            desiredPos = VAdd(playerPos, VGet(-150.0f, 250.0f, -300.0f));
        }
        else
        {
            desiredPos = VAdd(playerPos, VGet(0.0f, 300.0f, -400.0f));
        }

        trans_.pos = desiredPos;

        // 注視点をプレイヤーと敵の中間地点に設定
        targetPos_ = VScale(VAdd(playerPos, enemyPos), 0.5f);
        cameraUp_ = Utility::DIR_UP;
    }
    else if (isMouseMoved)
    {
        // ロックオン中でなくマウス操作がある場合はTPS操作に移行
        SetBeforeDrawTPSMouse();
    }
    else
    {
        // 非ロックオン時：プレイヤーの背後に滑らかに戻る処理
        constexpr VECTOR WORLD_BACK_OFFSET = { 0.0f, 200.0f, -400.0f };
        VECTOR targetCamPos = VAdd(playerPos, WORLD_BACK_OFFSET);

        // 線形補間（LERP）を用いてカメラ座標を目標位置へ近づける
        constexpr float LERP_RATE = 0.05f;
        trans_.pos = VAdd(trans_.pos, VScale(VSub(targetCamPos, trans_.pos), LERP_RATE));

        targetPos_ = playerPos;
        cameraUp_ = Utility::DIR_UP;
    }

    // マウスカーソルの固定
    SetMousePoint(screenCenterX_, screenCenterY_);
}

void Camera::Draw(void) const
{
#ifdef _DEBUG
    // デバッグモード：親クラスの描画と、カメラ自身の位置を示す球体を表示
    UnitBase::Draw();

    DrawSphere3D(trans_.pos, DEBUG_SPHERE_RADIUS, DEBUG_SPHERE_DIV,
        DEBUG_COLOR_YELLOW, DEBUG_COLOR_YELLOW, true);
#endif
}

void Camera::Release(void)
{
    // 衝突判定管理システムからの登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // 使用していたライトハンドルの破棄
    if (spotLightHandle_ != -1)
    {
        SetLightEnableHandle(spotLightHandle_, FALSE);
        DeleteLightHandle(spotLightHandle_);
        spotLightHandle_ = -1;
    }

    // 親クラスの解放処理（コライダの削除等）を呼び出す
    UnitBase::Release();
}

VECTOR Camera::GetPosition(void) const
{
    return trans_.pos;
}

void Camera::ChangeMode(MODE mode)
{
    mode_ = mode;

    switch (mode_)
    {
    case MODE::SHAKE:
        // シェイク時間の初期化
        shakeStep_ = SHAKE_TIME;
        // 揺らす方向の正規化（斜め方向に揺らす設定）
        shakeDir_ = VNorm(VGet(1.0f, 1.0f, 0.0f));
        // 揺れ終わった後に戻るための座標を保存
        defaultPos_ = trans_.pos;
        break;

    case MODE::FOLLOW_SPRING:
        // スプリング追従開始時に速度をリセットして急激な挙動を防ぐ
        velocity_ = Utility::VECTOR_ZERO;
        break;

    case MODE::FREE_MOUSE:
    case MODE::TPS_MOUSE:
        // マウス操作モード開始時にカーソルを中央へ固定
        SetMousePoint(screenCenterX_, screenCenterY_);
        break;

    default:
        break;
    }
}

void Camera::SetFollowTarget(const Transform* target)
{
    followTarget_ = target;
}

void Camera::SetPosition(const VECTOR& pos, const VECTOR& target)
{
    trans_.pos = pos;
    targetPos_ = target;
}

VECTOR Camera::GetFrontVector(void) const
{
    // カメラ座標から注視点へのベクトルを算出して正規化
    VECTOR direction = VSub(targetPos_, trans_.pos);
    return VNorm(direction);
}

VECTOR Camera::GetRightVector() const
{
    // 前方向とワールド上方向の外積から右方向ベクトルを算出
    const VECTOR worldUp = Utility::DIR_UP;
    VECTOR right = VCross(GetFrontVector(), worldUp);

    // 上下移動成分を排除して水平な右方向を取得
    right.y = 0.0f;
    return VNorm(right);
}

Camera::MODE Camera::GetMode(void) const
{
    return mode_;
}

void Camera::SetLockonEnabled(bool isEnabled)
{
    isLockonEnabled_ = isEnabled;
}

bool Camera::IsLockonEnabled(void) const
{
    return isLockonEnabled_;
}

void Camera::SetFollowFreeze(bool isFreeze)
{
    isFollowFrozen_ = isFreeze;

    if (isFollowFrozen_ && followTarget_)
    {
        // 凍結時の対象座標やカメラパラメータをスナップショットとして保存
        frozenTargetPos_ = followTarget_->pos;
        frozenCameraPos_ = trans_.pos;
        initialYaw_ = yaw_;

        VECTOR diff = VSub(trans_.pos, followTarget_->pos);
        initialDistance_ = VSize(diff);
        initialHeightOffset_ = diff.y;
    }
    else
    {
        // 解除時にキー回転速度をリセット
        keyRotateSpeed_ = 0.0f;
    }
}

VECTOR Camera::GetOrbitPosition(void) const
{
    if (!isFollowFrozen_)
    {
        return Utility::VECTOR_ZERO;
    }

    // 凍結中の回転角に基づいた球面座標計算
    float radPitch = Utility::Deg2RadF(pitch_);
    float radYaw = Utility::Deg2RadF(yaw_);

    VECTOR orbitOffset;
    orbitOffset.x = initialDistance_ * cosf(radPitch) * sinf(radYaw);
    orbitOffset.y = initialDistance_ * sinf(radPitch);
    orbitOffset.z = initialDistance_ * cosf(radPitch) * cosf(radYaw);

    // 保存された基準座標からの相対位置を返す
    return VAdd(frozenCameraPos_, orbitOffset);
}

void Camera::SetKeyRotationSpeed(float speed)
{
    keyRotateSpeed_ = speed;
}

void Camera::SetLockonTarget(const Transform* target)
{
    lockonTarget_ = target;
}

void Camera::SetUpDefault(void)
{
    // カメラの初期状態（定数参照）を設定
    trans_.pos = DEFAULT_POS;
    targetPos_ = VAdd(trans_.pos, RELATIVE_C2T_POS);

    cameraUp_ = Utility::DIR_UP;
    rotation_ = Quaternion::Identity();
    velocity_ = Utility::VECTOR_ZERO;
}

void Camera::SetUpLighting(void)
{
    if (spotLightHandle_ != -1)
    {
        DeleteLightHandle(spotLightHandle_);
    }

    // プレイヤー周辺を照らすスポットライトを生成
    spotLightHandle_ = CreateSpotLightHandle(trans_.pos, VGet(0.0f, -1.0f, 0.0f),
        LIGHT_OUTER_ANGLE, LIGHT_INNER_ANGLE, LIGHT_RANGE, 0.0f, LIGHT_ATTEN_QUAD,
        0.0f);

    SetLightEnableHandle(spotLightHandle_, true);
}

void Camera::ProcessMove(void)
{
    auto& input = InputManager::GetInstance();

    // WASDによる移動方向の入力検知と加速
    if (input.IsNew(KEY_INPUT_W)) { moveDir_ = Utility::DIR_F; Acceleration(MOVE_ACC_POW); }
    if (input.IsNew(KEY_INPUT_S)) { moveDir_ = Utility::DIR_B; Acceleration(MOVE_ACC_POW); }
    if (input.IsNew(KEY_INPUT_A)) { moveDir_ = Utility::DIR_L; Acceleration(MOVE_ACC_POW); }
    if (input.IsNew(KEY_INPUT_D)) { moveDir_ = Utility::DIR_R; Acceleration(MOVE_ACC_POW); }

    // 矢印キーによるカメラ回転入力の計算
    VECTOR axisInput = Utility::VECTOR_ZERO;
    if (input.IsNew(KEY_INPUT_UP)) { axisInput.x = -1.0f; }
    if (input.IsNew(KEY_INPUT_DOWN)) { axisInput.x = 1.0f; }
    if (input.IsNew(KEY_INPUT_LEFT)) { axisInput.y = -1.0f; }
    if (input.IsNew(KEY_INPUT_RIGHT)) { axisInput.y = 1.0f; }

    if (!Utility::EqualsVZero(axisInput))
    {
        // 各軸の回転をクォータニオンとして合成
        Quaternion deltaRot = Quaternion::Identity();
        deltaRot = deltaRot.Mult(Quaternion::AngleAxis(Utility::Deg2RadF(axisInput.z), Utility::AXIS_Z));
        deltaRot = deltaRot.Mult(Quaternion::AngleAxis(Utility::Deg2RadF(axisInput.x), Utility::AXIS_X));
        deltaRot = deltaRot.Mult(Quaternion::AngleAxis(Utility::Deg2RadF(axisInput.y), Utility::AXIS_Y));

        rotation_ = rotation_.Mult(deltaRot);

        // 回転後の注視点と上方向を再計算
        VECTOR rotatedLocalForward = rotation_.PosAxis(RELATIVE_C2T_POS);
        targetPos_ = VAdd(trans_.pos, rotatedLocalForward);
        cameraUp_ = rotation_.GetUp();
    }
}

void Camera::Move(void)
{
    if (Utility::EqualsVZero(moveDir_)) return;

    // ローカル移動方向を現在の回転に基づいてワールド方向に変換
    VECTOR worldDirection = rotation_.PosAxis(moveDir_);
    VECTOR moveAmount = VScale(worldDirection, moveSpeed_);

    // カメラ座標と注視点を同期させて移動
    trans_.pos = VAdd(trans_.pos, moveAmount);
    targetPos_ = VAdd(targetPos_, moveAmount);
}

void Camera::Acceleration(float speed)
{
    moveSpeed_ += speed;

    // 定数に基づいた最大速度制限
    if (moveSpeed_ > MAX_MOVE_SPEED)
    {
        moveSpeed_ = MAX_MOVE_SPEED;
    }
    if (moveSpeed_ < -MAX_MOVE_SPEED)
    {
        moveSpeed_ = -MAX_MOVE_SPEED;
    }
}

void Camera::Decelerate(float speed)
{
    // 速度の符号に応じて0に近づける減速処理
    if (moveSpeed_ > 0.0f)
    {
        moveSpeed_ -= speed;
        if (moveSpeed_ < 0.0f) moveSpeed_ = 0.0f;
    }
    if (moveSpeed_ < 0.0f)
    {
        moveSpeed_ += speed;
        if (moveSpeed_ > 0.0f) moveSpeed_ = 0.0f;
    }
}

void Camera::HandleCollisionPushback(void)
{
    // 地形を突き抜けて奈落に落ちるのを防ぐための最終セーフガード
    const float ABSOLUTE_MIN_Y = 5.0f;
    if (trans_.pos.y < ABSOLUTE_MIN_Y)
    {
        float pushY = ABSOLUTE_MIN_Y - trans_.pos.y;
        trans_.pos.y = ABSOLUTE_MIN_Y;
        targetPos_.y += pushY; // カメラが上がった分、注視点も上げて画角を維持
    }
}

float Camera::GetGroundHeight(const VECTOR& pos)
{
    float groundY = 0.0f;
    bool found = false;

    // 衝突判定リストから地面としてタグ付けされたモデルを探す
    for (const auto& hitCol : hitColliders_)
    {
        if (!hitCol || hitCol->GetCollisionTag() != ColliderBase::TAG::GROUND)
        {
            continue;
        }

        const ColliderModel* groundModel = dynamic_cast<const ColliderModel*>(hitCol);
        if (!groundModel) continue;

        int modelId = groundModel->GetFollowTarget()->modelId;
        if (modelId == -1) continue;

        // 判定用座標の上下1000ユニットの範囲で垂直にレイを飛ばす
        VECTOR rayStart = VAdd(pos, VGet(0, 1000.0f, 0));
        VECTOR rayEnd = VAdd(pos, VGet(0, -1000.0f, 0));

        // DXライブラリのモデル当たり判定関数を利用
        auto lineHits = MV1CollCheck_LineDim(modelId, -1, rayStart, rayEnd);

        if (lineHits.HitNum > 0)
        {
            // 重なっている複数のポリゴンがある場合、最も高い（足元に近い）地点を採用
            for (int i = 0; i < lineHits.HitNum; i++)
            {
                float hitY = lineHits.Dim[i].HitPosition.y;
                if (!found || hitY > groundY)
                {
                    groundY = hitY;
                    found = true;
                }
            }
            MV1CollResultPolyDimTerminate(lineHits);
        }
    }

    return groundY;
}