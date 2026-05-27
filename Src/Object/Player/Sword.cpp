#include "../../Pch.h"
#include "Sword.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Manager/System/CollisionController.h"

Sword::Sword(void)
    : framePos_(Utility::VECTOR_ZERO)
    , isAttacking_(false)
    , playerRot_(Quaternion::Identity())
    , UnitBase()
{
}

Sword::~Sword(void)
{
}

void Sword::Load(void)
{
    trans_.modelId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_SWORD).handleId_;
    trans_.SetModel(trans_.modelId);
}

void Sword::Initialize(void)
{
    // カプセルコライダの半径
    radius_ = CAPSULE_RADIUS;


    trans_.scl = SOWRD_SCALE;

    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);

    trans_.Update();

}

void Sword::Update(void)
{
    // スケール設定
    trans_.scl = SOWRD_SCALE;

    // 手の基本位置と回転を取得
    VECTOR handPos = VGet(handMatrix_.m[3][0], handMatrix_.m[3][1], handMatrix_.m[3][2]);
    Quaternion handRot = Quaternion::GetRotation(handMatrix_);
    handRot.Normalize();

    VECTOR localOffset = POSITION_OFFSET_Y;
    VECTOR worldOffset = handRot.PosAxis(localOffset);

    trans_.pos = VAdd(handPos, worldOffset);

    trans_.quaRot = handRot;

    Quaternion adjust = Quaternion::Axis(VGet(1.0f, 0.0f, 0.0f), Utility::Deg2RadF(90.0f));
    trans_.quaRotLocal = adjust;

    UnitBase::Update();
}

void Sword::Draw(void) const
{

    // 攻撃中でなければ描画しない
    if (!isAttacking_)
    {
        return;
    }

    if (trans_.modelId != -1)
    {
        UnitBase::Draw();
    }

#ifdef _DEBUG
    //// カプセルコライダの可視化
    //auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::CAPSULE));
    //if (it != ownColliders_.end())
    //{
    //    const ColliderCapsule* capsule = dynamic_cast<const ColliderCapsule*>(it->second);
    //    if (capsule)
    //    {
    //        VECTOR start = capsule->GetLocalStartPos();
    //        VECTOR end = capsule->GetLocalEndPos();
    //        float r = capsule->GetRadius();

    //        // 攻撃中は赤、それ以外は青
    //        unsigned int color = isAttacking_ ? GetColor(255, 0, 0) : GetColor(0, 0, 255);

    //        DrawCapsule3D(start, end, r, 8, color, color, false);

    //        // 始点と終点を球で表示
    //        DrawSphere3D(start, 5.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
    //        DrawSphere3D(end, 5.0f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);

    //        DrawFormatString(50, 50, GetColor(255, 255, 255), 
    //            "Player Rot : X: % .1f Y : % .1f Z : % .1f", 
    //            Utility::Rad2DegF(trans_.quaRotLocal.x), Utility::Rad2DegF(trans_.quaRotLocal.y),
    //            Utility::Rad2DegF(trans_.quaRotLocal.z));
    //    }
    //}
#endif // _DEBUG
}

void Sword::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // UnitBaseの解放（コライダも含む）
    UnitBase::Release();
}

void Sword::SetFramePos(const VECTOR& pos)
{
    framePos_ = pos;
}

void Sword::SetPlayerRotation(const Quaternion& playerRot)
{
    // プレイヤーの回転をそのまま適用
    playerRot_ = playerRot;
}

void Sword::SetAttacking(bool attacking)
{
    isAttacking_ = attacking;

    // 攻撃中のみコライダを有効化
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::CAPSULE));
    if (it != ownColliders_.end())
    {
        it->second->SetActive(attacking);
    }
}

bool Sword::IsAttacking(void) const
{
    return isAttacking_;
}

void Sword::InitCollider(void)
{
    VECTOR localStart = CAPSULE_STATE_POS;
    VECTOR localEnd = CAPSULE_END_POS;

    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::SWORD, &trans_, localStart, localEnd, CAPSULE_RADIUS
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}