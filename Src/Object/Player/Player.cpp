#include "Player.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/Camera.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Manager/Decoration/EffectManager.h"
#include "../Common/AnimationController.h"
#include "../Common/Transform.h"
#include "../../DrawUI/Font.h"
#include "Sword.h"
#include "FireAttack.h"
#include "WaterAttack.h"
#include "Glider.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Application.h"

// コンストラクタ
Player::Player(void)
    // モデルIDの初期化
    : modelId_(-1)

    // 剣オブジェクト生成
    , sword_(std::make_unique<Sword>())
    
    // 火攻撃オブジェクト生成
    , fireAttack_(std::make_unique<FireAttack>())
    
    // 水攻撃オブジェクト生成
    , waterAttack_(std::make_unique<WaterAttack>())
    
    // グライダーオブジェクト生成
    , glider_(std::make_unique<Glider>())

    // 原点位置の初期化
    , originPos_(Utility::VECTOR_ZERO)
    , localStartPos_(COL_CAPSULE_START_POS)
    , localEndPos_(COL_CAPSULE_END_POS)
    , hpDisplay_(param_.hp)
    , hpDelaySpeed_(DELAY_SPEED)
    , attackCoolTime_(0)
    , fireAttackCoolTime_(0)
    , waterAttackCoolTime_(0)
    , invincibleTime_(0)
    , lastHitEnemyTime_(0)
    , experience_(0)
    , isGround_(true)
    , isMoving_(false)
    , movementEnabled_(true)
    , isAttacking_(false)
    , isGliding_(false)
    , hasJumped_(false)
    , isAttack_(false)
    , UnitBase()
{
}

Player::~Player(void)
{
}

void Player::LoadParamCSV(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) { return; }

    std::string line;
    std::getline(file, line);

    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;

        std::getline(ss, value, ',');
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

void Player::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    modelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
    trans_.SetModel(modelId_);

    anim_ = std::make_unique<AnimationController>(modelId_);
    anim_->AddExternal(static_cast<int>(ANIM::IDEL), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_IDEL), 35.0f);
    anim_->AddExternal(static_cast<int>(ANIM::WALK), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_WALK), 30.0f);
    anim_->AddExternal(static_cast<int>(ANIM::ATTACK), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_ATTACK), 50.0f);
    anim_->AddExternal(static_cast<int>(ANIM::JUMP), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_JAMP), 10.0f);
    anim_->AddExternal(static_cast<int>(ANIM::GLIDE), res.LoadModelDuplicate(ResourceManager::SRC::ANIM_PLAYER_GLIDE), 30.0f);

    EffectManager::GetInstance().Add(EffectManager::EFFECT::FIRE, res.Load(ResourceManager::SRC::EFFECT_FIRE).handleId_);
    EffectManager::GetInstance().Add(EffectManager::EFFECT::WATER, res.Load(ResourceManager::SRC::EFFECT_WATER).handleId_);
    EffectManager::GetInstance().Add(EffectManager::EFFECT::LEVER_UP, res.Load(ResourceManager::SRC::EFFECT_LEVER_UP).handleId_);

    sword_->Load();
    fireAttack_->Load();
    waterAttack_->Load();
    glider_->Load();
}

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

    originPos_ = trans_.pos;

    PlayAnim(ANIM::IDEL, true, 0.0f);

    InitCollider();
    CollisionController::GetInstance().RegisterUnit(this);

    sword_->Init();
    fireAttack_->Init();
    waterAttack_->Init();
    glider_->Init();
}

