#include "../../Pch.h"
#include "FireAttack.h"
#include "../Player/Player.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Decoration/EffectManager.h"

FireAttack::FireAttack(void)
    : player_(nullptr)
    , offsetPos_(Utility::VECTOR_ZERO)
    , duration_(FIRE_DURATION)
    , elapsedTime_(0.0f)
    , isActive_(false)
    , effectHandle_(-1)
{
}

FireAttack::~FireAttack(void)
{
}

void FireAttack::Load(void)
{
}

void FireAttack::Initialize(void)
{
    // トランスフォームの初期リセット
    trans_.pos = Utility::VECTOR_ZERO;
    trans_.scl = VGet(1.0f, 1.0f, 1.0f);
    trans_.rot = Utility::VECTOR_ZERO;

    // 当たり判定の半径を設定
    radius_ = FIRE_RANGE; 

    // 初期状態は非アクティブ
    isActive_ = false;    
}

void FireAttack::Initialize(Player* player, const VECTOR& offsetPos)
{
    player_ = player;
    offsetPos_ = offsetPos;
    elapsedTime_ = 0.0f;
    isActive_ = true;
    radius_ = FIRE_RANGE;

    // プレイヤーの向きに合わせて出現位置を計算
    if (player_)
    {
        VECTOR forward = player_->GetTransform().quaRotLocal.GetForward();
        VECTOR playerPos = player_->GetPos();

        // プレイヤーの位置から前方へオフセット分ずらす
        trans_.pos = VAdd(playerPos, VScale(forward, FIRE_OFFSET_Z));
        trans_.pos = VAdd(trans_.pos, VGet(0, FIRE_OFFSET_Y, 0)); // 高さ調整
    }

    // コライダの生成
    InitCollider();

    // 衝突判定管理クラスへ自分を登録
    CollisionController::GetInstance().RegisterUnit(this);

    const float effcetSize = 35.0f;

    // エフェクトの再生
    EffectManager::GetInstance().Play(EffectManager::EFFECT::FIRE, trans_.pos,
        trans_.quaRot, effcetSize, SoundManager::SOUND::NONE);
}

void FireAttack::InitCollider(void)
{
    // 球体コライダを作成
    ColliderSphere* colSphere = new ColliderSphere(ColliderBase::TAG::FIRE_ATTACK,
        &trans_, Utility::VECTOR_ZERO, radius_);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), colSphere);
}

void FireAttack::Update(void)
{
    if (!isActive_) { return; }

    // デルタタイムを加算して持続時間を計測
    elapsedTime_ += SceneManager::GetInstance().GetDeltaTime();

    // 持続時間を超えたら終了処理
    if (elapsedTime_ >= duration_)
    {
        isActive_ = false;
        CollisionController::GetInstance().UnregisterUnit(this);
        EffectManager::GetInstance().Stop(EffectManager::EFFECT::FIRE);
        return;
    }

    // 発動中、常にプレイヤーの前方に追従させる
    if (player_)
    {
        VECTOR forward = player_->GetTransform().quaRotLocal.GetForward();
        VECTOR playerPos = player_->GetPos();

        trans_.pos = VAdd(playerPos, VScale(forward, FIRE_OFFSET_Z));
        trans_.pos = VAdd(trans_.pos, VGet(0, FIRE_OFFSET_Y, 0));
    }

    UnitBase::Update();

}

void FireAttack::Draw(void) const
{
    if (!isActive_) { return; }

#ifdef _DEBUG
    //// デバッグ用：攻撃範囲の円を描画（マジックナンバー: 32）
    //const int SEGMENTS = 32;
    //for (int i = 0; i < SEGMENTS; i++)
    //{
    //    float angle1 = (i * DX_TWO_PI_F) / SEGMENTS;
    //    float angle2 = ((i + 1) * DX_TWO_PI_F) / SEGMENTS;

    //    VECTOR p1 = VGet(
    //        trans_.pos.x + cosf(angle1) * radius_,
    //        trans_.pos.y,
    //        trans_.pos.z + sinf(angle1) * radius_
    //    );
    //    VECTOR p2 = VGet(
    //        trans_.pos.x + cosf(angle2) * radius_,
    //        trans_.pos.y,
    //        trans_.pos.z + sinf(angle2) * radius_
    //    );

    //    DrawLine3D(p1, p2, GetColor(255, 100, 0));
    //}

    //// 残り時間を頭上に表示（マジックナンバー: 50, 30, 1.0f）
    //VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 50, 0)));
    //if (screenPos.z > 0.0f && screenPos.z < 1.0f)
    //{
    //    DrawFormatString(
    //        static_cast<int>(screenPos.x) - 30,
    //        static_cast<int>(screenPos.y),
    //        GetColor(255, 150, 0),
    //        "火 残り:%.1f秒", duration_ - elapsedTime_
    //    );
    //}
#endif
}

void FireAttack::Release(void)
{
    // アクティブなまま解放される場合の安全策
    if (isActive_)
    {
        CollisionController::GetInstance().UnregisterUnit(this);
        EffectManager::GetInstance().Stop(EffectManager::EFFECT::FIRE);
    }

    UnitBase::Release();

    // 個別管理のエフェクトハンドルがあれば停止
    if (effectHandle_ != -1)
    {
        StopEffekseer3DEffect(effectHandle_);
        effectHandle_ = -1;
    }
}

bool FireAttack::IsActive(void) const
{
    return isActive_;
}

void FireAttack::SetActive(bool active)
{
    isActive_ = active;
}