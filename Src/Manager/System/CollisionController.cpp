#include "CollisionController.h"
#include "../../Object/UnitBase.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Collider/ColliderModel.h"
#include "../../Utility/Utility.h"
#include "../Generic/SceneManager.h"

// 静的インスタンスの初期化
CollisionController* CollisionController::instance_ = nullptr;

void CollisionController::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new CollisionController();
        instance_->Initialize();
    }
}

CollisionController& CollisionController::GetInstance(void)
{
    return *instance_;
}

void CollisionController:: DestroyInstance(void)
{
    delete instance_;
    instance_ = nullptr;
}

CollisionController::CollisionController(void)
    : enableDistanceCulling_(true)
    , cullingDistance_(DEFAULT_CULLING_DISTANCE)
    , updateTimer_(0.0f)
{
}

CollisionController::~CollisionController(void)
{
}

void CollisionController::Initialize(void)
{
    actors_.clear();
    updateTimer_ = 0.0f;
}

void CollisionController::Update(void)
{
    // デルタタイムを加算し、一定間隔で判定を更新する
    updateTimer_ += SceneManager::GetInstance().GetDeltaTime();

    if (updateTimer_ >= UPDATE_INTERVAL)
    {
        updateTimer_ = 0.0f;
        UpdateCollisionPairs();
    }
}

void CollisionController::UpdateCollisionPairs(void)
{
    // 全Actorの衝突リストをリセット
    for (auto& actor : actors_)
    {
        actor->ClearHitCollider();
    }

    size_t n = actors_.size();
    const float CULL_DISTANCE_SQ = cullingDistance_ * cullingDistance_;

    // 総当たり判定（iとjのペア）
    for (size_t i = 0; i < n; ++i)
    {
        auto& actorA = actors_[i];
        VECTOR posA = actorA->GetPos();
        const auto& collidersA = actorA->GetOwnColliders();

        for (size_t j = i + 1; j < n; ++j)
        {
            auto& actorB = actors_[j];
            VECTOR posB = actorB->GetPos();

            // 距離カリング：一定距離以上離れている場合は判定をスキップ
            if (enableDistanceCulling_)
            {
                float dx = posB.x - posA.x;
                float dz = posB.z - posA.z;
                float distSq = dx * dx + dz * dz;


                if (distSq > CULL_DISTANCE_SQ)
                {
                    continue;
                }
            }

            const auto& collidersB = actorB->GetOwnColliders();

            // 各Actorが持つコライダ同士の全組み合わせをチェック
            for (const auto& pairA : collidersA)
            {
                auto colA = pairA.second;

                for (const auto& pairB : collidersB)
                {
                    auto colB = pairB.second;

                    // タグによる衝突可否判定
                    if (CanCollide(colA->GetCollisionTag(), colB->GetCollisionTag()))
                    {
                        // 衝突リストに追加
                        actorA->AddHitCollider(colB);
                        actorB->AddHitCollider(colA);

                    }
                }
            }
        }
    }

}

void CollisionController::RegisterUnit(UnitBase* actor)
{
    if (actor == nullptr) return;

    // 二重登録防止
    for (const auto& a : actors_)
    {
        if (a == actor) return;
    }

    actors_.push_back(actor);

    UpdateCollisionPairs();
}

void CollisionController::UnregisterUnit(UnitBase* actor)
{
    actors_.erase(
        std::remove(actors_.begin(), actors_.end(), actor),
        actors_.end()
    );
}

void CollisionController::Clear(void)
{
    actors_.clear();
}

bool CollisionController::CheckCollision(const ColliderBase* col1, 
    const ColliderBase* col2, CollisionInfo& outInfo)
{
    if (!col1 || !col2) return false;

    auto shape1 = col1->GetShapeType();
    auto shape2 = col2->GetShapeType();

    // カプセル vs モデル
    if (shape1 == ColliderBase::SHAPE::CAPSULE && shape2 == ColliderBase::SHAPE::MODEL)
    {
        return CheckCapsuleVsModel(col1, col2, outInfo);
    }
    else if (shape1 == ColliderBase::SHAPE::MODEL && shape2 == ColliderBase::SHAPE::CAPSULE)
    {
        bool result = CheckCapsuleVsModel(col2, col1, outInfo);
        if (result)
        {
            // コライダーの順序を入れ替え
            const ColliderBase* temp = outInfo.myCollider;
            outInfo.myCollider = outInfo.hitCollider;
            outInfo.hitCollider = temp;
            outInfo.hitNormal = VScale(outInfo.hitNormal, -1.0f);
        }
        return result;
    }

    // 線分 vs モデル
    if (shape1 == ColliderBase::SHAPE::LINE && shape2 == ColliderBase::SHAPE::MODEL)
    {
        return CheckLineVsModel(col1, col2, outInfo);
    }
    else if (shape1 == ColliderBase::SHAPE::MODEL && shape2 == ColliderBase::SHAPE::LINE)
    {
        return CheckLineVsModel(col2, col1, outInfo);
    }

    // 球体 vs 球体
    if (shape1 == ColliderBase::SHAPE::SPHERE && shape2 == ColliderBase::SHAPE::SPHERE)
    {
        return CheckSphereVsSphere(col1, col2, outInfo);
    }

    // 球体 vs カプセル
    if (shape1 == ColliderBase::SHAPE::SPHERE && shape2 == ColliderBase::SHAPE::CAPSULE)
    {
        return CheckSphereVsCapsule(col1, col2, outInfo);
    }
    else if (shape1 == ColliderBase::SHAPE::CAPSULE && shape2 == ColliderBase::SHAPE::SPHERE)
    {
        return CheckSphereVsCapsule(col2, col1, outInfo);
    }

    return false;
}

