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

// コンストラクタ
Camera::Camera(void)
{
    // モードの初期化
    mode_ = MODE::NONE;
    currentMode_ = MODE::NONE;

    // defaultPos_ の初期化
    defaultPos_ = Utility::VECTOR_ZERO;

    // 追従対象の座標
    targetPos_ = Utility::VECTOR_ZERO;

    // ロックオン対象
    lockonTarget_ = nullptr;

    // 回転
    rot_ = Quaternion::Identity();

    // 追従対象
    followTransform_ = nullptr;

    // 左右回転
    yaw_ = CAMERA_YAW;

    // 上下回転
    pitch_ = CAMERA_PITCH;

    // 対象との距離
    distance_ = CAMERA_DISTANCE;

    // カメラの上方向
    cameraUp_ = { 0.0f, 1.0f, 0.0f };

    // 移動関連の初期化
    moveDIr_ = Utility::VECTOR_ZERO;
    moveSpeed_ = 0.0f;

    // カメラ揺らし関連の初期化
    shakeDir_ = Utility::VECTOR_ZERO;
    stepShake_ = 0.0f;

    // ライト関連の初期化
    spotLight_ = -1;

    // 速度の初期化
    velocity_ = Utility::VECTOR_ZERO;

    // マウス感度
    sensitivity_ = InputManager::GetInstance().GetMouseSensitivity();

    // x中央
    centerX_ = Application::SCREEN_SIZE_X / 2;

    // y中央
    centerY_ = Application::SCREEN_SIZE_Y / 2;

    // xの移動量
    deltaX_ = 0;

    // yの移動量
    deltaY_ = 0;

    // 位置
    offset_ = Utility::VECTOR_ZERO;

    // ロックオン
    lockonFlag_ = false;

    // 追従停止しているか
    freezeFollow_ = false;

    isPitchInverted_ = false;

    isYawInverted_ = false;

    // 固定された注視点
    frozenTargetPos_ = Utility::VECTOR_ZERO;

    // 固定されたカメラ位置
    frozenCameraPos_ = Utility::VECTOR_ZERO;

    // 凍結開始時のyaw角度
    initialYaw_ = 0.0f;

    // 凍結開始時のカメラとプレイヤーの距離
    initialDistance_ = 0.0f;

    // キー入力による回転速度
    keyRotateSpeed_ = 0.0f;

    // 凍結開始時のY軸オフセット
    initialHeightOffset_ = 0.0f;

    // 押し出し前の座標の初期化を追加
    prePos_ = Utility::VECTOR_ZERO;

    pauseMenu_ = Application::GetInstance().GetPauseMenu();
}

Camera::~Camera(void)
{
}

// 初期化処理
void Camera::Init(void)
{
    // 関数ポインタの設定
    setBeforeDrawMode_.emplace(MODE::NONE, std::bind(&Camera::SetBeforeDrawFollow, this));
    setBeforeDrawMode_.emplace(MODE::FIXED_POINT, std::bind(&Camera::SetBeforeDrawFixedPoint, this));
    setBeforeDrawMode_.emplace(MODE::FREE, std::bind(&Camera::SetBeforeDrawFree, this));
    setBeforeDrawMode_.emplace(MODE::FOLLOW, std::bind(&Camera::SetBeforeDrawFollow, this));
    setBeforeDrawMode_.emplace(MODE::FOLLOW_SPRING, std::bind(&Camera::SetBeforeDrawFollowSpring, this));
    setBeforeDrawMode_.emplace(MODE::FOLLOW_PERSPECTIVE, std::bind(&Camera::SetBeforeDrawFollowPerspective, this));
    setBeforeDrawMode_.emplace(MODE::SHAKE, std::bind(&Camera::SetBeforeDrawShake, this));
    setBeforeDrawMode_.emplace(MODE::FREE_MOUSE, std::bind(&Camera::SetBeforeDrawFreeMouse, this));
    setBeforeDrawMode_.emplace(MODE::TPS_MOUSE, std::bind(&Camera::SetBeforeDrawTPSMouse, this));
    setBeforeDrawMode_.emplace(MODE::VERSATILITY_LOCKON, std::bind(&Camera::SetBeforeDrawLockon, this));

    // カメラの初期設定
    SetDefault();

    // カメラのライト設定
    SetLighting();

    // 半径の設定
    radius_ = COLLISION_RADIUS;

    // 衝突判定の初期化
    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

// 衝突判定の初期化
void Camera::InitCollider(void)
{
    // 球体コライダの作成
    ColliderSphere* colSphere = new ColliderSphere(
        ColliderBase::TAG::CAMERA,
        &trans_,
        Utility::VECTOR_ZERO,  // ローカル座標（中心）
        COLLISION_RADIUS
    );

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::SPHERE),
        colSphere
    );
}

