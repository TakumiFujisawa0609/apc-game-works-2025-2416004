#pragma once
#include "ColliderBase.h"

class ColliderModel : public ColliderBase
{
public:
    // コンストラクタ
    ColliderModel(TAG tag, const Transform* follow);

    // デストラクタ
    ~ColliderModel(void) override;

protected:
    // デバッグ用描画(モデルは描画不要)
    void DrawDebug(int color) override {};
};