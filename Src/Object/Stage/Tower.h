// Tower.h
#pragma once

#include "../UnitBase.h"

// タワー固有のデータ
struct TowerData
{
    float attackRange;
    int damage;
    float attackInterval;
};

class Tower : public UnitBase
{
public:
    // コンストラクタ: 座標を受け取る
    Tower(const VECTOR& position);

    // デストラクタ
    virtual ~Tower() override;

    // リソースの読み込み (モデルIDを設定)
    virtual void Load(int modelId);

    // 初期化
    virtual void Init(void) override;

    // 更新処理
    virtual void Update(void) override; // ★ 修正箇所

    // 描画処理 (モデル描画を実装)
    virtual void Draw(void) const override;

    // 解放処理
    virtual void Release(void) override;

    // タワーのパラメータを設定
    void ApplyData(const TowerData& data);

    // 座標の設定
    void SetPosition(const VECTOR& position);

    // 地面との接触フラグを取得
    bool IsGround(void) const { return isGround_; }

protected:
    // 衝突イベントのオーバーライド
    virtual void OnCollisionEnter(const CollisionInfo& info) override;
    virtual void OnCollisionStay(const CollisionInfo& info) override;

private:

    // コライダー関連
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f };  // ラインライダー開始位置
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };  // ラインコライダー終了位置

    TowerData data_;
    float attackTimer_;

    // 地面との接触フラグ
    bool isGround_;

    void InitCollider(void) override;
};