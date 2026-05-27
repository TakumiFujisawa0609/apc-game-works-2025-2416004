#include "../../Pch.h"
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
#include "../../Collider/ColliderModel.h"

Player::Player(void)
    : modelId_(-1)
    , sword_(std::make_unique<Sword>())
    , fireAttack_(std::make_unique<FireAttack>())
    , waterAttack_(std::make_unique<WaterAttack>())
    , glider_(std::make_unique<Glider>())
    , originPos_(Utility::VECTOR_ZERO)
    , localStartPos_(COL_CAPSULE_START_POS)
    , localEndPos_(COL_CAPSULE_END_POS)
    , hpDisplay_(0) // Initialize内で設定
    , hpDelaySpeed_(DELAY_SPEED)
    , attackCoolTime_(0)
    , fireAttackCoolTime_(0)
    , waterAttackCoolTime_(0)
    , invincibleTime_(0)
    , lastHitEnemyTime_(0)
    , experience_(0)
    , glideExpTimer_(0.0f)
    , iconFire_(-1)
    , iconFireCD_(-1)
    , iconWater_(-1)
    , iconWaterCD_(-1)
    , isGround_(true)
    , isMoving_(false)
    , movementEnabled_(true)
    , isAttacking_(false)
    , isGliding_(false)
    , hasJumped_(false)
    , isAttack_(false)
    , UnitBase()
{
    speed_ = INITIALIZE_SPEED;
}
Player::~Player(void)
{
}

void Player::LoadParamCSV(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) { return; }

    // ヘッダー行をスキップ
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

    modelId_ = res.Load(ResourceManager::SRC::MODEL_PLAYER).handleId_;
    trans_.SetModel(modelId_);

    iconFire_ = res.Load(ResourceManager::SRC::IMG_FIRE_SUKILL).handleId_;
    iconFireCD_ = res.Load(ResourceManager::SRC::IMG_FIRE_SUKILL_CD).handleId_;
    iconWater_ = res.Load(ResourceManager::SRC::IMG_WATER_SUKILL).handleId_;
    iconWaterCD_ = res.Load(ResourceManager::SRC::IMG_WATER_SUKILL_CD).handleId_;

    sword_->Load();
    fireAttack_->Load();
    waterAttack_->Load();
    glider_->Load();
}

void Player::Initialize(void)
{
    auto& res = ResourceManager::GetInstance();

    // 初期化処理
    trans_.pos = Utility::VECTOR_ZERO;
    prePos_ = trans_.pos;
    trans_.rot = VGet(0, 0, 0);
    trans_.quaRotLocal = Quaternion::Identity();
    trans_.scl = PLAYER_SCL;
    radius_ = param_.collisionRadius;
    movePow_ = Utility::VECTOR_ZERO;
    jumpPow_ = Utility::VECTOR_ZERO;

    originPos_ = trans_.pos;

    const float animeSpIdel = 35.0f;
    const float animeSpWalk = 30.0f;
    const float animeSpAttack = 40.0f;
    const float animeSpJump = 50.0f;
    const float animeSpGlide = 30.0f;

    anim_ = std::make_unique<AnimationController>(modelId_);
    anim_->AddExternal(static_cast<int>(ANIM::IDEL), res.LoadModelDuplicate(
        ResourceManager::SRC::ANIM_PLAYER_IDEL), animeSpIdel);
    anim_->AddExternal(static_cast<int>(ANIM::WALK), res.LoadModelDuplicate(
        ResourceManager::SRC::ANIM_PLAYER_WALK), animeSpWalk);
    anim_->AddExternal(static_cast<int>(ANIM::ATTACK), res.LoadModelDuplicate(
        ResourceManager::SRC::ANIM_PLAYER_ATTACK), animeSpAttack);
    anim_->AddExternal(static_cast<int>(ANIM::JUMP), res.LoadModelDuplicate(
        ResourceManager::SRC::ANIM_PLAYER_JAMP), animeSpJump);
    anim_->AddExternal(static_cast<int>(ANIM::GLIDE), res.LoadModelDuplicate(
        ResourceManager::SRC::ANIM_PLAYER_GLIDE), animeSpGlide);

    EffectManager::GetInstance().Add(EffectManager::EFFECT::FIRE, 
        res.Load(ResourceManager::SRC::EFFECT_FIRE).handleId_);
    EffectManager::GetInstance().Add(EffectManager::EFFECT::WATER, 
        res.Load(ResourceManager::SRC::EFFECT_WATER).handleId_);
    EffectManager::GetInstance().Add(EffectManager::EFFECT::LEVER_UP, 
        res.Load(ResourceManager::SRC::EFFECT_LEVER_UP).handleId_);

    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_FIRE,
        res.Load(ResourceManager::SRC::SE_FIRE).handleId_);
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_WATER, 
        res.Load(ResourceManager::SRC::SE_WATER).handleId_);
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_LEVER_UP,
        res.Load(ResourceManager::SRC::SE_LEVER_UP).handleId_);
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SLASH, 
        res.Load(ResourceManager::SRC::SE_SLASH).handleId_);
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_GLIDER,
        res.Load(ResourceManager::SRC::SE_GLIDER).handleId_);


    PlayAnim(ANIM::IDEL, true, 0.0f);

    InitCollider();
    CollisionController::GetInstance().RegisterUnit(this);

    sword_->Initialize();
    fireAttack_->Initialize();
    waterAttack_->Initialize();
    glider_->Initialize();
}

