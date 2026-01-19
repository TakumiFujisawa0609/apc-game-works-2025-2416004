#pragma once
#include "../../Object/UnitBase.h"

class EnemySpawner : public UnitBase
{
public:
    // コンストラクタ
    EnemySpawner(const VECTOR& position, float spawnRange, const std::string& enemyType);

    // デストラクタ
    ~EnemySpawner() = default;

    // UnitBaseのオーバーライド
    void Init(void) override;
    void Update(void) override;       
    void Draw(void) const override;
    void Release(void) override;

    // 更新処理（deltaTime版）
    void Update(float deltaTime);

    // スポーン座標をランダムに生成
    VECTOR GetRandomSpawnPos(void) const;

    // Getter
    const VECTOR& GetPosition(void) const;
    float GetSpawnRange(void) const;
    const std::string& GetEnemyType(void) const;
    float GetSpawnInterval(void) const;
    int GetMaxEnemies(void) const;
    bool IsActive(void) const;
    float GetActivationRange(void) const;
    int GetEnemyLevel(void) const;
    bool IsRequirePlayerInRange(void) const;

    // Setter
    void SetSpawnInterval(float interval);
    void SetMaxEnemies(int max);
    void SetActive(bool active);
    void SetCurrentEnemyCount(int count);
    void SetActivationRange(float range);
    void SetRequirePlayerInRange(bool require);
    void SetEnemyLevel(int level);
    void SetModelId(int modelId);

    // スポーン可能かチェック
    bool CanSpawn(const VECTOR& playerPos) const;

    // スポーンタイマーをリセット
    void ResetSpawnTimer(void);

    // プレイヤーが範囲内にいるかチェック
    bool IsPlayerInRange(const VECTOR& playerPos) const;

    void CalcGravityPow(void) override;

protected:
    // 衝突イベントのオーバーライド
    void OnCollisionEnter(const CollisionInfo& info) override;
    void OnCollisionStay(const CollisionInfo& info) override;

    // ★【追加】コライダー初期化のオーバーライド
    void InitCollider(void) override;

private:
    // コライダー関連
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f };
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

    // スポナーの座標
    VECTOR position_;

    // スポーン範囲（半径）
    float spawnRange_;

    // スポーンするエネミーのタイプ
    std::string enemyType_;

    // スポーン間隔（秒）
    float spawnInterval_;

    // スポーンタイマー
    float spawnTimer_;

    // スポナーモデル
    int modelId_;

    // このスポナーから生成できる最大数
    int maxEnemies_;

    // 現在このスポナーから生成されているエネミー数
    int currentEnemyCount_;

    // スポナーが有効かどうか
    bool isActive_;

    // プレイヤーが入る必要がある範囲（0の場合は無制限）
    float activationRange_;

    // プレイヤーが範囲内にいる必要があるか
    bool requirePlayerInRange_;

    // スポーンするエネミーのレベル
    int enemyLevel_;
};