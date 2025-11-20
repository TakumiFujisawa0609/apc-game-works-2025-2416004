#include "CollisionController.h"
#include "../../Object/UnitBase.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Collider/ColliderModel.h"
#include "../../Utility/Utility.h"
#include "../Generic/SceneManager.h"

CollisionController* CollisionController::instance_ = nullptr;

// シングルトンインスタンスの生成
void CollisionController::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new CollisionController();
        instance_->Init();
    }
}

// シングルトンインスタンスの取得
CollisionController& CollisionController::GetInstance(void)
{
    return *instance_;
}

// シングルトンインスタンスの削除
void CollisionController::Destroy(void)
{
    delete instance_;
    instance_ = nullptr;
}

// コンストラクタ
CollisionController::CollisionController(void)
    : enableDistanceCulling_(true)
    , cullingDistance_(DEFAULT_CULLING_DISTANCE)
    , updateTimer_(0.0f)
{
}

// デストラクタ
CollisionController::~CollisionController(void)
{
}

// 初期化
void CollisionController::Init(void)
{
    actors_.clear();
    updateTimer_ = 0.0f;
}

// 更新（全ての衝突判定を実行）
void CollisionController::Update(void)
{
    // タイマー更新
    updateTimer_ += SceneManager::GetInstance().GetDeltaTime();

    // 一定間隔でのみ衝突相手の更新
    if (updateTimer_ >= UPDATE_INTERVAL)
    {
        updateTimer_ = 0.0f;

        // 全Actorのコライダ相手をクリア
        for (auto& actor : actors_)
        {
            actor->ClearHitCollider();
        }

        // 全Actor間で衝突判定の準備
        size_t n = actors_.size();

        // 距離カリング用
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

                // 距離カリング（早期リターン）
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

                // 互いのコライダを登録
                for (const auto& pair1 : colliders1)
                {
                    auto col1 = pair1.second;

                    for (const auto& pair2 : colliders2)
                    {
                        auto col2 = pair2.second;

                        // タグで衝突可能かチェック
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
}

// ユニットの登録
void CollisionController::RegisterUnit(UnitBase* actor)
{
    if (actor == nullptr) return;

    // 重複チェック
    for (const auto& a : actors_)
    {
        if (a == actor) return;
    }

    actors_.push_back(actor);
}

// ユニットの登録解除
void CollisionController::UnregisterUnit(UnitBase* actor)
{
    actors_.erase(
        std::remove(actors_.begin(), actors_.end(), actor),
        actors_.end()
    );
}

// 全ユニットのクリア
void CollisionController::Clear(void)
{
    actors_.clear();
}

// 2つのコライダ間の衝突判定
bool CollisionController::CheckCollision(const ColliderBase* col1, const ColliderBase* col2, CollisionInfo& outInfo)
{
    if (!col1 || !col2) return false;

    auto shape1 = col1->GetShape();
    auto shape2 = col2->GetShape();

    // 線分とモデル
    if (shape1 == ColliderBase::SHAPE::LINE && shape2 == ColliderBase::SHAPE::MODEL)
    {
        return CheckLineVsModel(col1, col2, outInfo);
    }
    else if (shape1 == ColliderBase::SHAPE::MODEL && shape2 == ColliderBase::SHAPE::LINE)
    {
        return CheckLineVsModel(col2, col1, outInfo);
    }

    // 球体同士
    if (shape1 == ColliderBase::SHAPE::SPHERE && shape2 == ColliderBase::SHAPE::SPHERE)
    {
        return CheckSphereVsSphere(col1, col2, outInfo);
    }

    // 球体とカプセル
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

// 線分とモデルの衝突判定
bool CollisionController::CheckLineVsModel( const ColliderBase* lineCol, const ColliderBase* modelCol, CollisionInfo& outInfo)
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
        outInfo.hitNormal = VGet(0, 1, 0); // 仮の法線（上向き）
        outInfo.penetration = 0.0f;
        outInfo.isValid = true;
        return true;
    }

    return false;
}

// 球体同士の衝突判定
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

// 球体とカプセルの衝突判定
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

    // カプセル線分上の最近点を求める
    VECTOR capVec = VSub(cEnd, cStart);
    VECTOR toSphere = VSub(sPos, cStart);
    float capLen = Utility::MagnitudeF(capVec);

    if (capLen < 0.0001f)
    {
        // カプセルが点の場合
        capVec = VGet(0, 1, 0);
        capLen = 1.0f;
    }

    VECTOR capDir = VScale(capVec, 1.0f / capLen);
    float t = VDot(toSphere, capDir);

    // クランプ
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

// 衝突可能かどうかの判定
bool CollisionController::CanCollide(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const
{
    // 同じタグ同士は衝突しない
    if (tagA == tagB) return false;

    // プレイヤーと地面
    if ((tagA == ColliderBase::TAG::PLAYER && tagB == ColliderBase::TAG::GROUND) ||
        (tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::PLAYER))
    {
        return true;
    }

    // プレイヤーとステージ
    if ((tagA == ColliderBase::TAG::PLAYER && tagB == ColliderBase::TAG::STAGE) ||
        (tagA == ColliderBase::TAG::STAGE && tagB == ColliderBase::TAG::PLAYER))
    {
        return true;
    }

    // 敵と地面
    if ((tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::GROUND) ||
        (tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::ENEMY))
    {
        return true;
    }

    // 敵とステージ
    if ((tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::STAGE) ||
        (tagA == ColliderBase::TAG::STAGE && tagB == ColliderBase::TAG::ENEMY))
    {
        return true;
    }

    // 敵同士（球体コライダ同士で押し出し）
    if (tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::ENEMY)
    {
        return true;
    }

    // プレイヤーと敵
    if ((tagA == ColliderBase::TAG::PLAYER && tagB == ColliderBase::TAG::ENEMY) ||
        (tagA == ColliderBase::TAG::ENEMY && tagB == ColliderBase::TAG::PLAYER))
    {
        return true;
    }

    // カメラと地面
    if ((tagA == ColliderBase::TAG::CAMERA && tagB == ColliderBase::TAG::GROUND) ||
        (tagA == ColliderBase::TAG::GROUND && tagB == ColliderBase::TAG::CAMERA))
    {
        return true;
    }

    // カメラと壁
    if ((tagA == ColliderBase::TAG::CAMERA && tagB == ColliderBase::TAG::WALL) ||
        (tagA == ColliderBase::TAG::WALL && tagB == ColliderBase::TAG::CAMERA))
    {
        return true;
    }

    return false;
}

// 距離カリングのチェック
bool CollisionController::IsInCullingRange(const VECTOR& pos1, const VECTOR& pos2) const
{
    float dx = pos2.x - pos1.x;
    float dz = pos2.z - pos1.z;
    float distSq = dx * dx + dz * dz;

    return distSq <= (cullingDistance_ * cullingDistance_);
}