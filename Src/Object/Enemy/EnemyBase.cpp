#include "EnemyBase.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderLine.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Manager/Generic/SceneManager.h"

// コンストラクタ
EnemyBase::EnemyBase(void)
    : hp_(0.0f)
    , lastHitTime_(0.0f)
    , maxHp_(0.0f)
    , attack_(0.0f)
    , defense_(0.0f)
    , level_(1)
    , moveSpeed_(0.0f)
    , viewRange_(0.0f)
    , lostRange_(0.0f)
    , forward_(Utility::DIR_F)
    , viewAngle_(0.0f)
    , type_("")
    , isChasing_(false)
    , isInView_(false)
    , currentStatus_(STATUS_EFFECT::NONE)
    , statusTimer_(0.0f)
    , burnTickTimer_(0.0f)
{
}

// 読み込み
void EnemyBase::Load(int modelId)
{
    trans_.modelId = modelId;
    trans_.SetModel(trans_.modelId);
}

// 初期化
void EnemyBase::Init(const VECTOR& startPos)
{
    trans_.pos = startPos;
    prePos_ = startPos;
    trans_.scl = Utility::VECTOR_ONE;
    movePow_ = Utility::VECTOR_ZERO;
    jumpPow_ = Utility::VECTOR_ZERO;
    currentAnim_ = ANIM::NONE;

    // 状態異常の初期化
    currentStatus_ = STATUS_EFFECT::NONE;
    statusTimer_ = 0.0f;
    burnTickTimer_ = 0.0f;

    // コライダ初期化
    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

// コライダ初期化
void EnemyBase::InitCollider(void)
{
    // 球体コライダの作成（敵同士・プレイヤーとの衝突用）
    ColliderSphere* colSphere = new ColliderSphere(
        ColliderBase::TAG::ENEMY,
        &trans_,
        Utility::VECTOR_ZERO,
        radius_
    );
    ownColliders_.emplace(
        static_cast<int>(UnitBase::COLLIDER_TYPE::SPHERE),
        colSphere
    );

    // 線分コライダの作成（地面判定用）
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::ENEMY,
        &trans_,
        COL_LINE_START_LOCAL_POS,
        COL_LINE_END_LOCAL_POS
    );
    ownColliders_.emplace(
        static_cast<int>(UnitBase::COLLIDER_TYPE::LINE),
        colLine
    );
}

// 更新処理
void EnemyBase::Update(void)
{
    float deltaTime = SceneManager::GetInstance().GetDeltaTime();

    // ヒット判定のクールタイム更新
    if (lastHitTime_ > 0.0f)
    {
        lastHitTime_ -= deltaTime;
        if (lastHitTime_ < 0.0f)
        {
            lastHitTime_ = 0.0f;
        }
    }

    // 状態異常の更新
    UpdateStatusEffect(deltaTime);

    // 押し出し前の位置を保存
    preCollisionPos_ = trans_.pos;

    // UnitBaseの更新（重力・衝突判定を含む）
    UnitBase::Update();

    // ステージ外に出た場合は元の位置に戻す
    if (!IsOnStage(trans_.pos))
    {
        trans_.pos = preCollisionPos_;
        movePow_ = Utility::VECTOR_ZERO;
    }
}

// 状態異常の更新
void EnemyBase::UpdateStatusEffect(float deltaTime)
{
    if (currentStatus_ == STATUS_EFFECT::NONE) return;

    // 状態異常のタイマー更新
    statusTimer_ -= deltaTime;

    // やけど状態の継続ダメージ
    if (currentStatus_ == STATUS_EFFECT::BURN)
    {
        burnTickTimer_ += deltaTime;

        // 1秒ごとにダメージ
        if (burnTickTimer_ >= 1.0f)
        {
            TakeDamage(BURN_DAMAGE_PER_SEC);
            burnTickTimer_ = 0.0f;

        }
    }

    // 状態異常が切れたらクリア
    if (statusTimer_ <= 0.0f)
    {
        ClearStatusEffect();
    }
}

