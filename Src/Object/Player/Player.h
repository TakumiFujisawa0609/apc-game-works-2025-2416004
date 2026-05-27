#pragma once

#include "../UnitBase.h"

class AnimationController;
class Sword;
class FireAttack;
class WaterAttack;
class Glider;

/// @brief プレイヤーキャラクターの制御・管理クラス
/// @details 移動、攻撃（通常・属性）、レベルアップ、グライド等の機能を包括する
class Player : public UnitBase
{
public:
    /// @brief プレイヤーのステータス情報を保持する構造体
    struct Param
    {
        int attack;                // 攻撃力
        int defensse;              // 防御力
        int hp;                    // 現在体力
        int maxHp;                 // 最大体力
        int stamina;               // スタミナ
        float collisionRadius;     // 衝突判定の半径
        int level;                 // 現在のレベル
        int maxLevel;              // レベル上限
        float jumpPower;           // ジャンプの初速度
    };

    /// @brief コンストラクタ
    Player(void);

    /// @brief デストラクタ
    ~Player(void) override;

    /// @brief 外部CSVファイルからパラメータを読み込む
    /// @param path CSVファイルのパス
    void LoadParamCSV(const std::string& path);

    /// @brief プレイヤーモデルや武器リソースの読み込み
    void Load(void) override;

    /// @brief 各種フラグやパラメータの初期化
    void Initialize(void) override;

    /// @brief 毎フレームの更新（入力、移動、衝突、スキル管理）
    void Update(void) override;

    /// @brief 描画処理（モデル、HPバー、スキルUI、デバッグ情報）
    void Draw(void) const override;

    /// @brief 解放処理（メモリ、ハンドルの破棄）
    void Release(void) override;

    /// @brief 入力による移動の可否を設定
    /// @param enabled 有効ならtrue
    void SetMovementEndbled(bool enabled);

    /// @brief 移動が可能か確認
    /// @return 可能ならtrue
    bool IsMovementEndbled(void) const;

    /// @brief 現在のパラメータを参照取得
    /// @return Param構造体への定数参照
    const Param& GetParam(void) const;

    /// @brief 画面上にHPバーを描画する
    void DrawHpBar(void) const;

    /// @brief ダメージを受けた際の減算処理
    /// @param damage 受けるダメージ量
    void TakeDamage(int damage);

    /// @brief 座標のポインタを取得（カメラ追従用など）
    /// @return VECTORポインタ
    VECTOR* GetPosPtr(void);

    /// @brief 攻撃アクション実行中か確認
    /// @return 実行中ならtrue
    bool IsAttacking(void) const;

    /// @brief 通常攻撃（近接）の開始
    void Attack(void);

    /// @brief 火属性スキルの発動
    void FireAttackAction(void);

    /// @brief 水属性スキルの発動
    void WaterAttackAction(void);

    /// @brief レベル計算の基準となる原点位置を設定
    /// @param pos 基準座標
    void SetOriginPos(const VECTOR& pos);

    /// @brief 設定された原点位置を取得
    /// @return VECTOR定数参照
    const VECTOR& GetOriginPos(void) const;

    /// @brief 原点からの直線距離を取得
    /// @return 距離
    float GetDistanceFromOrigin(void) const;

    /// @brief 原点からの水平距離(XZ平面)を取得
    /// @return 水平距離
    float GetDistanceFromOriginXZ(void) const;

    /// @brief レベルを1上昇させ、ステータスを更新する
    void LevelUp(void);

    /// @brief 経験値を加算し、必要に応じてレベルアップ判定を行う
    /// @param exp 加算する経験値量
    void AddExperinece(int exp);

    /// @brief 現在レベルアップが可能な状態か
    /// @return 可能なならtrue
    bool IsLevelUp(void) const;

    /// @brief 特定レベルに必要な累計経験値を計算
    /// @param level 計算対象のレベル
    /// @return 必要経験値
    int CalcRequiredExp(int level) const;

    /// @brief 現在のレベルを取得
    int GetLevel(void) const;

    /// @brief 現在保持している経験値を取得
    int GetExperinece(void) const;

    /// @brief 次のレベルまでに必要な総経験値を取得
    int GetRequireExp(void) const;

    /// @brief 画面上にレベル情報を描画する
    void DrawLevelInfo(void) const;

    /// @brief 画面上に原点からの距離UIを描画する
    void DrawDistanceUI(void) const;

    /// @brief 現在のHPを取得
    int GetHP(void);

protected:
    /// @brief 当たり判定（カプセル・ライン等）の初期化
    void InitCollider(void) override;

    /// @brief 他オブジェクトとの衝突開始時のコールバック
    /// @param info 衝突情報
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// @brief 他オブジェクトとの衝突継続時のコールバック
    /// @param info 衝突情報
    void OnCollisionStay(const CollisionInfo& info) override;

    /// @brief 地形モデルとの詳細な衝突判定
    void CollisionWithModel(void);

    /// @brief 毎フレームの衝突判定メイン処理
    void Collision(void) override;

