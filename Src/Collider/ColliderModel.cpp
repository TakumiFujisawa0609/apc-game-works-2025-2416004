#include "ColliderModel.h"

// コンストラクタ
ColliderModel::ColliderModel(TAG tag, const Transform* follow)
    : ColliderBase(SHAPE::MODEL, tag, follow)
{
}

// デストラクタ
ColliderModel::~ColliderModel(void)
{
}

// 指定された文字を含むフレームを衝突判定から除外
void ColliderModel::AddExcludeFrameIds(const std::string& name)
{
    if (!follow_ || follow_->modelId == -1) { return; }

    // フレーム数を取得
    int num = MV1GetFrameNum(follow_->modelId);

    for (int i = 0; i < num; i++)
    {
        // フレーム名を取得
        const char* frameName = MV1GetFrameName(follow_->modelId, i);
        std::string frameNameStr = frameName;

        // 指定された文字列が含まれているか
        if (frameNameStr.find(name) != std::string::npos)
        {
            // 除外フレームに追加
            excludeFrameIds_.push_back(i);
        }
    }
}

// 除外フレームのクリア
void ColliderModel::ClearExcludeFrame(void)
{
    excludeFrameIds_.clear();
}

// 除外フレーム判定
bool ColliderModel::IsExcludeFrame(int frameIdx) const
{
    return std::find(excludeFrameIds_.begin(), excludeFrameIds_.end(), frameIdx) != excludeFrameIds_.end();
}


