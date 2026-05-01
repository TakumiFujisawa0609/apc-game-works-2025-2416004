#pragma once
#include "../UnitBase.h"
#include "EnemyData.h"

// エネミーのベース
class EnemyBase : public UnitBase
{
public:
    // 状態異常の種類
    enum class STATUS_EFFECT
    {
        NONE,               // なし
        AMMONIUM_NITRATE,   // 硝酸アンモニウム状態（剣攻撃後）
        FROZEN,             // 凍結（硝酸アンモニウム状態で水攻撃）
        BURN,               // やけど（通常時に火攻撃）
        WET,                // 湿潤・鈍足（通常時に水攻撃）
        EXPLODED,           // 爆発済み（硝酸アンモニウム状態で火攻撃）
    };

    // コンストラクタ
    EnemyBase(void);

    // デストラクタ
    ~EnemyBase(void) override = default;

    // 読み込み
    virtual void Load(int modelId);

    // 初期化
    virtual void Initialize(const VECTOR& startPos);

    // 更新処理
    void Update(void) override;

    // 描画処理
    void Draw(void) const override;

    // 解放処理
    void Release(void);

    // CSVデータを適用
    virtual void ApplyData(const EnemyInfo& info);

    // 前方向を設定
    void SetForward(const VECTOR& forward);

    // 視野角を設定
    void SetViewAngle(float angle);

    // HPの設定
    void SetHp(float hp);

    // HPの取得
    float GetHp(void) const;

    // ダメージ処理
    virtual void TakeDamage(float damage);

    // 自身のタイプを返す
    const std::string& GetType(void) const;

    // 衝突時のコールバック
    void OnCollisionEnter(const CollisionInfo& info) override;

    // レベルの設定
    void SetLevel(int level);

    // レベルを取得
    int GetLevel(void) const;

    // 撃破時の経験値報酬
    int GetExpReward(void) const;

    // 攻撃力を取得
    float GetAttack(void) const;

    // 防御力を取得
    float GetDefense(void) const;

protected:
    // 地面判定用線分の開始位置（ローカル座標）
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

    // 地面判定用線分の終了位置（ローカル座標）
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -50.0f, 0.0f };

    // 地面からのオフセット
    static constexpr float GROUND_OFFSET = 2.0f;

    // ダメージ量
    static constexpr float SWORD_DAMAGE = 35.0f;           // 剣のダメージ
    static constexpr float FIRE_DAMAGE = 50.0f;            // 火のダメージ（通常）
    static constexpr float WATER_DAMAGE = 40.0f;           // 水のダメージ（通常）
    static constexpr float EXPLOSION_DAMAGE = 150.0f;      // 爆発ダメージ（硝酸アンモニウム+火）
    static constexpr float BURN_DAMAGE_PER_SEC = 10.0f;    // やけどの継続ダメージ（秒間）

    // 状態異常の持続時間
    static constexpr float AMMONIUM_NITRATE_DURATION = 8.0f;  // 硝酸アンモニウム状態
    static constexpr float FROZEN_DURATION = 10.0f;            // 凍結
    static constexpr float BURN_DURATION = 5.0f;              // やけど
    static constexpr float WET_DURATION = 4.0f;               // 湿潤

    // 状態異常の効果
    static constexpr float WET_SPEED_MULTIPLIER = 0.5f;       // 湿潤時の移動速度倍率（50%減）
    static constexpr float FROZEN_SPEED_MULTIPLIER = 0.0f;    // 凍結時の移動速度倍率（完全停止）

    // ヒット判定のクールタイム
    static constexpr float HIT_COOLDOWN = 0.3f;

    // レベルアップ時の上昇値
    static constexpr int LEVEL_UP_STAT = 10;

    // 撃破時の経験値報酬（レベル依存）
    static constexpr int BASE_EXP_REWARD = 50;

    // 最後にヒットした時間
    float lastHitTime_;

    // 衝突前の座標
    VECTOR preCollisionPos_;

    // 体力
    float hp_;

    // 最大体力
    float maxHp_;

    // 攻撃力
    float attack_;

    // 防御力
    float defense_;

    // レベル
    int level_;

    // 基本移動速度
    float moveSpeed_;

    // 視野距離
    float viewRange_;

    // 視野解除距離
    float lostRange_;

    // 視野角
    float viewAngle_;

    // 追跡中フラグ
    bool isChasing_;

    // 視野内かどうか
    bool isInView_;

    // 前方向
    VECTOR forward_;

    // タイプ
    std::string type_;

    // 状態異常関連
    STATUS_EFFECT currentStatus_;      // 現在の状態異常
    float statusTimer_;                // 状態異常の残り時間
    float burnTickTimer_;              // やけどのダメージ間隔タイマー
    int statusEffectHandle_;           // 状態以上エフェクトのハンドル 

    // ステージ上にいるかチェック
    bool IsOnStage(const VECTOR& pos) const;

    // 衝突判定の初期化
    void InitCollider(void) override;

    // レベル事のパラメータ計算
    void ApplyLevelParams(void);

    // 状態異常の更新
    void UpdateStatusEffect(float deltaTime);

    // 状態異常を適用
    void ApplyStatusEffect(STATUS_EFFECT status);

    // 状態異常再スタート
    void RestartStatusEffect(void);

    // 状態異常をクリア
    void ClearStatusEffect(void);

    // 現在の移動速度倍率を取得（状態異常による影響を含む）
    float GetSpeedMultiplier(void) const;

    // 状態異常のエフェクトを描画
    void DrawStatusEffect(void) const;

    // 剣攻撃を受けた時の処理
    void OnSwordHit(void);

    // 火攻撃を受けた時の処理
    void OnFireHit(void);

    // 水攻撃を受けた時の処理
    void OnWaterHit(void);
};