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
#include "../../Collider/ColliderCapsule.h"

    // コンストラクタ
    Player::Player(void)
        //　初期化リスト
        : modelId_(-1)                          // モデルIDの初期化
        , sword_(std::make_unique<Sword>())     // 剣の生成
        , originPos_(Utility::VECTOR_ZERO)      // 原点座標の初期化
        , localStartPos_(COL_CAPSULE_START_POS) // カプセルコライダー用始点の初期化
        , localEndPos_(COL_CAPSULE_END_POS)     // カプセルコライダー用終端初期化
        , hpDisplay_(0)                         // 表示用体力の初期化
        , hpDelaySpeed_(DELAY_SPEED)            // 体力遅延減少速度の初期化
        , experience_(0)                        // 経験値の初期化
        , isGround_(true)                       // 地面のフラグ初期化
        , isMoving_(false)                      // 移動中かフラグ初期化
        , movementEnabled_(true)                // 移動か可能か具ラグ初期化
        , isAttacking_(false)                   // 攻撃中華フラグ初期化  
        , UnitBase()
    {
        hpDisplay_ = param_.hp;
        hpDelaySpeed_ = DELAY_SPEED;
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

        originPos_ = trans_.pos;

        PlayAnim(ANIM::IDEL, true, 0.0f);

        InitCollider();
        CollisionController::GetInstance().RegisterUnit(this);
        sword_->Init();
    }

    // コライダ初期化
    void Player::InitCollider(void)
    {
        // ラインコライダーの登録
        ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::PLAYER, &trans_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
        ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

        // カプセルコライダーの登録
        ColliderCapsule* colCapsule = new ColliderCapsule(ColliderBase::TAG::PLAYER, &trans_, localStartPos_, localEndPos_, radius_);
        ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

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

        // 剣にプレイヤーの回転を設定
        if (sword_)
        {
            sword_->SetPlayerRotation(trans_.quaRotLocal);
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
        DrawLevelInfo();

    #ifdef _DEBUG
        DrawFormatString(0, 0, GetColor(255, 255, 255),"Player Rot : X: % .1f Y : % .1f Z : % .1f", Utility::Rad2DegF(trans_.quaRot.x), Utility::Rad2DegF(trans_.quaRot.y), Utility::Rad2DegF(trans_.quaRot.z));
   
    #endif 
    }

    // 解放処理
    void Player::Release(void)
    {
        CollisionController::GetInstance().UnregisterUnit(this);
        UnitBase::Release();

        if (sword_)
        {
            sword_->Release();
            sword_.reset();
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

    // デバック用コライダーの表示
    void Player::DrawCollisionCapsuleDebug(void) const
    {
        // カプセルコライダーの表示
        if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) > 0)
        {
            const ColliderCapsule* capsule = dynamic_cast<const ColliderCapsule*>(ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));

            if (capsule)
            {
                VECTOR start = capsule->GetPosStart();

                VECTOR end = capsule->GetPosEnd();

                float r = capsule->GetRadius();

                // カプセル描画(緑色)
                DrawCapsule3D(start, end, r, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), false);
            }
        }
    }

    void Player::OnCollisionEnter(const CollisionInfo& info)
    {
        // 敵との衝突
        if (info.hitCollider->GetTag() == ColliderBase::TAG::ENEMY)
        {
            // カプセルまたは球体コライダの衝突の場合
            if (info.myCollider->GetShape() == ColliderBase::SHAPE::CAPSULE || info.myCollider->GetShape() == ColliderBase::SHAPE::SPHERE)
            {
                // ヒット判定のクルータイム中は無効
                if (lastHitEnemyTime_ > 0.0f) { return; }

                // 無敵時間中はダメージを受けない
                if (invincibleTime_ <= 0.0f)
                {
                    // ダメージ処理
                    TakeDamage(10);

                    // ヒット判定のクールタイムを設定
                    lastHitEnemyTime_ = HIT_COOL_TIME;

#ifdef _DEBUG
                    printfDx("プレイヤーが敵からダメージを受けた！ HP: %d\n", param_.hp);
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

    void Player::Collision(void)
    {
        // 移動処理
        trans_.pos = VAdd(trans_.pos, movePow_);

        // ジャンプ量を加算
        trans_.pos = VAdd(trans_.pos, jumpPow_);

        // 敵との衝突（カプセルと球体の判定）
        CollisionWithEnemy();

        // カプセルとの衝突（剣の攻撃判定など）
        CollisionWithCapsule();

        // 地面との衝突
        CollisionGravity();
    }

    // 原点からの移動距離を取得（3D距離）
    float Player::GetDistanceFromOrigin(void) const
    {
        VECTOR diff = VSub(trans_.pos, originPos_);
        return VSize(diff);
    }

    // 原点からの移動距離（XZ平面のみ）を取得
    float Player::GetDistanceFromOriginXZ(void) const
    {
        VECTOR diff = VSub(trans_.pos, originPos_);
        diff.y = 0.0f;  // Y座標を無視
        return VSize(diff);
    }

    // 原点座標を設定
    void Player::SetOriginPos(const VECTOR& pos)
    {
        originPos_ = pos;
    }

    // 原点座標を取得
    const VECTOR& Player::GetOriginPos(void) const
    {
        return originPos_;
    }

    // レベルアップ処理
    void Player::LevelUp(void)
    {
        if (param_.level >= param_.maxLevel)
        {
            return;
        }

        param_.level++;

        // パラメター上昇
        param_.attack += LEVEL_UP_STAT;

        param_.defensse += LEVEL_UP_STAT;

        param_.maxHp += LEVEL_UP_STAT;

        // HPを全回復
        param_.hp = param_.maxHp;

        hpDisplay_ = param_.hp;

        // 経験値をリセット
        int required = CalcRequiredExp(param_.level - 1);

        experience_ -= required;

        if (experience_ < 0) { experience_ = 0; }

#ifdef _DEBUG
        printfDx("レベルアップ! Lv.%d → 攻撃力:%d 防御力%d HP:%d\n", param_.level, param_.attack, param_.defensse, param_.maxHp);
#endif 

    }

    // 経験値を追加
    void Player::AddExperinece(int exp)
    {
        if (param_.level >= param_.maxLevel) { return; }

        experience_ += exp;

#define _DEBUG
        printfDx("経験値獲得: +%d, (合計: %d / %d)\n", exp, experience_, CalcRequiredExp(param_.level));

        // レベルアップ判定
        while (IsLevelUp())
        {
            LevelUp();
        }
    }

    // レベルアップ可能か
    bool Player::IsLevelUp(void) const
    {
        if (param_.level >= param_.maxLevel)
        {
            return false;
        }

        return experience_ >= CalcRequiredExp(param_.level);
    }
    

    // 次のレベルになるまでに必要な経験値を計算
    int Player::CalcRequiredExp(int level) const
    {
        return static_cast<int>(BASE_EXP * pow(EXP_MULTIPLIER, level - 1));
    }

    // レベルを取得
    int Player::GetLevel(void) const
    {
        return param_.level;
    }

    // 現在の経験値を取得
    int Player::GetExperinece(void) const
    {
        return experience_;
    }

    // 次のレベルまでに必要な経験値を取得
    int Player::GetRequireExp(void) const
    {
        if (param_.level >= param_.maxLevel)
        {
            return 0;
        }

        return CalcRequiredExp(param_.level);
    }

    // レベル情報を表示
    void Player::DrawLevelInfo(void) const
    {
        auto& font = Font::GetInstance();

        int levelX = 20;
        int levelY = Application::SCREEN_SIZE_Y - 150;

        // レベル表示
        char levelStr[64];
        sprintf_s(levelStr, "Level: %d", param_.level);
        font.DrawDefaultText(levelX, levelY, levelStr, GetColor(255, 255, 255), 20);

        // 経験値バー
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

            // 枠
            DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(50, 50, 50), true);
            DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(255, 255, 255), false);

            // 経験値バー
            DrawBox(barX, barY, barX + expBarWidth, barY + barHeight, GetColor(100, 200, 255), true);

            // 経験値テキスト
            char expStr[64];
            sprintf_s(expStr, "%d / %d", experience_, required);
            int textWidth = font.GetDefaultTextWidth(expStr);
            int textX = barX + (barWidth / 2) - (textWidth / 2);
            int textY = barY + 1;
            font.DrawDefaultText(textX, textY, expStr, GetColor(255, 255, 255), 14);
        }
        else
        {
            // 最大レベル到達
            font.DrawDefaultText(levelX, levelY + 30, "MAX LEVEL", GetColor(255, 215, 0), 18);
        }
    }

