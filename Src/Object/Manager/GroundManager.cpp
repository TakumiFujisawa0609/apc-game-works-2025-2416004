#include "GroundManager.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/Camera.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Utility/Utility.h"

// コンストラクタ
GroundManager::GroundManager(void)
    : baseModelId_(-1)
    , isLoaded_(false)
    , grounds_()
    , enemyPoss_(1, VECTOR{0.0f, 0.0f, 0.0f})
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

// 周辺の地面を登録
void GroundManager::RegisterNearbyGrounds(void)
{
    // すでに登録済みの地面を解除
    for (auto& g : registeredGrounds_)
    {
        if (g)
        {
            CollisionController::GetInstance().UnregisterUnit(g.get());
        }
    }
    registeredGrounds_.clear();

    auto camera = SceneManager::GetInstance().GetCamera();
    if (!camera) return;

    VECTOR cameraPos = camera->GetPos();

    for (auto& g : grounds_)
    {
        bool shouldRegister = false;

        // カメラ位置チェック
        VECTOR cameraDiff = VSub(g->GetPos(), cameraPos);
        cameraDiff.y = 0.0f;
        if (VSquareSize(cameraDiff) <= REGISTER_RANGE_SQ)
        {
            shouldRegister = true;
        }

        // プレイヤー位置チェック
        if (!shouldRegister)
        {
            VECTOR playerDiff = VSub(g->GetPos(), playerPos_);
            playerDiff.y = 0.0f;
            if (VSquareSize(playerDiff) <= REGISTER_RANGE_SQ)
            {
                shouldRegister = true;
            }
        }

        // 全ての敵の位置チェック
        if (!shouldRegister)
        {
            for (const auto& enemyPos : enemyPoss_)
            {
                VECTOR enemyDiff = VSub(g->GetPos(), enemyPos);
                enemyDiff.y = 0.0f;
                if (VSquareSize(enemyDiff) <= REGISTER_RANGE_SQ)
                {
                    shouldRegister = true;
                    break;
                }
            }
        }

        if (shouldRegister)
        {
            CollisionController::GetInstance().RegisterUnit(g.get());
            registeredGrounds_.push_back(g);
        }
    }

}

// カメラ位置に近いタイルのモデルIDと位置を取得
std::vector<std::pair<int, VECTOR>> GroundManager::GetNearbyTiles(const VECTOR& cameraPos, float range) const
{
    std::vector<std::pair<int, VECTOR>> nearbyTiles;
    nearbyTiles.reserve(9);

    float rangeSq = range * range;

    for (const auto& g : grounds_)
    {
        VECTOR diff = VSub(g->GetPos(), cameraPos);
        diff.y = 0.0f;
        float distSq = VSquareSize(diff);

        if (distSq <= rangeSq)
        {
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

    if (updateTimer >= 0.2f) // 1秒ごと
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
