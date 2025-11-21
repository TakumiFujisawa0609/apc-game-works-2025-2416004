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

// 更新処理
void GroundManager::Update(void)
{
    // 一定間隔で周辺の地面を再登録
    static float updateTimer = 0.0f;
    updateTimer += SceneManager::GetInstance().GetDeltaTime();

    if (updateTimer >= 0.2f)
    {
        updateTimer = 0.0f;
        RegisterNearbyGrounds();
    }
}

// 描画処理
void GroundManager::Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir)
{
    // プレイヤーからの描画距離制限
    const float cullDistance = 10000.0f;

    // 視野80度
    const float viewAngleCos = cosf(Utility::Deg2RadF(80.0f));

    for (auto& g : grounds_)
    {
        VECTOR toGround = VSub(g->GetPos(), centerPos);

        float distSq = VSquareSize(toGround);

        // 一定距離外なら描画しない
        if (distSq > cullDistance * cullDistance) continue;

        // 視野外（カメラ後方）なら描画しない
        VECTOR toGroundCam = VNorm(VSub(g->GetPos(), cameraPos));

        float dot = VDot(cameraDir, toGroundCam);

        if (dot < viewAngleCos) continue;

        // 表示
        g->Draw();
    }

#ifdef _DEBUG
    // 登録済みの地面を赤枠で表示
    for (auto& g : registeredGrounds_)
    {
        VECTOR pos = g->GetPos();
        VECTOR corners[4] = {
            VGet(pos.x - TILE_SIZE * 0.5f, pos.y + 5.0f, pos.z - TILE_SIZE * 0.5f),
            VGet(pos.x + TILE_SIZE * 0.5f, pos.y + 5.0f, pos.z - TILE_SIZE * 0.5f),
            VGet(pos.x + TILE_SIZE * 0.5f, pos.y + 5.0f, pos.z + TILE_SIZE * 0.5f),
            VGet(pos.x - TILE_SIZE * 0.5f, pos.y + 5.0f, pos.z + TILE_SIZE * 0.5f),
        };

        DrawLine3D(corners[0], corners[1], GetColor(255, 0, 0));
        DrawLine3D(corners[1], corners[2], GetColor(255, 0, 0));
        DrawLine3D(corners[2], corners[3], GetColor(255, 0, 0));
        DrawLine3D(corners[3], corners[0], GetColor(255, 0, 0));
    }

    // デバッグ情報表示
    DrawFormatString(10, 200, GetColor(255, 255, 255), "Registered Grounds: %d", registeredGrounds_.size());
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