// 衝突判定のコールバック（Update()メソッドの後あたりに追加）
void Camera::OnCollisionStay(const CollisionInfo& info)
{
   
}

// 重力計算
void Camera::CalcGravityPow(void)
{
}

// 衝突判定
void Camera::Collision(void)
{
}

void Camera::SetPitchInvert(bool invert)
{
    isPitchInverted_ = invert;
}

void Camera::SetYawInvert(bool invert)
{
    isYawInverted_ = invert;
}

// 更新処理（衝突判定前）
void Camera::UpdateBeforeCollision(void)
{
    // 押し出し前の座標を保存
    prePos_ = trans_.pos;
}

// 更新処理
void Camera::Update(void)
{

    // 基底クラスの更新（衝突判定を実行）
    UnitBase::Update();

    // カメラ押し出し処理（地面との衝突対応）
    HandleCollisionPushback();

    // ライトの移動
    SetLightPositionHandle(spotLight_, trans_.pos);

    // ライトの向き更新
    SetLightDirectionHandle(spotLight_, rot_.ToEuler());
}

// 描画前処理
void Camera::SetBeforeDraw(void)
{
    // クリップ距離を設定する(SetDrawScreenでリセットされる)
    SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

    // モードによる設定切り替え
    setBeforeDrawMode_[mode_]();

    // カメラの設定(位置と注視点による制御)
    SetCameraPositionAndTargetAndUpVec(
        trans_.pos,  // pos_ → trans_.pos
        targetPos_,
        cameraUp_
    );

    // DXライブラリのカメラとEffekseerのカメラを同期する
    Effekseer_Sync3DSetting();
}

// 定点カメラ
void Camera::SetBeforeDrawFixedPoint(void)
{
}

// フリーカメラ
void Camera::SetBeforeDrawFree(void)
{
    auto& ins = InputManager::GetInstance();

    // 移動操作
    ProcessMove();

    // 減速
    Decelerate(MOVE_DEC);

    // 移動
    Move();
}

// 追従カメラ
void Camera::SetBeforeDrawFollow(void)
{
    if (!followTransform_) return;

    // 追従対象の位置
    VECTOR followPos = followTransform_->pos;

    // 追従対象の向き
    Quaternion followRot = followTransform_->quaRot;

    // 追従対象からカメラまでの相対座標
    VECTOR relativeCPos = followRot.PosAxis(RELATIVE_F2C_POS_FOLLOW);

    // カメラの位置の更新
    trans_.pos = VAdd(followPos, relativeCPos);  // pos_ → trans_.pos

    // カメラ位置から注視点までの相対座標
    VECTOR relativeTPos = followRot.PosAxis(RELATIVE_C2T_POS);

    // 注視点の更新
    targetPos_ = VAdd(trans_.pos, relativeTPos);  // pos_ → trans_.pos

    // カメラの上方向
    cameraUp_ = followRot.PosAxis(rot_.GetUp());
}

