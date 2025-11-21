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
    : modelId_(-1)
    , isGround_(true)
    , sword_(std::make_unique<Sword>())
    , UnitBase()
{
    isMoving_ = false;
    movementEnabled_ = true;
    hpDisplay_ = param_.hp;
    hpDelaySpeed_ = DELAY_SPEED;
    isAttacking_ = false;
    attackCoolTime_ = 0.0f;
    invincibleTime_ = 0.0f;
    lastHitEnemyTime_ = 0.0f;
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
    std::getline(file, line); // ヘッダを飛ばす

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;

        std::getline(ss, value, ','); // 名前
        std::getline(ss, value, ','); param_.attack = std::stoi(value);
        std::getline(ss, value, ','); param_.defensse = std::stoi(value);
        std::getline(ss, value, ','); param_.hp = std::stoi(value);
        std::getline(ss, value, ','); param_.maxHp = std::stoi(value);
        std::getline(ss, value, ','); param_.stamina = std::stoi(value);
        std::getline(ss, value, ','); param_.collisionRadius = std::stof(value);
        std::getline(ss, value, ','); param_.level = std::stoi(value);
        std::getline(ss, value, ','); param_.maxLevel = std::stoi(value);
        std::getline(ss, value, ','); param_.jumpPower = std::stof(value);
    }

    file.close();
}

// リソースの読み込み
void Player::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    modelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
    trans_.SetModel(modelId_);

    anim_ = std::make_unique<AnimationController>(modelId_);
    anim_->AddExternal(static_cast<int>(ANIM::IDEL), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_IDEL), 35.0f);
    anim_->AddExternal(static_cast<int>(ANIM::WALK), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_WALK), 30.0f);
    anim_->AddExternal(static_cast<int>(ANIM::ATTACK), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_ATTACK), 50.0f);

    sword_->Load();
}

// 初期化
void Player::Init(void)
{
    trans_.pos = Utility::VECTOR_ZERO;
    prePos_ = trans_.pos;
    trans_.rot = VGet(0, 0, 0);
    trans_.quaRotLocal = Quaternion::Identity();
    trans_.scl = PLAYER_SCL;
    radius_ = param_.collisionRadius;
    movePow_ = Utility::VECTOR_ZERO;
    jumpPow_ = Utility::VECTOR_ZERO;

    anim_->Play(static_cast<int>(ANIM::IDEL), true, 0.0f);

    InitCollider();
    CollisionController::GetInstance().RegisterUnit(this);
    sword_->Init();
}

// コライダ初期化
void Player::InitCollider(void)
{
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::PLAYER,
        &trans_,
        COL_LINE_START_LOCAL_POS,
        COL_LINE_END_LOCAL_POS
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    ColliderSphere* colSphere = new ColliderSphere(
        ColliderBase::TAG::PLAYER,
        &trans_,
        COL_SPHERE_LOCAL_POS,
        param_.collisionRadius
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colSphere);
}

// 更新処理
void Player::Update(void)
{
    float deltaTime = SceneManager::GetInstance().GetDeltaTime();

    // 無敵時間の更新
    if (invincibleTime_ > 0.0f)
    {
        invincibleTime_ -= deltaTime;
    }

    // ヒット判定のクールタイム更新
    if (lastHitEnemyTime_ > 0.0f)
    {
        lastHitEnemyTime_ -= deltaTime;
    }

    // 入力による移動制御
    ProcessMove();

    // 攻撃入力チェック
    auto& input = InputManager::GetInstance();
    if (input.IsTrgMouseLeft() && attackCoolTime_ <= 0.0f && !isAttacking_)
    {
        Attack();
    }

    // 攻撃クールタイムの更新
    if (attackCoolTime_ > 0.0f)
    {
        attackCoolTime_ -= deltaTime;
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

            // 攻撃終了時に剣の判定を無効化
            if (sword_)
            {
                sword_->SetAttacking(false);
            }
        }
    }

    // UnitBaseの更新（重力計算・衝突判定を含む）
    UnitBase::Update();

    // 地面判定の更新
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

    // 剣の位置を更新
    if (sword_)
    {
        VECTOR swordFrame = MV1GetFramePosition(modelId_, 26);
        sword_->SetFramePos(swordFrame);

        // 攻撃中かどうかを剣に伝える
        sword_->SetAttacking(isAttacking_);
    }

    // 剣の更新
    sword_->Update();
}

