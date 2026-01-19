#include "GroundManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/Camera.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Utility/Utility.h"
#include <unordered_set>

// コンストラクタ
GroundManager::GroundManager(void)
    : baseModelId_(-1)
    , isLoaded_(false)
    , grounds_()
    , enemyPoss_(1, VECTOR{ 0.0f, 0.0f, 0.0f })
    , playerPos_(Utility::VECTOR_ZERO)
{
}

// デストラクタ
GroundManager::~GroundManager(void)
{
}

// 読み込み
void GroundManager::Load(void)
{
    if (isLoaded_) return;

    auto& res = ResourceManager::GetInstance();
    baseModelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_GROUND);

    if (baseModelId_ == -1)
    {
        printfDx("モデル読み込み失敗\n");
        return;
    }

    isLoaded_ = true;
}

// 初期化
void GroundManager::Init(void)
{
    if (!isLoaded_)
    {
        printfDx("ステージロード失敗\n");
        return;
    }

    grounds_.reserve(TILE_COUNT * TILE_COUNT);

    float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

    for (int z = 0; z < TILE_COUNT; z++)
    {
        for (int x = 0; x < TILE_COUNT; x++)
        {
            float worldX = (x * TILE_SIZE) - halfSize;
            float worldZ = (z * TILE_SIZE) - halfSize;
            VECTOR pos = VGet(worldX, 0.0f, worldZ);

            // Groundをshared_ptrで生成
            auto ground = std::make_shared<Ground>();

            // モデルを複製
            int modelId = MV1DuplicateModel(baseModelId_);

            // CollisionController登録なしで初期化
            ground->InitWithoutRegister(pos, modelId);

            grounds_.push_back(ground);
        }
    }

    // 周辺の地面のみ登録
    RegisterNearbyGrounds();
}

// 座標からグリッドインデックスを取得
int GroundManager::GetGridIndex(const VECTOR& pos) const
{
    float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

    int x = static_cast<int>((pos.x + halfSize) / TILE_SIZE);
    int z = static_cast<int>((pos.z + halfSize) / TILE_SIZE);

    // 範囲外チェック
    if (x < 0 || x >= TILE_COUNT || z < 0 || z >= TILE_COUNT)
    {
        return -1;
    }

    return z * TILE_COUNT + x;
}

// 周辺のグリッド範囲を取得
std::vector<int> GroundManager::GetNearbyGridIndices(const VECTOR& pos, float range) const
{
    std::vector<int> indices;

    float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

    // 中心のグリッド座標
    int centerX = static_cast<int>((pos.x + halfSize) / TILE_SIZE);
    int centerZ = static_cast<int>((pos.z + halfSize) / TILE_SIZE);

    // 検索範囲（タイル数）
    int searchRadius = static_cast<int>(range / TILE_SIZE) + 1;

    // 周辺グリッドをチェック
    for (int dz = -searchRadius; dz <= searchRadius; ++dz)
    {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx)
        {
            int x = centerX + dx;
            int z = centerZ + dz;

            // 範囲内チェック
            if (x >= 0 && x < TILE_COUNT && z >= 0 && z < TILE_COUNT)
            {
                int index = z * TILE_COUNT + x;
                indices.push_back(index);
            }
        }
    }

    return indices;
}

// 周辺の地面を登録
void GroundManager::RegisterNearbyGrounds(void)
{
    auto camera = SceneManager::GetInstance().GetCamera();
    if (!camera) return;

    VECTOR cameraPos = camera->GetPos();

    // 登録すべき地面のインデックスセット
    std::unordered_set<int> shouldBeRegisteredIndices;

    // カメラ周辺
    auto cameraIndices = GetNearbyGridIndices(cameraPos, REGISTER_RANGE);
    shouldBeRegisteredIndices.insert(cameraIndices.begin(), cameraIndices.end());

    // プレイヤー周辺
    auto playerIndices = GetNearbyGridIndices(playerPos_, REGISTER_RANGE);
    shouldBeRegisteredIndices.insert(playerIndices.begin(), playerIndices.end());

    // 敵周辺
    for (const auto& enemyPos : enemyPoss_)
    {
        auto enemyIndices = GetNearbyGridIndices(enemyPos, REGISTER_RANGE);
        shouldBeRegisteredIndices.insert(enemyIndices.begin(), enemyIndices.end());
    }

    // 現在登録されている地面のインデックスを取得
    std::unordered_set<int> currentRegisteredIndices;
    for (const auto& g : registeredGrounds_)
    {
        int index = GetGridIndex(g->GetPos());
        if (index >= 0)
        {
            currentRegisteredIndices.insert(index);
        }
    }

    // 登録解除すべき地面
    std::vector<int> toUnregister;
    for (int index : currentRegisteredIndices)
    {
        if (shouldBeRegisteredIndices.find(index) == shouldBeRegisteredIndices.end())
        {
            toUnregister.push_back(index);
        }
    }

    // 新規登録すべき地面
    std::vector<int> toRegister;
    for (int index : shouldBeRegisteredIndices)
    {
        if (currentRegisteredIndices.find(index) == currentRegisteredIndices.end())
        {
            toRegister.push_back(index);
        }
    }

    // 登録解除
    for (int index : toUnregister)
    {
        if (index >= 0 && index < static_cast<int>(grounds_.size()))
        {
            auto& ground = grounds_[index];
            CollisionController::GetInstance().UnregisterUnit(ground.get());

            // registeredGrounds_から削除
            registeredGrounds_.erase(
                std::remove_if(registeredGrounds_.begin(), registeredGrounds_.end(),
                    [&ground](const std::shared_ptr<Ground>& g) { return g == ground; }),
                registeredGrounds_.end()
            );

        }
    }

    // 新規登録
    for (int index : toRegister)
    {
        if (index >= 0 && index < static_cast<int>(grounds_.size()))
        {
            auto& ground = grounds_[index];
            CollisionController::GetInstance().RegisterUnit(ground.get());
            registeredGrounds_.push_back(ground);

        }
    }

#ifdef _DEBUG
    DrawFormatString(10, 220, GetColor(0, 255, 255), "Should register: %d", shouldBeRegisteredIndices.size());
    DrawFormatString(10, 240, GetColor(255, 0, 255), "Unregistered: %d, Registered: %d",
        toUnregister.size(), toRegister.size());
#endif
}