// ばね付き追従カメラ
void Camera::SetBeforeDrawFollowSpring(void)
{
    if (!followTransform_) return;
    auto& ins = InputManager::GetInstance();

    // Cキー押下でカメラを揺らす
    if (ins.IsTrgDown(KEY_INPUT_C))
    {
        currentMode_ = mode_;
        ChangeMode(MODE::SHAKE);
    }

    // ばね定数(ばねの強さ)
    float POW_SPRING = 50.0f;

    // ばね定数(ばねの抵抗)
    float dampening = 2.0f * sqrt(POW_SPRING);

    // デルタタイム
    float delta = SceneManager::GetInstance().GetDeltaTime();

    // 追従対象の位置
    VECTOR followPos = followTransform_->pos;

    // 追従対象の向き
    Quaternion followRot = followTransform_->quaRot;
    VECTOR zero = { 0.0f, 0.0f, 0.0f };

    // カメラの方向を固定する用
    Quaternion forward = Quaternion::Euler(zero);

    // 追従対象からカメラまでの相対座標
    VECTOR relativeCPos = forward.PosAxis(RELATIVE_F2C_POS_FOLLOW);

    // 理想位置
    VECTOR idealPos = VAdd(followPos, relativeCPos);

    // 実際と理想の差
    VECTOR diff = VSub(trans_.pos, idealPos);  // pos_ → trans_.pos

    // 力 =- ばねの強さ × ばねの伸び - 抵抗 × カメラ速度
    VECTOR force = VScale(diff, -POW_SPRING);
    force = VSub(force, VScale(velocity_, dampening));

    // 速度の更新
    velocity_ = VAdd(trans_.pos, VScale(velocity_, delta));  // pos_ → trans_.pos

    // カメラ位置の更新
    trans_.pos = VAdd(trans_.pos, VScale(velocity_, delta));  // pos_ → trans_.pos

    // カメラ位置から注視点までの相対座標
    VECTOR relativeTPos = forward.PosAxis(RELATIVE_C2T_POS);

    // 注視点の更新
    targetPos_ = VAdd(trans_.pos, relativeTPos);  // pos_ → trans_.pos

    // カメラの上方向
    cameraUp_ = forward.PosAxis(rot_.GetUp());
}

// 追従対象視点カメラ
void Camera::SetBeforeDrawFollowPerspective(void)
{
    // 追従対象の位置
    VECTOR followPos = followTransform_->pos;

    // 追従対象の向き
    Quaternion followRot = followTransform_->quaRot;

    // カメラ位置から注視点までの相対座標
    VECTOR relativeTPos = followRot.PosAxis(RELATIVE_C2T_POS_FOLLOW_PERSPECTIVE);

    // 注視点の更新
    targetPos_ = VAdd(trans_.pos, relativeTPos);  // pos_ → trans_.pos

    // カメラの上方向
    cameraUp_ = followRot.PosAxis(rot_.GetUp());
}

// カメラ揺らし
void Camera::SetBeforeDrawShake(void)
{
    // 一定時間カメラを揺らす
    stepShake_ -= SceneManager::GetInstance().GetDeltaTime();

    if (stepShake_ < 0.0f)
    {
        trans_.pos = defaultPos_;  // pos_ → trans_.pos

        ChangeMode(MODE::FOLLOW_SPRING);
        return;
    }

    // -1.0f～1.0f
    float f = sinf(stepShake_ * SPEED_SHAKE);

    // -1000.0f～1000.0f
    f *= 1000.0f;

    // -1000 or 1000
    int d = static_cast<int>(f);

    // 0 or 1
    int shake = d % 2;

    // 0 or 2
    shake *= 2;

    // -1 or 1
    shake -= 1;

    // 移動量
    VECTOR velocity = VScale(shakeDir_, (float)(shake)*WIDTH_SHAKE);

    // 移動先座標
    trans_.pos = VAdd(defaultPos_, velocity);  // pos_ → trans_.pos
}

