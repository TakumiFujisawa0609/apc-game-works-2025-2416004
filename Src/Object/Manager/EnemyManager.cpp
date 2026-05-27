#include "../../Pch.h"
#include "EnemyManager.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Enemy/EnemySlime.h"

EnemyManager::EnemyManager(void)
{
    targetPos_ = Utility::VECTOR_ZERO;
    deathCount_ = 0;
    pendingExpRewards_.clear();
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    // スライムのモデルをリソースマネージャー経由で読み込み
    res.Load(ResourceManager::SRC::MODEL_SLIME).handleId_;
    
}

void EnemyManager::Initialize(void)
{
    enemies_.clear();
    spawners_.clear();
    enemyToSpawnerMap_.clear();
    pendingExpRewards_.clear();
}

void EnemyManager::Update(float deltaTime, const VECTOR& playerPos)
{
    // 各スポナーの出現タイマー等を更新
    for (auto& spawner : spawners_)
    {
        spawner->Update(deltaTime);
    }

    // エネミーデータの取得
    static EnemyData enemyData;
    static bool dataLoaded = false;

    if (!dataLoaded)
    {
        enemyData.LoadCSV("Data/CSV/EnemyData.csv");
        dataLoaded = true;
    }

    // スポナーからのスポーン処理
    ProcessSpawners(enemyData, playerPos);

    // 各エネミーの更新処理
    for (auto& enemy : enemies_)
    {
        // スライムの場合、ターゲット座標を設定
        if (auto slime = dynamic_cast<EnemySlime*>(enemy.get()))
        {
            slime->SetTargetPos(targetPos_);
        }

        enemy->Update();
    }

    // 死亡したエネミーを削除
    for (int i = static_cast<int>(enemies_.size()) - 1; i >= 0; i--)
    {
        if (enemies_[i]->GetHp() <= 0)
        {
            deathCount_++;

            // 経験値報酬を保存（削除前に取得）
            int expReward = enemies_[i]->GetExpReward();
            pendingExpRewards_.push_back(expReward);

            // Release()を呼んでから削除
            enemies_[i]->Release();

            // スポナーとの紐付けを確認
            auto it = enemyToSpawnerMap_.find(i);
            if (it != enemyToSpawnerMap_.end())
            {
                int spawnerIndex = it->second;
                if (spawnerIndex < static_cast<int>(spawners_.size()))
                {
                    spawners_[spawnerIndex]->SetCurrentEnemyCount(
                        CountEnemiesFromSpawner(spawnerIndex) - 1
                    );
                }
                enemyToSpawnerMap_.erase(it);
            }

            // エネミーを削除
            enemies_.erase(enemies_.begin() + i);

            // マップのインデックスを調整
            std::map<int, int> newMap;
            for (auto& pair : enemyToSpawnerMap_)
            {
                if (pair.first > i)
                {
                    newMap[pair.first - 1] = pair.second;
                }
                else if (pair.first < i)
                {
                    newMap[pair.first] = pair.second;
                }
            }
            enemyToSpawnerMap_ = newMap;
        }
    }
}

void EnemyManager::Draw(void)
{
    for (auto& enemy : enemies_)
    {
        enemy->Draw();
    }

    // スポナーの描画（デバッグ用）
    for (auto& spawner : spawners_)
    {
        spawner->Draw();
    }

#ifdef _DEBUG
    // デバッグ情報表示
    DrawFormatString(10, 100, GetColor(255, 255, 255),
        "Enemies: %d / Spawners: %d",
        static_cast<int>(enemies_.size()),
        static_cast<int>(spawners_.size())
    );
#endif
}

