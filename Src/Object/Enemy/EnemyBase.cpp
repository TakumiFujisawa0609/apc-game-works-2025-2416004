#include "../../Pch.h"
#include "EnemyBase.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderLine.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Decoration/EffectManager.h"
#include "../../Manager/Generic/ResourceManager.h"

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
    , statusEffectHandle_(-1)
{
}

void EnemyBase::Load(int modelId)
{
    trans_.modelId = modelId;
    trans_.SetModel(trans_.modelId);

    // 凍結エフェクトの読み込み
    EffectManager::GetInstance().Add(EffectManager::EFFECT::FREEZE, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::EFFECT_FREEZE).handleId_);

    // 爆発エフェクトの読み込み
    EffectManager::GetInstance().Add(EffectManager::EFFECT::BLAST, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::EFFECT_BLAST).handleId_);

    // 硝酸アンモニウムエフェクトの読み込み
    EffectManager::GetInstance().Add(EffectManager::EFFECT::AMMONIUM, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::EFFECT_AMMONIUM).handleId_);

    // やけどエフェクトの読み込み
    EffectManager::GetInstance().Add(EffectManager::EFFECT::FIRE_BURN, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::EFFECT_FIRE_BURN).handleId_);

    // 湿潤エフェクトの読み込み
    EffectManager::GetInstance().Add(EffectManager::EFFECT::WET, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::EFFECT_WET).handleId_);
}

void EnemyBase::Initialize(const VECTOR& startPos)
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

    // 当たり判定の初期化とシステムへの登録
    InitCollider();
    CollisionController::GetInstance().RegisterUnit(this);

    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_BLAST,
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_BLAST).handleId_);
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_FREEZE, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_FREEZE).handleId_);
}

void EnemyBase::InitCollider(void)
{
    // 球体コライダの作成（敵同士・プレイヤーとの衝突用）
    ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::ENEMY, &trans_,
        Utility::VECTOR_ZERO, radius_);
    ownColliders_.emplace(static_cast<int>(UnitBase::COLLIDER_TYPE::SPHERE), colSphere);

    // 線分コライダの作成（地面判定用）
    ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::ENEMY, &trans_,
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(UnitBase::COLLIDER_TYPE::LINE), colLine);
}

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

    // 状態異常のタイマー・ダメージ更新
    UpdateStatusEffect(deltaTime);

    // 座標更新前の位置をバックアップ
    preCollisionPos_ = trans_.pos;

    // 基底クラスの物理・重力処理
    UnitBase::Update();

    // ステージ外落下・侵入防止
    if (!IsOnStage(trans_.pos))
    {
        trans_.pos = preCollisionPos_;
        movePow_ = Utility::VECTOR_ZERO;
    }

    const float effectSize = 50.0f;

    // エフェクトの追従
    if (statusEffectHandle_ != -1) {
        EffectManager::GetInstance().SyncEffect(statusEffectHandle_, trans_.pos, 
            trans_.quaRot, effectSize);
    }
}

void EnemyBase::UpdateStatusEffect(float deltaTime)
{
    if (currentStatus_ == STATUS_EFFECT::NONE) { return; }

    EffectManager& em = EffectManager::GetInstance();

    // ループエフェクトの再生維持チェック
    if (statusEffectHandle_ == -1 || !em.IsPlayEffect(statusEffectHandle_))
    {
        RestartStatusEffect();
    }

    statusTimer_ -= deltaTime;

    // やけどの継続ダメージ処理
    if (currentStatus_ == STATUS_EFFECT::BURN)
    {
        burnTickTimer_ += deltaTime;
        if (burnTickTimer_ >= 1.0f)
        {
            TakeDamage(BURN_DAMAGE_PER_SEC);
            burnTickTimer_ = 0.0f;
        }
    }

    // 時間切れで解除
    if (statusTimer_ <= 0.0f)
    {
        ClearStatusEffect();
    }
}

