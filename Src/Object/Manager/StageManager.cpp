#include "../../Pch.h"
#include "StageManager.h"
#include "EnemyManager.h"
#include "../Stage/EnemySpawner.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Stage/Tower.h"

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

StageManager::~StageManager(void)
{
}

void StageManager::Load(void)
{
    if (isLoaded_) return;

    auto& res = ResourceManager::GetInstance();

    // タワーのマスターモデルを読み込む
    towerModelId_ = res.Load(ResourceManager::SRC::MODEL_TOWER).handleId_;


    isLoaded_ = true;
}

void StageManager::Initialize(EnemyManager* enemyManager)
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

void StageManager::Update(const VECTOR& playerPos, float deltaTime)
{
    const float updateInterval = 0.2f;

    // 全タワーの更新
    for (const auto& tower : towers_)
    {
        if (tower) { tower->Update(); }
    }

    // 負荷軽減のため、一定時間ごとに周辺状況を更新
    updateTimer_ += deltaTime;

    if (updateTimer_ >= updateInterval)
    {
        updateTimer_ = 0.0f;

        // プレイヤー周辺のスポナーとタワーの配置状況を更新
        RegisterNearbySpawners(playerPos);
        UpdateTowerPlacement(playerPos);

        lastPlayerPos_ = playerPos;
    }
}

void StageManager::Draw(void) const
{
    for (const auto& tower : towers_)
    {
        tower->Draw();
    }
#ifdef _DEBUG
    //// アクティブなスポナー位置を可視化
    //for (const auto& pair : spawners_)
    //{
    //    const auto& info = pair.second;
    //    if (info.isActive)
    //    {
    //        // アクティブなスポナーはオレンジ色
    //        DrawSphere3D(info.gridPos, 30.0f, 8, GetColor(255, 200, 0), GetColor(255, 200, 0), TRUE);
    //    }
    //    else
    //    {
    //        // 非アクティブなスポナーはグレー
    //        DrawSphere3D(info.gridPos, 20.0f, 8, GetColor(128, 128, 128), GetColor(128, 128, 128), TRUE);
    //    }
    //}
#endif
}

void StageManager::Draw(const VECTOR& cameraPos, const VECTOR& cameraDir) const
{
    // カリング距離を大きくする
    const float cullDistance = 10000.0f;  // プレイヤーから10000.0f以上離れたら非表示

    // カウンタの初期化 (デバッグ情報用)
    int drawnTowers = 0;
    int culledTowers = 0;

    for (const auto& tower : towers_)
    {
        if (!tower) continue;

        VECTOR towerPos = tower->GetPos();

        // XZ平面での水平距離を計算
        VECTOR toTower = VSub(towerPos, cameraPos);
        toTower.y = 0.0f;
        float distSq = VSquareSize(toTower);
        float dist = sqrtf(distSq);

        // 距離カリング
        if (distSq > cullDistance * cullDistance)
        {
            culledTowers++;
            continue;
        }

        // 描画
        tower->Draw();
        drawnTowers++;
    }

#ifdef _DEBUG

    //// スポナー位置を可視化（カリング適用）
    //int drawnSpawners = 0;
    //int culledSpawners = 0;

    //for (const auto& pair : spawners_)
    //{
    //    const auto& info = pair.second;

    //    // プレイヤー位置からの距離をXZ平面のみで計算（高さを無視）
    //    VECTOR toSpawner = VSub(info.gridPos, cameraPos);
    //    toSpawner.y = 0.0f;  // Y軸を無視して水平距離のみ計算
    //    float distSq = VSquareSize(toSpawner);

    //    // 距離カリング
    //    if (distSq > cullDistance * cullDistance)
    //    {
    //        culledSpawners++;
    //        continue;
    //    }

    //    // 描画
    //    if (info.isActive)
    //    {
    //        // アクティブなスポナーはオレンジ色
    //        DrawSphere3D(info.gridPos, 30.0f, 8, GetColor(255, 200, 0), GetColor(255, 200, 0), TRUE);
    //    }
    //    else
    //    {
    //        // 非アクティブなスポナーはグレー
    //        DrawSphere3D(info.gridPos, 20.0f, 8, GetColor(128, 128, 128), GetColor(128, 128, 128), TRUE);
    //    }

    //    drawnSpawners++;
    //}

#endif // _DEBUG
}

void StageManager::Release(void)
{
    spawners_.clear();
    activeSpawners_.clear();
    placedTowers_.clear();
    towers_.clear();
    enemyManager_ = nullptr;
    isLoaded_ = false;
}

void StageManager::SetSpawnerSettings(float spawnRange, const std::string& enemyType,
    float activationRange, float spawnInterval, int maxEnemies)
{
    spawnRange_ = spawnRange;
    enemyType_ = enemyType;
    activationRange_ = activationRange;
    spawnInterval_ = spawnInterval;
    maxEnemies_ = maxEnemies;
}

int StageManager::GetSpawnerCount(void) const
{
    return static_cast<int>(spawners_.size());
}

int StageManager::GetActiveSpawnerCount(void) const
{
    return static_cast<int>(activeSpawners_.size());
}

void StageManager::EnableDynamicLevel(bool enable, float distancePerLevel)
{
    useDynamicLevel_ = enable;
    levelIncreaseDistance_ = distancePerLevel;

}

void StageManager::SetOriginPos(const VECTOR& origin)
{
    originPos_ = origin;

}

