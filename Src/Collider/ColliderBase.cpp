#include "ColliderBase.h"

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

// 描画
void ColliderBase::Draw(void)
{
	int color = COLOR_INVALID;

	if (isValid_)
	{
		color = COLOR_VALID;
	}

	DrawDebug(color);
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// 追従相手の回転に合わせて指定ローカル座標を回転し、
	// 基準座標に加えることでワールド座標へ変換
	VECTOR localRotPos = follow_->quaRot.PosAxis(localPos);
	return VAdd(follow_->pos, localRotPos);
}

// 追従先の取得
const Transform* ColliderBase::GetFollow(void) const
{
	return follow_;
}

// 追従先の設定
void ColliderBase::SetFollow(Transform* follow)
{
	follow_ = follow;
}

// 形状の取得
ColliderBase::SHAPE ColliderBase::GetShape(void) const
{
	return shape_;
}

// 衝突種別の取得
ColliderBase::TAG ColliderBase::GetTag(void) const
{
	return tag_;
}

// 有効フラグの取得
bool ColliderBase::IsValid(void) const
{
	return isValid_;
}

// 有効フラグの設定
void ColliderBase::SetValid(bool valid)
{
	isValid_ = valid;
}
