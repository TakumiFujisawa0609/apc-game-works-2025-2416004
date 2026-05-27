#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <DxLib.h>

class EnemyManager;
class Tower;

/// @brief ステージ上のスポナーやタワーの動的生成・配置を管理するクラス
/// @details プレイヤーの周囲にのみオブジェクトを配置し、負荷を最適化する
class StageManager
{
public:
    /// @brief コンストラクタ
    StageManager(void);

    /// @brief デストラクタ
    ~StageManager(void);

    /// @brief モデルリソースなどの読み込み
    void Load(void);

    /// @brief 初期化処理
    /// @param enemyManager エネミーの生成を依頼するEnemyManagerへのポインタ
    void Initialize(EnemyManager* enemyManager);

    /// @brief 更新処理（スポナー・タワーの動的登録など）
    /// @param playerPos プレイヤーの現在座標
    /// @param deltaTime デルタタイム
    void Update(const VECTOR& playerPos, float deltaTime);

    /// @brief 標準の描画処理
    void Draw(void) const;

    /// @brief カリングを考慮した描画処理
    /// @param cameraPos カメラの座標
    /// @param cameraDir カメラの方向ベクトル
    void Draw(const VECTOR& cameraPos, const VECTOR& cameraDir) const;

    /// @brief 解放処理
    void Release(void);

    /// @brief 生成されるエネミースポナーの基本設定を行う
    /// @param spawnRange エネミーが出現する範囲
    /// @param enemyType 出現させる敵の種類名
    /// @param activationRange プレイヤーがこの距離に入るとスポナーが稼働
    /// @param spawnInterval スポーンの間隔（秒）
    /// @param maxEnemies このスポナーから出現する最大数
    void SetSpawnerSettings(float spawnRange, const std::string& enemyType,
        float activationRange, float spawnInterval, int maxEnemies);

    /// @brief 特定のグリッド座標にタワーを生成する
    /// @param gridX グリッドX
    /// @param gridZ グリッドZ
    void CreateTower(int gridX, int gridZ);

    /// @brief プレイヤーからの距離に応じた敵レベルの動的変動を設定する
    /// @param enable 有効化フラグ
    /// @param distancePerLevel 何メートル（単位）ごとにレベルを1上げるか
    void EnableDynamicLevel(bool enable, float distancePerLevel = 1000.0f);

    /// @brief レベル計算の基準点（難易度0の地点）を設定する
    /// @param origin 基準となる座標
    void SetOriginPos(const VECTOR& origin);

    /// @brief 現在登録されている総スポナー数を取得
    /// @return スポナー数
    int GetSpawnerCount(void) const;

    /// @brief プレイヤーの周囲で稼働しているアクティブなスポナー数を取得
    /// @return アクティブなスポナー数
    int GetActiveSpawnerCount(void) const;

private:
    /// @brief スポナーの管理用情報
    struct SpawnerInfo
    {
        VECTOR gridPos;        // グリッド座標（ワールド座標）
        int spawnerIndex;      // EnemyManager内の管理用インデックス
        bool isActive;         // 稼働中かどうか
    };

    /// @brief タワーの管理用情報
    struct TowerInfo
    {
        VECTOR gridPos;        // グリッド座標（ワールド座標）
        int towerIndex;        // towers_リスト内の要素番号
    };

    // 配置ルール関連の定数
    static constexpr float SPAWNER_INTERVAL = 3000.0f;        // スポナーを配置するグリッド間隔
    static constexpr float REGISTER_RANGE = 3000.0f;         // プレイヤーの周囲どれくらいの範囲にスポナーを作るか
    static constexpr int TOWER_PLACE_GRID_INTERVAL = 1000;    // タワーを配置するグリッド間隔

    /// @brief グリッド座標(X, Z)から一意のハッシュキーを生成する
    int GetGridKey(int gridX, int gridZ) const;

    /// @brief ワールド座標をグリッド座標（整数値）に変換する
    void WorldToGrid(const VECTOR& worldPos, int& outGridX, int& outGridZ) const;

    /// @brief グリッド座標をワールド座標に変換する
    VECTOR GridToWorld(int gridX, int gridZ) const;

    /// @brief 指定座標の周辺にある有効なグリッド座標リストを取得する
    std::vector<std::pair<int, int>> GetNearbyGrids(const VECTOR& centerPos, float range) const;

    /// @brief プレイヤーの移動に合わせて周辺のスポナーを動的に追加・削除する
    void RegisterNearbySpawners(const VECTOR& playerPos);

    /// @brief スポナーを実体化し、EnemyManagerへ登録する
    void CreateSpawner(int gridX, int gridZ);

    /// @brief スポナーを破棄し、EnemyManagerから除外する
    void RemoveSpawner(int gridX, int gridZ);

    /// @brief 基準点からの距離に基づいてエネミーのレベルを算出する
    int CalculateEnemyLevelByDistance(const VECTOR& spawnPos) const;

private:
    // 外部参照・状態関連
    EnemyManager* enemyManager_;                              // エネミー管理クラスへの参照
    bool isLoaded_;                                           // リソースロード済みフラグ

    // スポナー設定関連
    float spawnRange_;                                        // 敵の出現半径
    std::string enemyType_;                                   // 出現する敵の型名
    float activationRange_;                                   // スポナーが起動するプレイヤーとの距離
    float spawnInterval_;                                     // 敵が出る間隔
    int maxEnemies_;                                          // 同時生存最大数

    // レベル・難易度関連
    bool useDynamicLevel_;                                    // 距離でレベルを変えるか
    float levelIncreaseDistance_;                             // レベルアップに必要な距離
    VECTOR originPos_;                                        // レベル計算の開始地点
    int maxEnemyLevel_;                                       // 出現するレベルの上限

    // スポナー管理関連
    std::unordered_map<int, SpawnerInfo> spawners_;          // 配置済みの全スポナー（Key: GridKey）
    std::unordered_set<int> activeSpawners_;                  // 現在稼働中のスポナーキー

    // タワー管理関連
    std::unordered_map<int, TowerInfo> placedTowers_;         // 配置済みの全タワー（Key: GridKey）
    std::vector<std::unique_ptr<Tower>> towers_;             // タワーの実体リスト
    int towerModelId_;                                        // タワーの共通モデルID

    // 最適化・更新制御関連
    VECTOR lastPlayerPos_;                                    // 距離判定用：前回の更新時のプレイヤー座標
    float updateTimer_;                                       // 更新頻度を抑えるためのタイマー

    /// @brief プレイヤーの移動に応じてタワーの自動配置・更新を行う
    void UpdateTowerPlacement(const VECTOR& playerPos);
};