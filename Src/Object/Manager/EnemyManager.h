#pragma once
#include <vector>
#include <memory>
#include <map>
#include "../Enemy/EnemyBase.h"
#include "../Stage/EnemySpawner.h"
#include "../Enemy/EnemyData.h"

class EnemyManager
{
public:
    // コンストラクタ
    EnemyManager(void);

    // デストラクタ
    ~EnemyManager(void);

    // モデル読み込み
    void Load(void);

    // 初期化
    void Initialize(void);

    // 更新処理
    void Update(float deltaTime, const VECTOR& playerPos);

    // 描画処理
    void Draw(void);

    // 描画処理（カリング対応）
    void Draw(const VECTOR& cameraPos, const VECTOR& cameraDir);

    // 解放処理
    void Release(void);

    // スポナーを追加
    void AddSpawner(const VECTOR& position, float spawnRange, const std::string& enemyType, int level);

    // スポナーを削除
    void RemoveSpawner(int index);

    // 全スポナーをクリア
    void ClearSpawners(void);

    // スポナーを取得（設定変更用）
    EnemySpawner* GetSpawner(int index);

    // 追従対象の設定
    void SetTargetPos(const VECTOR& pos);

    // エネミーの中心座標を取得
    VECTOR GetEnemyPos(void) const;

    // 全てのエネミーの座標を取得
    std::vector<VECTOR> GetAllEnemyPositions(void) const;

    // エネミー数を取得
    int GetEnemyCount(void) const;

    // スポナー数を取得
    int GetSpawnerCount(void) const;

    // 死亡数を取得
    int GetDeathCount(void) const;

    // 死亡数をリセット
    void ResetDeathCount(void);

    std::vector<const ColliderBase*> GetAllEnemyColliders(void) const;

    //void SetPlayerColliders(const std::vector<const ColliderBase*>& colliders);

    // 保留中の経験値報酬を取得してクリア
    std::vector<int> GetAndClearExpRewards(void);

private:
    // エネミーリスト
    std::vector<std::shared_ptr<EnemyBase>> enemies_;

    // スポナーリスト
    std::vector<std::unique_ptr<EnemySpawner>> spawners_;

    // エネミーとスポナーの紐付けマップ（enemyIndex -> spawnerIndex）
    std::map<int, int> enemyToSpawnerMap_;

    // 保留中の経験値報酬
    std::vector<int> pendingExpRewards_;  

    // 追従対象座標
    VECTOR targetPos_;

    // 死亡数カウンター
    int deathCount_;

    // エネミーを生成（内部処理）
    std::shared_ptr<EnemyBase> CreateEnemy(const EnemyInfo& info);

    // スポナーからのスポーン処理
    void ProcessSpawners(const EnemyData& data, const VECTOR& playerPos);

    // 特定のスポナーに属するエネミー数をカウント
    int CountEnemiesFromSpawner(int spawnerIndex) const;
};