void Player::InitCollider(void)
{
    // 接地判定用のライン
    ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &trans_, 
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    // 体当たり・被弾判定用のカプセル
    ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::PLAYER, &trans_, 
        localStartPos_, localEndPos_, radius_);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Player::Update(void)
{
    auto& input = InputManager::GetInstance();
    float deltaTime = SceneManager::GetInstance().GetDeltaTime();
    bool isAttackTrigger = input.IsTrgMouseLeft() ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER); 
    bool isFireTrigger = input.IsTrgDown(KEY_INPUT_Q) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::TOP);
    bool isWaterTrigger = input.IsTrgDown(KEY_INPUT_E) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);

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

    // 攻撃処理
    if (isAttackTrigger && attackCoolTime_ <= 0.0f && !isAttacking_)
    {
        Attack();
    }

    // 火魔法スキル
    if (isFireTrigger && fireAttackCoolTime_ <= 0.0f && !isAttacking_)
    {
        isAttack_ = true;
        FireAttackAction();
        PlayAnim(ANIM::ATTACK, false, 0.2f);
    }

    // 水魔法スキル
    if (isWaterTrigger && waterAttackCoolTime_ <= 0.0f && !isAttacking_)
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
        // 手のフレーム（27番）の行列（位置・回転・スケール全て）を取得
        MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(modelId_, 27);

        // 剣に行列を渡す（この後、Swordクラスにこの関数を作ります）
        sword_->SetHandMatrix(handMatrix);

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
            isGround_ = false;
            hasJumped_ = true;
            isGliding_ = false;
            glideExpTimer_ = 0.0f;
        }
        else
        {
            // 空中ならグライドのON/OFF切り替え
            isGliding_ = !isGliding_;
        }
    }

    // グライド中の処理
    if (isGliding_)
    {
        ProcessGlide();
        if (!SoundManager::GetInstance().IsPlaying(SoundManager::SOUND::SE_GLIDER))
        {
            SoundManager::GetInstance().Play(SoundManager::SOUND::SE_GLIDER);
        }
        
    }
    else
    {
        SoundManager::GetInstance().Stop(SoundManager::SOUND::SE_GLIDER);
    }
}

