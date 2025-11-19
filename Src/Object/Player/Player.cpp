#include "Player.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/Camera.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/System/CollisionController.h"
#include "../Common/Transform.h"
#include "../../DrawUI/Font.h"
#include "Sword.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"

// コンストラクタ
Player::Player(void)
{
    // モデルの初期化
    modelId_ = -1;

    // 移動制限xの初期化
    blockedDirX_ = 0;

    // 移動制限zの初期化
    blockedDirZ_ = 0;

    // 地面にいるかどうか
    isGround_ = true;

    // 動きているかどうか
    isMoving_ = false;

    // 移動可能かの初期化
    movementEnabled_ = true;

    // 表示上のHP
    hpDisplay_ = param_.hp;

    // 遅延スピード
    hpDelaySpeed_ = DELAY_SPEED;

    // 攻撃状態の初期化
    isAttacking_ = false;

    // 攻撃クールタイムの初期化
    attackCoolTime_ = 0.0f;

    // 無敵時間の初期化
    invincibleTime_ = 0.0f;

    // 剣の生成
    sword_ = std::make_shared<Sword>();
}

// デストラクタ
Player::~Player(void)
{
}

// CSV読み込み
void Player::LoadParamCSV(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { return; }

    std::string line;

    // ヘッダを飛ばす
    std::getline(file, line);

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;

        // 名前
        std::getline(ss, value, ',');

        // 攻撃力
        std::getline(ss, value, ','); param_.attack = std::stoi(value);

        // 防御力
        std::getline(ss, value, ','); param_.defensse = std::stoi(value);

        // 現在体力
        std::getline(ss, value, ','); param_.hp = std::stoi(value);

        // 最大体力
        std::getline(ss, value, ','); param_.maxHp = std::stoi(value);

        // スタミナ
        std::getline(ss, value, ','); param_.stamina = std::stoi(value);

        // 当たり判定(球体)
        std::getline(ss, value, ','); param_.collisionRadius = std::stof(value);

        // レベル
        std::getline(ss, value, ','); param_.level = std::stoi(value);

        // 最大レベル
        std::getline(ss, value, ','); param_.maxLevel = std::stoi(value);

        // ジャンプ力
        std::getline(ss, value, ','); param_.jumpPower = std::stof(value);
    }

    file.close();
}

// リソースの読み込み
void Player::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    // モデル読み込み
    modelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
    trans_.SetModel(modelId_);

    sword_->Load();

    // アニメーションの読み込み
    anim_ = std::make_unique<AnimationController>(modelId_);
    anim_->AddExternal(static_cast<int>(ANIM::IDEL), res.Load(ResourceManager::SRC::ANIM_PLAYER_IDEL).handleId_, 35.0f);
    anim_->AddExternal(static_cast<int>(ANIM::WALK), res.Load(ResourceManager::SRC::ANIM_PLAYER_WALK).handleId_, 30.0f);
    anim_->AddExternal(static_cast<int>(ANIM::ATTACK), res.Load(ResourceManager::SRC::ANIM_PLAYER_ATTACK).handleId_, 50.0f);
}

// 初期化
void Player::Init(void)
{
    auto& res = ResourceManager::GetInstance();

    // 初期化座標
    trans_.pos = Utility::VECTOR_ZERO;

    // 前座標の初期化
    prePos_ = trans_.pos;

    // 回転の初期化
    trans_.rot = VGet(0, 0, 0);

    // ローカル回転の初期化
    trans_.quaRotLocal = Quaternion::Identity();

    // スケールの初期化
    trans_.scl = PLAYER_SCL;

    // 当たり判定（球体コライダの半径）
    radius_ = param_.collisionRadius;

    // 移動量の初期化
    movePow_ = Utility::VECTOR_ZERO;

    // ジャンプ量の初期化
    jumpPow_ = Utility::VECTOR_ZERO;

    // コライダ初期化
    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);

    // ソードの初期化
    if (sword_)
    {
        sword_->Init();
        sword_->SetPlayer(selfPtr_);
        sword_->SetPositionOffset(VGet(0.0f, 0.0f, 0.0f));
        sword_->SetRotationOffset(VGet(DX_PI_F, 0.0f, 0.0f));
    }
}

// コライダ初期化
void Player::InitCollider(void)
{
    // 地面判定用の線分コライダ
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::PLAYER,
        &trans_,
        COL_LINE_START_LOCAL_POS,
        COL_LINE_END_LOCAL_POS
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    // 本体の球体コライダ（敵との衝突・押し出し用）
    ColliderSphere* colSphere = new ColliderSphere(
        ColliderBase::TAG::PLAYER,
        &trans_,
        COL_SPHERE_LOCAL_POS,
        param_.collisionRadius
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colSphere);
}

