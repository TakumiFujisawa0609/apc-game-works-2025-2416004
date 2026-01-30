#include "Sword.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Manager/System/CollisionController.h"

// コンストラクタ
Sword::Sword(void)
    : framePos_(Utility::VECTOR_ZERO)
    , isAttacking_(false)
    , playerRot_(Quaternion::Identity())
    , UnitBase()
{
}

// デストラクタ
Sword::~Sword(void)
{
}

// リソースの読み込み
void Sword::Load(void)
{
    trans_.modelId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_SWORD).handleId_;
    trans_.SetModel(trans_.modelId);
}

// 初期化
void Sword::Init(void)
{
    // カプセルコライダの半径
    radius_ = CAPSULE_RADIUS;


    trans_.scl = SOWRD_SCALE;

    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);

    trans_.Update();

}

// 更新処理
void Sword::Update(void)
{
    // 1. スケール設定
    trans_.scl = SOWRD_SCALE;

    // 2. 手の基本位置と回転を取得
    VECTOR handPos = VGet(handMatrix_.m[3][0], handMatrix_.m[3][1], handMatrix_.m[3][2]);
    Quaternion handRot = Quaternion::GetRotation(handMatrix_);
    handRot.Normalize();

    // 3. 【重要】手に対するローカルオフセットを設定
    // VGet(右, 上, 前) です。ここの数値を調整して「握り位置」を合わせます。
    // 手のひらから少し上に上げたい場合は Y を増やします。
    VECTOR localOffset = POSITION_OFFSET_Y;

    // 4. ローカルオフセットを手の回転に合わせて変換
    // これにより、手が横を向いていても「手にとっての上」に計算されます
    VECTOR worldOffset = handRot.PosAxis(localOffset);

    // 5. 最終的な座標を適用
    trans_.pos = VAdd(handPos, worldOffset);

    // 6. 回転の適用（前回同様）
    trans_.quaRot = handRot;

    // 剣を握っている角度の微調整（モデルに合わせて X, Y, Z を調整）
    Quaternion adjust = Quaternion::Axis(VGet(1.0f, 0.0f, 0.0f), Utility::Deg2RadF(90.0f));
    trans_.quaRotLocal = adjust;

    UnitBase::Update();
}

// 描画処理
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
    // カプセルコライダの可視化
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::CAPSULE));
    if (it != ownColliders_.end())
    {
        const ColliderCapsule* capsule = dynamic_cast<const ColliderCapsule*>(it->second);
        if (capsule)
        {
            VECTOR start = capsule->GetPosStart();
            VECTOR end = capsule->GetPosEnd();
            float r = capsule->GetRadius();

            // 攻撃中は赤、それ以外は青
            unsigned int color = isAttacking_ ? GetColor(255, 0, 0) : GetColor(0, 0, 255);

            DrawCapsule3D(start, end, r, 8, color, color, false);

            // 始点と終点を球で表示
            DrawSphere3D(start, 5.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
            DrawSphere3D(end, 5.0f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);

            DrawFormatString(50, 50, GetColor(255, 255, 255), "Player Rot : X: % .1f Y : % .1f Z : % .1f", Utility::Rad2DegF(trans_.quaRotLocal.x), Utility::Rad2DegF(trans_.quaRotLocal.y), Utility::Rad2DegF(trans_.quaRotLocal.z));
        }
    }
#endif // _DEBUG
}

// 解放処理
void Sword::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // UnitBaseの解放（コライダも含む）
    UnitBase::Release();
}

// フレームの座標の設定
void Sword::SetFramePos(const VECTOR& pos)
{
    framePos_ = pos;
}

// プレイヤーの回転を設定
void Sword::SetPlayerRotation(const Quaternion& playerRot)
{
    // プレイヤーの回転をそのまま適用
    playerRot_ = playerRot;
}

// 攻撃状態の設定
void Sword::SetAttacking(bool attacking)
{
    isAttacking_ = attacking;

    // 攻撃中のみコライダを有効化
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::CAPSULE));
    if (it != ownColliders_.end())
    {
        it->second->SetValid(attacking);
    }
}
// 攻撃中かどうか
bool Sword::IsAttacking(void) const
{
    return isAttacking_;
}

// コライダーの初期化
void Sword::InitCollider(void)
{
    VECTOR localStart = CAPSULE_STATE_POS;
    // Y ではなく X に数値を入れる
    VECTOR localEnd = CAPSULE_END_POS;

    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::SWORD, &trans_, localStart, localEnd, CAPSULE_RADIUS
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}