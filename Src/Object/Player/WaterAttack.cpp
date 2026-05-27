#include "../../Pch.h"
#include "WaterAttack.h"
#include "../Player/Player.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Decoration/EffectManager.h"

WaterAttack::WaterAttack(void)
    : player_(nullptr)
    , offsetPos_(Utility::VECTOR_ZERO)
    , duration_(WATER_DURATION)
    , elapsedTime_(0.0f)
    , isActive_(false)
    , effectHandle_(-1)
{
}

WaterAttack::~WaterAttack(void)
{
}

void WaterAttack::Load(void)
{
}

void WaterAttack::Initialize(void)
{
    // トランスフォーム初期化
    trans_.pos = Utility::VECTOR_ZERO;
    trans_.scl = VGet(1.0f, 1.0f, 1.0f);
    trans_.rot = Utility::VECTOR_ZERO;

    // 状態初期値
    radius_ = WATER_RANGE;
    isActive_ = false;
}

void WaterAttack::Initialize(Player* player, const VECTOR& offsetPos)
{
    // パラメータ設定
    player_ = player;
    offsetPos_ = offsetPos;
    elapsedTime_ = 0.0f;
    isActive_ = true;
    radius_ = WATER_RANGE;

    // 出現座標計算
    if (player_)
    {
        // プレイヤーの向きに合わせて前方に配置
        VECTOR forward = player_->GetTransform().quaRotLocal.GetForward();
        VECTOR playerPos = player_->GetPos();

        trans_.pos = VAdd(playerPos, VScale(forward, WATER_OFFSET_Z));
        trans_.pos = VAdd(trans_.pos, VGet(0, WATER_OFFSET_Y, 0));
    }

    // コライダ生成
    InitCollider();

    // システム登録
    CollisionController::GetInstance().RegisterUnit(this);

    const float effectSize = 40.0f;

    // エフェクト再生
    EffectManager::GetInstance().Play(EffectManager::EFFECT::WATER,
        trans_.pos, trans_.quaRot,effectSize, SoundManager::SOUND::NONE);
}

void WaterAttack::InitCollider(void)
{
    // 当たり判定生成
    ColliderSphere* colSphere = new ColliderSphere(
        ColliderBase::TAG::WATER_ATTACK, &trans_,
        Utility::VECTOR_ZERO, radius_);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colSphere);
}

void WaterAttack::Update(void)
{
    if (!isActive_) { return; }

    // 寿命管理
    elapsedTime_ += SceneManager::GetInstance().GetDeltaTime();

    if (elapsedTime_ >= duration_)
    {
        isActive_ = false;
        CollisionController::GetInstance().UnregisterUnit(this);
        EffectManager::GetInstance().Stop(EffectManager::EFFECT::FIRE);
        return;
    }

    // プレイヤー追従
    if (player_)
    {
        VECTOR forward = player_->GetTransform().quaRotLocal.GetForward();
        VECTOR playerPos = player_->GetPos();

        trans_.pos = VAdd(playerPos, VScale(forward, WATER_OFFSET_Z));
        trans_.pos = VAdd(trans_.pos, VGet(0, WATER_OFFSET_Y, 0));
    }

    // ベースクラス更新
    UnitBase::Update();
}

void WaterAttack::Draw(void) const
{
    if (!isActive_) { return; }
}

void WaterAttack::Release(void)
{
    // 終了時の登録解除
    if (isActive_)
    {
        CollisionController::GetInstance().UnregisterUnit(this);
        EffectManager::GetInstance().Stop(EffectManager::EFFECT::WATER);
    }

    // ベースクラス解放
    UnitBase::Release();

    // エフェクトハンドル解放
    if (effectHandle_ != -1)
    {
        StopEffekseer3DEffect(effectHandle_);
        effectHandle_ = -1;
    }
}

bool WaterAttack::IsActive(void) const
{
    return isActive_;
}

void WaterAttack::SetActive(bool active)
{
    isActive_ = active;
}