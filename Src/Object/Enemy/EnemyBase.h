#pragma once
#include "../UnitBase.h"
#include "EnemyData.h"

/// @brief 全てのエネミーの基底となるクラス
/// @details 共通のパラメータ（HP、攻撃力など）や状態異常システムの管理を行う
class EnemyBase : public UnitBase
{
public:

    /// @brief 状態異常の種類
    enum class STATUS_EFFECT
    {
        NONE,               // なし
        AMMONIUM_NITRATE,   // 硝酸アンモニウム状態（剣攻撃後に付着）
        FROZEN,             // 凍結（硝酸アンモニウム状態で水攻撃を受けると発生）
        BURN,               // やけど（通常時に火攻撃を受けると発生）
        WET,                // 湿潤・鈍足（通常時に水攻撃を受けると発生）
        EXPLODED,           // 爆発済み（硝酸アンモニウム状態で火攻撃を受けると発生）
    };

    /// @brief コンストラクタ
    EnemyBase(void);

    /// @brief デストラクタ
    ~EnemyBase(void) override = default;

    /// @brief モデル等のリソース読み込み
    /// @param modelId 使用するモデルのID
    virtual void Load(int modelId);

    /// @brief 初期化処理
    /// @param startPos 出現位置
    virtual void Initialize(const VECTOR& startPos);

    /// @brief 更新処理（毎フレーム）
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) const override;

    /// @brief 解放処理
    void Release(void);

    /// @brief CSV等から読み込んだ設定データを適用する
    /// @param info エネミーの設定情報構造体
    virtual void ApplyData(const EnemyInfo& info);

    /// @brief 前方向（向き）を設定する
    /// @param forward 前方向ベクトル
    void SetForward(const VECTOR& forward);

    /// @brief 視野角を設定する
    /// @param angle 視野の角度（ラジアン）
    void SetViewAngle(float angle);

    /// @brief 現在HPを直接設定する
    /// @param hp 設定するHP
    void SetHp(float hp);

    /// @brief 現在HPを取得する
    /// @return 現在のHP
    float GetHp(void) const;

    /// @brief ダメージ算出・適用処理
    /// @param damage 受けるダメージ量
    virtual void TakeDamage(float damage);

    /// @brief エネミーのタイプ名を取得する
    /// @return タイプ名の文字列参照
    const std::string& GetType(void) const;

    /// @brief 衝突が発生した際のコールバック
    /// @param info 衝突情報
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// @brief レベルを設定する
    /// @param level 設定するレベル
    void SetLevel(int level);

    /// @brief 現在のレベルを取得する
    /// @return レベル
    int GetLevel(void) const;

    /// @brief 撃破時に得られる経験値報酬を計算して取得する
    /// @return 経験値量
    int GetExpReward(void) const;

    /// @brief 現在の攻撃力を取得
    /// @return 攻撃力
    float GetAttack(void) const;

    /// @brief 現在の防御力を取得
    /// @return 防御力
    float GetDefense(void) const;

protected:

    // 判定・オフセット関連
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 50.0f, 0.0f };  // 地面判定用線分の開始位置
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -50.0f, 0.0f };   // 地面判定用線分の終了位置
    static constexpr float GROUND_OFFSET = 2.0f;                               // 地面からの浮かせオフセット

    // ダメージ設定関連
    static constexpr float SWORD_DAMAGE = 35.0f;           // 剣のダメージ
    static constexpr float FIRE_DAMAGE = 50.0f;            // 火のダメージ（通常）
    static constexpr float WATER_DAMAGE = 40.0f;           // 水のダメージ（通常）
    static constexpr float EXPLOSION_DAMAGE = 150.0f;      // 爆発ダメージ（硝酸アンモニウム+火）
    static constexpr float BURN_DAMAGE_PER_SEC = 10.0f;    // やけどの継続ダメージ（秒間）

    // 状態異常持続時間関連
    static constexpr float AMMONIUM_NITRATE_DURATION = 8.0f;  // 硝酸アンモニウム状態の持続時間
    static constexpr float FROZEN_DURATION = 10.0f;           // 凍結の持続時間
    static constexpr float BURN_DURATION = 5.0f;              // やけどの持続時間
    static constexpr float WET_DURATION = 4.0f;               // 湿潤の持続時間

    // 状態異常デバフ効果関連
    static constexpr float WET_SPEED_MULTIPLIER = 0.5f;      // 湿潤時の移動速度倍率
    static constexpr float FROZEN_SPEED_MULTIPLIER = 0.0f;   // 凍結時の移動速度倍率

    // システム設定関連
    static constexpr float HIT_COOLDOWN = 0.3f;              // ヒット判定のクールタイム
    static constexpr int LEVEL_UP_STAT = 10;                 // レベルアップ時の基礎ステータス上昇値
    static constexpr int BASE_EXP_REWARD = 50;               // 撃破時の基礎経験値報酬

    // ステータス関連
    float hp_;          // 現在の体力
    float maxHp_;       // 最大体力
    float attack_;      // 現在の攻撃力
    float defense_;     // 現在の防御力
    int level_;         // 現在のレベル
    float moveSpeed_;   // 基本移動速度

    // 索敵・視野関連
    float viewRange_;   // ターゲットを発見する距離
    float lostRange_;   // ターゲットを見失う距離
    float viewAngle_;   // 視野角（ラジアン）
    bool isChasing_;    // ターゲットを追跡中かどうか
    bool isInView_;     // ターゲットが視界内かどうか
    VECTOR forward_;    // エネミーの現在の前方向ベクトル

    // 状態異常関連
    STATUS_EFFECT currentStatus_; // 現在かかっている状態異常
    float statusTimer_;           // 状態異常の残り持続時間
    float burnTickTimer_;         // やけどダメージを発生させる間隔タイマー
    int statusEffectHandle_;      // 再生中の状態異常エフェクトハンドル 

    // 履歴・識別関連
    float lastHitTime_;       // 最後に攻撃が当たったゲーム内時間
    VECTOR preCollisionPos_;  // 衝突が起きる前の座標
    std::string type_;        // エネミーのタイプ（識別用文字列）

    /// @brief 指定した座標が移動可能エリア内かチェックする
    /// @param pos チェックする座標
    /// @return エリア内ならtrue
    bool IsOnStage(const VECTOR& pos) const;

    /// @brief 当たり判定の初期化（UnitBaseのオーバーライド）
    void InitCollider(void) override;

    /// @brief レベルに応じたステータスの再計算と適用
    void ApplyLevelParams(void);

    /// @brief 状態異常のタイマー更新と効果適用（毎フレーム）
    /// @param deltaTime 1フレームの経過時間
    void UpdateStatusEffect(float deltaTime);

    /// @brief 新しい状態異常をセットする
    /// @param status 適用する状態
    void ApplyStatusEffect(STATUS_EFFECT status);

    /// @brief 同じ状態異常を重ねた際のタイマー再スタート
    void RestartStatusEffect(void);

    /// @brief 全ての状態異常を強制解除する
    void ClearStatusEffect(void);

    /// @brief 現在の状態異常に基づいた速度補正倍率を取得
    /// @return 速度倍率（0.0 ～ 1.0）
    float GetSpeedMultiplier(void) const;

    /// @brief 状態異常に応じた特殊な描画（エフェクト等）を行う
    void DrawStatusEffect(void) const;

    /// @brief プレイヤーの剣攻撃が当たった時の内部処理
    void OnSwordHit(void);

    /// @brief 火属性の攻撃が当たった時の内部処理
    void OnFireHit(void);

    /// @brief 水属性の攻撃が当たった時の内部処理
    void OnWaterHit(void);
};