// 状態異常を適用
void EnemyBase::ApplyStatusEffect(STATUS_EFFECT status)
{
    currentStatus_ = status;
    burnTickTimer_ = 0.0f;

    switch (status)
    {
    case STATUS_EFFECT::AMMONIUM_NITRATE:
        statusTimer_ = AMMONIUM_NITRATE_DURATION;
        break;

    case STATUS_EFFECT::FROZEN:
        statusTimer_ = FROZEN_DURATION;
        break;

    case STATUS_EFFECT::BURN:
        statusTimer_ = BURN_DURATION;
        break;

    case STATUS_EFFECT::WET:
        statusTimer_ = WET_DURATION;
        break;

    case STATUS_EFFECT::EXPLODED:
        // 爆発は即座に終了
        statusTimer_ = 0.0f;
        break;

    default:
        break;
    }
}

// 状態異常をクリア
void EnemyBase::ClearStatusEffect(void)
{
    currentStatus_ = STATUS_EFFECT::NONE;
    statusTimer_ = 0.0f;
    burnTickTimer_ = 0.0f;
}

// 現在の移動速度倍率を取得
float EnemyBase::GetSpeedMultiplier(void) const
{
    switch (currentStatus_)
    {
    case STATUS_EFFECT::FROZEN:
        return FROZEN_SPEED_MULTIPLIER;  // 0.0f（完全停止）

    case STATUS_EFFECT::WET:
        return WET_SPEED_MULTIPLIER;     // 0.5f（50%減）

    default:
        return 1.0f;  // 通常速度
    }
}

// ステージ上にいるかチェック
bool EnemyBase::IsOnStage(const VECTOR& pos) const
{
    const float TILE_COUNT = 100.0f;
    const float TILE_SIZE = 100.0f;
    const float HALF_STAGE_SIZE = (TILE_COUNT * TILE_SIZE) * 0.5f;
    const float MARGIN = 100.0f;
    const float MAX_X = HALF_STAGE_SIZE - MARGIN;
    const float MIN_X = -HALF_STAGE_SIZE + MARGIN;
    const float MAX_Z = HALF_STAGE_SIZE - MARGIN;
    const float MIN_Z = -HALF_STAGE_SIZE + MARGIN;

    if (pos.x < MIN_X || pos.x > MAX_X ||
        pos.z < MIN_Z || pos.z > MAX_Z)
    {
        return false;
    }

    if (pos.y < -500.0f)
    {
        return false;
    }

    return true;
}

// 描画処理
void EnemyBase::Draw(void) const
{
    // モデル描画（UnitBaseの描画）
    UnitBase::Draw();

    //// 状態異常のエフェクト描画
    //DrawStatusEffect();

    //// HPバーの表示
    //VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 80, 0)));
    //int barWidth = 60;
    //int barHeight = 8;
    //int barX = static_cast<int>(screenPos.x) - barWidth / 2;
    //int barY = static_cast<int>(screenPos.y);

    //float hpRate = hp_ / maxHp_;
    //int hpBarWidth = static_cast<int>(barWidth * hpRate);

    //DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), false);
    //DrawBox(barX, barY, barX + hpBarWidth, barY + barHeight, GetColor(0, 255, 0), true);

#ifdef _DEBUG
    DrawCapsule3D(trans_.pos, trans_.pos, radius_, 12, 0xff0000, 0xff0000, false);

    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::LINE));
    if (it != ownColliders_.end())
    {
        const ColliderLine* line = dynamic_cast<const ColliderLine*>(it->second);
        if (line)
        {
            VECTOR start = line->GetPosStart();
            VECTOR end = line->GetPosEnd();
            DrawLine3D(start, end, GetColor(255, 255, 0));
            DrawSphere3D(start, 5.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
            DrawSphere3D(end, 5.0f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
        }
    }

    // 状態異常のテキスト表示
    VECTOR statusScreenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 100, 0)));
    const char* statusText = "";
    unsigned int statusColor = GetColor(255, 255, 255);

    switch (currentStatus_)
    {
    case STATUS_EFFECT::AMMONIUM_NITRATE:
        statusText = "硝酸アンモニウム";
        statusColor = GetColor(255, 255, 0);
        break;
    case STATUS_EFFECT::FROZEN:
        statusText = "凍結";
        statusColor = GetColor(100, 200, 255);
        break;
    case STATUS_EFFECT::BURN:
        statusText = "やけど";
        statusColor = GetColor(255, 100, 0);
        break;
    case STATUS_EFFECT::WET:
        statusText = "湿潤";
        statusColor = GetColor(0, 150, 255);
        break;
    default:
        break;
    }

    if (currentStatus_ != STATUS_EFFECT::NONE)
    {
        DrawFormatString(
            static_cast<int>(statusScreenPos.x) - 30,
            static_cast<int>(statusScreenPos.y),
            statusColor,
            "%s (%.1fs)", statusText, statusTimer_
        );
    }