void EnemyBase::ApplyStatusEffect(STATUS_EFFECT status)
{
    // 同じ状態異常なら時間をリセットして終了
    if (currentStatus_ == status && status != STATUS_EFFECT::EXPLODED) 
    {
        if (status == STATUS_EFFECT::AMMONIUM_NITRATE) 
        { 
            statusTimer_ = AMMONIUM_NITRATE_DURATION; 
        }
        else if (status == STATUS_EFFECT::FROZEN) { statusTimer_ = FROZEN_DURATION; }
        else if (status == STATUS_EFFECT::BURN) { statusTimer_ = BURN_DURATION; }
        else if (status == STATUS_EFFECT::WET) { statusTimer_ = WET_DURATION; }
        return;
    }

    // 別の状態になるなら古いエフェクトを破棄
    if (statusEffectHandle_ != -1) {
        EffectManager::GetInstance().StopHandle(statusEffectHandle_);
        statusEffectHandle_ = -1;
    }

    currentStatus_ = status;
    burnTickTimer_ = 0.0f;

    const float effectSize = 50.0f;

    // 新しい状態のタイマー設定とエフェクト開始
    EffectManager& em = EffectManager::GetInstance();
    switch (status)
    {
    case STATUS_EFFECT::AMMONIUM_NITRATE:
        statusTimer_ = AMMONIUM_NITRATE_DURATION;
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::AMMONIUM, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;

    case STATUS_EFFECT::FROZEN:
        statusTimer_ = FROZEN_DURATION;
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::FREEZE, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        SoundManager::GetInstance().Play(SoundManager::SOUND::SE_FREEZE);
        break;

    case STATUS_EFFECT::EXPLODED:
        statusTimer_ = 2.0f; // 爆発の余韻時間
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::BLAST, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        SoundManager::GetInstance().Play(SoundManager::SOUND::SE_BLAST);
        break;

    case STATUS_EFFECT::BURN:
        statusTimer_ = BURN_DURATION;
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::FIRE_BURN, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;

    case STATUS_EFFECT::WET:
        statusTimer_ = WET_DURATION;
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::WET, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;
    }
}

void EnemyBase::RestartStatusEffect(void)
{
    EffectManager& em = EffectManager::GetInstance();

    const float effectSize = 50.0f;

    // 古いハンドルがあれば念のため止める
    if (statusEffectHandle_ != -1) { em.StopHandle(statusEffectHandle_); }

    switch (currentStatus_)
    {
    case STATUS_EFFECT::AMMONIUM_NITRATE:
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::AMMONIUM, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;

    case STATUS_EFFECT::FROZEN:
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::FREEZE, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;

    case STATUS_EFFECT::EXPLODED:
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::BLAST, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;

    case STATUS_EFFECT::BURN:
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::FIRE_BURN, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;

    case STATUS_EFFECT::WET:
        statusEffectHandle_ = em.PlayAndGetHandle(EffectManager::EFFECT::WET, trans_.pos, trans_.quaRot, effectSize, SoundManager::SOUND::NONE);
        break;
    }
}

void EnemyBase::ClearStatusEffect(void)
{
    currentStatus_ = STATUS_EFFECT::NONE;
    statusTimer_ = 0.0f;
    burnTickTimer_ = 0.0f;

    if (statusEffectHandle_ != -1)
    {
        EffectManager::GetInstance().StopHandle(statusEffectHandle_);
        statusEffectHandle_ = -1; 
    }

    currentStatus_ = STATUS_EFFECT::NONE;
    statusTimer_ = 0.0f;
}

float EnemyBase::GetSpeedMultiplier(void) const
{
    const float defaultSpeed = 1.0f;

    switch (currentStatus_)
    {
    case STATUS_EFFECT::FROZEN:
        return FROZEN_SPEED_MULTIPLIER;  

    case STATUS_EFFECT::WET:
        return WET_SPEED_MULTIPLIER;  

    default:
        return defaultSpeed; 
    }
}

bool EnemyBase::IsOnStage(const VECTOR& pos) const
{
    const float tileCount = 100.0f;
    const float tileSize = 100.0f;
    const float halfStageSize = (tileCount * tileSize) * 0.5f;
    const float margin = 100.0f;
    const float maxX = halfStageSize - margin;
    const float minX = -halfStageSize + margin;
    const float maxZ = halfStageSize - margin;
    const float minZ = -halfStageSize + margin;
    const float falldetection = 500.0f;

    if (pos.x < minX || pos.x > maxX ||
        pos.z < minZ || pos.z > maxZ)
    {
        return false;
    }

    // 落下判定（高さ）
    if (pos.y < -falldetection)
    {
        return false;
    }

    return true;
}

void EnemyBase::Draw(void) const
{
    UnitBase::Draw();
    DrawStatusEffect();

    // HPバーの表示
    VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 80, 0)));
    int barWidth = 60;
    int barHeight = 8;
    int barX = static_cast<int>(screenPos.x) - barWidth / 2;
    int barY = static_cast<int>(screenPos.y);

    float hpRate = hp_ / maxHp_;
    int hpBarWidth = static_cast<int>(barWidth * hpRate);

    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), false);
    DrawBox(barX, barY, barX + hpBarWidth, barY + barHeight, GetColor(0, 255, 0), true);

