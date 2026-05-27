#pragma once

#include <DxLib.h>

#include "../UnitBase.h"
#include "../../Manager/System/CollisionController.h"



/// @brief 防衛拠点となるタワーを制御するクラス
class Tower : public UnitBase
{
public:

    /// @brief タワー固有のパラメータを管理する構造体
    struct TowerData
    {
        float attackRange;      // 攻撃範囲（半径）
        int damage;             // 与えるダメージ量
        float attackInterval;   // 次の攻撃までの待機時間（秒）
    };

    /// @brief コンストラクタ
    /// @param position 設置する座標
    Tower(const VECTOR& position);

    /// @brief デストラクタ
    virtual ~Tower(void) override;

    /// @brief リソースの読み込み
    /// @param modelId 使用するモデルのハンドル
    virtual void Load(int modelId);

    /// @brief 初期化処理
    virtual void Initialize(void) override;

    /// @brief 更新処理
    virtual void Update(void) override;

    /// @brief 描画処理
    virtual void Draw(void) const override;

    /// @brief 解放処理
    virtual void Release(void) override;

    /// @brief タワーのパラメータを一括設定
    /// @param data 設定するタワーデータ
    void ApplyData(const TowerData& data);

    /// @brief 座標の設定
    /// @param position 設定する座標
    void SetPosition(const VECTOR& position);

    /// @brief 地面との接触フラグを取得
    /// @return 接地していればtrue
    bool IsGround(void) const { return isGround_; }

    /// @brief 重力計算のオーバーライド
    void CalcGravityPow(void) override;

protected:
    /// @brief 衝突開始イベント
    /// @param info 衝突相手の情報
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// @brief 衝突継続イベント
    /// @param info 衝突相手の情報
    void OnCollisionStay(const CollisionInfo& info) override;

private:
    // コライダー関連定数
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 1000.0f, 0.0f };  // ラインコライダー開始（上空）
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };      // ラインコライダー終了（足元）

    // パラメータ・タイマー関連
    TowerData data_;            // 攻撃力、範囲などの基礎データ
    float attackTimer_;         // 攻撃間隔を計測するタイマー

    // 地面と接触しているかどうかのフラグ
    bool isGround_;             

    /// @brief 衝突判定の初期化
    void InitCollider(void) override;
};