    /// @brief 重力による垂直移動量の計算（グライド対応）
    void CalcGravityPow(void) override;

private:
    // 攻撃・スキル関連定数
    static constexpr float ATTACK_COOL_TIME_MAX = 0.5f;         // 通常攻撃の再使用待ち時間
    static constexpr float FIRE_ATTACK_COOL_TIME_MAX = 8.0f;    // 火スキルの再使用待ち時間
    static constexpr float WATER_ATTACK_COOL_TIME_MAX = 5.0f;   // 水スキルの再使用待ち時間

    // ダメージ・防御関連定数
    static constexpr float INVINCIBLE_DURATION = 1.0f;          // 被弾後の無敵持続時間
    static constexpr float HIT_COOL_TIME = 0.5f;                // 連続被弾を防止する間隔

    // 成長システム関連定数
    static constexpr int BASE_EXP = 100;                        // 初期レベルの必要経験値
    static constexpr float EXP_MULTIPLIER = 1.5f;               // レベル毎の必要経験値上昇倍率
    static constexpr int LEVEL_UP_STAT = 10;                    // レベルアップ時の能力上昇値
    static constexpr float GLIDE_EXP_INTERVAL = 1.0f;           // グライド経験値付与の間隔（秒）
    static constexpr int GLIDE_EXP_AMOUNT = 5;                  // グライド1秒あたりの獲得経験値
    static constexpr float INITIALIZE_SPEED = 5.5f;             // 初期速度

    // UI演出関連定数
    static constexpr int HP_BAR_WIDTH = 300;                    // HPバーの最大ピクセル幅
    static constexpr int HP_BAR_HEIGHT = 20;                    // HPバーの縦幅
    static constexpr int BAR_Y = 100;                           // HPバーの画面Y座標
    static constexpr float DELAY_SPEED = 2.0f;                  // ダメージ演出バーの追従速度

    // アクション性能関連定数
    static constexpr float GLIDE_FALL_SPEED = 3.0f;            // グライド中の最大降下速度
    static constexpr float GLIDE_HORIZONTAL_SPEED = 8.0f;      // グライド中の水平移動最高速度
    static constexpr float MAX_FALL_SPEED = 15.0f;             // 通常落下の最大終端速度

    // 外見・形状関連定数
    static constexpr VECTOR PLAYER_SCL = { 3.5f, 3.5f, 3.5f };  // プレイヤーの表示倍率
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f }; // 接地判定ラインの始点
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };   // 接地判定ラインの終点
    static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f, 130.0f, 0.0f };    // 全体判定カプセルの上端
    static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f, 15.0f, 0.0f };       // 全体判定カプセルの下端

    // 構成オブジェクト
    int modelId_;                               // 本体モデルの識別ID
    std::unique_ptr<Sword> sword_;              // 近接武器オブジェクト
    std::unique_ptr<FireAttack> fireAttack_;    // 火属性攻撃オブジェクト
    std::unique_ptr<WaterAttack> waterAttack_;  // 水属性攻撃オブジェクト
    std::unique_ptr<Glider> glider_;            // 滑空用装備オブジェクト

    // パラメータ・ステータス
    Param param_;                               // 現在のステータス構造体
    VECTOR originPos_;                          // 冒険の開始地点（距離計算用）
    VECTOR localStartPos_;                      // ライン判定の開始ローカル位置
    VECTOR localEndPos_;                        // ライン判定の終了ローカル位置

    // UI表示用変数
    float hpDisplay_;                           // HPバーのアニメーション用現在値
    float hpDelaySpeed_;                        // バーが追いつくまでの計算速度

    // 成長管理変数
    int experience_;                            // 累積獲得経験値
    bool isLevelUpEffectPlaying_;               // レベルアップ演出が有効か
    float levelUpEffectTimer_;                  // 演出の残り時間
    float glideExpTimer_ = 0.0f;                // グライド時間計測用

    // 状態フラグ
    bool isGround_;                             // 地面に接しているか
    bool isMoving_;                             // 移動入力があるか
    bool movementEnabled_;                      // 入力を受け付けているか
    bool isAttacking_;                          // 攻撃の「構え」中か
    bool isAttack_;                             // 攻撃の「判定発生」中か
    bool isGliding_;                            // 滑空アクション中か
    bool hasJumped_;                            // 空中ジャンプを消費したか

    // 時間管理カウンタ
    float attackCoolTime_;                      // 通常攻撃の残り待ち時間
    float fireAttackCoolTime_;                  // 火スキルの残り待ち時間
    float waterAttackCoolTime_;                 // 水スキルの残り待ち時間
    float invincibleTime_;                      // 被弾無敵の残り時間
    float lastHitEnemyTime_;                    // 敵と最後に接触した時刻

    // UIリソース
    int iconFire_;                              // 火スキルの通常アイコン
    int iconWater_;                             // 水スキルの通常アイコン
    int iconFireCD_;                            // 火スキルのクールダウン中アイコン
    int iconWaterCD_;                           // 水スキルのクールダウン中アイコン

    /// @brief 移動入力の処理
    void ProcessMove(void);      

    /// @brief ジャンプ入力の処理
    void ProcessJump(void);          

    /// @brief グライド状態の遷移処理
    void ProcessGlide(void);      

    /// @brief 状態に応じたアニメーションの切り替え
    void UpdateAnimation(void);                

    /// @brief 当たり判定の可視化
    void DrawCollisionCapsuleDebug(void) const;  

    /// @brief スキルアイコンとCDの描画
    void DrawSkillUI(void) const;                
};