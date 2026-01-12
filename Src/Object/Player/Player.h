#pragma once
#include "../UnitBase.h"

class AnimationController;
class Sword;
class FireAttack;
class WaterAttack;
class Glider;

class Player : public UnitBase
{
public:

    // パラメーター構造体
    struct Param
    {
        int attack;                // 攻撃力
        int defensse;              // 防御力
        int hp;                    // 現在体力
        int maxHp;                 // 最大体力
        int stamina;               // スタミナ
        float collisionRadius;     // 衝突半径
        int level;                 // 現在レベル
        int maxLevel;              // 最大レベル
        float jumpPower;           // ジャンプ力
    };

    // コンストラクタ
    Player(void);

    // デストラクタ
    ~Player(void) override;

    // パラメータCSV読み込み
    void LoadParamCSV(const std::string& path);

    // リソース読み込み
    void Load(void) override;

    // 初期化
    void Init(void) override;

    // 更新処理
    void Update(void) override;

    // 描画処理
    void Draw(void) const override;

    // 解放処理
    void Release(void) override;

    // 移動可否の設定
    void SetMovementEndbled(bool enabled);

    // 移動可能か
    bool IsMovementEndbled(void) const;

    // キャラパラメータ取得
    const Param& GetParam(void) const;

    // HPバー
    void DrawHpBar(void) const;

    // ダメージ処理
    void TakeDamage(int damage);

    // ワールド座標取得ポインタ
    VECTOR* GetPosPtr(void);

    // 攻撃中か
    bool IsAttacking(void) const;

    // 通常攻撃
    void Attack(void);

    // 火攻撃
    void FireAttackAction(void);

    //　水攻撃
    void WaterAttackAction(void);

    // 原点位置の設定
    void SetOriginPos(const VECTOR& pos);

    // 原点位置を取得
    const VECTOR& GetOriginPos(void) const;

    // 原点からの距離取得
    float GetDistanceFromOrigin(void) const;

    // 原点からの距離(XZ軸)取得
    float GetDistanceFromOriginXZ(void) const;

    // レベルアップ
    void LevelUp(void);

    // 経験値加算処理
    void AddExperinece(int exp);

    // レベルアップ可能か？
    bool IsLevelUp(void) const;

    // 必要経験値の計算
    int CalcRequiredExp(int level) const;

    // 現在のレベルを取得
    int GetLevel(void) const;

    // 現在の経験値を取得
    int GetExperinece(void) const;

    // 必要経験値を取得
    int GetRequireExp(void) const;

    // レベルUI描画
    void DrawLevelInfo(void) const;

protected:
    // コライダーの初期化
    void InitCollider(void) override;

    // 衝突開始
    void OnCollisionEnter(const CollisionInfo& info) override;

    // 衝突継続
    void OnCollisionStay(const CollisionInfo& info) override;

    // 毎フレーム衝突処理
    void Collision(void) override;

    // 重力計算（オーバーライド）
    void CalcGravityPow(void) override;


private:

    // 攻撃関連
    static constexpr float ATTACK_COOL_TIME_MAX = 0.5f;         // 通常攻撃のクルータイム
    static constexpr float FIRE_ATTACK_COOL_TIME_MAX = 3.0f;    // 火攻撃のクールタイム
    static constexpr float WATER_ATTACK_COOL_TIME_MAX = 3.0f;   //　水攻撃のクールタイム

    // 被ダメージ関連
    static constexpr float INVINCIBLE_DURATION = 1.0f;          // 無敵時間
    static constexpr float HIT_COOL_TIME = 0.5f;                // 被弾間隔

    // レベルアップ関連
    static constexpr int BASE_EXP = 100;                        // 基礎必要経験値
    static constexpr float EXP_MULTIPLIER = 1.5f;               // 経験値倍率
    static constexpr int LEVEL_UP_STAT = 10;                    // レベルアップ時のステータス上昇

    // HPバー関連
    static constexpr int HP_BAR_WIDTH = 300;                    // HPバーの幅
    static constexpr int HP_BAR_HEIGHT = 20;                    // HPバーの高さ
    static constexpr int BAR_Y = 50;                            // HPバーのY位置
    static constexpr float DELAY_SPEED = 2.0f;                  // HPバーの遅延速度

    // ジャンプ&グライド関連
    static constexpr float GLIDE_FALL_SPEED = 3.0f;            // グライド時の降下速度
    static constexpr float GLIDE_HORIZONTAL_SPEED = 8.0f;      // グライド時の水平移動速度
    static constexpr float MAX_FALL_SPEED = 15.0f;             // 通常時の最大落下速度（下げる)

    // スケール
    static constexpr VECTOR PLAYER_SCL = { 3.5f, 3.5f, 3.5f };  // プレイヤーのスケール

    // コライダー関連
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f };  // ラインライダー開始位置
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };  // ラインコライダー終了位置
    static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f, 130.0f, 0.0f };     // カプセル始点
    static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f, 15.0f, 0.0f };       // カプセル終点

    // モデルと武器
    int modelId_;                               // プレイヤーモデルID
    std::unique_ptr<Sword> sword_;              // 剣
    std::unique_ptr<FireAttack> fireAttack_;    // 火攻撃
    std::unique_ptr<WaterAttack> waterAttack_;  // 水攻撃
    std::unique_ptr<Glider> glider_;

    // パラメーター
    Param param_;                               // プレイヤーのパラメータ

    // 原点座標
    VECTOR originPos_;                          // 原点座標

    // コライダー用
    VECTOR localStartPos_;                      // ラインコライダー開始のローカル位置
    VECTOR localEndPos_;                        // ラインコライダー終了のローカル位置

    // HPバー関連
    float hpDisplay_;                           // 表示用のHP(遅延演出用)
    float hpDelaySpeed_;                        // HP減少遅延速度

    // 経験値関連
    int experience_;                            // 現在の経験値
    bool isLevelUpEffectPlaying_;               // レベルアップ演出中かどうか
    float levelUpEffectTimer_;                  // レベルアップ演出のタイマー

    // フラグ
    bool isGround_;                             // 地面にいるか
    bool isMoving_;                             // 移動中か
    bool movementEnabled_;                      // 移動可能か
    bool isAttacking_;                          // 攻撃中か
    bool isAttack_;                             // 攻撃アクション中か
    bool isGliding_;                            // グライド中か
    bool hasJumped_;                            // ジャンプ済みか(二段ジャンプ防止用)

    // クールタイム関連
    float attackCoolTime_;                      // 通常攻撃クールタイム
    float fireAttackCoolTime_;                  // 火攻撃クールタイム
    float waterAttackCoolTime_;                 // 水攻撃クールタイム
    float invincibleTime_;                      // 無敵時間カウンタ
    float lastHitEnemyTime_;                    // 最後に敵に当たった時間

    // 移動処理
    void ProcessMove(void);

    // ジャンプ処理
    void ProcessJump(void);

    // グライド処理
    void ProcessGlide(void);

    // アニメーション更新用関数
    void UpdateAnimation(void);

    // カプセルデバッグ描画
    void DrawCollisionCapsuleDebug(void) const;
};