void Player::SetSelfPtr(std::shared_ptr<Player> ptr)
{
    selfPtr_ = ptr;
}

// 更新処理
void Player::Update(void)
{
    // 無敵時間の更新
    if (invincibleTime_ > 0.0f)
    {
        invincibleTime_ -= SceneManager::GetInstance().GetDeltaTime();
    }

    // 入力による移動制御
    ProcessMove();

    // 攻撃入力チェック（移動処理の後）
    auto& input = InputManager::GetInstance();
    if (input.IsTrgMouseLeft() && attackCoolTime_ <= 0.0f && !isAttacking_)
    {
        Attack();
    }

    // 攻撃クールタイムの更新
    if (attackCoolTime_ > 0.0f)
    {
        attackCoolTime_ -= SceneManager::GetInstance().GetDeltaTime();
        if (attackCoolTime_ < 0.0f)
        {
            attackCoolTime_ = 0.0f;
        }
    }

    // 攻撃アニメーション終了チェック
    if (isAttacking_ && anim_)
    {
        if (!anim_->IsPlaying(static_cast<int>(ANIM::ATTACK)))
        {
            isAttacking_ = false;
            attackCoolTime_ = ATTACK_COOL_TIME_MAX;
        }
    }

    // UnitBaseの更新（重力計算・衝突判定を含む）
    UnitBase::Update();

    // 地面判定の更新
    // ジャンプ量がゼロで地面に接触している = 地面にいる
    isGround_ = (Utility::EqualsVZero(jumpPow_) || jumpPow_.y >= -0.1f);

    // HPバーの遅延表示
    if (hpDisplay_ > param_.hp)
    {
        hpDisplay_ -= hpDelaySpeed_;
        if (hpDisplay_ < param_.hp)
        {
            hpDisplay_ = param_.hp;
        }
    }
    else
    {
        hpDisplay_ = param_.hp;
    }

    // ソードの更新
    if (sword_)
    {
        sword_->Update();
    }
}

// 描画処理
void Player::Draw(void) const
{
    // UnitBaseの描画（モデル＋コライダ）
    UnitBase::Draw();

    DrawHpBar();

    // ソードの描画
    if (sword_)
    {
        sword_->Draw();
    }

    // デバック表示
   // デバック表示
#ifdef _DEBUG
    DrawFormatString(0, 40, 0xffffff, "Player Pos:(%.2f, %.2f, %.2f)", trans_.pos.x, trans_.pos.y, trans_.pos.z);
    DrawFormatString(0, 60, 0xffffff, "IsGround: %s", isGround_ ? "TRUE" : "FALSE");
    DrawFormatString(0, 80, 0xffffff, "JumpPow: (%.2f, %.2f, %.2f)", jumpPow_.x, jumpPow_.y, jumpPow_.z);
    DrawFormatString(0, 100, 0xffffff, "Invincible: %.2f", invincibleTime_);

    // ★キャラクターの向きを矢印で表示
    VECTOR forward = trans_.quaRot.GetForward();
    VECTOR arrowStart = VAdd(trans_.pos, VGet(0, 50, 0)); // 腰の高さ
    VECTOR arrowEnd = VAdd(arrowStart, VScale(forward, 100.0f)); // 100単位の矢印

    // 赤い矢印で正面方向を表示
    DrawLine3D(arrowStart, arrowEnd, GetColor(255, 0, 0));
    DrawSphere3D(arrowEnd, 10.0f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);

    // 移動方向を緑の矢印で表示
    if (!Utility::EqualsVZero(movePow_))
    {
        VECTOR moveDir = VNorm(movePow_);
        VECTOR moveArrowEnd = VAdd(arrowStart, VScale(moveDir, 80.0f));
        DrawLine3D(arrowStart, moveArrowEnd, GetColor(0, 255, 0));
        DrawSphere3D(moveArrowEnd, 8.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
    }
#endif 
}

// 解放処理
void Player::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // UnitBaseの解放（コライダも含む）
    UnitBase::Release();

    if (anim_)
    {
        anim_->Release();
        anim_.reset();
    }

    if (sword_)
    {
        sword_->Release();
        sword_.reset();
    }
}

// 移動可能かを設定
void Player::SetMovementEndbled(bool enabled)
{
    movementEnabled_ = enabled;
}

// 移動かのかを取得
bool Player::IsMovementEndbled(void) const
{
    return movementEnabled_;
}