void Player::InitCollider(void)
{
    ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &trans_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::PLAYER, &trans_, localStartPos_, localEndPos_, radius_);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Player::Update(void)
{
    float deltaTime = SceneManager::GetInstance().GetDeltaTime();

    if (invincibleTime_ > 0.0f)
    {
        invincibleTime_ -= deltaTime;
    }

    if (lastHitEnemyTime_ > 0.0f)
    {
        lastHitEnemyTime_ -= deltaTime;
    }

    ProcessMove();
    ProcessJump();

    auto& input = InputManager::GetInstance();

    if (input.IsTrgMouseLeft() && attackCoolTime_ <= 0.0f && !isAttacking_)
    {
        Attack();
    }

    if (input.IsTrgDown(KEY_INPUT_Q) && fireAttackCoolTime_ <= 0.0f && !isAttacking_)
    {
        isAttack_ = true;
        FireAttackAction();
        PlayAnim(ANIM::ATTACK, false, 0.2f);
    }

    if (input.IsTrgDown(KEY_INPUT_E) && waterAttackCoolTime_ <= 0.0f && !isAttacking_)
    {
        isAttack_ = true;

        WaterAttackAction();

        PlayAnim(ANIM::ATTACK, false, 0.2f);
    }

    if (attackCoolTime_ > 0.0f)
    {
        attackCoolTime_ -= deltaTime;
        if (attackCoolTime_ < 0.0f) attackCoolTime_ = 0.0f;
    }

    if (fireAttackCoolTime_ > 0.0f)
    {
        fireAttackCoolTime_ -= deltaTime;
        if (fireAttackCoolTime_ < 0.0f)
        {
            fireAttackCoolTime_ = 0.0f;
        }
    }

    if (waterAttackCoolTime_ > 0.0f)
    {
        waterAttackCoolTime_ -= deltaTime;
        if (waterAttackCoolTime_ < 0.0f)
        {
            waterAttackCoolTime_ = 0.0f;
        }
    }

    if (isAttack_ && anim_)
    {
        if (!anim_->IsPlaying(static_cast<int>(ANIM::ATTACK)))
        {
            isAttack_ = false;
        }
    }

    if (isAttacking_ && anim_)
    {
        if (!anim_->IsPlaying(static_cast<int>(ANIM::ATTACK)))
        {
            isAttacking_ = false;
            attackCoolTime_ = ATTACK_COOL_TIME_MAX;

            if (sword_)
            {
                sword_->SetAttacking(false);
            }
        }
    }

    UpdateAnimation();

    UnitBase::Update();


    if (hpDisplay_ > param_.hp)
    {
        hpDisplay_ -= hpDelaySpeed_;
        if (hpDisplay_ < param_.hp) hpDisplay_ = param_.hp;
    }
    else
    {
        hpDisplay_ = param_.hp;
    }

    if (sword_)
    {
        sword_->SetPlayerRotation(trans_.quaRotLocal);
        VECTOR swordFrame = MV1GetFramePosition(modelId_, 26);
        sword_->SetFramePos(swordFrame);
        sword_->SetAttacking(isAttacking_);
        sword_->Update();
    }

    if (fireAttack_ && fireAttack_->IsActive())
    {
        fireAttack_->Update();
    }

    if (waterAttack_ && waterAttack_->IsActive())
    {
        waterAttack_->Update();
    }

    if (glider_)
    {
        glider_->SetPlayerRotation(trans_.quaRotLocal);
        VECTOR gliderFrame = MV1GetFramePosition(modelId_, 26);
        glider_->SetFramePos(gliderFrame);
        glider_->SetGliding(isGliding_);
        glider_->Update();
    }
}

void Player::ProcessJump(void)
{
    auto& input = InputManager::GetInstance();
    bool jumpInput = input.IsTrgDown(KEY_INPUT_SPACE) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

    if (jumpInput)
    {
        if (isGround_)
        {
            // 地面にいるならジャンプ
            jumpPow_ = VGet(0, param_.jumpPower, 0);
            isGround_ = false;  // 即座に空中扱いにする
            hasJumped_ = true;
            isGliding_ = false;
        }
        else
        {
            // 空中ならグライドのON/OFF切り替え
            // hasJumped_ のチェックは外し、「崖から落ちた時」でもグライドできるようにする
            isGliding_ = !isGliding_;
        }
    }

    // グライド中の処理
    if (isGliding_)
    {
        ProcessGlide();
    }
}
void Player::ProcessGlide(void)
{
    auto& input = InputManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    // グライド中はゆっくり降下
    jumpPow_.y = -GLIDE_FALL_SPEED;

    // 前方方向を取得（現在のプレイヤーの向き）
    VECTOR forward = trans_.quaRotLocal.GetForward();
    forward.y = 0.0f;

    // 前方向が有効な場合、自動的に前進（グライド感を出す）
    if (VSize(forward) > 0.0001f)
    {
        forward = VNorm(forward);
        VECTOR autoMove = VScale(forward, -3.0f);
        trans_.pos = VAdd(trans_.pos, autoMove);
    }

    // 水平方向の入力を取得
    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    if (input.IsPress(KEY_INPUT_W)) { forwardInput += 1.0f; }
    if (input.IsPress(KEY_INPUT_S)) { forwardInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_A)) { rightInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_D)) { rightInput += 1.0f; }

    InputManager::JOYPAD_IN_STATE padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    VECTOR padInputDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    forwardInput += padInputDir.z;
    rightInput += padInputDir.x;

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
            VECTOR movement = VScale(moveDir, GLIDE_HORIZONTAL_SPEED);
            trans_.pos = VAdd(trans_.pos, movement);

            Quaternion targetLocalRot = Quaternion::LookRotation(VScale(moveDir, -1.0f));
            trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 15.0f);
        }
    }
}

