#pragma once

#include "../UnitBase.h"

class Player;

/// @brief プレイヤーが放つ炎攻撃のクラス
/// @details プレイヤーの前方に攻撃判定とエフェクトを発生させる
class FireAttack : public UnitBase
{
public:
    /// @brief コンストラクタ
    FireAttack(void);

    /// @brief デストラクタ
    ~FireAttack(void) override;

    /// @brief リソース（エフェクト等）の読み込み
    void Load(void) override;

    /// @brief 初期化処理（基本）
    void Initialize(void) override;

    /// @brief 初期化処理（プレイヤー追従設定用）
    /// @param player 攻撃を発動したプレイヤーへのポインタ
    /// @param offsetPos プレイヤーからの相対出現位置
    void Initialize(Player* player, const VECTOR& offsetPos);

    /// @brief 更新処理（持続時間の管理や位置更新）
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) const override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief 攻撃が現在有効かどうかを取得
    /// @return 有効ならtrue
    bool IsActive(void) const;

    /// @brief 攻撃の有効/無効を設定
    /// @param active 設定するフラグ
    void SetActive(bool active);

protected:
    /// @brief 当たり判定の初期化
    void InitCollider(void) override;

private:

    // 攻撃パラメーター定数
    static constexpr float FIRE_RANGE = 180.0f;    // 炎の攻撃射程（当たり判定サイズ）
    static constexpr float FIRE_DURATION = 0.8f;   // 炎が出続ける時間
    static constexpr float FIRE_OFFSET_Z = -120.0f; // プレイヤー前方へのオフセット（モデル向き依存）
    static constexpr float FIRE_OFFSET_Y = 50.0f;   // 地面からの高さオフセット

    // プレイヤー連携関連
    Player* player_;              // 発動者であるプレイヤーへの参照
    VECTOR offsetPos_;            // プレイヤーからの相対的な配置オフセット

    // 状態管理関連
    float duration_;              // 攻撃の総持続時間
    float elapsedTime_;           // 発動からの経過時間
    bool isActive_;               // 攻撃判定・更新が有効かどうかのフラグ
    int effectHandle_;            // 再生中のエフェクトハンドル

};