#include "../../Pch.h"
#include "GroundManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/Camera.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Utility/Utility.h"

GroundManager::GroundManager(void)
    : baseModelId_(-1)
    , isLoaded_(false)
    , grounds_()
    , enemyPoss_(1, VECTOR{ 0.0f, 0.0f, 0.0f })
    , playerPos_(Utility::VECTOR_ZERO)
{
}

GroundManager::~GroundManager(void)
{
}

void GroundManager::Load(void)
{
    if (isLoaded_) { return; }

    auto& res = ResourceManager::GetInstance();
    // モデルリソースの読み込み
    baseModelId_ = res.Load(ResourceManager::SRC::MODEL_GROUND).handleId_;

    if (baseModelId_ == -1)
    {
        printfDx("モデル読み込み失敗\n");
        return;
    }

    isLoaded_ = true;
}

void GroundManager::Initialize(void)
{
    if (!isLoaded_)
    {
        printfDx("ステージロード失敗\n");
        return;
    }

    // 以前のタイルリストをクリア
    grounds_.clear();
    grounds_.reserve(TILE_COUNT * TILE_COUNT);

    float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

    // 二次元グリッド状に地面を配置
    for (int z = 0; z < TILE_COUNT; z++)
    {
        for (int x = 0; x < TILE_COUNT; x++)
        {
            float worldX = (x * TILE_SIZE) - halfSize;
            float worldZ = (z * TILE_SIZE) - halfSize;
            VECTOR pos = VGet(worldX, 0.0f, worldZ);

            auto ground = std::make_shared<Ground>();
            int modelId = MV1DuplicateModel(baseModelId_);

            // 初期状態では衝突判定登録を行わずに初期化
            ground->InitWithoutRegister(pos, modelId);
            grounds_.push_back(ground);
        }
    }

    // 初回の周辺登録
    RegisterNearbyGrounds();
}

int GroundManager::GetGridIndex(const VECTOR& pos) const
{
    float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

    // 座標をインデックスに変換
    int x = static_cast<int>((pos.x + halfSize) / TILE_SIZE);
    int z = static_cast<int>((pos.z + halfSize) / TILE_SIZE);

    // 配列の範囲外チェック
    if (x < 0 || x >= TILE_COUNT || z < 0 || z >= TILE_COUNT)
    {
        return -1;
    }

    return z * TILE_COUNT + x;
}

std::vector<int> GroundManager::GetNearbyGridIndices(const VECTOR& pos, float range) const
{
    std::vector<int> indices;
    float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

    int centerX = static_cast<int>((pos.x + halfSize) / TILE_SIZE);
    int centerZ = static_cast<int>((pos.z + halfSize) / TILE_SIZE);

    // 検索半径をタイル数に換算
    int searchRadius = static_cast<int>(range / TILE_SIZE) + 1;

    for (int dz = -searchRadius; dz <= searchRadius; ++dz)
    {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx)
        {
            int x = centerX + dx;
            int z = centerZ + dz;

            if (x >= 0 && x < TILE_COUNT && z >= 0 && z < TILE_COUNT)
            {
                indices.push_back(z * TILE_COUNT + x);
            }
        }
    }

    return indices;
}

void GroundManager::RegisterNearbyGrounds(void)
{
    // プレイヤー中心の検索範囲を算出
    int centerX = static_cast<int>(round(playerPos_.x / TILE_SIZE));
    int centerZ = static_cast<int>(round(playerPos_.z / TILE_SIZE));
    int searchRadius = static_cast<int>(REGISTER_RANGE / TILE_SIZE) + 1;

    // 登録すべき座標のキーを保持（マジックナンバー: 100000）
    std::unordered_set<int> shouldBeRegistered;

    for (int dz = -searchRadius; dz <= searchRadius; ++dz)
    {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx)
        {
            int gx = centerX + dx;
            int gz = centerZ + dz;

            GridPos gp = { gx, gz };
            if (activeGrounds_.find(gp) != activeGrounds_.end())
            {
                shouldBeRegistered.insert(gx * 100000 + gz);
            }
        }
    }

    // 不要になった登録の解除
    registeredGrounds_.erase(
        std::remove_if(registeredGrounds_.begin(), registeredGrounds_.end(),
            [&](const std::shared_ptr<Ground>& g)
            {
                int gx = static_cast<int>(round(g->GetPos().x / TILE_SIZE));
                int gz = static_cast<int>(round(g->GetPos().z / TILE_SIZE));
                int key = gx * 100000 + gz;

                if (shouldBeRegistered.find(key) == shouldBeRegistered.end())
                {
                    CollisionController::GetInstance().UnregisterUnit(g.get());
                    return true;
                }
                return false;
            }),
        registeredGrounds_.end()
    );

    // 現在の登録状況を把握
    std::unordered_set<int> currentKeys;
    for (const auto& g : registeredGrounds_)
    {
        int gx = static_cast<int>(round(g->GetPos().x / TILE_SIZE));
        int gz = static_cast<int>(round(g->GetPos().z / TILE_SIZE));
        currentKeys.insert(gx * 100000 + gz);
    }

    // 新規登録
    for (int dz = -searchRadius; dz <= searchRadius; ++dz)
    {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx)
        {
            int gx = centerX + dx;
            int gz = centerZ + dz;
            int key = gx * 100000 + gz;

            if (shouldBeRegistered.find(key) == shouldBeRegistered.end()) { continue; }
            if (currentKeys.find(key) != currentKeys.end()) { continue; }

            GridPos gp = { gx, gz };
            auto it = activeGrounds_.find(gp);
            if (it == activeGrounds_.end()) { continue; }

            CollisionController::GetInstance().RegisterUnit(it->second.get());
            registeredGrounds_.push_back(it->second);
        }
    }
}

