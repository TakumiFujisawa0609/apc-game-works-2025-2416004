#include "ColliderBase.h"
#include "../Object/Common/Transform.h"

// コンストラクタ
ColliderBase::ColliderBase(SHAPE shape, TAG tag, const Transform* follow)
    : shape_(shape)
    , tag_(tag)
    , follow_(follow)
    , isValid_(true)
{
}

// デストラクタ
ColliderBase::~ColliderBase(void)
{
}

// 描画処理
void ColliderBase::Draw(void)
{
    int color = COLOR_INVALID;
    if (isValid_)
    {
        color = COLOR_VALID;
    }
    DrawDebug(color);
}

void ColliderBase::SetFollow(Transform* follow)
{
    follow_ = follow;
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
    // 追従相手の回転に合わせて指定ローカル座標を回転し、
    // 基準座標に加えることでワールド座標へ変換
    VECTOR localRotPos = follow_->quaRot.PosAxis(localPos);
    return VAdd(follow_->pos, localRotPos);
}

const Transform* ColliderBase::GetFollow(void) const
{
    return follow_;
}

bool ColliderBase::IsValid(void) const
{
    return isValid_;
}

void ColliderBase::SetValid(bool valid)
{
    isValid_ = valid;
}

ColliderBase::SHAPE ColliderBase::GetShape(void) const
{
    return shape_;
}

ColliderBase::TAG ColliderBase::GetTag(void) const
{
    return tag_;
}
