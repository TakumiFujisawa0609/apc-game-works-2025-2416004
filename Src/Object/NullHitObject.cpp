#include "NullHitObject.h"

HitObject::HIT_TYPE NullHitObject::GetHitType(void) const
{
    return HIT_TYPE();
}

VECTOR NullHitObject::GetHitPosition(void) const
{
    return { 0,0,0 };
}

float NullHitObject::GetHitRadius(void) const
{
    return 0.0f;
}

VECTOR NullHitObject::GetHitMin(void) const
{
    return VECTOR();
}

VECTOR NullHitObject::GetHitMax(void) const
{
    return VECTOR();
}

void NullHitObject::ShowUI(void)
{

}

void NullHitObject::HideUI(void)
{
}

bool NullHitObject::IsValid(void) const
{
    return false;
}

void NullHitObject::OnPlayerHit(void)
{

}

void NullHitObject::OnPlayerExit(void)
{
}
