#include "ColliderLine.h"
#include "../Object/Common/Transform.h"

// コンストラクタ
ColliderLine::ColliderLine(TAG tag, const Transform* follow, const VECTOR& localPosStart, const VECTOR& localPosEnd)
    : ColliderBase(SHAPE::LINE, tag, follow)
    , localPosStart_(localPosStart)
    , localPosEnd_(localPosEnd)
{
}

// デストラクタ
ColliderLine::~ColliderLine(void)
{
}

// ローカル座標での設定(開始地点)
void ColliderLine::SetLocalPosStart(const VECTOR& pos)
{
    localPosStart_ = pos;
}

// ローカル座標での設定(終了地点)
void ColliderLine::SetLocalPosEnd(const VECTOR& pos)
{
    localPosEnd_ = pos;
}

// ローカル座標の取得(開始地点)
const VECTOR& ColliderLine::GetLocalPosStart(void) const
{
    return localPosStart_;
}

// ローカル座標の取得(終了地点)
const VECTOR& ColliderLine::GetLocalPosEnd(void) const
{
    return localPosEnd_;
}

// ワールド座標の取得(開始地点)
VECTOR ColliderLine::GetPosStart(void) const
{
    return GetRotPos(localPosStart_);
}

// ワールド座標の取得(終了地点)
VECTOR ColliderLine::GetPosEnd(void) const
{
    return GetRotPos(localPosEnd_);
}

// デバック用描画
void ColliderLine::DrawDebug(int color)
{
    VECTOR s = GetPosStart();
    VECTOR e = GetPosEnd();

    // 線分を描画
    DrawLine3D(s, e, color);

    // 始点・終点を球体で補助表示
    DrawSphere3D(s, RADIUS, DIV_NUM, color, color, true);
    DrawSphere3D(e, RADIUS, DIV_NUM, color, color, true);
}