// マウス自由操作カメラ
void Camera::SetBeforeDrawFreeMouse(void)
{

    auto& ins = InputManager::GetInstance();
    // マウス座標を取得
    Vector2 mousePos = ins.GetMousePos();

    // 移動量
    deltaX_ = static_cast<int>(mousePos.x) - centerX_;
    deltaY_ = static_cast<int>(mousePos.y) - centerY_;

    // 水平回転を適用
    Quaternion yaw = Quaternion::AngleAxis(Utility::Deg2RadF(deltaX_ * sensitivity_), Utility::AXIS_Y);

    // 垂直回転を適用
    Quaternion pitch = Quaternion::AngleAxis(Utility::Deg2RadF(deltaY_ * sensitivity_), Utility::AXIS_X);

    // 回転を適用
    rot_ = yaw.Mult(rot_);
    rot_ = pitch.Mult(rot_);

    // 注視点更新
    VECTOR rotLocalPos = rot_.PosAxis(RELATIVE_C2T_POS);
    targetPos_ = VAdd(trans_.pos, rotLocalPos);

    // 上ベクトル更新
    cameraUp_ = rot_.GetUp();

    // マウスを中央に戻す
    SetMousePoint(centerX_, centerY_);
}

// TPSマウス操作カメラ
void Camera::SetBeforeDrawTPSMouse(void)
{
    if (pauseMenu_ != nullptr)
    {
        if (pauseMenu_->IsVisible() == true)
        {
            return;
        }
        else
        {
            SetMouseDispFlag(FALSE);
        }
    }
    // もしコンストラクタ時点での取得に失敗している場合を考慮して再取得を試みるのも手です
    else
    {
        pauseMenu_ = Application::GetInstance().GetPauseMenu();
    }

    if (!followTransform_) return;

    auto& ins = InputManager::GetInstance();

    // マウス座標を取得
    Vector2 mousePos = ins.GetMousePos();

    // 移動量
    deltaX_ = static_cast<int>(mousePos.x) - centerX_;
    deltaY_ = static_cast<int>(mousePos.y) - centerY_;

    // Yaw（左右回転）は常に更新
    float yawDelta = 0.0f;
    if (isYawInverted_)
    {
        // 左右反転：マウスを右に動かすと左に回転
        yawDelta = -(deltaX_ * sensitivity_);
    }
    else
    {
        // 通常：マウスを右に動かすと右に回転
        yawDelta = (deltaX_ * sensitivity_);
    }
    yaw_ += yawDelta;

    // Pitch（上下回転）の処理
    float pitchDelta = 0.0f;
    if (isPitchInverted_)
    {
        // 反転：マウス移動量そのまま（マウスを下げると deltaY+ なので、ピッチも+）
        pitchDelta = (deltaY_ * sensitivity_);
    }
    else
    {
        // 通常：マウス移動量にマイナスを掛ける（マウスを下げると deltaY+ なので、ピッチは-）
        pitchDelta = -(deltaY_ * sensitivity_);
    }
    pitch_ += pitchDelta;

    // キー入力による回転を適用（凍結中のみ）
    if (freezeFollow_)
    {
        yaw_ += keyRotateSpeed_;
    }

    // ピッチ制限（通常の上下限）
    if (pitch_ > PITCH_UP) { pitch_ = PITCH_UP; }
    if (pitch_ < PITCH_DWON) { pitch_ = PITCH_DWON; }

    // 注視点の高さオフセット（プレイヤーの胸の高さ）
    const float TARGET_HEIGHT_OFFSET = 100.0f;

    // 追従が凍結されていない場合の通常処理
    if (!freezeFollow_)
    {
        VECTOR followPos = followTransform_->pos;

        // 球面座標でカメラのオフセットを計算
        offset_.x = distance_ * cosf(Utility::Deg2RadF(pitch_)) * sinf(Utility::Deg2RadF(yaw_));
        offset_.y = distance_ * sinf(Utility::Deg2RadF(pitch_));
        offset_.z = distance_ * cosf(Utility::Deg2RadF(pitch_)) * cosf(Utility::Deg2RadF(yaw_));

        // 理想的なカメラ位置を計算
        VECTOR idealPos = VAdd(followPos, offset_);

        // 地面の高さを取得
        float groundY = GetGroundHeight(idealPos);

        // カメラの球体コライダ半径を考慮した最低高度
        float minCameraY = groundY + COLLISION_RADIUS + MIN_CAMERA_Y;

        // 地面より下にならないように制限
        if (idealPos.y < minCameraY)
        {
            idealPos.y = minCameraY;

        }

        // 距離が非常に近い場合、Y座標を地面より上に補正
        const float MIN_CAMERA_HEIGHT = 50.0f;
        if (distance_ < 100.0f && idealPos.y < followPos.y + MIN_CAMERA_HEIGHT)
        {
            idealPos.y = followPos.y + MIN_CAMERA_HEIGHT;
        }

        // プレイヤーからカメラへの距離（XZ平面のみ）
        VECTOR toIdeal = VSub(idealPos, followPos);
        float idealDistXZ = sqrtf(toIdeal.x * toIdeal.x + toIdeal.z * toIdeal.z);

        // 通常時の処理
        if (idealDistXZ < 1.0f)
        {
            trans_.pos.x = followPos.x;
            trans_.pos.z = followPos.z;
            trans_.pos.y = idealPos.y;
        }
        else
        {
            trans_.pos = idealPos;
        }

        // 注視点（プレイヤーの胸の高さを見る）
        targetPos_ = VGet(followPos.x, followPos.y + TARGET_HEIGHT_OFFSET, followPos.z);
    }
    else
    {
        // 凍結中の処理
        VECTOR playerPos = followTransform_->pos;

        // 球面座標でカメラのオフセットを計算（通常時と同じ）
        offset_.x = distance_ * cosf(Utility::Deg2RadF(pitch_)) * sinf(Utility::Deg2RadF(yaw_));
        offset_.y = distance_ * sinf(Utility::Deg2RadF(pitch_));
        offset_.z = distance_ * cosf(Utility::Deg2RadF(pitch_)) * cosf(Utility::Deg2RadF(yaw_));

        // 理想的なカメラ位置を計算
        VECTOR idealPos = VAdd(playerPos, offset_);

        // 地面の高さを取得
        float groundY = GetGroundHeight(idealPos);

        // カメラの球体コライダ半径を考慮した最低高度
        float minCameraY = groundY + COLLISION_RADIUS + MIN_CAMERA_Y;

        // 地面より下にならないように制限
        if (idealPos.y < minCameraY)
        {
            idealPos.y = minCameraY;

        }

        trans_.pos = idealPos;

        // 注視点（プレイヤーの胸の高さを見る）
        targetPos_ = VGet(playerPos.x, playerPos.y + TARGET_HEIGHT_OFFSET, playerPos.z);
    }

    // 上方向は固定
    cameraUp_ = VGet(0, 1, 0);

    // マウスを中央に戻す
    SetMousePoint(centerX_, centerY_);
}