// 攻撃処理の実装
void Player::Attack(void)
{
    isAttacking_ = true;
    PlayAnim(ANIM::ATTACK, false, 0.1f);

    // 剣の攻撃判定を有効化
    if (sword_)
    {
        sword_->SetAttacking(true);
    }
}

// 描画処理
void Player::Draw(void) const
{
    UnitBase::Draw();
    sword_->Draw();
    DrawHpBar();

#ifdef _DEBUG
    DrawFormatString(0, 40, 0xffffff, "Player Pos:(%.2f, %.2f, %.2f)", trans_.pos.x, trans_.pos.y, trans_.pos.z);
    DrawFormatString(0, 60, 0xffffff, "IsGround: %s", isGround_ ? "TRUE" : "FALSE");
    DrawFormatString(0, 80, 0xffffff, "JumpPow: (%.2f, %.2f, %.2f)", jumpPow_.x, jumpPow_.y, jumpPow_.z);
    DrawFormatString(0, 100, 0xffffff, "Invincible: %.2f", invincibleTime_);
    DrawFormatString(0, 120, 0xffffff, "HitCooldown: %.2f", lastHitEnemyTime_);
#endif 
}

// 解放処理
void Player::Release(void)
{
    CollisionController::GetInstance().UnregisterUnit(this);
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

void Player::SetMovementEndbled(bool enabled)
{
    movementEnabled_ = enabled;
}

bool Player::IsMovementEndbled(void) const
{
    return movementEnabled_;
}

const Player::Param& Player::GetParam(void) const
{
    return param_;
}

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

    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), false);
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), true);
    DrawBox(barX, barY, barX + dispHPWidth, barY + barHeight, GetColor(255, 60, 60), true);
    DrawBox(barX, barY, barX + curHPWidth, barY + barHeight, GetColor(0, 255, 0), true);

    char hpStr[32];
    sprintf_s(hpStr, "%d / %d", param_.hp, param_.maxHp);

    int textWidth = font.GetDefaultTextWidth(hpStr);
    int textX = barX + (barWidth / 2) - (textWidth / 2);
    int textY = barY + (barHeight / 2) - 8;

    font.DrawDefaultText(textX, textY, hpStr, GetColor(255, 255, 255), 18);
}