#endif
}

// 状態異常のエフェクト描画
void EnemyBase::DrawStatusEffect(void) const
{
    if (currentStatus_ == STATUS_EFFECT::NONE) return;

    float time = statusTimer_;
    VECTOR effectPos = VAdd(trans_.pos, VGet(0, 40, 0));

    switch (currentStatus_)
    {
    case STATUS_EFFECT::AMMONIUM_NITRATE:
        // 黄色の粒子エフェクト
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
        DrawSphere3D(effectPos, radius_ * 0.8f, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), FALSE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        break;

    case STATUS_EFFECT::FROZEN:
        // 青白い氷のエフェクト
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
        DrawSphere3D(trans_.pos, radius_ * 1.1f, 12, GetColor(150, 200, 255), GetColor(150, 200, 255), FALSE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        break;

    case STATUS_EFFECT::BURN:
        // 炎のエフェクト
    {
        float flameOffset = sinf(time * 10.0f) * 5.0f;
        SetDrawBlendMode(DX_BLENDMODE_ADD, 120);
        DrawSphere3D(VAdd(effectPos, VGet(0, flameOffset, 0)), 15.0f, 8, GetColor(255, 100, 0), GetColor(255, 100, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    break;

    case STATUS_EFFECT::WET:
        // 水滴のエフェクト
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
        DrawSphere3D(effectPos, radius_ * 0.6f, 8, GetColor(50, 150, 255), GetColor(50, 150, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        break;

    default:
        break;
    }
}

// 解放処理
void EnemyBase::Release(void)
{
    for (auto& pair : ownColliders_)
    {
        if (pair.second != nullptr)
        {
            pair.second->SetValid(false);
        }
    }

    CollisionController::GetInstance().UnregisterUnit(this);
    UnitBase::Release();
}

// CSVデータを適用
void EnemyBase::ApplyData(const EnemyInfo& info)
{
    type_ = info.type;
    maxHp_ = info.param.maxHp;
    hp_ = info.param.hp;
    attack_ = info.param.attack;
    defense_ = info.param.defense;
    moveSpeed_ = info.param.speed;
    radius_ = info.param.radius;
    level_ = info.param.level;
}

// 剣攻撃を受けた時の処理
void EnemyBase::OnSwordHit(void)
{
    TakeDamage(SWORD_DAMAGE);

    // 硝酸アンモニウム状態を付与
    ApplyStatusEffect(STATUS_EFFECT::AMMONIUM_NITRATE);

    lastHitTime_ = HIT_COOLDOWN;
}

// 火攻撃を受けた時の処理
void EnemyBase::OnFireHit(void)
{
    if (currentStatus_ == STATUS_EFFECT::AMMONIUM_NITRATE)
    {
        // 硝酸アンモニウム状態で火攻撃 → 爆発！
        TakeDamage(EXPLOSION_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::EXPLODED);

        // 爆発エフェクト（簡易版）
        VECTOR explosionPos = trans_.pos;
        for (int i = 0; i < 8; i++)
        {
            float angle = (i * DX_TWO_PI_F) / 8.0f;
            VECTOR particlePos = VAdd(explosionPos, VGet(cosf(angle) * 50.0f, 30.0f, sinf(angle) * 50.0f));
            // 実際のゲームではパーティクルシステムを使用
        }
    }
    else if (currentStatus_ == STATUS_EFFECT::WET || currentStatus_ == STATUS_EFFECT::FROZEN)
    {
        // 湿潤状態または凍結状態に火攻撃 → 状態異常を打ち消す
        TakeDamage(FIRE_DAMAGE);
        ClearStatusEffect();
    }
    else
    {
        // 通常の火攻撃 → やけど状態
        TakeDamage(FIRE_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::BURN);
    }

    lastHitTime_ = HIT_COOLDOWN;
}

// 水攻撃を受けた時の処理
void EnemyBase::OnWaterHit(void)
{
    if (currentStatus_ == STATUS_EFFECT::AMMONIUM_NITRATE)
    {
        // 硝酸アンモニウム状態で水攻撃 → 凍結
        TakeDamage(WATER_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::FROZEN);
    }
    else if (currentStatus_ == STATUS_EFFECT::BURN)
    {
        // やけど状態に水攻撃 → 状態異常を打ち消す
        TakeDamage(WATER_DAMAGE);
        ClearStatusEffect();

    }
    else
    {
        // 通常の水攻撃 → 湿潤状態（鈍足）
        TakeDamage(WATER_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::WET);
    }

    lastHitTime_ = HIT_COOLDOWN;
}

// 衝突時のコールバック
void EnemyBase::OnCollisionEnter(const CollisionInfo& info)
{

    if (lastHitTime_ > 0.0f)
    {
        return;
    }

    // 剣との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::SWORD &&
        info.hitCollider->GetShape() == ColliderBase::SHAPE::CAPSULE)
    {
        OnSwordHit();
    }

    // 火攻撃との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::FIRE_ATTACK)
    {
        OnFireHit();
    }

    // 水攻撃との衝突
    if (info.hitCollider->GetTag() == ColliderBase::TAG::WATER_ATTACK)
    {
        OnWaterHit();
    }
}

// その他のメソッド（変更なし）
void EnemyBase::SetForward(const VECTOR& forward)
{
    forward_ = forward;
}

void EnemyBase::SetViewAngle(float angle)
{
    viewAngle_ = angle;
}

void EnemyBase::SetHp(float hp)
{
    hp_ = std::clamp(hp, 0.0f, maxHp_);
}

float EnemyBase::GetHp(void) const
{
    return hp_;
}

void EnemyBase::TakeDamage(float damage)
{
    hp_ -= damage;
    if (hp_ < 0.0f)
    {
        hp_ = 0.0f;
    }
}

const std::string& EnemyBase::GetType(void) const
{
    return type_;
}

void EnemyBase::SetLevel(int level)
{
    level_ = level;
    ApplyLevelParams();
}

int EnemyBase::GetLevel(void) const
{
    return level_;
}

void EnemyBase::ApplyLevelParams(void)
{
    if (level_ <= 1) { return; }

    int levelDiff = level_ - 1;
    maxHp_ += LEVEL_UP_STAT * levelDiff;
    hp_ = maxHp_;
    attack_ += LEVEL_UP_STAT * levelDiff;
    defense_ += LEVEL_UP_STAT * levelDiff;
}

int EnemyBase::GetExpReward(void) const
{
    return BASE_EXP_REWARD * level_;
}

float EnemyBase::GetAttack(void) const
{
    return attack_;
}

float EnemyBase::GetDefense(void) const
{
    return defense_;
}