// ロックオンカメラ
void Camera::SetBeforeDrawLockon(void)
{
    if (!followTransform_) return;
    VECTOR playerPos = followTransform_->pos;

    auto& ins = InputManager::GetInstance();
    bool mouseMoved = ins.GetMousePos().x != centerX_ || ins.GetMousePos().y != centerY_;

    if (lockonFlag_ && lockonTarget_)
    {
        VECTOR enemyPos = lockonTarget_->pos;

        VECTOR localBackOffset = { 0.0f, 200.0f, -400.0f };

        Quaternion playerRot = followTransform_->quaRot.Mult(followTransform_->quaRotLocal);

        VECTOR rotatedOffset = playerRot.PosAxis(localBackOffset);

        VECTOR desiredPos = VAdd(playerPos, rotatedOffset);

        float deltaX = trans_.pos.x - playerPos.x;  // pos_ → trans_.pos
        if (deltaX > 50.0f)
        {
            desiredPos.x = playerPos.x + 150.0f;
            desiredPos.y = playerPos.y + 250.0f;
            desiredPos.z = playerPos.z - 300.0f;
        }
        else if (deltaX < -50.0f)
        {
            desiredPos.x = playerPos.x - 150.0f;
            desiredPos.y = playerPos.y + 250.0f;
            desiredPos.z = playerPos.z - 300.0f;
        }
        else
        {
            desiredPos.x = playerPos.x;
            desiredPos.y = playerPos.y + 300.0f;
            desiredPos.z = playerPos.z - 400.0f;
        }

        trans_.pos = desiredPos;  // pos_ → trans_.pos
        targetPos_ = VScale(VAdd(playerPos, enemyPos), 0.5f);
        cameraUp_ = VGet(0, 1, 0);
    }
    else if (mouseMoved)
    {
        SetBeforeDrawTPSMouse();
    }
    else
    {
        VECTOR worldBackOffset = { 0.0f, 200.0f, -400.0f };
        VECTOR targetCamPos = VAdd(playerPos, worldBackOffset);

        float lerpRate = 0.05f;
        trans_.pos.x += (targetCamPos.x - trans_.pos.x) * lerpRate;  // pos_ → trans_.pos
        trans_.pos.y += (targetCamPos.y - trans_.pos.y) * lerpRate;  // pos_ → trans_.pos
        trans_.pos.z += (targetCamPos.z - trans_.pos.z) * lerpRate;  // pos_ → trans_.pos

        targetPos_ = playerPos;
        cameraUp_ = VGet(0, 1, 0);
    }

    SetMousePoint(centerX_, centerY_);
}

