#include "../../Pch.h"
#include "EnemySlime.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"

EnemySlime::EnemySlime(void)
{
    targetPos_ = Utility::VECTOR_ZERO;
    viewRange_ = VIEW_RANGE;
    lostRange_ = LOST_RANGE;
    forward_ = Utility::DIR_F;
    viewAngle_ = VIEW_ANGLE;
    isChasing_ = false;
    isInView_ = false;
}

void EnemySlime::Load(int modelId)
{
    trans_.modelId = modelId;
    trans_.SetModel(trans_.modelId);

    EnemyBase::Load(modelId);
}

void EnemySlime::Initialize(const VECTOR& startPos)
{
    EnemyBase::Initialize(startPos);
    trans_.scl = VGet(0.5f, 0.5f, 0.5f);
    UpdateColliderOffset();
}

void EnemySlime::UpdateColliderOffset(void)
{
    // 既存の球体コライダーを探す
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
    if (it != ownColliders_.end())
    {
        delete it->second;
        ownColliders_.erase(it);
    }

    // Y軸に COLLISION_HEIGHT_OFFSET 分浮かせた位置に判定を作成
    ColliderSphere* sphere = new ColliderSphere(
        ColliderBase::TAG::ENEMY,              
        &trans_,                               
        VGet(0.0f, COLLISION_HEIGHT_OFFSET, 0.0f), 
        radius_                                
    );

    // コライダーリストに再登録
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::SPHERE), sphere);
}

void EnemySlime::SetTargetPos(const VECTOR& pos)
{
    targetPos_ = pos;
}

void EnemySlime::Update(void)
{
    // プレイヤーへの方向ベクトル
    VECTOR dirPlayer = VSub(targetPos_, trans_.pos);
    float distance = static_cast<float>(Utility::MagnitudeF(dirPlayer));
    VECTOR dirNorm = Utility::VNormalize(dirPlayer);

    // プレイヤーが視野内にいるか判定
    double angle = Utility::AngleDeg(forward_, dirNorm);
    bool inView = (distance <= viewRange_ && angle <= viewAngle_ * 0.5);

    // 状態更新
    if (inView)
    {
        isChasing_ = true;
    }
    else if (distance > lostRange_)
    {
        isChasing_ = false;
    }

    // 移動処理
    if (isChasing_)
    {
        // 基本の移動速度に状態異常の倍率を適用
        float speedMultiplier = GetSpeedMultiplier();
        movePow_ = VScale(dirNorm, moveSpeed_ * speedMultiplier);
    }
    else
    {
        movePow_ = Utility::VECTOR_ZERO;
    }

    // 正面方向更新
    if (!Utility::EqualsVZero(movePow_))
    {
        forward_ = Utility::VNormalize(movePow_);
    }

    // 共通更新処理
    EnemyBase::Update();
}

void EnemySlime::Draw(void) const
{
    // 共通描画処理
    EnemyBase::Draw();

#ifdef _DEBUG
    //// 球体コライダのオフセット位置を可視化
    //auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
    //if (it != ownColliders_.end())
    //{
    //    const ColliderSphere* sphere = dynamic_cast<const ColliderSphere*>(it->second);
    //    if (sphere)
    //    {
    //        VECTOR colliderPos = sphere->GetLocalPosition();
    //        DrawSphere3D(colliderPos, 10.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
    //        DrawLine3D(trans_.pos, colliderPos, GetColor(0, 255, 255));

    //        VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(colliderPos, VGet(0, 50, 0)));
    //        DrawFormatString(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
    //            GetColor(0, 255, 255), "Offset Y: 30");
    //    }
    //}
#endif
}

void EnemySlime::ApplyData(const EnemyInfo& info)
{
    EnemyBase::ApplyData(info);
}