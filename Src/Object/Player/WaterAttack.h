#pragma once
#include "../UnitBase.h"

class Player;

/// @brief プレイヤーが使用する水魔法攻撃を制御するクラス
class WaterAttack : public UnitBase
{
public:
    /// @brief コンストラクタ
    WaterAttack(void);

    /// @brief デストラクタ
    ~WaterAttack(void) override;

    /// @brief リソースの読み込み
    void Load(void) override;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief プレイヤー情報を伴う初期化
    /// @param player プレイヤーへのポインタ
    /// @param offsetPos 出現位置のオフセット
    void Initialize(Player* player, const VECTOR& offsetPos);

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) const override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief スキルが実行中かどうかを取得
    /// @return 実行中ならtrue
    bool IsActive(void) const;

    /// @brief スキルの有効状態を設定
    /// @param active 有効にするならtrue
    void SetActive(bool active);

protected:
    /// @brief 当たり判定の初期化
    void InitCollider(void) override;

private:
    // 定数関連
    static constexpr float WATER_RANGE = 150.0f;     // 攻撃判定の半径
    static constexpr float WATER_DURATION = 1.0f;    // 攻撃の持続秒数
    static constexpr float WATER_OFFSET_Z = -100.0f; // プレイヤー前方へのオフセット（モデル向きにより負数）
    static constexpr float WATER_OFFSET_Y = 50.0f;   // プレイヤー高さへのオフセット

    // 使用者（プレイヤー）の参照
    Player* player_;             

    // プレイヤーからの相対出現位置
    VECTOR offsetPos_;           

    // 時間管理関連
    float duration_;             // スキルの持続時間
    float elapsedTime_;          // 実行されてからの経過時間

    // スキルがアクティブかどうか
    bool isActive_;              

    // 演出用エフェクトのハンドル
    int effectHandle_;           
};