void Player::ProcessGlide(void)
{
    auto& input = InputManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    glideExpTimer_ += SceneManager::GetInstance().GetDeltaTime();
    if (glideExpTimer_ >= GLIDE_EXP_INTERVAL)
    {
        AddExperinece(GLIDE_EXP_AMOUNT);
        glideExpTimer_ = 0.0f; 
    }

    // グライド中はゆっくり降下する
    jumpPow_.y = -GLIDE_FALL_SPEED;

    // 自動前進処理
    VECTOR forward = trans_.quaRotLocal.GetForward();
    forward.y = 0.0f; // 水平移動に限定

    if (VSize(forward) > 0.0001f)
    {
        forward = VNorm(forward);

        float autoGlideSpeed = speed_ * 0.3f;

        VECTOR autoMove = VScale(forward, -autoGlideSpeed);
        trans_.pos = VAdd(trans_.pos, autoMove);
    }

    // 入力による移動制御
    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    // キーボード入力の取得
    if (input.IsPress(KEY_INPUT_W)) { forwardInput += 1.0f; }
    if (input.IsPress(KEY_INPUT_S)) { forwardInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_A)) { rightInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_D)) { rightInput += 1.0f; }

    // パッド入力の取得
    InputManager::JOYPAD_IN_STATE padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    VECTOR padInputDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    forwardInput += padInputDir.z;
    rightInput += padInputDir.x;

    // 入力がある場合のみ移動処理を行う
    if (forwardInput != 0.0f || rightInput != 0.0f)
    {
        // カメラの向きに基づいた移動方向の計算
        VECTOR camForward = camera->GetFrontVector();
        camForward.y = 0.0f;
        camForward = VNorm(camForward);

        VECTOR camRight = camera->GetRightVector();
        camRight.y = 0.0f;
        camRight = VNorm(camRight);

        VECTOR moveDir = Utility::VECTOR_ZERO;
        moveDir = VAdd(moveDir, VScale(camForward, forwardInput));
        moveDir = VAdd(moveDir, VScale(camRight, -rightInput));
        moveDir.y = 0.0f;

        if (VSize(moveDir) > 0.0001f)
        {
            moveDir = VNorm(moveDir);

            // 操作時のグライド速度を地上移動速度(speed_)の1.2倍に設定
            float glideControlSpeed = speed_ * 1.2f;
            VECTOR movement = VScale(moveDir, glideControlSpeed);

            trans_.pos = VAdd(trans_.pos, movement);

            // プレイヤーを移動方向に向かせる回転処理
            Quaternion targetLocalRot = Quaternion::LookRotation(VScale(moveDir, -1.0f));
            trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 15.0f);
        }
    }
}

void Player::UpdateAnimation(void)
{
    // 攻撃中は他のアニメーションに上書きしない
    if (isAttacking_) return;

    if (isAttack_) return;

    // グライド中かどうかの判定
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

    // 空中にいる場合（純粋な落下）
    else if (!isGround_)
    {
        if (jumpPow_.y > 0.0f)
        {
            // 上昇中（ジャンプ）
            if (anim_->GetPlayType() != static_cast<int>(ANIM::JUMP))
            {
                // ジャンプの出だしはループさせず(false)、勢いを見せる
                PlayAnim(ANIM::JUMP, false, 0.1f);
            }
        }
        else
        {
            // 落下中
            if (anim_->GetPlayType() != static_cast<int>(ANIM::IDEL))
            {
                // 落下は着地まで続くのでループ(true)させる
                PlayAnim(ANIM::IDEL, true, 0.2f);
            }
        }
    }
    // 地面にいる場合
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

    SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SLASH);

    if (sword_)
    {
        sword_->SetAttacking(true);
    }
}

void Player::FireAttackAction(void)
{
    if (!fireAttack_) return;

    fireAttack_->Initialize(this, Utility::VECTOR_ZERO);
    SoundManager::GetInstance().Play(SoundManager::SOUND::SE_FIRE);
    fireAttackCoolTime_ = FIRE_ATTACK_COOL_TIME_MAX;
}