bool CollisionController::CheckLineVsModel(const ColliderBase* lineCol, 
    const ColliderBase* modelCol, CollisionInfo& outInfo)
{
    const ColliderLine* line = dynamic_cast<const ColliderLine*>(lineCol);
    const ColliderModel* model = dynamic_cast<const ColliderModel*>(modelCol);

    if (!line || !model) return false;

    VECTOR start = line->GetWorldStartPos();
    VECTOR end = line->GetWorldEndPos();

    // DxLibのモデル対線分判定
    auto hit = MV1CollCheck_Line(
        model->GetFollowTarget()->modelId, -1, start, end
    );

    if (hit.HitFlag > 0)
    {
        outInfo.myCollider = lineCol;
        outInfo.hitCollider = modelCol;
        outInfo.hitPosition = hit.HitPosition;
        outInfo.hitNormal = VGet(0, 1, 0);
        outInfo.penetration = 0.0f;
        outInfo.isValid = true;
        return true;
    }

    return false;
}

bool CollisionController::CheckSphereVsSphere(const ColliderBase* sphereA, 
    const ColliderBase* sphereB, CollisionInfo& outInfo)
{
    const ColliderSphere* s1 = dynamic_cast<const ColliderSphere*>(sphereA);
    const ColliderSphere* s2 = dynamic_cast<const ColliderSphere*>(sphereB);

    if (!s1 || !s2) return false;

    VECTOR posA = s1->GetLocalPosition();
    VECTOR posB = s2->GetLocalPosition();
    float r1 = s1->GetRadius();
    float r2 = s2->GetRadius();

    VECTOR diff = VSub(posB, posA);
    float distSq = VDot(diff, diff);
    float radiusSum = r1 + r2;

    // 距離の2乗で比較して高速化
    if (distSq < radiusSum * radiusSum)
    {
        float dist = sqrtf(distSq);

        outInfo.myCollider = sphereA;
        outInfo.hitCollider = sphereB;
        outInfo.hitPosition = VAdd(posA, VScale(diff, r1 / dist));
        outInfo.hitNormal = dist > 0.0001f ? VScale(diff, 1.0f / dist) : VGet(0, 1, 0);
        outInfo.penetration = radiusSum - dist;
        outInfo.isValid = true;
        return true;
    }

    return false;
}

bool CollisionController::CheckSphereVsCapsule(const ColliderBase* sphere, 
    const ColliderBase* capsule, CollisionInfo& outInfo)
{
    const ColliderSphere* s = dynamic_cast<const ColliderSphere*>(sphere);
    const ColliderCapsule* c = dynamic_cast<const ColliderCapsule*>(capsule);

    if (!s || !c) return false;

    VECTOR sPos = s->GetWorldPosition();
    float sRadius = s->GetRadius();
    VECTOR cStart = c->GetWorldStartPos();
    VECTOR cEnd = c->GetWorldEndPos();
    float cRadius = c->GetRadius();

    // カプセルの線分上の最近点を求める
    VECTOR capVec = VSub(cEnd, cStart);
    VECTOR toSphere = VSub(sPos, cStart);
    float capLen = Utility::MagnitudeF(capVec);

    if (capLen < 0.0001f)
    {
        capVec = VGet(0, 1, 0);
        capLen = 1.0f;
    }

    VECTOR capDir = VScale(capVec, 1.0f / capLen);
    float t = VDot(toSphere, capDir);

    // 線分の範囲内にクランプ
    if (t < 0.0f) t = 0.0f;
    if (t > capLen) t = capLen;

    VECTOR nearest = VAdd(cStart, VScale(capDir, t));
    VECTOR diff = VSub(sPos, nearest);
    float dist = Utility::MagnitudeF(diff);
    float radiusSum = sRadius + cRadius;

    if (dist < radiusSum)
    {
        outInfo.myCollider = sphere;
        outInfo.hitCollider = capsule;
        outInfo.hitPosition = VAdd(nearest, VScale(diff, cRadius / (dist + 0.0001f)));
        outInfo.hitNormal = dist > 0.0001f ? VScale(diff, 1.0f / dist) : VGet(0, 1, 0);
        outInfo.penetration = radiusSum - dist;
        outInfo.isValid = true;
        return true;
    }

    return false;
}

