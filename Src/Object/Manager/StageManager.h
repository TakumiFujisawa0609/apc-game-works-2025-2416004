#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <DxLib.h>

class EnemyManager;

class StageManager
{
public:
    // コンストラクタ
    StageManager(void);

    // デストラクタ
    ~StageManager(void);

    // 読み込み
    void Load(void);

    // 初期化
    void Init(EnemyManager* enemyManager);

    // 更新処理
    void Update(const VECTOR& playerPos, float deltaTime);

    // 描画処理
    void Draw(void) const;

    // 描画処理（カリング対応）
    void Draw(const VECTOR& cameraPos, const VECTOR& cameraDir) const;

    // 解放処理
    void Release(void);

    // スポナー設定
    void SetSpawnerSettings(float spawnRange, const std::string& enemyType,
        float activationRange, float spawnInterval, int maxEnemies);

    // 動的レベル設定を有効化
    void EnableDynamicLevel(bool enable, float distancePerLevel = 1000.0f);

    // 原点座標を設定（レベル計算の基準点）
    void SetOriginPos(const VECTOR& origin);

    // スポナー数を取得
    int GetSpawnerCount(void) const;

    // アクティブなスポナー数を取得
    int GetActiveSpawnerCount(void) const;

private:
    // スポナー情報
    struct SpawnerInfo
    {
        VECTOR gridPos;        // グリッド座標（ワールド座標）
        int spawnerIndex;      // EnemyManager内のスポナーインデックス
        bool isActive;         // アクティブ状態
    };

    // スポナーの間隔（グリッド単位）
    static constexpr float SPAWNER_INTERVAL = 2000.0f;

    // スポナーの登録範囲（プレイヤーからの距離）
    static constexpr float REGISTER_RANGE = 3000.0f;

    // グリッド座標からキーを生成
    int GetGridKey(int gridX, int gridZ) const;

    // ワールド座標からグリッド座標を取得
    void WorldToGrid(const VECTOR& worldPos, int& outGridX, int& outGridZ) const;

    // グリッド座標からワールド座標を取得
    VECTOR GridToWorld(int gridX, int gridZ) const;

    // 周辺のグリッド座標を取得
    std::vector<std::pair<int, int>> GetNearbyGrids(const VECTOR& centerPos, float range) const;

    // 周辺のスポナーを登録・解除
    void RegisterNearbySpawners(const VECTOR& playerPos);

    // スポナーを生成（グリッド座標指定）
    void CreateSpawner(int gridX, int gridZ);

    // スポナーを削除（グリッド座標指定）
    void RemoveSpawner(int gridX, int gridZ);

    // 距離に応じたレベル計算
    int CalculateEnemyLevelByDistance(const VECTOR& spawnPos) const;

private:
    EnemyManager* enemyManager_;                              // EnemyManagerへの参照

    bool isLoaded_;                                           // 読み込み済みフラグ

    // スポナー設定
    float spawnRange_;                                        // エネミーのスポーン範囲
    std::string enemyType_;                                   // エネミータイプ
    float activationRange_;                                   // プレイヤー検知範囲
    float spawnInterval_;                                     // スポーン間隔
    int maxEnemies_;                                          // 最大エネミー数

    // レベル関連
    bool useDynamicLevel_;                                    // 距離に応じてレベルを変動させるか
    float levelIncreaseDistance_;                             // この距離ごとにレベル+1
    VECTOR originPos_;                                        // レベル計算の基準点（原点）
    int maxEnemyLevel_;                                       // エネミーの最大レベル

    // スポナー管理
    std::unordered_map<int, SpawnerInfo> spawners_;          // 生成済みスポナー（キー: グリッドキー）
    std::unordered_set<int> activeSpawners_;                  // アクティブなスポナーのキー

    // 最適化用
    VECTOR lastPlayerPos_;                                    // 前回のプレイヤー座標
    float updateTimer_;                                       // 更新タイマー
};