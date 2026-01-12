#include "EnemySlime.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Manager/Decoration/SoundManager.h"

// コンストラクタ
EnemySlime::EnemySlime(void)
{
    targetPos_ = Utility::VECTOR_ZERO;
    viewRange_ = VIEW_RANGE;
    lostRange_ = LOST_RANGE;
    forward_ = Utility::DIR_F;
    viewAngle_ = VIEW_ANGLE;
    isChasing_ = false;
    isInView_ = false;
}

// 読み込み
void EnemySlime::Load(int modelId)
{
    trans_.modelId = modelId;
    trans_.SetModel(trans_.modelId);
}

// 初期化
void EnemySlime::Init(const VECTOR& startPos)
{
    EnemyBase::Init(startPos);
    trans_.scl = VGet(0.5f, 0.5f, 0.5f);
    UpdateColliderOffset();
}

// コライダのオフセットを更新
void EnemySlime::UpdateColliderOffset(void)
{
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
    if (it != ownColliders_.end())
    {
        ColliderSphere* sphere = dynamic_cast<ColliderSphere*>(it->second);
        if (sphere)
        {
            delete sphere;
            ownColliders_.erase(it);

            VECTOR newOffset = VGet(0.0f, 30.0f, 0.0f);
            ColliderSphere* newSphere = new ColliderSphere(
                ColliderBase::TAG::ENEMY,
                &trans_,
                newOffset,
                radius_
            );
            ownColliders_.emplace(
                static_cast<int>(COLLIDER_TYPE::SPHERE),
                newSphere
            );
        }
    }
}

// 追従対象
void EnemySlime::SetTargetPos(const VECTOR& pos)
{
    targetPos_ = pos;
}

// 更新処理
void EnemySlime::Update(void)
{
    // プレイヤーへの方向ベクトル
    VECTOR dirPlayer = VSub(targetPos_, trans_.pos);
    float distance = static_cast<float>(Utility::MagnitudeF(dirPlayer));
    VECTOR dirNorm = Utility::VNormalize(dirPlayer);

    // プレイヤーが視野内にいるか判定
    double angle = Utility::AngleDeg(forward_, dirNorm);
    bool inView = (distance <= viewRange_ && angle <= viewAngle_ * 0.5);

    // 状態更新
    if (inView)
    {
        isChasing_ = true;
    }
    else if (distance > lostRange_)
    {
        isChasing_ = false;
    }

    // 移動処理（状態異常による速度補正を適用）
    if (isChasing_)
    {
        // 基本の移動速度に状態異常の倍率を適用
        float speedMultiplier = GetSpeedMultiplier();
        movePow_ = VScale(dirNorm, moveSpeed_ * speedMultiplier);
    }
    else
    {
        movePow_ = Utility::VECTOR_ZERO;
    }

    // 正面方向更新
    if (!Utility::EqualsVZero(movePow_))
    {
        forward_ = Utility::VNormalize(movePow_);
    }

    // 共通更新処理
    EnemyBase::Update();
}

void EnemySlime::Draw(void) const
{
    // 共通描画処理
    EnemyBase::Draw();

#ifdef _DEBUG
    // 球体コライダのオフセット位置を可視化
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
    if (it != ownColliders_.end())
    {
        const ColliderSphere* sphere = dynamic_cast<const ColliderSphere*>(it->second);
        if (sphere)
        {
            VECTOR colliderPos = sphere->GetPos();
            DrawSphere3D(colliderPos, 10.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
            DrawLine3D(trans_.pos, colliderPos, GetColor(0, 255, 255));

            VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(colliderPos, VGet(0, 50, 0)));
            DrawFormatString(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
                GetColor(0, 255, 255), "Offset Y: 30");
        }
    }
#endif
}

void EnemySlime::ApplyData(const EnemyInfo& info)
{
    EnemyBase::ApplyData(info);
}