void Camera::Draw(void) const
{
#ifdef _DEBUG
    UnitBase::Draw();

    // カメラ位置を可視化
    DrawSphere3D(trans_.pos, 10.0f, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);

#endif
}

void Camera::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // ライト無効化
    SetLightEnableHandle(spotLight_, false);

    // ライトハンドル削除
    DeleteLightHandle(spotLight_);

    // 基底クラスの解放
    UnitBase::Release();
}

VECTOR Camera::GetPos(void) const
{
    return trans_.pos;  // pos_ → trans_.pos
}

void Camera::ChangeMode(MODE mode)
{
    mode_ = mode;

    switch (mode_)
    {
    case Camera::MODE::FIXED_POINT:
        break;

    case Camera::MODE::FREE:
        break;

    case Camera::MODE::FOLLOW:
        break;

    case Camera::MODE::FOLLOW_SPRING:
        break;

    case Camera::MODE::SHAKE:
        stepShake_ = TIME_SHAKE;
        shakeDir_ = VNorm({ 0.7f, 0.7f, 0.0f });
        defaultPos_ = trans_.pos;  // pos_ → trans_.pos
        break;
    }
}

const void Camera::SetFollow(const Transform* follow)
{
    followTransform_ = follow;
}

void Camera::SetPos(const VECTOR& pos, const VECTOR& target)
{
    trans_.pos = pos;  // pos_ → trans_.pos
    targetPos_ = target;
}

VECTOR Camera::GetFrontVec(void) const
{
    VECTOR front = VSub(targetPos_, trans_.pos);  // pos_ → trans_.pos
    float length = sqrtf(front.x * front.x + front.y * front.y + front.z * front.z);
    if (length > 0.0001f)
    {
        front.x /= length;
        front.y /= length;
        front.z /= length;
    }
    return front;
}

VECTOR Camera::GetRightVec(void) const
{
    VECTOR up = { 0.0f, 1.0f, 0.0f };
    VECTOR front = GetFrontVec();
    VECTOR right = VCross(front, up);
    right.y = 0.0f;
    right = VNorm(right);
    return right;
}

Camera::MODE Camera::GetMode(void) const
{
    return mode_;
}

void Camera::SetLockon(bool loc)
{
    lockonFlag_ = loc;
}

bool Camera::IsLockon(void) const
{
    return lockonFlag_;
}

