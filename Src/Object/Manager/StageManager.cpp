#include "StageManager.h"
#include "EnemyManager.h"
#include "../Stage/EnemySpawner.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Stage/Tower.h"

// コンストラクタ
StageManager::StageManager(void)
    : enemyManager_(nullptr)
    , isLoaded_(false)
    , spawnRange_(500.0f)
    , enemyType_("SLIME")
    , activationRange_(800.0f)
    , spawnInterval_(5.0f)
    , maxEnemies_(5)
    , useDynamicLevel_(false)          
    , levelIncreaseDistance_(1000.0f)  
    , originPos_(Utility::VECTOR_ZERO) 
    , maxEnemyLevel_(10)               
    , lastPlayerPos_(Utility::VECTOR_ZERO)
    , updateTimer_(0.0f)
    , towerModelId_(-1)
{
}

// デストラクタ
StageManager::~StageManager(void)
{
}

// 読み込み
void StageManager::Load(void)
{
    if (isLoaded_) return;

    auto& res = ResourceManager::GetInstance();

    // タワーのマスターモデルを読み込む
    towerModelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_TOWER);


    isLoaded_ = true;
}

// 初期化
void StageManager::Init(EnemyManager* enemyManager)
{
    if (!enemyManager)
    {
        printfDx("EnemyManager is null!\n");
        return;
    }

    enemyManager_ = enemyManager;
    spawners_.clear();
    activeSpawners_.clear();
    lastPlayerPos_ = Utility::VECTOR_ZERO;
    updateTimer_ = 0.0f;
}

// 更新処理
void StageManager::Update(const VECTOR& playerPos, float deltaTime)
{
    for (const auto& tower : towers_)
    {
        if (tower) tower->Update();
    }

    // 一定間隔で周辺のスポナーを更新
    updateTimer_ += deltaTime;

    if (updateTimer_ >= 0.2f)
    {
        updateTimer_ = 0.0f;
        RegisterNearbySpawners(playerPos);
        UpdateTowerPlacement(playerPos);
        lastPlayerPos_ = playerPos;
    }
}

// 描画処理
void StageManager::Draw(void) const
{
    for (const auto& tower : towers_)
    {
        tower->Draw();
    }
#ifdef _DEBUG
    // アクティブなスポナー位置を可視化
    for (const auto& pair : spawners_)
    {
        const auto& info = pair.second;
        if (info.isActive)
        {
            // アクティブなスポナーはオレンジ色
            DrawSphere3D(info.gridPos, 30.0f, 8, GetColor(255, 200, 0), GetColor(255, 200, 0), TRUE);
        }
        else
        {
            // 非アクティブなスポナーはグレー
            DrawSphere3D(info.gridPos, 20.0f, 8, GetColor(128, 128, 128), GetColor(128, 128, 128), TRUE);
        }
    }
#endif
}

// 描画処理（カリング対応）
// StageManager.cpp の StageManager::Draw(const VECTOR& cameraPos, const VECTOR& cameraDir) const 関数全体

