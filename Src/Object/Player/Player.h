#pragma once
#include "../UnitBase.h"

class Sword;
class AnimationController;

class Player : public UnitBase
{
public:
    // パラメータ構造体
    struct Param
    {
        int attack;
        int defensse;
        int hp;
        int maxHp;
        int stamina;
        float collisionRadius;
        int level;
        int maxLevel;
        float jumpPower;
    };

    // コンストラクタ
    Player(void);

    // デストラクタ
    ~Player(void);

    // CSV読み込み
    void LoadParamCSV(const std::string& path);

    // リソースの読み込み
    void Load(void);

    // 初期化
    void Init(void);

    // 更新処理
    void Update(void);

    // 描画処理
    void Draw(void) const;

    // 解放処理
    void Release(void);

    // 移動可能フラグの設定
    void SetMovementEndbled(bool enabled);

    // 移動可能フラグの取得
    bool IsMovementEndbled(void) const;

    // パラメータの取得
    const Param& GetParam(void) const;

    // ダメージ処理
    void TakeDamage(int damage);

    // 座標ポインタの取得
    VECTOR* GetPosPtr(void);

    // 攻撃中かどうか
    bool IsAttacking(void) const;

    // 原点からの移動距離を取得
    float GetDistanceFromOrigin(void) const;

    // 原点からの移動距離（XZ平面のみ）を取得
    float GetDistanceFromOriginXZ(void) const;

    // 原点座標を設定（スタート地点を変更する場合）
    void SetOriginPos(const VECTOR& pos);

    // 原点座標を取得
    const VECTOR& GetOriginPos(void) const;

    // 衝突コールバック
    void OnCollisionEnter(const CollisionInfo& info) override;
    void OnCollisionStay(const CollisionInfo& info) override;

    void Collision(void) override;

    // レベルアップ処理
    void LevelUp(void);

    // 経験値を追加
    void AddExperinece(int exp);

    // レベルを取得
    int GetLevel(void) const;

    // 現在の経験値を取得
    int GetExperinece(void) const;

    // 次のレベルに必要な経験値を取得
    int GetRequireExp(void) const;

    // レベルアップ可能か
    bool IsLevelUp(void) const;

private:

    // 定数
    static constexpr float INVINCIBLE_DURATION = 1.0f;
    static constexpr float ATTACK_COOL_TIME_MAX = 1.0f;
    static constexpr float DELAY_SPEED = 0.5f;
    static constexpr int HP_BAR_WIDTH = 400;
    static constexpr int HP_BAR_HEIGHT = 30;
    static constexpr int BAR_Y = 100;
    static constexpr VECTOR PLAYER_SCL = { 2.5f, 2.5f, 2.5f };
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 120.0f, 0.0f };
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -20.0f, 0.0f };
    static constexpr VECTOR COL_SPHERE_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

    // ヒット判定のクールタイム
    static constexpr float HIT_COOL_TIME = 0.5f;

    // カプセルコライダー用初期値座標
    static constexpr VECTOR COL_CAPSULE_START_POS = { 0.0f, 50.0f, 0.0f };

    // カプセルコライダー用終端座標
    static constexpr VECTOR COL_CAPSULE_END_POS = { 0.0f, 100.0f,0.0f };


    // 経験値テーブル
    static constexpr int LEVEL_UP_STAT = 10;       // レベルアップ時の上昇量

    static constexpr int BASE_EXP = 100;           // レベル1から2に上がるための基本経験値

    static constexpr float EXP_MULTIPLIER = 1.5f;  // 必要経験値の倍率


    // パラメータ
    Param param_;

    // カプセル初期座標
    VECTOR localStartPos_;

    // カプセル終端座標
    VECTOR localEndPos_;

    // モデルID
    int modelId_;

    // 移動可能フラグ
    bool movementEnabled_;

    // 地面フラグ
    bool isGround_;

    // 移動中フラグ
    bool isMoving_;

    // 攻撃中フラグ
    bool isAttacking_;

    // 攻撃クールタイム
    float attackCoolTime_;

    // 無敵時間
    float invincibleTime_;

    // 敵との衝突クールタイム
    float lastHitEnemyTime_;

    // HP表示用
    float hpDisplay_;
    float hpDelaySpeed_;

    // レベルシステム
    int experience_;   // 現在の経験値

    // 剣
    std::unique_ptr<Sword> sword_;

    // 原点座標（スタート地点）
    VECTOR originPos_;

    // コライダ初期化
    void InitCollider(void);

    // 移動処理
    void ProcessMove(void);

    // 攻撃処理
    void Attack(void);

    // HPバー描画
    void DrawHpBar(void) const;

    // デバッグ用コリジョンカプセル描画
    void DrawCollisionCapsuleDebug(void) const;

    // レベルアップに必要な経験値を計算
    int CalcRequiredExp(int level) const;

    // レベル情報の描画
    void DrawLevelInfo(void) const;
};