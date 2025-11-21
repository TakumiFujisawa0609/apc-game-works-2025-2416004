#pragma once
#include "../UnitBase.h"

// ステージの地面部品クラス
class Ground : public UnitBase
{
public:
    // コンストラクタ
    Ground(void);

    // デストラクタ
    ~Ground(void) override;

    // 初期化
    void Init(const VECTOR& pos, int modelId);

    // 初期化（CollisionController登録なし)
    void InitWithoutRegister(const VECTOR& pos, int modelId);

    // リソース読み込み（使用しない）
    void Load(void) override {}

    // 更新処理（地面は動かないので空実装）
    void Update(void) override;

    // 描画処理
    void Draw(void) const override;

    // 解放
    void Release(void) override;

    // タイルサイズを取得
    float GetTileSize(void) const;

protected:
    // 衝突判定の初期化
    void InitCollider(void) override;

private:
    // タイルサイズ（スケール適用前）
    static constexpr float BASE_TILE_SIZE = 100.0f;

    // スケール
    static constexpr float TILE_SCALE = 10.0f;

    // CollisionControllerに登録済みか
    bool isRegistered_;
};