// カメラ位置に近いタイルのモデルIDと位置を取得
std::vector<std::pair<int, VECTOR>> GroundManager::GetNearbyTiles(const VECTOR& cameraPos, float range) const
{
    std::vector<std::pair<int, VECTOR>> nearbyTiles;
    nearbyTiles.reserve(9);

    auto indices = GetNearbyGridIndices(cameraPos, range);

    for (int index : indices)
    {
        if (index >= 0 && index < static_cast<int>(grounds_.size()))
        {
            const auto& g = grounds_[index];
            nearbyTiles.push_back({ g->GetTransform().modelId, g->GetPos() });
        }
    }

    return nearbyTiles;
}

void GroundManager::Update(void) {
    auto camera = SceneManager::GetInstance().GetCamera();
    if (!camera) return;

    VECTOR camPos = camera->GetPos();
    VECTOR camDir = camera->GetFrontVec(); // カメラの注視方向

    // 1. プレイヤーやカメラの周囲、一定範囲のグリッドを算出
    int centerX = static_cast<int>(round(camPos.x / TILE_SIZE));
    int centerZ = static_cast<int>(round(camPos.z / TILE_SIZE));
    int radius = static_cast<int>(SPAWN_RANGE / TILE_SIZE);

    std::set<GridPos> requiredIndices;

    for (int z = centerZ - radius; z <= centerZ + radius; ++z) {
        for (int x = centerX - radius; x <= centerX + radius; ++x) {
            VECTOR tilePos = VGet(x * TILE_SIZE, 0.0f, z * TILE_SIZE);
            VECTOR toTile = VSub(tilePos, camPos);

            // 距離チェック
            if (VSize(toTile) > SPAWN_RANGE) continue;

            // 前方判定（簡易的なカリング：真後ろにあるものは生成しない）
            float dot = VDot(VNorm(toTile), camDir);
            if (dot < -0.2f) continue;

            requiredIndices.insert({ x, z });
        }
    }

    // 2. 不要なタイルを削除（範囲外になったもの）
    for (auto it = activeGrounds_.begin(); it != activeGrounds_.end(); ) {
        if (requiredIndices.find(it->first) == requiredIndices.end()) {
            // 衝突判定から解除
            CollisionController::GetInstance().UnregisterUnit(it->second.get());
            // モデル削除と解放
            MV1DeleteModel(it->second->GetTransform().modelId);
            it->second->Release();
            it = activeGrounds_.erase(it);
        }
        else {
            ++it;
        }
    }

    // 3. 足りないタイルを生成
    for (auto& pos : requiredIndices) {
        if (activeGrounds_.find(pos) == activeGrounds_.end()) {
            auto ground = std::make_shared<Ground>();
            int modelId = MV1DuplicateModel(baseModelId_); //

            VECTOR worldPos = VGet(pos.x * TILE_SIZE, 0.0f, pos.z * TILE_SIZE);
            ground->InitWithoutRegister(worldPos, modelId);

            // 常に衝突判定が必要な範囲なら登録
            CollisionController::GetInstance().RegisterUnit(ground.get());

            activeGrounds_[pos] = ground;
        }
    }
}

void GroundManager::Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir)
{
    // 古い grounds_ ではなく、現在アクティブな activeGrounds_ を描画する
    for (auto& pair : activeGrounds_)
    {
        auto& g = pair.second;
        if (!g) continue;

        // すでに Update() の段階で生成範囲（SPAWN_RANGE）や前方判定は行っていますが、
        // 描画用の細かいカリングが必要であればここで行います。

        // 基本的には activeGrounds_ に入っているものは全て描画してOKです
        g->Draw();
    }

#ifdef _DEBUG
    // デバッグ表示：activeGrounds_ の数を表示するように変更
    DrawFormatString(10, 200, GetColor(255, 255, 255), "Active Tiles: %d", activeGrounds_.size());
    }
#endif
}

// 解放処理
void GroundManager::Release(void)
{
    // 登録済みの地面を解除
    for (auto& g : registeredGrounds_)
    {
        if (g)
        {
            CollisionController::GetInstance().UnregisterUnit(g.get());
        }
    }
    registeredGrounds_.clear();

    // 各タイルの解放
    for (auto& g : grounds_)
    {
        if (g)
        {
            // モデルを削除
            if (g->GetTransform().modelId != -1)
            {
                MV1DeleteModel(g->GetTransform().modelId);
            }

            // Groundの解放
            g->Release();
        }
    }

    grounds_.clear();

    if (baseModelId_ != -1)
    {
        MV1DeleteModel(baseModelId_);
        baseModelId_ = -1;
    }

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