#ifdef _DEBUG
    //DrawCapsule3D(trans_.pos, trans_.pos, radius_, 12, 0xff0000, 0xff0000, false);

    //auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::LINE));
    //if (it != ownColliders_.end())
    //{
    //    const ColliderLine* line = dynamic_cast<const ColliderLine*>(it->second);
    //    if (line)
    //    {
    //        VECTOR start = line->GetLocalStartPos();
    //        VECTOR end = line->GetLocalEndPos();
    //        DrawLine3D(start, end, GetColor(255, 255, 0));
    //        DrawSphere3D(start, 5.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
    //        DrawSphere3D(end, 5.0f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
    //    }
    //}

    //// 状態異常のテキスト表示
    //VECTOR statusScreenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 100, 0)));
    //const char* statusText = "";
    //unsigned int statusColor = GetColor(255, 255, 255);

    //switch (currentStatus_)
    //{
    //case STATUS_EFFECT::AMMONIUM_NITRATE:
    //    statusText = "硝酸アンモニウム";
    //    statusColor = GetColor(255, 255, 0);
    //    break;
    //case STATUS_EFFECT::FROZEN:
    //    statusText = "凍結";
    //    statusColor = GetColor(100, 200, 255);
    //    break;
    //case STATUS_EFFECT::BURN:
    //    statusText = "やけど";
    //    statusColor = GetColor(255, 100, 0);
    //    break;
    //case STATUS_EFFECT::WET:
    //    statusText = "湿潤";
    //    statusColor = GetColor(0, 150, 255);
    //    break;
    //default:
    //    break;
    //}

    //if (currentStatus_ != STATUS_EFFECT::NONE)
    //{
    //    DrawFormatString(
    //        static_cast<int>(statusScreenPos.x) - 30,
    //        static_cast<int>(statusScreenPos.y),
    //        statusColor,
    //        "%s (%.1fs)", statusText, statusTimer_
    //    );
    //}
#endif
}

void EnemyBase::DrawStatusEffect(void) const
{
    if (currentStatus_ == STATUS_EFFECT::NONE) { return; }

    float time = statusTimer_;
    VECTOR effectPos = VAdd(trans_.pos, VGet(0, 40, 0));
}

void EnemyBase::Release(void)
{
    ClearStatusEffect();

    for (auto& pair : ownColliders_)
    {
        if (pair.second != nullptr)
        {
            pair.second->SetActive(false);
        }
    }

    CollisionController::GetInstance().UnregisterUnit(this);
    UnitBase::Release();
}

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

void EnemyBase::OnSwordHit(void)
{
    TakeDamage(SWORD_DAMAGE);

    if (currentStatus_ == STATUS_EFFECT::FROZEN)
    {
        return;
    }

    if (currentStatus_ == STATUS_EFFECT::NONE)
    {
        ApplyStatusEffect(STATUS_EFFECT::AMMONIUM_NITRATE);
    }
    else if (currentStatus_ == STATUS_EFFECT::WET)
    {
        ApplyStatusEffect(STATUS_EFFECT::FROZEN);
    }
    else if (currentStatus_ == STATUS_EFFECT::BURN)
    {
        ApplyStatusEffect(STATUS_EFFECT::EXPLODED);
    }

   

    lastHitTime_ = HIT_COOLDOWN;
}

void EnemyBase::OnFireHit(void)
{
    if (currentStatus_ == STATUS_EFFECT::AMMONIUM_NITRATE)
    {
        TakeDamage(EXPLOSION_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::EXPLODED);
    }
    else if (currentStatus_ == STATUS_EFFECT::WET || currentStatus_ == STATUS_EFFECT::FROZEN)
    {
        TakeDamage(FIRE_DAMAGE);
        ClearStatusEffect();
    }
    else
    {
        TakeDamage(FIRE_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::BURN);
    }

    lastHitTime_ = HIT_COOLDOWN;
}

void EnemyBase::OnWaterHit(void)
{
    if (currentStatus_ == STATUS_EFFECT::FROZEN) { return; }

    if (currentStatus_ == STATUS_EFFECT::AMMONIUM_NITRATE)
    {
        TakeDamage(WATER_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::FROZEN);
    }
    else if (currentStatus_ == STATUS_EFFECT::BURN)
    {
        TakeDamage(WATER_DAMAGE);
        ClearStatusEffect();

    }
    else
    {
        TakeDamage(WATER_DAMAGE);
        ApplyStatusEffect(STATUS_EFFECT::WET);
    }

    lastHitTime_ = HIT_COOLDOWN;
}

void EnemyBase::OnCollisionEnter(const CollisionInfo& info)
{

    if (lastHitTime_ > 0.0f)
    {
        return;
    }

    // 剣との衝突
    if (info.hitCollider->GetCollisionTag() == ColliderBase::TAG::SWORD &&
        info.hitCollider->GetShapeType() == ColliderBase::SHAPE::CAPSULE)
    {
        OnSwordHit();
    }

    // 火攻撃との衝突
    if (info.hitCollider->GetCollisionTag() == ColliderBase::TAG::FIRE_ATTACK)
    {
        OnFireHit();
    }

    // 水攻撃との衝突
    if (info.hitCollider->GetCollisionTag() == ColliderBase::TAG::WATER_ATTACK)
    {
        OnWaterHit();
    }
}

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