void Player::WaterAttackAction(void)
{
    if (!waterAttack_) return;

    waterAttack_->Initialize(this, Utility::VECTOR_ZERO);
    SoundManager::GetInstance().Play(SoundManager::SOUND::SE_WATER);
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
    DrawHpBar();
    DrawLevelInfo();
    DrawSkillUI();
    DrawDistanceUI();

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

    // 座標・サイズ計算関連
    int barWidth = HP_BAR_WIDTH;
    int barHeight = HP_BAR_HEIGHT;
    int barX = (Application::SCREEN_SIZE_X - barWidth) / 2;
    int barY = Application::SCREEN_SIZE_Y - BAR_Y;

    // HP割合計算関連
    float hpRate = (float)param_.hp / param_.maxHp;
    float dispRate = hpDisplay_ / param_.maxHp;

    // 0.0～1.0の範囲に収める
    hpRate = std::clamp(hpRate, 0.0f, 1.0f);
    dispRate = std::clamp(dispRate, 0.0f, 1.0f);

    // 描画幅の計算
    int curHPWidth = (int)(barWidth * hpRate);
    int dispHPWidth = (int)(barWidth * dispRate);

    // 図形描画関連
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), false);
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), true);
    DrawBox(barX, barY, barX + dispHPWidth, barY + barHeight, GetColor(255, 60, 60), true);
    DrawBox(barX, barY, barX + curHPWidth, barY + barHeight, GetColor(0, 255, 0), true);

    // テキスト描画関連
    char hpStr[32];
    sprintf_s(hpStr, "%d / %d", param_.hp, param_.maxHp);

    // テキストをバーの中央に配置するための計算
    int textWidth = font.GetDefaultTextWidth(hpStr);
    int textX = barX + (barWidth / 2) - (textWidth / 2);
    int textY = barY + (barHeight / 2) - 8;

    // HP数値の描画
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
    if (!movementEnabled_) { return; }
    if (isAttacking_) { return; }
    if (fireAttack_->IsActive()) { return; }
    if (waterAttack_->IsActive()) { return; }

    auto& input = InputManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    // 移動前の座標を保存
    prePos_ = trans_.pos;

    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    // キーボード入力
    if (input.IsPress(KEY_INPUT_W)) { forwardInput += 1.0f; }
    if (input.IsPress(KEY_INPUT_S)) { forwardInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_A)) { rightInput -= 1.0f; }
    if (input.IsPress(KEY_INPUT_D)) { rightInput += 1.0f; }

    // パッド入力
    InputManager::JOYPAD_IN_STATE padState;
    padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    VECTOR padInputDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    forwardInput += padInputDir.z;
    rightInput += padInputDir.x;

    // 移動中かどうかの判定（微小な入力は無視）
    isMoving_ = (fabs(forwardInput) > 0.0001f || fabs(rightInput) > 0.0001f);
    
    // 左右入力のみの場合
    if (rightInput != 0.0f && forwardInput == 0.0f)
    {
        // カメラの追従を一時停止し、手動制御
        camera->SetFollowFreeze(true);

        // 回転速度の計算（1.5fは回転速度係数）
        float keyRotSpeed = rightInput * 1.5f;
        camera->SetKeyRotationSpeed(keyRotSpeed);

        // カメラからプレイヤーへのベクトルを計算
        VECTOR camPos = camera->GetPos();
        VECTOR toPlayer = VSub(trans_.pos, camPos);
        float currentHeight = toPlayer.y;
        toPlayer.y = 0.0f;

        // 円運動の計算（極座標変換）
        float radius = VSize(toPlayer);
        float currentAngle = atan2f(toPlayer.x, toPlayer.z);
        float newAngle = currentAngle + Utility::Deg2RadF(keyRotSpeed);

        // 新しい座標を算出
        VECTOR newOffset;
        newOffset.x = radius * sinf(newAngle);
        newOffset.y = currentHeight;
        newOffset.z = radius * cosf(newAngle);

        trans_.pos = VAdd(camPos, newOffset);
        isMoving_ = true;

        // 回転処理：移動方向（接線方向）を向く
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

        // クォータニオンによる回転補間
        Quaternion targetLocalRot = Quaternion::LookRotation(tangent);
        trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
    }
    else
    {
        camera->SetFollowFreeze(false);

        if (forwardInput != 0.0f || rightInput != 0.0f)
        {
            // カメラの向きベクトルを水平面に投影
            VECTOR camForward = camera->GetFrontVector();
            camForward.y = 0.0f;
            camForward = VNorm(camForward);

            VECTOR camRight = camera->GetRightVector();
            camRight.y = 0.0f;
            camRight = VNorm(camRight);

            // 合成移動方向の計算
            VECTOR moveDir = Utility::VECTOR_ZERO;
            moveDir = VAdd(moveDir, VScale(camForward, forwardInput));
            moveDir = VAdd(moveDir, VScale(camRight, -rightInput));
            moveDir.y = 0.0f;

            if (VSize(moveDir) > 0.0001f)
            {
                moveDir = VNorm(moveDir);

                // 座標更新
                VECTOR movement = VScale(moveDir, 10.5f);
                trans_.pos = VAdd(trans_.pos, movement);

                isMoving_ = true;

                // モデルの向きを移動方向に合わせる
                Quaternion targetLocalRot = Quaternion::LookRotation(VScale(moveDir, -1.0f));
                trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
            }
        }
    }
}

