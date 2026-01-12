#include "CollisionController.h"
#include "../../Object/UnitBase.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Collider/ColliderModel.h"
#include "../../Utility/Utility.h"
#include "../Generic/SceneManager.h"

CollisionController* CollisionController::instance_ = nullptr;

void CollisionController::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new CollisionController();
        instance_->Init();
    }
}

CollisionController& CollisionController::GetInstance(void)
{
    return *instance_;
}

void CollisionController::Destroy(void)
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

void CollisionController::Init(void)
{
    actors_.clear();
    updateTimer_ = 0.0f;
}

void CollisionController::Update(void)
{
    updateTimer_ += SceneManager::GetInstance().GetDeltaTime();

    if (updateTimer_ >= UPDATE_INTERVAL)
    {
        updateTimer_ = 0.0f;
        UpdateCollisionPairs();
    }
}

// コライダペアの更新処理を分離
void CollisionController::UpdateCollisionPairs(void)
{
    // 全Actorのコライダ相手をクリア
    for (auto& actor : actors_)
    {
        actor->ClearHitCollider();
    }

    size_t n = actors_.size();
    const float CULL_DISTANCE_SQ = cullingDistance_ * cullingDistance_;


    for (size_t i = 0; i < n; ++i)
    {
        auto& actor1 = actors_[i];
        VECTOR pos1 = actor1->GetPos();
        const auto& colliders1 = actor1->GetOwnColliders();

        for (size_t j = i + 1; j < n; ++j)
        {
            auto& actor2 = actors_[j];
            VECTOR pos2 = actor2->GetPos();

            // 距離カリング
            if (enableDistanceCulling_)
            {
                float dx = pos2.x - pos1.x;
                float dz = pos2.z - pos1.z;
                float distSq = dx * dx + dz * dz;


                if (distSq > CULL_DISTANCE_SQ)
                {
                    continue;
                }
            }

            const auto& colliders2 = actor2->GetOwnColliders();

            for (const auto& pair1 : colliders1)
            {
                auto col1 = pair1.second;

                for (const auto& pair2 : colliders2)
                {
                    auto col2 = pair2.second;

                    if (CanCollide(col1->GetTag(), col2->GetTag()))
                    {
                        actor1->AddHitCollider(col2);
                        actor2->AddHitCollider(col1);

                    }
                }
            }
        }
    }

}

void CollisionController::RegisterUnit(UnitBase* actor)
{
    if (actor == nullptr) return;

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

bool CollisionController::CheckCollision(const ColliderBase* col1, const ColliderBase* col2, CollisionInfo& outInfo)
{
    if (!col1 || !col2) return false;

    auto shape1 = col1->GetShape();
    auto shape2 = col2->GetShape();

    if (shape1 == ColliderBase::SHAPE::LINE && shape2 == ColliderBase::SHAPE::MODEL)
    {
        return CheckLineVsModel(col1, col2, outInfo);
    }
    else if (shape1 == ColliderBase::SHAPE::MODEL && shape2 == ColliderBase::SHAPE::LINE)
    {
        return CheckLineVsModel(col2, col1, outInfo);
    }

    if (shape1 == ColliderBase::SHAPE::SPHERE && shape2 == ColliderBase::SHAPE::SPHERE)
    {
        return CheckSphereVsSphere(col1, col2, outInfo);
    }

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

bool CollisionController::CheckLineVsModel(const ColliderBase* lineCol, const ColliderBase* modelCol, CollisionInfo& outInfo)
{
    const ColliderLine* line = dynamic_cast<const ColliderLine*>(lineCol);
    const ColliderModel* model = dynamic_cast<const ColliderModel*>(modelCol);

    if (!line || !model) return false;

    VECTOR start = line->GetPosStart();
    VECTOR end = line->GetPosEnd();

    auto hit = MV1CollCheck_Line(
        model->GetFollow()->modelId, -1, start, end
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

bool CollisionController::CheckSphereVsSphere(const ColliderBase* sphere1, const ColliderBase* sphere2, CollisionInfo& outInfo)
{
    const ColliderSphere* s1 = dynamic_cast<const ColliderSphere*>(sphere1);
    const ColliderSphere* s2 = dynamic_cast<const ColliderSphere*>(sphere2);

    if (!s1 || !s2) return false;

    VECTOR pos1 = s1->GetPos();
    VECTOR pos2 = s2->GetPos();
    float r1 = s1->GetRadius();
    float r2 = s2->GetRadius();

    VECTOR diff = VSub(pos2, pos1);
    float distSq = VDot(diff, diff);
    float radiusSum = r1 + r2;

    if (distSq < radiusSum * radiusSum)
    {
        float dist = sqrtf(distSq);

        outInfo.myCollider = sphere1;
        outInfo.hitCollider = sphere2;
        outInfo.hitPosition = VAdd(pos1, VScale(diff, r1 / dist));
        outInfo.hitNormal = dist > 0.0001f ? VScale(diff, 1.0f / dist) : VGet(0, 1, 0);
        outInfo.penetration = radiusSum - dist;
        outInfo.isValid = true;
        return true;
    }

    return false;
}

bool CollisionController::CheckSphereVsCapsule(const ColliderBase* sphere, const ColliderBase* capsule, CollisionInfo& outInfo)
{
    const ColliderSphere* s = dynamic_cast<const ColliderSphere*>(sphere);
    const ColliderCapsule* c = dynamic_cast<const ColliderCapsule*>(capsule);

    if (!s || !c) return false;

    VECTOR sPos = s->GetPos();
    float sRadius = s->GetRadius();
    VECTOR cStart = c->GetPosStart();
    VECTOR cEnd = c->GetPosEnd();
    float cRadius = c->GetRadius();

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

    return false;
}

bool CollisionController::IsInCullingRange(const VECTOR& pos1, const VECTOR& pos2) const
{
    float dx = pos2.x - pos1.x;
    float dz = pos2.z - pos1.z;
    float distSq = dx * dx + dz * dz;

    return distSq <= (cullingDistance_ * cullingDistance_);
}