void Player::UpdateAnimation(void)
{
    // 1. 攻撃中は他のアニメーションに上書きしない
    if (isAttacking_) return;

    if (isAttack_) return;

    // 2. ★最優先：グライド中かどうかの判定
    if (isGliding_)
    {
        // まだグライドアニメーションになっていないなら再生開始
        if (anim_->GetPlayType() != static_cast<int>(ANIM::GLIDE))
        {
            // ブレンド時間を短め(0.1f)に設定して素早く切り替える
            PlayAnim(ANIM::GLIDE, false, 0.1f);
        }

        // 目標フレームに達したら停止
        if (anim_->GetCurrentFrame() >= 2000)
        {
            anim_->PauseAtFrame(2000);
        }

    }

    // 3. 空中にいる場合（純粋な落下）
    else if (!isGround_)
    {
        // ここが ANIM::IDEL になっていたのを、適切な落下アニメ（JUMPなど）に変更
        if (anim_->GetPlayType() != static_cast<int>(ANIM::IDEL))
        {
            PlayAnim(ANIM::IDEL, false, 0.2f);
        }
    }
    // 4. 地面にいる場合
    else
    {
        if (isMoving_)
        {
            if (anim_->GetPlayType() != static_cast<int>(ANIM::WALK))
            {
                PlayAnim(ANIM::WALK, true, 0.2f);
            }
        }
        else
        {
            if (anim_->GetPlayType() != static_cast<int>(ANIM::IDEL))
            {
                PlayAnim(ANIM::IDEL, true, 0.2f);
            }
        }
    }

    if (anim_->IsPaused())
    {
        anim_->Resume();
    }
}

void Player::Attack(void)
{
    isAttacking_ = true;
    PlayAnim(ANIM::ATTACK, false, 0.1f);

    if (sword_)
    {
        sword_->SetAttacking(true);
    }
}

void Player::FireAttackAction(void)
{
    if (!fireAttack_) return;

    fireAttack_->Init(this, Utility::VECTOR_ZERO);
    fireAttackCoolTime_ = FIRE_ATTACK_COOL_TIME_MAX;
}

void Player::WaterAttackAction(void)
{
    if (!waterAttack_) return;

    waterAttack_->Init(this, Utility::VECTOR_ZERO);
    waterAttackCoolTime_ = WATER_ATTACK_COOL_TIME_MAX;
}

void Player::Draw(void) const
{
    UnitBase::Draw();
    sword_->Draw();

    if (fireAttack_ && fireAttack_->IsActive())
    {
        fireAttack_->Draw();
    }

    if (waterAttack_ && waterAttack_->IsActive())
    {
        waterAttack_->Draw();
    }

    if (glider_)
    {
        glider_->Draw();
    }
   // DrawHpBar();
   // DrawLevelInfo();

#ifdef _DEBUG
#endif
}

void Player::Release(void)
{
    CollisionController::GetInstance().UnregisterUnit(this);
    UnitBase::Release();

    if (sword_)
    {
        sword_->Release();
        sword_.reset();
    }

    if (fireAttack_)
    {
        fireAttack_->Release();
        fireAttack_.reset();
    }

    if (waterAttack_)
    {
        waterAttack_->Release();
        waterAttack_.reset();
    }

    if (glider_)
    {
        glider_->Release();
        glider_.reset();
    }

    if (anim_)
    {
        anim_->Release();
        anim_.reset();
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

    if (input.IsPress(KEY_INPUT_W)) { forwardInput += 1.0f; }
    if (input.IsPress(KEY_INPUT_S)) { forwardInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_A)) { rightInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_D)) { rightInput += 1.0f; }

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
}

void Player::DrawCollisionCapsuleDebug(void) const
{
    if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) > 0)
    {
        const ColliderCapsule* capsule = dynamic_cast<const ColliderCapsule*>(ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));

        if (capsule)
        {
            VECTOR start = capsule->GetPosStart();
            VECTOR end = capsule->GetPosEnd();
            float r = capsule->GetRadius();

            DrawCapsule3D(start, end, r, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
        }
    }
}

void Player::OnCollisionEnter(const CollisionInfo& info)
{
    if (info.hitCollider->GetTag() == ColliderBase::TAG::ENEMY)
    {
        if (info.myCollider->GetShape() == ColliderBase::SHAPE::CAPSULE || info.myCollider->GetShape() == ColliderBase::SHAPE::SPHERE)
        {
            if (lastHitEnemyTime_ > 0.0f) { return; }

            if (invincibleTime_ <= 0.0f)
            {
                TakeDamage(10);
                lastHitEnemyTime_ = HIT_COOL_TIME;
            }
        }
    }
}