void EnemyManager::Draw(const VECTOR& cameraPos, const VECTOR& cameraDir)
{
    // カリング設定
    const float cullDistance = 8000.0f;                          // 描画距離制限
    const float viewAngleCos = cosf(Utility::Deg2RadF(100.0f));  // 視野角100度

    int drawnEnemies = 0;
    int culledEnemies = 0;

    // エネミーの描画（カリング適用）
    for (auto& enemy : enemies_)
    {
        VECTOR enemyPos = enemy->GetPos();
        VECTOR toEnemy = VSub(enemyPos, cameraPos);

        float distSq = VSquareSize(toEnemy);

        // 距離カリング
        if (distSq > cullDistance * cullDistance)
        {
            culledEnemies++;
            continue;
        }

        // 視野カリング（カメラ後方なら描画しない）
        if (distSq > 100.0f)
        {
            VECTOR toEnemyNorm = VNorm(toEnemy);
            float dot = VDot(cameraDir, toEnemyNorm);

            if (dot < viewAngleCos)
            {
                culledEnemies++;
                continue;
            }
        }

        // 描画
        enemy->Draw();
        drawnEnemies++;
    }

    int drawnSpawners = 0;
    int culledSpawners = 0;

    for (auto& spawner : spawners_)
    {
        if (!spawner) continue;

        VECTOR spawnerPos = spawner->GetPosition();
        VECTOR toSpawner = VSub(spawnerPos, cameraPos);

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
            VECTOR toSpawnerNorm = VNorm(toSpawner);
            float dot = VDot(cameraDir, toSpawnerNorm);

            if (dot < viewAngleCos)
            {
                culledSpawners++;
                continue;
            }
        }

        // 描画
        spawner->Draw();
        drawnSpawners++;
    }

    // スポナーの描画（カリング適用・デバッグのみ）
#ifdef _DEBUG
 

    // カリング情報表示
    DrawFormatString(10, 100, GetColor(255, 255, 255),
        "Enemies: %d (Drawn: %d, Culled: %d)",
        static_cast<int>(enemies_.size()), drawnEnemies, culledEnemies);
    DrawFormatString(10, 120, GetColor(255, 255, 255),
        "Spawners: %d (Drawn: %d, Culled: %d)",
        static_cast<int>(spawners_.size()), drawnSpawners, culledSpawners);
#endif
}

void EnemyManager::Release(void)
{
    enemies_.clear();
    spawners_.clear();
    enemyToSpawnerMap_.clear();
    pendingExpRewards_.clear();
}

void EnemyManager::AddSpawner(const VECTOR& position, float spawnRange, const std::string& enemyType, int level)
{
    // 1. 新しいスポナーオブジェクトを生成
    auto newSpawner = std::make_unique<EnemySpawner>(position, spawnRange, enemyType);

    int modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_ENEMYSPAWNER);
    newSpawner->SetModelId(modelId); 

    // 3. レベルを設定
    newSpawner->SetEnemyLevel(level);

    // 4. リストに追加
    spawners_.push_back(std::move(newSpawner));
}

void EnemyManager::RemoveSpawner(int index)
{
    if (index >= 0 && index < static_cast<int>(spawners_.size()))
    {
        spawners_.erase(spawners_.begin() + index);

        // 紐付けマップの更新
        std::map<int, int> newMap;
        for (auto& pair : enemyToSpawnerMap_)
        {
            if (pair.second < index)
            {
                newMap[pair.first] = pair.second;
            }
            else if (pair.second > index)
            {
                newMap[pair.first] = pair.second - 1;
            }
        }
        enemyToSpawnerMap_ = newMap;
    }
}


void EnemyManager::ClearSpawners(void)
{
    spawners_.clear();
    enemyToSpawnerMap_.clear();
}

EnemySpawner* EnemyManager::GetSpawner(int index)
{
    if (index >= 0 && index < static_cast<int>(spawners_.size()))
    {
        return spawners_[index].get();
    }
    return nullptr;
}

void EnemyManager::SetTargetPos(const VECTOR& pos)
{
    targetPos_ = pos;
}

VECTOR EnemyManager::GetEnemyPos(void) const
{
    if (enemies_.empty())
    {
        return Utility::VECTOR_ZERO;
    }

    VECTOR center = Utility::VECTOR_ZERO;
    for (const auto& enemy : enemies_)
    {
        center = VAdd(center, enemy->GetPos());
    }
    center = VScale(center, 1.0f / static_cast<float>(enemies_.size()));
    return center;
}