// パラメータの取得
const Player::Param& Player::GetParam(void) const
{
    return param_;
}

// プレイヤーのHPバー
void Player::DrawHpBar(void) const
{
    auto& font = Font::GetInstance();

    int barWidth = HP_BAR_WIDTH;
    int barHeight = HP_BAR_HEIGHT;
    int barX = (Application::SCREEN_SIZE_X - barWidth) / 2;
    int barY = Application::SCREEN_SIZE_Y - BAR_Y;

    float hpRate = (float)param_.hp / param_.maxHp;
    float dispRate = hpDisplay_ / param_.maxHp;

    hpRate = std::clamp(hpRate, 0.0f, 1.0f);
    dispRate = std::clamp(dispRate, 0.0f, 1.0f);

    int curHPWidth = (int)(barWidth * hpRate);
    int dispHPWidth = (int)(barWidth * dispRate);

    // 枠
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), false);
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), true);

    // 遅延バー（赤）
    DrawBox(barX, barY, barX + dispHPWidth, barY + barHeight, GetColor(255, 60, 60), true);

    // 現在HPバー（緑）
    DrawBox(barX, barY, barX + curHPWidth, barY + barHeight, GetColor(0, 255, 0), true);

    // 数値
    char hpStr[32];
    sprintf_s(hpStr, "%d / %d", param_.hp, param_.maxHp);

    int textWidth = font.GetDefaultTextWidth(hpStr);
    int textX = barX + (barWidth / 2) - (textWidth / 2);
    int textY = barY + (barHeight / 2) - 8;

    font.DrawDefaultText(textX, textY, hpStr, GetColor(255, 255, 255), 18);
}

void Player::TakeDamage(int damage)
{
    // 無敵時間中はダメージを受けない
    if (invincibleTime_ > 0.0f) return;

    param_.hp -= damage;
    if (param_.hp < 0) param_.hp = 0;

    // 無敵時間設定
    invincibleTime_ = INVINCIBLE_DURATION;
}

VECTOR* Player::GetPosPtr(void)
{
    return &trans_.pos;
}

bool Player::IsAttacking(void) const
{
    return isAttacking_;
}


