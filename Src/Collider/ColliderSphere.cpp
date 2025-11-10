#include "ColliderSphere.h"
#include "../Object/Common/Transform.h"

ColliderSphere::ColliderSphere(
    TAG tag, const Transform* follow,
    const VECTOR& localPos, float radius)
    : ColliderBase(SHAPE::SPHERE, tag, follow)
    , localPos_(localPos)
    , radius_(radius)
{
}

ColliderSphere::~ColliderSphere(void)
{
}

void ColliderSphere::SetLocalPos(const VECTOR& pos)
{
    localPos_ = pos;
}

void ColliderSphere::SetRadius(float radius)
{
    radius_ = radius;
}

const VECTOR& ColliderSphere::GetLocalPos(void) const
{
    return localPos_;
}

VECTOR ColliderSphere::GetPos(void) const
{
    return GetRotPos(localPos_);
}

float ColliderSphere::GetRadius(void) const
{
    return radius_;
}

void ColliderSphere::DrawDebug(int color)
{
    VECTOR pos = GetPos();
    DrawSphere3D(pos, radius_, DIV_NUM, color, color, false);
}