int StageManager::GetGridKey(int gridX, int gridZ) const
{
    // カントール対関数を使用してユニークなキーを生成
    return ((gridX + gridZ) * (gridX + gridZ + 1)) / 2 + gridZ;
}

void StageManager::WorldToGrid(const VECTOR& worldPos, int& outGridX, int& outGridZ) const
{
    outGridX = static_cast<int>(floorf(worldPos.x / SPAWNER_INTERVAL));
    outGridZ = static_cast<int>(floorf(worldPos.z / SPAWNER_INTERVAL));
}

VECTOR StageManager::GridToWorld(int gridX, int gridZ) const
{
    float worldX = (gridX * SPAWNER_INTERVAL) + (SPAWNER_INTERVAL * 0.5f);
    float worldZ = (gridZ * SPAWNER_INTERVAL) + (SPAWNER_INTERVAL * 0.5f);
    return VGet(worldX, 0.0f, worldZ);
}

std::vector<std::pair<int, int>> StageManager::GetNearbyGrids(const VECTOR& centerPos, 
    float range) const
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

            float dx_val = gridWorldPos.x - centerPos.x;
            float dz_val = gridWorldPos.z - centerPos.z;

            float distSq = (dx_val * dx_val) + (dz_val * dz_val);

            if (distSq <= range * range)
            {
                grids.push_back({ gridX, gridZ });
            }
        }
    }

    return grids;
}

void StageManager::RegisterNearbySpawners(const VECTOR& playerPos)
{
    if (!enemyManager_) { return; }

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
}

void StageManager::CreateSpawner(int gridX, int gridZ)
{
    if (!enemyManager_) { return; }

    int key = GetGridKey(gridX, gridZ);
    if (spawners_.find(key) != spawners_.end()) return;

    VECTOR worldPos = GridToWorld(gridX, gridZ);
    int enemyLevel = CalculateEnemyLevelByDistance(worldPos);

    enemyManager_->AddSpawner(worldPos, spawnRange_, enemyType_, enemyLevel);

    int spawnerIndex = enemyManager_->GetSpawnerCount() - 1;
    auto* spawner = enemyManager_->GetSpawner(spawnerIndex);

    if (spawner)
    {
        auto& res = ResourceManager::GetInstance();
        int masterModelId = res.GetHandle(ResourceManager::SRC::MODEL_ENEMYSPAWNER);
        int myModelId = MV1DuplicateModel(masterModelId);

        spawner->SetModelId(myModelId);

        spawner->SetActivationRange(activationRange_);
        spawner->SetRequirePlayerInRange(true);
        spawner->SetSpawnInterval(spawnInterval_);
        spawner->SetMaxEnemies(maxEnemies_);
        spawner->SetActive(false);

        spawner->Initialize();
    }

    // スポナー情報を記録
    SpawnerInfo info;
    info.gridPos = worldPos;
    info.spawnerIndex = spawnerIndex;
    info.isActive = false;
    spawners_[key] = info;
}

void StageManager::RemoveSpawner(int gridX, int gridZ)
{
    int key = GetGridKey(gridX, gridZ);

    auto it = spawners_.find(key);
    if (it != spawners_.end())
    {

        activeSpawners_.erase(key);
        spawners_.erase(it);
    }
}

int StageManager::CalculateEnemyLevelByDistance(const VECTOR& spawnPos) const
{
    if (!useDynamicLevel_)
    {
        return 1;
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

    // ワールド座標を計算（オフセット適用）
    // 元のグリッド座標からワールド座標を取得
    VECTOR baseWorldPos = GridToWorld(gridX, gridZ);
    const float towerOffset = 0.25f;

    // タワーをスポナーからずらす（グリッドの半分だけオフセット）
    VECTOR worldPos = VGet(
        baseWorldPos.x + (SPAWNER_INTERVAL * towerOffset), 
        baseWorldPos.y,
        baseWorldPos.z + (SPAWNER_INTERVAL * towerOffset)
    );

    // タワーを生成
    auto tower = std::make_unique<Tower>(worldPos);

    // マスターの towerModelId_ から「自分専用」をさらに複製する
    int myModelId = MV1DuplicateModel(towerModelId_);

    // 自分専用のIDを渡す
    tower->Load(myModelId);
    tower->Initialize();
    // リストに追加
    int towerIndex = static_cast<int>(towers_.size());
    towers_.push_back(std::move(tower));

    // 配置情報を記録
    TowerInfo info;
    info.gridPos = worldPos; 
    info.towerIndex = towerIndex;
    placedTowers_[key] = info;

}

void StageManager::UpdateTowerPlacement(const VECTOR& playerPos)
{
    // スポナーのREGISTER_RANGEと同じ範囲をチェック
    const float REGISTER_RANGE = 5000.0f;
    auto nearbyGrids = GetNearbyGrids(playerPos, REGISTER_RANGE);

    // タワー配置の間隔（スポナーより密に配置）
    const int TOWER_INTERVAL = 2; 

    // 周辺のグリッドをチェック
    for (const auto& grid : nearbyGrids)
    {
        int gx = grid.first;
        int gz = grid.second;

        // タワー配置の間隔チェック
        if (gx % TOWER_INTERVAL != 0 || gz % TOWER_INTERVAL != 0)
        {
            continue;
        }

        int key = GetGridKey(gx, gz);

        // 既にタワーが配置されているかチェック
        if (placedTowers_.count(key) == 0)
        {
            // タワーを生成
            CreateTower(gx, gz);
        }
    }
}