// 入力による移動制御
void Player::ProcessMove(void)
{
    if (!movementEnabled_) return;

    auto& input = InputManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    // 前回位置保存
    prePos_ = trans_.pos;

    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    //前
    if (input.IsNew(KEY_INPUT_W)) { forwardInput += 1.0f; }

    //後ろ
    if (input.IsNew(KEY_INPUT_S)) { forwardInput -= 1.0f; }

    //左
    if (input.IsNew(KEY_INPUT_A)) { rightInput -= 1.0f; }

    //右
    if (input.IsNew(KEY_INPUT_D)) { rightInput += 1.0f; }

    isMoving_ = false;

    // 左右入力中の処理
    if (rightInput != 0.0f && forwardInput == 0.0f)
    {
        // カメラの追従を凍結（座標移動は停止、回転は追従）
        camera->SetFreezeFollow(true);

        // カメラの位置を取得
        VECTOR camPos = camera->GetPos();

        // カメラからプレイヤーへのベクトル（XZ平面のみ）
        VECTOR toPlayer = VSub(trans_.pos, camPos);

        // Y座標は保持
        float currentHeight = toPlayer.y;
        toPlayer.y = 0.0f;

        // 現在の距離を保存
        float radius = VSize(toPlayer);

        // 現在の角度を計算
        float currentAngle = atan2f(toPlayer.x, toPlayer.z);

        // 回転速度
        float rotSpeed = rightInput * 1.5f;

        // 新しい角度（キー入力による回転を加える）
        float newAngle = currentAngle + Utility::Deg2RadF(rotSpeed);

        // 新しい位置を計算（極座標→直交座標）
        VECTOR newOffset;
        newOffset.x = radius * sinf(newAngle);
        newOffset.y = currentHeight;
        newOffset.z = radius * cosf(newAngle);

        // カメラ位置を基準に新しい位置を設定
        trans_.pos = VAdd(camPos, newOffset);

        isMoving_ = true;

        // プレイヤーの向きを接線方向に設定
        VECTOR tangent;
        if (rightInput > 0)
        {
            // 右回り：接線は反時計回り
            tangent.x = -toPlayer.z;
            tangent.z = toPlayer.x;
        }
        else
        {
            // 左回り：接線は時計回り
            tangent.x = toPlayer.z;
            tangent.z = -toPlayer.x;
        }

        tangent.y = 0.0f;
        tangent = VNorm(tangent);

        // プレイヤーの向きを接線方向に設定
        Quaternion targetLocalRot = Quaternion::LookRotation(tangent);
        trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
    }
    else
    {
        // 左右移動していない時はカメラの追従を再開
        camera->SetFreezeFollow(false);

        //前後左右入力がある場合
        if (forwardInput != 0.0f || rightInput != 0.0f)
        {
            // カメラの前方向
            VECTOR camForward = camera->GetFrontVec();
            camForward.y = 0.0f;
            camForward = VNorm(camForward);

            // カメラの右方向
            VECTOR camRight = camera->GetRightVec();
            camRight.y = 0.0f;
            camRight = VNorm(camRight);

            // 移動方向を合成
            VECTOR moveDir = Utility::VECTOR_ZERO;
            moveDir = VAdd(moveDir, VScale(camForward, forwardInput));
            moveDir = VAdd(moveDir, VScale(camRight, -rightInput));
            moveDir.y = 0.0f;

            // 移動方向を正規化
            if (VSize(moveDir) > 0.0001f)
            {
                moveDir = VNorm(moveDir);

                // 移動量を適用
                VECTOR movement = VScale(moveDir, 10.5f);
                trans_.pos = VAdd(trans_.pos, movement);

                isMoving_ = true;

                // 移動方向に向く
                Quaternion targetLocalRot = Quaternion::LookRotation(VScale(moveDir, -1.0f));
                trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
            }
        }
    }

    // アニメーション制御
    if (isMoving_)
    {
        PlayAnim(ANIM::WALK, true, 0.2f);
    }
    else
    {
        PlayAnim(ANIM::IDEL, true, 0.2f);
    }

    // 移動制限（壁など）
    VECTOR moveDir = VSub(trans_.pos, prePos_);
    if ((blockedDirX_ == 1 && moveDir.x < 0) || (blockedDirX_ == -1 && moveDir.x > 0))
    {
        trans_.pos.x = prePos_.x;
    }

    if ((blockedDirZ_ == 1 && moveDir.z < 0) || (blockedDirZ_ == -1 && moveDir.z > 0))
    {
        trans_.pos.z = prePos_.z;
    }

    // ジャンプ処理

    if (isGround_ && input.IsTrgDown(KEY_INPUT_SPACE))

    {

        jumpPow_ = VGet(0, param_.jumpPower, 0);

        isGround_ = false;

    }
}


// Playerのカプセルを描画
void Player::DrawCollisionCapsuleDebug(void) const
{
    // 球体コライダの可視化
    if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::SPHERE)) > 0)
    {
        const ColliderSphere* sphere =
            dynamic_cast<const ColliderSphere*>(
                ownColliders_.at(static_cast<int>(COLLIDER_TYPE::SPHERE))
                );

        if (sphere)
        {
            VECTOR pos = sphere->GetPos();
            float r = sphere->GetRadius();
            DrawSphere3D(pos, r, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
        }
    }
}

// 攻撃処理の実装
void Player::Attack(void)
{
    isAttacking_ = true;
    PlayAnim(ANIM::ATTACK, false, 0.1f);
}


void Player::OnCollisionEnter(const CollisionInfo& info)
{
    // 敵との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::ENEMY)
    {
        // 球体コライダでの衝突の場合
        if (info.myCollider->GetShape() == ColliderBase::SHAPE::SPHERE)
        {
            // 無敵時間中はダメージを受けない
            if (invincibleTime_ <= 0.0f)
            {
                TakeDamage(10);

#ifdef _DEBUG
                printfDx("プレイヤーが敵からダメージを受けた！\n");
#endif

                // ノックバック
                VECTOR knockback = VScale(info.hitNormal, -50.0f);
                knockback.y = 0.0f; // Y方向のノックバックは無し
                movePow_ = VAdd(movePow_, knockback);
            }
        }
    }
}

void Player::OnCollisionStay(const CollisionInfo& info)
{
    // 地面との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::GROUND)
    {
        // 線分コライダでの衝突の場合
        if (info.myCollider->GetShape() == ColliderBase::SHAPE::LINE)
        {
            // 上昇中は地面との衝突を無視（木へのめり込み防止）
            if (jumpPow_.y > 0.1f)
            {
                return;
            }

            // 地面に接地している
            trans_.pos = VAdd(info.hitPosition, VScale(Utility::DIR_U, 2.0f));

            // ジャンプ量をリセット
            jumpPow_ = Utility::VECTOR_ZERO;

            // 着地状態にする
            isGround_ = true;
        }
    }
}