void Player::DrawCollisionCapsuleDebug(void) const
{
    // カプセルコライダが存在するか確認
    if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) > 0)
    {
        // 基底クラスから派生クラスへキャストして情報を取得
        const ColliderCapsule* capsule = dynamic_cast<const ColliderCapsule*>
            (ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));

        if (capsule)
        {
            VECTOR start = capsule->GetLocalStartPos();
            VECTOR end = capsule->GetLocalEndPos();
            float r = capsule->GetRadius();

            DrawCapsule3D(start, end, r, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
        }
    }
}

void Player::DrawSkillUI(void) const
{
    int screenH = Application::SCREEN_SIZE_Y;
    int startX = 0;  
    int startY = 0;
    int iconSize = 64;
    int spacing = 80;

    auto& font = Font::GetInstance();
    int fireX = startX;

    if (fireAttackCoolTime_ > 0.0f)
    {
        // クールタイム中
        DrawRotaGraph3(fireX, startY, 0, 0, 1.0f, 1.0f, 0.0f, iconFireCD_, true);
    }
    else 
    {
        // 使用可能
        DrawRotaGraph3(fireX, startY, 0, 0, 1.0f, 1.0f, 0.0f, iconFire_, true);
    }

    
    if (waterAttackCoolTime_ > 0.0f) 
    {
        // クールタイム中
        DrawRotaGraph3(fireX, startY, 0, 0, 1.0f, 1.0f, 0.0f, iconWaterCD_, true);
    }
    else
    {
        // 使用可能
        DrawRotaGraph3(fireX, startY, 0, 0, 1.0f, 1.0f, 0.0f, iconWater_, true);
    }
}

void Player::OnCollisionEnter(const CollisionInfo& info)
{
    if (info.hitCollider->GetCollisionTag() == ColliderBase::TAG::ENEMY)
    {
        if (info.myCollider->GetShapeType() == ColliderBase::SHAPE::CAPSULE ||
            info.myCollider->GetShapeType() == ColliderBase::SHAPE::SPHERE)
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
    if (info.hitCollider->GetCollisionTag() == ColliderBase::TAG::GROUND || 
        info.hitCollider->GetCollisionTag() == ColliderBase::TAG::STAGE)
    {
        if (info.myCollider->GetShapeType() == ColliderBase::SHAPE::LINE)
        {
            if (jumpPow_.y > 0.1f)
            {
                return;
            }

            trans_.pos = VAdd(info.hitPosition, VScale(Utility::DIR_UP, 2.0f));
            jumpPow_ = Utility::VECTOR_ZERO;
            isGround_ = true;
        }
    }
}

void Player::CollisionWithModel(void)
{
    int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);
    if (ownColliders_.count(capsuleType) == 0) return;

    ColliderCapsule* myCapsule = dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));
    if (!myCapsule) return;

    // カプセルの情報を取得
    VECTOR capsuleStart = myCapsule->GetWorldStartPos();
    VECTOR capsuleEnd = myCapsule->GetWorldEndPos();
    float capsuleRadius = myCapsule->GetRadius();
    // 判定用の中心点（簡易的に足元から少し上の位置などで調整）
    VECTOR capsuleCenter = VScale(VAdd(capsuleStart, capsuleEnd), 0.5f);

    for (const auto& hitCol : hitColliders_)
    {
        if (hitCol->GetCollisionTag() != ColliderBase::TAG::STAGE) continue;
        if (hitCol->GetShapeType() != ColliderBase::SHAPE::MODEL) continue;

        const ColliderModel* hitModel = dynamic_cast<const ColliderModel*>(hitCol);
        if (!hitModel) continue;

        int modelId = hitModel->GetFollowTarget()->modelId;
        if (modelId < 0) continue;

        // 周辺ポリゴンを取得（検索半径はカプセル半径 + 余裕分）
        float searchRadius = capsuleRadius + 20.0f;
        MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Sphere(modelId, -1, capsuleCenter, searchRadius);

        if (hitResult.HitNum > 0)
        {
            for (int i = 0; i < hitResult.HitNum; i++)
            {
                MV1_COLL_RESULT_POLY poly = hitResult.Dim[i];
                if (hitModel->IsExcludedFrame(poly.FrameIndex)) continue;

                VECTOR v = VSub(capsuleCenter, poly.Position[0]);

                // 法線方向への投影距離（平面から球体中心までの最短距離）
                float dist = VDot(v, poly.Normal);

                // 距離が半径より小さければ衝突している
                if (dist < searchRadius)
                {
                    // 水平方向の押し出し成分を抽出
                    VECTOR pushNormal = VGet(poly.Normal.x, 0.0f, poly.Normal.z);
                    if (VSquareSize(pushNormal) > 0.0001f)
                    {
                        pushNormal = VNorm(pushNormal);
                        // めり込み量分だけ押し出す
                        float pushDist = searchRadius - dist;
                        trans_.pos = VAdd(trans_.pos, VScale(pushNormal, pushDist));

                        // 座標更新に合わせてカプセルの中心もズラす（連続衝突への対応）
                        capsuleCenter = VScale(VAdd(myCapsule->GetWorldStartPos(), myCapsule->GetWorldEndPos()), 0.5f);
                    }
                }
            }
        }
        MV1CollResultPolyDimTerminate(hitResult);
    }
}