void Camera::SetFreezeFollow(bool freeze)
{
    freezeFollow_ = freeze;

    if (freeze && followTransform_)
    {
        frozenTargetPos_ = followTransform_->pos;
        frozenCameraPos_ = trans_.pos;  // pos_ → trans_.pos
        initialYaw_ = yaw_;

        VECTOR diff = VSub(trans_.pos, followTransform_->pos);  // pos_ → trans_.pos
        initialDistance_ = VSize(diff);

        initialHeightOffset_ = diff.y;
    }
    else
    {
        keyRotateSpeed_ = 0.0f;
    }
}

VECTOR Camera::GetOrbitPosition(void) const
{
    if (!freezeFollow_)
    {
        return Utility::VECTOR_ZERO;
    }

    float deltaYaw = yaw_ - initialYaw_;

    VECTOR direction;
    direction.x = initialDistance_ * cosf(Utility::Deg2RadF(pitch_)) * sinf(Utility::Deg2RadF(yaw_));
    direction.y = initialDistance_ * sinf(Utility::Deg2RadF(pitch_));
    direction.z = initialDistance_ * cosf(Utility::Deg2RadF(pitch_)) * cosf(Utility::Deg2RadF(yaw_));

    return VAdd(frozenCameraPos_, direction);
}

void Camera::SetKeyRotation(float rotSpeed)
{
    keyRotateSpeed_ = rotSpeed;
}

void Camera::SetLockonTarget(const Transform* target)
{
    lockonTarget_ = target;
}

void Camera::SetDefault(void)
{
    trans_.pos = DEFAULT_CAMERA_POS;  // pos_ → trans_.pos

    targetPos_ = VAdd(trans_.pos, RELATIVE_C2T_POS);  // pos_ → trans_.pos

    cameraUp_ = { 0.0f, 1.0f, 0.0f };

    rot_ = Quaternion::Identity();

    velocity_ = Utility::VECTOR_ZERO;
}

void Camera::SetLighting(void)
{
    spotLight_ = CreateSpotLightHandle(
        trans_.pos,  // pos_ → trans_.pos
        VGet(0.0f, -1.0f, 0.0f),
        DX_PI_F / 2.0f,
        DX_PI_F / 4.0f,
        2000.0f,
        0.0f,
        0.002f,
        0.0f
    );

    // ライト有効化
    SetLightEnableHandle(spotLight_, true);
}

// 移動操作
void Camera::ProcessMove(void)
{
    auto& ins = InputManager::GetInstance();

    // 移動
    if (ins.IsNew(KEY_INPUT_W)) { moveDIr_ = Utility::DIR_F; Acceleration(MOVE_ACC); }
    if (ins.IsNew(KEY_INPUT_S)) { moveDIr_ = Utility::DIR_B; Acceleration(MOVE_ACC); }
    if (ins.IsNew(KEY_INPUT_A)) { moveDIr_ = Utility::DIR_L; Acceleration(MOVE_ACC); }
    if (ins.IsNew(KEY_INPUT_D)) { moveDIr_ = Utility::DIR_R; Acceleration(MOVE_ACC); }

    // 回転軸と量を決める
    const float ROT_POW = 1.0f;
    VECTOR axisDeg = Utility::VECTOR_ZERO;
    if (ins.IsNew(KEY_INPUT_UP)) { axisDeg.x = -1.0f; }
    if (ins.IsNew(KEY_INPUT_DOWN)) { axisDeg.x = 1.0f; }
    if (ins.IsNew(KEY_INPUT_LEFT)) { axisDeg.y = -1.0f; }
    if (ins.IsNew(KEY_INPUT_RIGHT)) { axisDeg.y = 1.0f; }

    // カメラ座標を中心として、注視点を回転させる
    if (!Utility::EqualsVZero(axisDeg))
    {
        // 今回の回転量を合成
        Quaternion rotPow;
        rotPow = rotPow.Mult(
            Quaternion::AngleAxis(Utility::Deg2RadF(axisDeg.z), Utility::AXIS_Z));

        rotPow = rotPow.Mult(
            Quaternion::AngleAxis(Utility::Deg2RadF(axisDeg.x), Utility::AXIS_X));

        rotPow = rotPow.Mult(
            Quaternion::AngleAxis(Utility::Deg2RadF(axisDeg.y), Utility::AXIS_Y));

        // カメラの回転の今回の回転量を加える(合成)
        rot_ = rot_.Mult(rotPow);

        // 注視点の相対座標を回転させる
        VECTOR rotLocalPos = rot_.PosAxis(RELATIVE_C2T_POS);

        // 注視点更新
        targetPos_ = VAdd(trans_.pos, rotLocalPos);  // pos_ → trans_.pos

        // カメラの上方向
        cameraUp_ = rot_.GetUp();
    }
}

