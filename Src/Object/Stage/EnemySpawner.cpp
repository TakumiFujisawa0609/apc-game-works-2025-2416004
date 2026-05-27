#include "../../Pch.h"
#include "EnemySpawner.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderModel.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderLine.h"

EnemySpawner::EnemySpawner(const VECTOR& position, float spawnRange, const std::string& enemyType)
    : UnitBase()
    , position_(position)
    , spawnRange_(spawnRange)
    , enemyType_(enemyType)
    , spawnInterval_(5.0f)
    , spawnTimer_(0.0f)
    , maxEnemies_(5)
    , currentEnemyCount_(0)
    , isActive_(true)
    , activationRange_(1000.0f)
    , requirePlayerInRange_(true)
    , enemyLevel_(1)
    , modelId_(-1)
{
    trans_.pos = position;
    trans_.scl = VGet(6.0f, 6.0f, 6.0f);
}

void EnemySpawner::Initialize(void)
{
    UnitBase::Initialize();

    trans_.Update();

    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

void EnemySpawner::InitCollider(void)
{
    // 地面判定用のラインコライダー
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::STAGE,
        &trans_,
        COL_LINE_START_LOCAL_POS,
        COL_LINE_END_LOCAL_POS
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    // プレイヤーの押し返し用
    ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &trans_);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void EnemySpawner::Update(void)
{
    if (!isActive_) return;

    UnitBase::Update();
}

void EnemySpawner::Update(float deltaTime)
{
    if (!isActive_) return;

    spawnTimer_ += deltaTime;

    Update();
}

void EnemySpawner::Draw(void) const
{
    UnitBase::Draw();

#ifdef _DEBUG
    //// レベルに応じた色（レベルが高いほど赤く）
    //int colorR = 100 + (enemyLevel_ * 15);
    //int colorG = 200 - (enemyLevel_ * 15);
    //int colorB = 0;

    //if (colorR > 255) colorR = 255;
    //if (colorG < 0) colorG = 0;

    //// スポナーの位置を表示（レベルに応じた色）
    //DrawSphere3D(position_, 20.0f, 16, GetColor(colorR, colorG, colorB), GetColor(colorR, colorG, colorB), TRUE);

    //// スポーン範囲を円で表示（黄色）
    //const int SEGMENTS = 32;
    //for (int i = 0; i < SEGMENTS; i++)
    //{
    //    float angle1 = (i * DX_TWO_PI_F) / SEGMENTS;
    //    float angle2 = ((i + 1) * DX_TWO_PI_F) / SEGMENTS;

    //    VECTOR p1 = VGet(
    //        position_.x + cosf(angle1) * spawnRange_,
    //        position_.y,
    //        position_.z + sinf(angle1) * spawnRange_
    //    );
    //    VECTOR p2 = VGet(
    //        position_.x + cosf(angle2) * spawnRange_,
    //        position_.y,
    //        position_.z + sinf(angle2) * spawnRange_
    //    );

    //    DrawLine3D(p1, p2, GetColor(255, 255, 0));
    //}

    //// プレイヤー検知範囲を円で表示（シアン）
    //if (requirePlayerInRange_ && activationRange_ > 0.0f)
    //{
    //    for (int i = 0; i < SEGMENTS; i++)
    //    {
    //        float angle1 = (i * DX_TWO_PI_F) / SEGMENTS;
    //        float angle2 = ((i + 1) * DX_TWO_PI_F) / SEGMENTS;

    //        VECTOR p1 = VGet(
    //            position_.x + cosf(angle1) * activationRange_,
    //            position_.y,
    //            position_.z + sinf(angle1) * activationRange_
    //        );
    //        VECTOR p2 = VGet(
    //            position_.x + cosf(angle2) * activationRange_,
    //            position_.y,
    //            position_.z + sinf(angle2) * activationRange_
    //        );

    //        DrawLine3D(p1, p2, GetColor(0, 255, 255));
    //    }
    //}

    //// スポナー情報を画面に表示（レベル情報を追加）
    //VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(position_, VGet(0, 50, 0)));
    //if (screenPos.z > 0.0f && screenPos.z < 1.0f)
    //{
    //    int color = isActive_ ? GetColor(0, 255, 0) : GetColor(128, 128, 128);
    //    DrawFormatString(
    //        static_cast<int>(screenPos.x) - 40,
    //        static_cast<int>(screenPos.y),
    //        color,
    //        "Spawner[%s] Lv.%d\n%d/%d %s",
    //        enemyType_.c_str(),
    //        enemyLevel_,
    //        currentEnemyCount_,
    //        maxEnemies_,
    //        requirePlayerInRange_ ? "[Range]" : ""
    //    );
    //}
#endif
}

void EnemySpawner::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    UnitBase::Release();

    // モデルの解放
    modelId_ = -1;
}