void Player::TakeDamage(int damage)
{
    if (invincibleTime_ > 0.0f) return;

    param_.hp -= damage;
    if (param_.hp < 0) param_.hp = 0;

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

void Player::ProcessMove(void)
{
    if (!movementEnabled_) return;
    if (isAttacking_) return;

    auto& input = InputManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    prePos_ = trans_.pos;

    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    if (input.IsNew(KEY_INPUT_W)) { forwardInput += 1.0f; }
    if (input.IsNew(KEY_INPUT_S)) { forwardInput -= 1.0f; }
    if (input.IsNew(KEY_INPUT_A)) { rightInput -= 1.0f; }
    if (input.IsNew(KEY_INPUT_D)) { rightInput += 1.0f; }

    InputManager::JOYPAD_IN_STATE padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    VECTOR padInputDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    forwardInput += padInputDir.z;
    rightInput += padInputDir.x;

    isMoving_ = (fabs(forwardInput) > 0.0001f || fabs(rightInput) > 0.0001f);

    if (rightInput != 0.0f && forwardInput == 0.0f)
    {
        camera->SetFreezeFollow(true);

        float keyRotSpeed = rightInput * 1.5f;
        camera->SetKeyRotation(keyRotSpeed);

        VECTOR camPos = camera->GetPos();
        VECTOR toPlayer = VSub(trans_.pos, camPos);
        float currentHeight = toPlayer.y;
        toPlayer.y = 0.0f;

        float radius = VSize(toPlayer);
        float currentAngle = atan2f(toPlayer.x, toPlayer.z);
        float newAngle = currentAngle + Utility::Deg2RadF(keyRotSpeed);

        VECTOR newOffset;
        newOffset.x = radius * sinf(newAngle);
        newOffset.y = currentHeight;
        newOffset.z = radius * cosf(newAngle);

        trans_.pos = VAdd(camPos, newOffset);
        isMoving_ = true;

        VECTOR tangent;
        if (rightInput > 0)
        {
            tangent.x = -toPlayer.z;
            tangent.z = toPlayer.x;
        }
        else
        {
            tangent.x = toPlayer.z;
            tangent.z = -toPlayer.x;
        }

        tangent.y = 0.0f;
        tangent = VNorm(tangent);

        Quaternion targetLocalRot = Quaternion::LookRotation(tangent);
        trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
    }
    else
    {
        camera->SetFreezeFollow(false);

        if (forwardInput != 0.0f || rightInput != 0.0f)
        {
            VECTOR camForward = camera->GetFrontVec();
            camForward.y = 0.0f;
            camForward = VNorm(camForward);

            VECTOR camRight = camera->GetRightVec();
            camRight.y = 0.0f;
            camRight = VNorm(camRight);

            VECTOR moveDir = Utility::VECTOR_ZERO;
            moveDir = VAdd(moveDir, VScale(camForward, forwardInput));
            moveDir = VAdd(moveDir, VScale(camRight, -rightInput));
            moveDir.y = 0.0f;

            if (VSize(moveDir) > 0.0001f)
            {
                moveDir = VNorm(moveDir);

                VECTOR movement = VScale(moveDir, 10.5f);
                trans_.pos = VAdd(trans_.pos, movement);

                isMoving_ = true;

                Quaternion targetLocalRot = Quaternion::LookRotation(VScale(moveDir, -1.0f));
                trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
            }
        }
    }

    if (isMoving_)
    {
        PlayAnim(ANIM::WALK, true, 0.2f);
    }
    else
    {
        PlayAnim(ANIM::IDEL, true, 0.2f);
    }

    if (isGround_ && (input.IsTrgDown(KEY_INPUT_SPACE) || input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)))
    {
        jumpPow_ = VGet(0, param_.jumpPower, 0);
        isGround_ = false;
    }
}

void Player::DrawCollisionCapsuleDebug(void) const
{
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

void Player::OnCollisionEnter(const CollisionInfo& info)
{
    // 敵との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::ENEMY)
    {
        // 球体コライダでの衝突の場合
        if (info.myCollider->GetShape() == ColliderBase::SHAPE::SPHERE)
        {
            // ヒット判定のクールタイム中はダメージを受けない
            if (lastHitEnemyTime_ > 0.0f) return;

            // 無敵時間中はダメージを受けない
            if (invincibleTime_ <= 0.0f)
            {
                TakeDamage(10);

                // ヒット判定のクールタイムを設定
                lastHitEnemyTime_ = 0.5f;

#ifdef _DEBUG
                printfDx("プレイヤーが敵からダメージを受けた！\n");
#endif
            }
        }
    }
}

void Player::OnCollisionStay(const CollisionInfo& info)
{
    // 地面との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::GROUND)
    {
        if (info.myCollider->GetShape() == ColliderBase::SHAPE::LINE)
        {
            if (jumpPow_.y > 0.1f)
            {
                return;
            }

            trans_.pos = VAdd(info.hitPosition, VScale(Utility::DIR_U, 2.0f));
            jumpPow_ = Utility::VECTOR_ZERO;
            isGround_ = true;
        }
    }
}