bool CollisionController::CheckCapsuleVsModel(const ColliderBase* capsuleCol, 
    const ColliderBase* modelCol, CollisionInfo& outInfo)
{
    const auto* capsule = dynamic_cast<const ColliderCapsule*>(capsuleCol);
    const auto* model = dynamic_cast<const ColliderModel*>(modelCol);

    if (!capsule || !model) { return false; }

    int modelId = model->GetFollowTarget()->modelId;
    if (modelId < 0) { return false; }

    // 姿勢の更新
    MV1RefreshCollInfo(modelId, COLL_TARGET_ALL);

    // モデルとカプセルの衝突判定実行
    MV1_COLL_RESULT_POLY_DIM collResult = MV1CollCheck_Capsule(
        modelId, COLL_TARGET_ALL,
        capsule->GetWorldStartPos(),
        capsule->GetWorldEndPos(),
        capsule->GetRadius()
    );

    bool isHit = false;
    if (collResult.HitNum > 0)
    {
        for (int i = 0; i < collResult.HitNum; i++)
        {
            const auto& poly = collResult.Dim[i];

            // 除外設定（足元など）をチェック
            if (model->IsExcludedFrame(poly.FrameIndex)) continue;

            outInfo.myCollider = capsuleCol;
            outInfo.hitCollider = modelCol;
            outInfo.hitPosition = poly.HitPosition;
            outInfo.hitNormal = poly.Normal;

            // 貫通深度の計算
            float distToPoly = VDot(VSub(capsule->GetLocalStartPos(), poly.HitPosition), poly.Normal);
            outInfo.penetration = capsule->GetRadius() - distToPoly;

            // 微小なめり込みによるガタつき防止の補正
            if (outInfo.penetration < 0.1f) {
                outInfo.penetration = MIN_PUSH_OUT;
            }

            outInfo.isValid = true;
            isHit = true;
            break;
        }
    }

    MV1CollResultPolyDimTerminate(collResult);
    return isHit;
}

bool CollisionController::CanCollide(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const
{
    if (tagA == tagB)
    {
        if (tagA == ColliderBase::TAG::ENEMY)
        {
            return true;
        }
        return false;
    }

    if ((tagA == ColliderBase::TAG::PLAYER && tagB == ColliderBase::TAG::GROUND) ||
        (tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::PLAYER))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::PLAYER && tagB == ColliderBase::TAG::STAGE) ||
        (tagA == ColliderBase::TAG::STAGE && tagB == ColliderBase::TAG::PLAYER))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::GROUND) ||
        (tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::ENEMY))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::STAGE) ||
        (tagA == ColliderBase::TAG::STAGE && tagB == ColliderBase::TAG::ENEMY))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::PLAYER && tagB == ColliderBase::TAG::ENEMY) ||
        (tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::PLAYER))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::CAMERA && tagB == ColliderBase::TAG::GROUND) ||
        (tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::CAMERA))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::CAMERA && tagB == ColliderBase::TAG::WALL) ||
        (tagA == ColliderBase::TAG::WALL && tagB == ColliderBase::TAG::CAMERA))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::SWORD && tagB == ColliderBase::TAG::ENEMY) ||
        (tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::SWORD))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::WATER_ATTACK && tagB == ColliderBase::TAG::ENEMY) ||
        (tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::WATER_ATTACK))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::FIRE_ATTACK && tagB == ColliderBase::TAG::ENEMY) ||
        (tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::FIRE_ATTACK))
    {
        return true;
    }

    if ((tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::STAGE) ||
        (tagA == ColliderBase::TAG::STAGE && tagB == ColliderBase::TAG::GROUND))
    {
        return true;
    }

    return false;
}

bool CollisionController::IsInCullingRange(const VECTOR& posA, const VECTOR& posB) const
{
    float dx = posB.x - posA.x;
    float dz = posB.z - posA.z;
    float distSq = dx * dx + dz * dz;

    return distSq <= (cullingDistance_ * cullingDistance_);
}