VECTOR EnemySpawner::GetRandomSpawnPos(void) const
{
    // スポナー位置を中心に、範囲内のランダムな座標を生成
    float angle = Utility::RandRangeF(0.0f, DX_TWO_PI_F);
    float distance = Utility::RandRangeF(0.0f, spawnRange_);

    float x = position_.x + cosf(angle) * distance;
    float z = position_.z + sinf(angle) * distance;

    return VGet(x, position_.y, z);
}

const VECTOR& EnemySpawner::GetPosition(void) const
{
    return position_;
}

float EnemySpawner::GetSpawnRange(void) const
{
    return spawnRange_;
}

const std::string& EnemySpawner::GetEnemyType(void) const
{
    return enemyType_;
}

float EnemySpawner::GetSpawnInterval(void) const
{
    return spawnInterval_;
}

int EnemySpawner::GetMaxEnemies(void) const
{
    return maxEnemies_;
}

bool EnemySpawner::IsActive(void) const
{
    return isActive_;
}

float EnemySpawner::GetActivationRange(void) const
{
    return activationRange_;
}

bool EnemySpawner::IsRequirePlayerInRange(void) const
{
    return requirePlayerInRange_;
}

int EnemySpawner::GetEnemyLevel(void) const
{
    return enemyLevel_;
}

void EnemySpawner::SetSpawnInterval(float interval)
{
    spawnInterval_ = interval;
}

void EnemySpawner::SetMaxEnemies(int max)
{
    maxEnemies_ = max;
}

void EnemySpawner::SetActive(bool active)
{
    isActive_ = active;
}

void EnemySpawner::SetCurrentEnemyCount(int count)
{
    currentEnemyCount_ = count;
}

void EnemySpawner::SetActivationRange(float range)
{
    activationRange_ = range;
}

void EnemySpawner::SetRequirePlayerInRange(bool require)
{
    requirePlayerInRange_ = require;
}

void EnemySpawner::SetEnemyLevel(int level)
{
    enemyLevel_ = level;
}

bool EnemySpawner::CanSpawn(const VECTOR& playerPos) const
{
    // 非アクティブな場合はスポーン不可
    if (!isActive_) { return false; }

    // 最大数に達している場合はスポーン不可
    if (currentEnemyCount_ >= maxEnemies_) return false;

    // スポーン間隔が経過していない場合はスポーン不可
    if (spawnTimer_ < spawnInterval_) return false;

    // プレイヤー範囲チェックが有効な場合
    if (requirePlayerInRange_)
    {
        // プレイヤーが範囲内にいない場合はスポーン不可
        if (!IsPlayerInRange(playerPos)) return false;
    }

    return true;
}

void EnemySpawner::ResetSpawnTimer(void)
{
    spawnTimer_ = 0.0f;
}

bool EnemySpawner::IsPlayerInRange(const VECTOR& playerPos) const
{
    // activationRange_が0以下の場合は無制限（常にtrue）
    if (activationRange_ <= 0.0f) return true;

    // プレイヤーとスポナーの距離を計算
    VECTOR diff = VSub(playerPos, position_);
    float distanceSq = VSquareSize(diff);
    float rangeSq = activationRange_ * activationRange_;

    // 範囲内かチェック
    return distanceSq <= rangeSq;
}

void EnemySpawner::CalcGravityPow(void)
{
}

void EnemySpawner::SetModelId(int modelId)
{
    modelId_ = modelId;
    trans_.modelId = modelId;
    if (modelId_ >= 0) {
        // モデルのスケールを適用
        MV1SetScale(modelId_, trans_.scl);
        MV1SetupCollInfo(modelId_, -1);
    }
    trans_.Update();
}

void EnemySpawner::OnCollisionEnter(const CollisionInfo& info)
{
}

void EnemySpawner::OnCollisionStay(const CollisionInfo& info)
{
}