void StageManager::Draw(const VECTOR& cameraPos, const VECTOR& cameraDir) const
{
    // カリング設定
    const float cullDistance = 5000.0f;
    const float viewAngleCos = cosf(Utility::Deg2RadF(100.0f));

    // カウンタの初期化 (デバッグ情報用)
    int drawnTowers = 0;
    int culledTowers = 0;

    for (const auto& tower : towers_)
    {
        if (!tower) continue;

        VECTOR towerPos = tower->GetPos(); // UnitBase::GetPos()を使用
        VECTOR toTower = VSub(towerPos, cameraPos);

        float distSq = VSquareSize(toTower);

        // 距離カリング
        if (distSq > cullDistance * cullDistance)
        {
            culledTowers++;
            continue;
        }

        // 視野カリング
        if (distSq > 100.0f)
        {
            // ★【重要修正】カメラからタワーへのベクトル (toTower) を正規化する
            // 以前の VNorm(towerPos) はタワーのワールド座標を正規化しており、誤りでした。
            VECTOR toTowerNorm = VNorm(toTower);
            float dot = VDot(cameraDir, toTowerNorm);

            if (dot < viewAngleCos)
            {
                culledTowers++;
                continue;
            }
        }

        // 描画
        tower->Draw();
        drawnTowers++;

        // ----------------------------------------------------
        // ★【追加】タワー位置のデバッグ描画
        // ----------------------------------------------------
#ifdef _DEBUG
        // タワー位置に円を描画（スポナーと区別するため、緑色の球体: 半径50.0f）
        DrawSphere3D(towerPos, 50.0f, 8, GetColor(0, 200, 50), GetColor(0, 255, 0), TRUE);
#endif
        // ----------------------------------------------------
    }

#ifdef _DEBUG
    // 既存のスポナーデバッグ表示の前に、タワーのデバッグ情報を追加

    printfDx("=== Tower Debug Info ===\n");
    printfDx("Total Towers: %d, Drawn: %d, Culled: %d\n", (int)towers_.size(), drawnTowers, culledTowers);
    printfDx("------------------------\n");

    // スポナー位置を可視化（カリング適用）
    // NOTE: 元のコードで culledSpawners の初期化がこのブロックの外にあるため、
    // ここで一旦初期化し直します（または既存のスポナーコードから drawnSpawners/culledSpawners の初期化を削除）。
    int drawnSpawners = 0;
    int culledSpawners = 0;

    for (const auto& pair : spawners_)
    {
        const auto& info = pair.second;

        VECTOR toSpawner = VSub(info.gridPos, cameraPos);
        float distSq = VSquareSize(toSpawner);

        // 距離カリング
        if (distSq > cullDistance * cullDistance)
        {
            culledSpawners++;
            continue;
        }

        // 視野カリング
        if (distSq > 100.0f)
        {
            // スポナーの視野カリングは toSpawner を正規化しており、ロジックは正しい
            VECTOR toSpawnerNorm = VNorm(toSpawner);
            float dot = VDot(cameraDir, toSpawnerNorm);

            if (dot < viewAngleCos)
            {
                culledSpawners++;
                continue;
            }
        }

        // 描画
        if (info.isActive)
        {
            // アクティブなスポナーはオレンジ色
            DrawSphere3D(info.gridPos, 30.0f, 8, GetColor(255, 200, 0), GetColor(255, 200, 0), TRUE);
        }
        else
        {
            // 非アクティブなスポナーはグレー
            DrawSphere3D(info.gridPos, 20.0f, 8, GetColor(128, 128, 128), GetColor(128, 128, 128), TRUE);
        }

        drawnSpawners++;
    }

    printfDx("=== Spawner Debug Info ===\n");
    printfDx("Total Spawners: %d, Drawn: %d, Culled: %d\n", (int)spawners_.size(), drawnSpawners, culledSpawners);
    printfDx("--------------------------\n");

#endif // _DEBUG
}

// 解放処理
void StageManager::Release(void)
{
    spawners_.clear();
    activeSpawners_.clear();
    placedTowers_.clear();
    towers_.clear();
    enemyManager_ = nullptr;
    isLoaded_ = false;
}

// スポナー設定
void StageManager::SetSpawnerSettings(float spawnRange, const std::string& enemyType,
    float activationRange, float spawnInterval, int maxEnemies)
{
    spawnRange_ = spawnRange;
    enemyType_ = enemyType;
    activationRange_ = activationRange;
    spawnInterval_ = spawnInterval;
    maxEnemies_ = maxEnemies;
}

// スポナー数を取得
int StageManager::GetSpawnerCount(void) const
{
    return static_cast<int>(spawners_.size());
}

// アクティブなスポナー数を取得
int StageManager::GetActiveSpawnerCount(void) const
{
    return static_cast<int>(activeSpawners_.size());
}

// 動的レベル設定を有効化
void StageManager::EnableDynamicLevel(bool enable, float distancePerLevel)
{
    useDynamicLevel_ = enable;
    levelIncreaseDistance_ = distancePerLevel;

}

// 原点座標を設定
void StageManager::SetOriginPos(const VECTOR& origin)
{
    originPos_ = origin;

}

// グリッド座標からキーを生成
int StageManager::GetGridKey(int gridX, int gridZ) const
{
    // カントール対関数を使用してユニークなキーを生成
    return ((gridX + gridZ) * (gridX + gridZ + 1)) / 2 + gridZ;
}

// ワールド座標からグリッド座標を取得
void StageManager::WorldToGrid(const VECTOR& worldPos, int& outGridX, int& outGridZ) const
{
    outGridX = static_cast<int>(floorf(worldPos.x / SPAWNER_INTERVAL));
    outGridZ = static_cast<int>(floorf(worldPos.z / SPAWNER_INTERVAL));
}