void Player::OnCollisionStay(const CollisionInfo& info)
{
    if (info.hitCollider->GetTag() == ColliderBase::TAG::GROUND || info.hitCollider->GetTag() == ColliderBase::TAG::STAGE)
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

void Player::Collision(void)
{
    UnitBase::Collision();

    bool wasJumping = (jumpPow_.y != 0.0f);

    if (Utility::EqualsVZero(jumpPow_))
    {
        isGround_ = true;
        hasJumped_ = false; // ジャンプフラグのリセット
        isGliding_ = false; // 着地したらグライド解除
    }
    else
    {
        // 速度が残っている（上昇中、または落下中）なら空中
        isGround_ = false;
    }
}

void Player::CalcGravityPow(void)
{
    if (isGliding_)
    {
        return;
    }

    VECTOR dirGravity = Utility::DIR_D;
    VECTOR gravity = VScale(dirGravity, GRAVITY_POW);
    jumpPow_ = VAdd(jumpPow_, gravity);

    float currentSpeed = -jumpPow_.y;

    if (currentSpeed > MAX_FALL_SPEED)
    {
        jumpPow_.y = -MAX_FALL_SPEED;
    }
}

float Player::GetDistanceFromOrigin(void) const
{
    VECTOR diff = VSub(trans_.pos, originPos_);
    return VSize(diff);
}

float Player::GetDistanceFromOriginXZ(void) const
{
    VECTOR diff = VSub(trans_.pos, originPos_);
    diff.y = 0.0f;
    return VSize(diff);
}

void Player::SetOriginPos(const VECTOR& pos)
{
    originPos_ = pos;
}

const VECTOR& Player::GetOriginPos(void) const
{
    return originPos_;
}

void Player::LevelUp(void)
{
    if (param_.level >= param_.maxLevel)
    {
        return;
    }

    param_.level++;
    param_.attack += LEVEL_UP_STAT;
    param_.defensse += LEVEL_UP_STAT;
    param_.maxHp += LEVEL_UP_STAT;
    param_.hp = param_.maxHp;
    hpDisplay_ = param_.hp;

    int required = CalcRequiredExp(param_.level - 1);
    experience_ -= required;
    if (experience_ < 0) { experience_ = 0; }
}

void Player::AddExperinece(int exp)
{
    if (param_.level >= param_.maxLevel) { return; }

    experience_ += exp;

    while (IsLevelUp())
    {
        LevelUp();
    }
}

bool Player::IsLevelUp(void) const
{
    if (param_.level >= param_.maxLevel)
    {
        return false;
    }

    return experience_ >= CalcRequiredExp(param_.level);
}

int Player::CalcRequiredExp(int level) const
{
    return static_cast<int>(BASE_EXP * pow(EXP_MULTIPLIER, level - 1));
}

int Player::GetLevel(void) const
{
    return param_.level;
}

int Player::GetExperinece(void) const
{
    return experience_;
}

int Player::GetRequireExp(void) const
{
    if (param_.level >= param_.maxLevel)
    {
        return 0;
    }

    return CalcRequiredExp(param_.level);
}

void Player::DrawLevelInfo(void) const
{
    auto& font = Font::GetInstance();

    int levelX = 20;
    int levelY = Application::SCREEN_SIZE_Y - 150;

    char levelStr[64];
    sprintf_s(levelStr, "Level: %d", param_.level);
    font.DrawDefaultText(levelX, levelY, levelStr, GetColor(255, 255, 255), 20);

    if (param_.level < param_.maxLevel)
    {
        int barWidth = 200;
        int barHeight = 15;
        int barX = levelX;
        int barY = levelY + 30;

        int required = GetRequireExp();
        float expRate = (float)experience_ / required;
        expRate = std::clamp(expRate, 0.0f, 1.0f);

        int expBarWidth = (int)(barWidth * expRate);

        DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(50, 50, 50), true);
        DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(255, 255, 255), false);
        DrawBox(barX, barY, barX + expBarWidth, barY + barHeight, GetColor(100, 200, 255), true);

        char expStr[64];
        sprintf_s(expStr, "%d / %d", experience_, required);
        int textWidth = font.GetDefaultTextWidth(expStr);
        int textX = barX + (barWidth / 2) - (textWidth / 2);
        int textY = barY + 1;
        font.DrawDefaultText(textX, textY, expStr, GetColor(255, 255, 255), 14);
    }
    else
    {
        font.DrawDefaultText(levelX, levelY + 30, "MAX LEVEL", GetColor(255, 215, 0), 18);
    }
}