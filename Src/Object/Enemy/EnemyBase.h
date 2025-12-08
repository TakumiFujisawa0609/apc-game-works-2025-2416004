#pragma once
#include "../UnitBase.h"
#include "EnemyData.h"

// エネミーのベース
class EnemyBase : public UnitBase
{
public:
    // コンストラクタ
    EnemyBase(void);

    // デストラクタ
    ~EnemyBase(void) override = default;

    // 読み込み
    virtual void Load(int modelId);

    // 初期化
    virtual void Init(const VECTOR& startPos);

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

    // 剣からのダメージ量
    static constexpr float SWORD_DAMAGE = 35.0f;

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

    // ステージ上にいるかチェック
    bool IsOnStage(const VECTOR& pos) const;

    // 衝突判定の初期化
    void InitCollider(void) override;

    // レベル事のパラメータ計算
    void ApplyLevelParams(void);
};