// グリッド座標からワールド座標を取得
VECTOR StageManager::GridToWorld(int gridX, int gridZ) const
{
    float worldX = (gridX * SPAWNER_INTERVAL) + (SPAWNER_INTERVAL * 0.5f);
    float worldZ = (gridZ * SPAWNER_INTERVAL) + (SPAWNER_INTERVAL * 0.5f);
    return VGet(worldX, 0.0f, worldZ);
}

// 周辺のグリッド座標を取得
std::vector<std::pair<int, int>> StageManager::GetNearbyGrids(const VECTOR& centerPos, float range) const
{
    std::vector<std::pair<int, int>> grids;

    int centerGridX, centerGridZ;
    WorldToGrid(centerPos, centerGridX, centerGridZ);

    // 検索範囲（グリッド数）
    int searchRadius = static_cast<int>(ceilf(range / SPAWNER_INTERVAL)) + 1;

    for (int dz = -searchRadius; dz <= searchRadius; ++dz)
    {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx)
        {
            int gridX = centerGridX + dx;
            int gridZ = centerGridZ + dz;

            // 実際の距離チェック
            VECTOR gridWorldPos = GridToWorld(gridX, gridZ);
            VECTOR diff = VSub(gridWorldPos, centerPos);
            float distSq = VSquareSize(diff);

            if (distSq <= range * range)
            {
                grids.push_back({ gridX, gridZ });
            }
        }
    }

    return grids;
}

// 周辺のスポナーを登録・解除
void StageManager::RegisterNearbySpawners(const VECTOR& playerPos)
{
    if (!enemyManager_) return;

    // 登録すべきグリッド座標を取得
    auto nearbyGrids = GetNearbyGrids(playerPos, REGISTER_RANGE);

    std::unordered_set<int> shouldBeActiveKeys;
    for (const auto& grid : nearbyGrids)
    {
        int key = GetGridKey(grid.first, grid.second);
        shouldBeActiveKeys.insert(key);
    }

    // 新規作成すべきスポナー
    for (int key : shouldBeActiveKeys)
    {
        if (spawners_.find(key) == spawners_.end())
        {
            // まだ存在しないスポナーを作成
            for (const auto& grid : nearbyGrids)
            {
                int gridKey = GetGridKey(grid.first, grid.second);
                if (gridKey == key)
                {
                    CreateSpawner(grid.first, grid.second);
                    break;
                }
            }
        }
    }

    // アクティブ状態の更新
    std::unordered_set<int> toDeactivate;
    for (int key : activeSpawners_)
    {
        if (shouldBeActiveKeys.find(key) == shouldBeActiveKeys.end())
        {
            toDeactivate.insert(key);
        }
    }

    // 非アクティブ化
    for (int key : toDeactivate)
    {
        auto it = spawners_.find(key);
        if (it != spawners_.end())
        {
            auto& info = it->second;
            if (info.spawnerIndex >= 0)
            {
                auto* spawner = enemyManager_->GetSpawner(info.spawnerIndex);
                if (spawner)
                {
                    spawner->SetActive(false);
                }
            }
            info.isActive = false;
            activeSpawners_.erase(key);
        }
    }

    // アクティブ化
    for (int key : shouldBeActiveKeys)
    {
        if (activeSpawners_.find(key) == activeSpawners_.end())
        {
            auto it = spawners_.find(key);
            if (it != spawners_.end())
            {
                auto& info = it->second;
                if (info.spawnerIndex >= 0)
                {
                    auto* spawner = enemyManager_->GetSpawner(info.spawnerIndex);
                    if (spawner)
                    {
                        spawner->SetActive(true);
                    }
                }
                info.isActive = true;
                activeSpawners_.insert(key);
            }
        }
    }

#ifdef _DEBUG
#endif
}

// スポナーを生成（グリッド座標指定）
void StageManager::CreateSpawner(int gridX, int gridZ)
{
    if (!enemyManager_) return;

    int key = GetGridKey(gridX, gridZ);

    // 既に存在する場合はスキップ
    if (spawners_.find(key) != spawners_.end()) return;

    // ワールド座標を計算
    VECTOR worldPos = GridToWorld(gridX, gridZ);

    // 距離に応じたレベルを計算
    int enemyLevel = CalculateEnemyLevelByDistance(worldPos);

    // EnemyManagerにスポナーを追加（レベル指定版）
    enemyManager_->AddSpawner(worldPos, spawnRange_, enemyType_, enemyLevel);

    // 追加したスポナーの設定
    int spawnerIndex = enemyManager_->GetSpawnerCount() - 1;
    auto* spawner = enemyManager_->GetSpawner(spawnerIndex);
    if (spawner)
    {
        spawner->SetActivationRange(activationRange_);
        spawner->SetRequirePlayerInRange(true);
        spawner->SetSpawnInterval(spawnInterval_);
        spawner->SetMaxEnemies(maxEnemies_);
        spawner->SetActive(false); // 最初は非アクティブ
    }

    // スポナー情報を記録
    SpawnerInfo info;
    info.gridPos = worldPos;
    info.spawnerIndex = spawnerIndex;
    info.isActive = false;
    spawners_[key] = info;
}

