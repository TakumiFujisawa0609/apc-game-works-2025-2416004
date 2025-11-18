#pragma once
#include "ColliderBase.h"

class ColliderModel : public ColliderBase
{
public:
    // コンストラクタ
    ColliderModel(TAG tag, const Transform* follow);

    // デストラクタ
    ~ColliderModel(void) override;

    // 指定された文字を含むフレームを衝突判定から除外
    void AddExcludeFrameIds(const std::string& name);

    // 除外フレームのクリア
    void ClearExcludeFrame(void);

    // 除外フレーム判定
    bool IsExcludeFrame(int frameIdx) const;
protected:
    // デバッグ用描画(モデルは描画不要)
    void DrawDebug(int color) override {};

private:
    // 衝突判定から除外するフレーム番号
    std::vector<int> excludeFrameIds_;
};