void Player::Collision(void)
{
    UnitBase::Collision();

    CollisionWithModel();

    bool wasJumping = (jumpPow_.y != 0.0f);

    if (Utility::EqualsVZero(jumpPow_))
    {
        isGround_ = true;
        hasJumped_ = false;
        isGliding_ = false; 
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

    VECTOR dirGravity = Utility::DIR_DOWN;
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
    speed_ += LEVEL_UP_STAT * 0.5f;
    param_.hp = param_.maxHp;
    hpDisplay_ = param_.hp;

    int required = CalcRequiredExp(param_.level - 1);
    experience_ -= required;
    if (experience_ < 0) { experience_ = 0; }

    EffectManager::GetInstance().Play(EffectManager::EFFECT::LEVER_UP, trans_.pos,
        trans_.quaRot,
        40.0f,
        SoundManager::SOUND::NONE);

    SoundManager::GetInstance().Play(SoundManager::SOUND::SE_LEVER_UP);
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

    // 左下からのオフセット位置
    int levelX = 40;
    int levelY = Application::SCREEN_SIZE_Y - 80;

    char levelStr[64];

    sprintf_s(levelStr, "Lv.%d", param_.level);

    unsigned int color = (param_.level >= param_.maxLevel) ?
        GetColor(255, 215, 0) : GetColor(255, 255, 255);

    font.DrawDefaultText(levelX, levelY, levelStr, color, 48, Font::FONT_TYPE_ANTIALIASING_EDGE);

    if (param_.level >= param_.maxLevel)
    {
        font.DrawDefaultText(levelX + 160, levelY + 20, "MAX", color, 20, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }
}

void Player::DrawDistanceUI(void) const
{
    auto& font = Font::GetInstance();

    // 距離を取得 (XZ平面での原点からの距離)
    float distance = GetDistanceFromOriginXZ();

    // 表示用文字列の作成
    char distStr[64];
    sprintf_s(distStr, "現在の移動距離: %.1f m", distance);

    // 描画座標の計算
    int textWidth = font.GetDefaultTextWidth(distStr);
    int drawX = (Application::SCREEN_SIZE_X / 2) - textWidth;
    int drawY = 30;

    // 描画 
    unsigned int color = GetColor(255, 255, 255); 
    font.DrawDefaultText(drawX, drawY, distStr, color, 42, Font::FONT_TYPE_ANTIALIASING_EDGE);
}

int Player::GetHP(void)
{
    return param_.hp;
}