// スポナーを削除（グリッド座標指定）
void StageManager::RemoveSpawner(int gridX, int gridZ)
{
    int key = GetGridKey(gridX, gridZ);

    auto it = spawners_.find(key);
    if (it != spawners_.end())
    {
        // EnemyManagerからは削除しない（パフォーマンスのため非アクティブ化のみ）
        // 完全に削除したい場合は enemyManager_->RemoveSpawner() を呼ぶ

        activeSpawners_.erase(key);
        spawners_.erase(it);
    }
}

// 距離に応じたレベル計算
int StageManager::CalculateEnemyLevelByDistance(const VECTOR& spawnPos) const
{
    if (!useDynamicLevel_)
    {
        return 1;  // 動的レベルが無効な場合はレベル1
    }

    // 原点からの距離を計算（XZ平面のみ、Y軸は無視）
    VECTOR diff = VSub(spawnPos, originPos_);
    diff.y = 0.0f;
    float distance = VSize(diff);

    // 距離に応じてレベルを増加
    int level = 1 + static_cast<int>(distance / levelIncreaseDistance_);

    // 最大レベルでクランプ
    if (level > maxEnemyLevel_)
    {
        level = maxEnemyLevel_;
    }

    return level;
}

void StageManager::CreateTower(int gridX, int gridZ)
{
    if (towerModelId_ < 0)
    {
        printfDx("Tower model not loaded!\n");
        return;
    }

    int key = GetGridKey(gridX, gridZ);

    if (placedTowers_.count(key) > 0)
    {
        return;
    }

    // ★【修正】ワールド座標を計算（オフセット適用）
    // 元のグリッド座標からワールド座標を取得
    VECTOR baseWorldPos = GridToWorld(gridX, gridZ);

    // タワーをスポナーからずらす（グリッドの半分だけオフセット）
    VECTOR worldPos = VGet(
        baseWorldPos.x + (SPAWNER_INTERVAL * 0.25f), // X方向に1/4グリッド分ずらす
        baseWorldPos.y,
        baseWorldPos.z + (SPAWNER_INTERVAL * 0.25f)  // Z方向に1/4グリッド分ずらす
    );

    // タワーを生成
    auto tower = std::make_unique<Tower>(worldPos);

    // モデルIDを設定
    tower->Load(towerModelId_);
    tower->Init();

    // リストに追加
    int towerIndex = static_cast<int>(towers_.size());
    towers_.push_back(std::move(tower));

    // 配置情報を記録
    TowerInfo info;
    info.gridPos = worldPos;  // オフセット適用後の座標を記録
    info.towerIndex = towerIndex;
    placedTowers_[key] = info;

#ifdef _DEBUG
    printfDx("Tower created at Grid(%d, %d) -> World(%.1f, %.1f, %.1f)\n",
        gridX, gridZ, worldPos.x, worldPos.y, worldPos.z);
#endif
}

void StageManager::UpdateTowerPlacement(const VECTOR& playerPos)
{
    // スポナーのREGISTER_RANGEと同じ範囲をチェック
    const float REGISTER_RANGE = 5000.0f;
    auto nearbyGrids = GetNearbyGrids(playerPos, REGISTER_RANGE);

    // タワー配置の間隔（スポナーより密に配置）
    const int TOWER_INTERVAL = 2; // 2グリッドごとに配置

    // 周辺のグリッドをチェック
    for (const auto& grid : nearbyGrids)
    {
        int gx = grid.first;
        int gz = grid.second;

        // タワー配置の間隔チェック（2グリッドごと）
        if (gx % TOWER_INTERVAL != 0 || gz % TOWER_INTERVAL != 0)
        {
            continue;
        }

        int key = GetGridKey(gx, gz);

        // 既にタワーが配置されているかチェック
        if (placedTowers_.count(key) == 0)
        {
            // タワーを生成 (CreateTower内で placedTowers_ に登録されます)
            CreateTower(gx, gz);
        }
    }
}
