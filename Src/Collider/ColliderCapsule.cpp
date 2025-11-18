#include "ColliderCapsule.h"
#include "../Object/Common/Transform.h"

// コンストラクタ
ColliderCapsule::ColliderCapsule(TAG tag, const Transform* follow, const VECTOR& localPosStart, const VECTOR& localPosEnd, float radius)
    : ColliderBase(SHAPE::CAPSULE, tag, follow)
    , localPosStart_(localPosStart)
    , localPosEnd_(localPosEnd)
    , radius_(radius)
{
}

// デストラクタ
ColliderCapsule::~ColliderCapsule(void)
{
}

// ローカル座標での設定(開始地点)
void ColliderCapsule::SetLocalPosStart(const VECTOR& pos)
{
    localPosStart_ = pos;
}

// ローカル座標での設定(終了地点)
void ColliderCapsule::SetLocalPosEnd(const VECTOR& pos)
{
    localPosEnd_ = pos;
}

// 衝突半径の設定
void ColliderCapsule::SetRadius(float radius)
{
    radius_ = radius;
}

// ローカル座標の取得(開始地点)
const VECTOR& ColliderCapsule::GetLocalPosStart(void) const
{
    return localPosStart_;
}

// ローカル座標の取得(終了地点)
const VECTOR& ColliderCapsule::GetLocalPosEnd(void) const
{
    return localPosEnd_;
}

// ワールド座標の取得(開始地点)
VECTOR ColliderCapsule::GetPosStart(void) const
{
    return GetRotPos(localPosStart_);
}

// ワールド座標の取得(終了地点)
VECTOR ColliderCapsule::GetPosEnd(void) const
{
    return GetRotPos(localPosEnd_);
}

// 半径の取得
float ColliderCapsule::GetRadius(void) const
{
    return radius_;
}

// デバッグ用描画
void ColliderCapsule::DrawDebug(int color)
{
    VECTOR s = GetPosStart();
    VECTOR e = GetPosEnd();

    // カプセルを描画
    DrawCapsule3D(s, e, radius_, DIV_NUM, color, color, false);
}