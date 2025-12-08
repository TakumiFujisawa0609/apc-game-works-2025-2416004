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

    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

// 更新処理
void Sword::Update(void)
{
    // フレーム位置から少し上にオフセット
    trans_.pos = VAdd(framePos_, VGet(0.0f, POSITION_OFFSET_Y, 0.0f));

    // 攻撃中は剣を下に向ける、それ以外は通常の角度
    float tiltAngle = isAttacking_ ? SWORD_ATTACK_ANGLE : SWORD_TILT_ANGLE;

    // プレイヤーのローカル回転をそのまま剣のグローバル回転に設定
    trans_.quaRot = playerRot_;

    // プレイヤーの向きに相対的にX軸で傾ける（ローカル回転）
    trans_.quaRotLocal = Quaternion::AngleAxis(
        Utility::Deg2RadF(tiltAngle),
        VGet(1.0f, 0.0f, 0.0f)  // X軸
    );

    UnitBase::Update();
}

// 描画処理
void Sword::Draw(void) const
{
    UnitBase::Draw();

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
    // カプセルコライダの作成
    // ローカル座標で剣の刃の部分を指定 

    // 剣の柄の位置（基準点から少し下、Z方向に傾ける）
    VECTOR localStart = VGet(0.0f, -10.0f, -15.0f);

    // 剣の先端位置（基準点から上方向、Z方向に傾ける）
    VECTOR localEnd = VGet(0.0f, 80.0f, 25.0f);

    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::SWORD,  // プレイヤーの攻撃判定として扱う
        &trans_,
        localStart,
        localEnd,
        CAPSULE_RADIUS
    );

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::CAPSULE),
        colCapsule
    );

}