// 移動
void Camera::Move(void)
{
    // 移動処理
    if (!Utility::EqualsVZero(moveDIr_))  // 修正: 条件を反転
    {
        // 移動 = 座標 * 移動量
        // 移動量 =  方向 * スピード

        // 入力された方向をカメラの回転情報を使って、
        // カメラの進行方向に変換する
        VECTOR direction = rot_.PosAxis(moveDIr_);

        // 移動量
        VECTOR movePow = VScale(direction, moveSpeed_);

        // 移動処理
        trans_.pos = VAdd(trans_.pos, movePow);  // pos_ → trans_.pos

        targetPos_ = VAdd(targetPos_, movePow);
    }
}

// 加速
void Camera::Acceleration(float speed)
{
    moveSpeed_ += speed;

    // 速度制限(右方向)
    if (moveSpeed_ > MAX_MOVE_SPEED)
    {
        moveSpeed_ = MAX_MOVE_SPEED;
    }

    // 速度制限(左方向)
    if (moveSpeed_ < -MAX_MOVE_SPEED)
    {
        moveSpeed_ = -MAX_MOVE_SPEED;
    }
}

// 減速
void Camera::Decelerate(float speed)
{
    // 右方向の移動を減速させる
    if (moveSpeed_ > 0.0f)
    {
        moveSpeed_ -= speed;

        if (moveSpeed_ < 0.0f)
        {
            moveSpeed_ = 0.0f;
        }
    }

    // 左方向の移動を減速させる
    if (moveSpeed_ < 0.0f)
    {
        moveSpeed_ += speed;

        if (moveSpeed_ > 0.0f)
        {
            moveSpeed_ = 0.0f;  // 修正: speed → 0.0f
        }
    }
}

// 衝突処理
void Camera::HandleCollisionPushback(void)
{
    // マウス操作で既に高さ制限されているので、
    // ここでは最終的な安全チェックのみ行う

    // 最低高度の安全装置
    const float ABSOLUTE_MIN_Y = 5.0f;
    if (trans_.pos.y < ABSOLUTE_MIN_Y)
    {
        float pushY = ABSOLUTE_MIN_Y - trans_.pos.y;
        trans_.pos.y = ABSOLUTE_MIN_Y;
        targetPos_.y += pushY;
    }
}

// 地面の高さを取得
float Camera::GetGroundHeight(const VECTOR& pos)
{
    // デフォルト値（地面が見つからない場合）
    float groundY = 0.0f;
    bool found = false;

    // 衝突候補から地面を探す
    for (const auto& hitCol : hitColliders_)
    {
        if (!hitCol) continue;

        // 地面タグのみ処理
        if (hitCol->GetTag() != ColliderBase::TAG::GROUND)
        {
            continue;
        }

        // モデルコライダにキャスト
        const ColliderModel* groundModel = dynamic_cast<const ColliderModel*>(hitCol);
        if (!groundModel) continue;

        int modelId = groundModel->GetFollow()->modelId;
        if (modelId == -1) continue;

        // 指定座標から真下にレイキャスト
        VECTOR rayStart = VAdd(pos, VGet(0, 1000.0f, 0));  // 十分高い位置から
        VECTOR rayEnd = VAdd(pos, VGet(0, -1000.0f, 0));   // 十分低い位置まで

        auto lineHits = MV1CollCheck_LineDim(modelId, -1, rayStart, rayEnd);

        if (lineHits.HitNum > 0)
        {
            // 最も高い地面を取得
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