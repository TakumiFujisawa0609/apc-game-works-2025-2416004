#include "../Pch.h"
#include "ColliderSphere.h"
#include "../Object/Common/Transform.h"

ColliderSphere::ColliderSphere(TAG collisionTag, const Transform* followTarget, const VECTOR& localPosition, float radius)
    : ColliderBase(SHAPE::SPHERE, collisionTag, followTarget)
    , localPosition_(localPosition)
    , radius_(radius)
{
}

void ColliderSphere::SetLocalPosition(const VECTOR& position)
{
    localPosition_ = position;
}

void ColliderSphere::SetRadius(float radius)
{
    // 半径が負にならないように制限
    radius_ = std::max(radius, 0.0f);
}

const VECTOR& ColliderSphere::GetLocalPosition(void) const
{
    return localPosition_;
}

VECTOR ColliderSphere::GetWorldPosition(void) const
{
    // ローカル座標をワールド座標へ変換
    return TransformLocalToWorld(localPosition_);
}

float ColliderSphere::GetRadius(void) const
{
    return radius_;
}

void ColliderSphere::DrawDebug(int debugColor) const
{
    const VECTOR worldPosition = GetWorldPosition();

    // デバッグ用に球体を描画
    DrawSphere3D(worldPosition, radius_, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
}