#include "ColliderCapsule.h"
#include "../Object/Common/Transform.h"

ColliderCapsule::ColliderCapsule(
    TAG tag, const Transform* follow,
    const VECTOR& localPosStart, const VECTOR& localPosEnd, float radius)
    : ColliderBase(SHAPE::CAPSULE, tag, follow)
    , localPosStart_(localPosStart)
    , localPosEnd_(localPosEnd)
    , radius_(radius)
{
}

ColliderCapsule::~ColliderCapsule(void)
{
}

void ColliderCapsule::SetLocalPosStart(const VECTOR& pos)
{
    localPosStart_ = pos;
}

void ColliderCapsule::SetLocalPosEnd(const VECTOR& pos)
{
    localPosEnd_ = pos;
}

void ColliderCapsule::SetRadius(float radius)
{
    radius_ = radius;
}

const VECTOR& ColliderCapsule::GetLocalPosStart(void) const
{
    return localPosStart_;
}

const VECTOR& ColliderCapsule::GetLocalPosEnd(void) const
{
    return localPosEnd_;
}

VECTOR ColliderCapsule::GetPosStart(void) const
{
    return GetRotPos(localPosStart_);
}

VECTOR ColliderCapsule::GetPosEnd(void) const
{
    return GetRotPos(localPosEnd_);
}

float ColliderCapsule::GetRadius(void) const
{
    return radius_;
}

void ColliderCapsule::DrawDebug(int color)
{
    VECTOR s = GetPosStart();
    VECTOR e = GetPosEnd();

    // ƒJƒvƒZƒ‹‚ð•`‰æ
    DrawCapsule3D(s, e, radius_, DIV_NUM, color, color, false);
}