std::vector<VECTOR> EnemyManager::GetAllEnemyPositions(void) const
{
    std::vector<VECTOR> positions;
    positions.reserve(enemies_.size());
    for (const auto& enemy : enemies_)
    {
        positions.push_back(enemy->GetPos());
    }
    return positions;
}

int EnemyManager::GetEnemyCount(void) const
{
    return static_cast<int>(enemies_.size());
}

int EnemyManager::GetSpawnerCount(void) const
{
    return static_cast<int>(spawners_.size());
}

std::shared_ptr<EnemyBase> EnemyManager::CreateEnemy(const EnemyInfo& info)
{
    auto& res = ResourceManager::GetInstance();

    if (info.type == "SLIME")
    {
        auto slime = std::make_shared<EnemySlime>();

        // スライムモデルロード
        int modelId = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_SLIME);

        // モデルIDを渡す
        slime->Load(modelId);

        // 初期データを適用
        slime->ApplyData(info);
        return slime;
    }

    // 対応するタイプがなければnullptrを返す
    return nullptr;
}

void EnemyManager::ProcessSpawners(const EnemyData& data, const VECTOR& playerPos)
{
    for (int i = 0; i < static_cast<int>(spawners_.size()); i++)
    {
        auto& spawner = spawners_[i];

        // スポーン可能かチェック
        if (!spawner->CanSpawn(playerPos)) continue;

        // エネミーデータを取得
        const EnemyInfo* info = data.GetData(spawner->GetEnemyType());
        if (!info) continue;

        // スポーン座標を生成
        VECTOR spawnPos = spawner->GetRandomSpawnPos();

        // エネミーを生成
        auto enemy = CreateEnemy(*info);
        if (!enemy) continue;

        // エネミーを初期化
        enemy->Initialize(spawnPos);

        // スポナーのレベルを適用
        enemy->SetLevel(spawner->GetEnemyLevel());

        // エネミーリストに追加
        int enemyIndex = static_cast<int>(enemies_.size());
        enemies_.push_back(enemy);

        // スポナーとの紐付けを記録
        enemyToSpawnerMap_[enemyIndex] = i;

        // スポナーのカウントを更新
        spawner->SetCurrentEnemyCount(CountEnemiesFromSpawner(i));

        // スポーンタイマーをリセット
        spawner->ResetSpawnTimer();
    }
}

int EnemyManager::CountEnemiesFromSpawner(int spawnerIndex) const
{
    int count = 0;
    for (const auto& pair : enemyToSpawnerMap_)
    {
        if (pair.second == spawnerIndex)
        {
            count++;
        }
    }
    return count;
}

int EnemyManager::GetDeathCount(void) const
{
    return deathCount_;
}

void EnemyManager::ResetDeathCount(void)
{
    deathCount_ = 0;
}

std::vector<const ColliderBase*> EnemyManager::GetAllEnemyColliders(void) const
{
    std::vector<const ColliderBase*> colliders;
    colliders.reserve(enemies_.size());

    for (const auto& enemy : enemies_)
    {
        if (!enemy) continue;

        // 球体コライダを取得
        int sphereType = static_cast<int>(UnitBase::COLLIDER_TYPE::SPHERE);
        const ColliderBase* col = enemy->GetOwnCollider(sphereType);

        if (col)
        {
            colliders.emplace_back(col);
        }
        else
        {
            // 球体がなければカプセルを取得
            int capsuleType = static_cast<int>(UnitBase::COLLIDER_TYPE::CAPSULE);
            col = enemy->GetOwnCollider(capsuleType);
            if (col)
            {
                colliders.emplace_back(col);
            }
        }
    }

    return colliders;
}

std::vector<int> EnemyManager::GetAndClearExpRewards(void)
{
    std::vector<int> rewards = pendingExpRewards_;
    pendingExpRewards_.clear();
    return rewards;
}