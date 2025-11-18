#include "ColliderSphere.h"
#include "../Object/Common/Transform.h"

// コンストラクタ
ColliderSphere::ColliderSphere(TAG tag, const Transform* follow, const VECTOR& localPos, float radius)
    : ColliderBase(SHAPE::SPHERE, tag, follow)
    , localPos_(localPos)
    , radius_(radius)
{
}

// デストラクタ
ColliderSphere::~ColliderSphere(void)
{
}

// ローカル座標の設定
void ColliderSphere::SetLocalPos(const VECTOR& pos)
{
    localPos_ = pos;
}

// 衝突半径の設定
void ColliderSphere::SetRadius(float radius)
{
    radius_ = radius;
}

// ローカル座標の取得
const VECTOR& ColliderSphere::GetLocalPos(void) const
{
    return localPos_;
}

// ワールド座標の取得
VECTOR ColliderSphere::GetPos(void) const
{
    return GetRotPos(localPos_);
}

// 半径の取得
float ColliderSphere::GetRadius(void) const
{
    return radius_;
}

// デバッグ用描画
void ColliderSphere::DrawDebug(int color)
{
    VECTOR pos = GetPos();
    DrawSphere3D(pos, radius_, DIV_NUM, color, color, false);
}