void GroundManager::Update(void)
{
    auto camera = SceneManager::GetInstance().GetCamera();
    if (!camera) { return; }

    VECTOR camPos = camera->GetPos();
    VECTOR camDir = camera->GetFrontVector();

    // 生成すべき範囲の算出
    int centerX = static_cast<int>(round(camPos.x / TILE_SIZE));
    int centerZ = static_cast<int>(round(camPos.z / TILE_SIZE));
    int radius = static_cast<int>(SPAWN_RANGE / TILE_SIZE);

    std::set<GridPos> requiredIndices;

    for (int z = centerZ - radius; z <= centerZ + radius; ++z)
    {
        for (int x = centerX - radius; x <= centerX + radius; ++x) 
        {
            VECTOR tilePos = VGet(x * TILE_SIZE, 0.0f, z * TILE_SIZE);
            VECTOR toTile = VSub(tilePos, camPos);

            // 距離による動的生成判定
            if (VSize(toTile) > SPAWN_RANGE) { continue; }

            // 背後にあるタイルは生成しない
            float dot = VDot(VNorm(toTile), camDir);
            const float viewDot = 0.2f;

            if (dot < viewDot) { continue; }

            requiredIndices.insert({ x, z });
        }
    }

    // 範囲外タイルの削除
    for (auto it = activeGrounds_.begin(); it != activeGrounds_.end(); )
    {
        if (requiredIndices.find(it->first) == requiredIndices.end()) 
        {
            CollisionController::GetInstance().UnregisterUnit(it->second.get());
            MV1DeleteModel(it->second->GetTransform().modelId);
            it->second->Release();
            it = activeGrounds_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // 新規タイルの生成
    for (auto& pos : requiredIndices)
    {
        if (activeGrounds_.find(pos) == activeGrounds_.end())
        {
            auto ground = std::make_shared<Ground>();
            int modelId = MV1DuplicateModel(baseModelId_);

            VECTOR worldPos = VGet(pos.x * TILE_SIZE, 0.0f, pos.z * TILE_SIZE);
            ground->InitWithoutRegister(worldPos, modelId);

            CollisionController::GetInstance().RegisterUnit(ground.get());
            activeGrounds_[pos] = ground;
        }
    }

    RegisterNearbyGrounds();
}

void GroundManager::Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir)
{
    // アクティブなタイルのみループして描画
    for (auto& pair : activeGrounds_)
    {
        auto& g = pair.second;
        if (!g) { continue; }
        g->Draw();
    }

#ifdef _DEBUG
    // デバッグ情報表示（座標や登録数など）
    DrawFormatString(10, 260, GetColor(255, 255, 0), "PlayerPos: (%.0f, %.0f)", playerPos_.x, playerPos_.z);
    DrawFormatString(10, 280, GetColor(255, 255, 0), "Registered: %d", registeredGrounds_.size());
    DrawFormatString(10, 300, GetColor(255, 255, 0), "Active: %d", activeGrounds_.size());
#endif
}

void GroundManager::Release(void)
{
    // 衝突判定登録の解除
    for (auto& g : registeredGrounds_)
    {
        if (g) { CollisionController::GetInstance().UnregisterUnit(g.get()); }
    }
    registeredGrounds_.clear();

    // アクティブタイルの解放
    for (auto& pair : activeGrounds_)
    {
        auto& g = pair.second;
        if (g)
        {
            if (g->GetTransform().modelId != -1) { MV1DeleteModel(g->GetTransform().modelId); }
            g->Release();
        }
    }
    activeGrounds_.clear();

    // 予備リストの解放
    for (auto& g : grounds_)
    {
        if (g)
        {
            if (g->GetTransform().modelId != -1) { MV1DeleteModel(g->GetTransform().modelId); }
            g->Release();
        }
    }
    grounds_.clear();

    baseModelId_ = -1;
    isLoaded_ = false;
}

void GroundManager::SetPlayerPos(const VECTOR& pos)
{
    playerPos_ = pos;
}

void GroundManager::SetEnemyPos(const std::vector<VECTOR>& positions)
{
    enemyPoss_ = positions;
}