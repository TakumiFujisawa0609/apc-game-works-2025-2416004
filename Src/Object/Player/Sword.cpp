#include "Sword.h"
#include "Player.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../Object/Enemy/EnemyBase.h"
#include "../../Utility/Utility.h"

// コンストラクタ
Sword::Sword(void)
    : attachFrameIndex_(26)
    , positionOffset_(Utility::VECTOR_ZERO)
    , rotationOffset_(Utility::VECTOR_ZERO)
    , isVisible_(true)
    , localCapsuleStart_(Utility::VECTOR_ZERO)
    , localCapsuleEnd_(VGet(0.0f, SWORD_LENGTH, 0.0f))
{
}

// デストラクタ
Sword::~Sword(void)
{
}

// リソースの読み込み
void Sword::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    // ソードモデルの読み込み
    trans_.modelId = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_SWORD);
    trans_.SetModel(trans_.modelId);
}

// 初期化
void Sword::Init(void)
{
    // スケールの初期化
    trans_.scl = VGet(1.0f, 1.0f, 1.0f);

    // 回転の初期化
    trans_.rot = Utility::VECTOR_ZERO;

    // 位置の初期化
    trans_.pos = Utility::VECTOR_ZERO;

    // デフォルトのオフセット設定（必要に応じて調整）
    positionOffset_ = VGet(0.0f, 0.0f, 0.0f);
    rotationOffset_ = VGet(0.0f, 0.0f, 0.0f);

    // ローカル座標でのカプセル設定
    localCapsuleStart_ = Utility::VECTOR_ZERO;  // 柄の位置
    localCapsuleEnd_ = VGet(0.0f, SWORD_LENGTH, 0.0f);  // 先端の位置

    // 衝突判定の初期化
    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

// 衝突判定の初期化
void Sword::InitCollider(void)
{
    // カプセルコライダの作成
    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::PLAYER,  // または専用のTAG::SWORDを追加
        &trans_,
        localCapsuleStart_,  // ローカル座標（柄）
        localCapsuleEnd_,    // ローカル座標（先端）
        CAPSULE_RADIUS
    );

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::CAPSULE),
        colCapsule
    );
}

// 更新
void Sword::Update(void)
{
    // プレイヤーのフレームに追従
    FollowPlayerFrame();

    // カプセルコライダの位置を更新
    UpdateCollider();

    // 基底クラスの更新
    UnitBase::Update();
}

// カプセルコライダの位置を更新
void Sword::UpdateCollider(void)
{
    // カプセルコライダを取得
    int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);

    if (ownColliders_.count(capsuleType) == 0) return;

    ColliderCapsule* colCapsule =
        dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));

    if (!colCapsule) return;

    // プレイヤーが有効かチェック
    auto player = player_.lock();
    if (!player) return;

    // プレイヤーのモデルハンドルを取得
    int playerModelHandle = player->GetTransform().modelId;

    if (playerModelHandle == -1) return;

    // フレームのワールド座標行列を取得
    MATRIX frameMatrix = MV1GetFrameLocalWorldMatrix(playerModelHandle, attachFrameIndex_);

    // 剣の forward 方向（行列のZ軸）
    VECTOR forward = VGet(frameMatrix.m[2][0], frameMatrix.m[2][1], frameMatrix.m[2][2]);
    forward = VNorm(forward);

    // カプセルの開始位置と終了位置をローカル座標で更新
    localCapsuleStart_ = Utility::VECTOR_ZERO;  // 柄の位置（剣の基点）
    localCapsuleEnd_ = VScale(forward, SWORD_LENGTH);  // 先端の位置

    // コライダに反映
    colCapsule->SetLocalPosStart(localCapsuleStart_);
    colCapsule->SetLocalPosEnd(localCapsuleEnd_);
}

// 描画
void Sword::Draw(void) const
{
    if (!isVisible_)
    {
        return;
    }

    // 基底クラスの描画
    UnitBase::Draw();
}

// 解放
void Sword::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // 基底クラスの解放
    UnitBase::Release();
}

// プレイヤーの設定
void Sword::SetPlayer(std::weak_ptr<Player> player)
{
    player_ = player;
}

// フレーム番号の設定
void Sword::SetAttachFrame(int frameIndex)
{
    attachFrameIndex_ = frameIndex;
}

// 表示/非表示設定
void Sword::SetVisible(bool visible)
{
    isVisible_ = visible;
}

// 表示中か
bool Sword::IsVisible(void) const
{
    return isVisible_;
}

// 座標の微調整
void Sword::SetPositionOffset(const VECTOR& offset)
{
    positionOffset_ = offset;
}

// 回転の微調整
void Sword::SetRotationOffset(const VECTOR& offset)
{
    rotationOffset_ = offset;
}

// 衝突判定のコールバック
void Sword::OnCollisionEnter(const CollisionInfo& info)
{
    // 敵との衝突の場合
    if (info.hitCollider->GetTag() == ColliderBase::TAG::ENEMY)
    {
        // 敵にダメージを与える処理
        // 注意: EnemyBaseへのアクセスが必要な場合は、
        // CollisionInfoに追加情報を持たせるか、別の方法で取得する必要があります

        // 例: デバッグ出力
#ifdef _DEBUG
        printfDx("剣が敵に当たった！\n");
#endif
    }
}

// プレイヤーのフレームに追従
void Sword::FollowPlayerFrame(void)
{
    // プレイヤーが有効かチェック
    auto player = player_.lock();
    if (!player)
    {
        return;
    }

    // プレイヤーのモデルハンドルを取得
    int playerModelHandle = player->GetTransform().modelId;

    if (playerModelHandle == -1 || trans_.modelId == -1)
    {
        return;
    }

    // フレームのワールド座標行列を取得
    MATRIX frameMatrix = MV1GetFrameLocalWorldMatrix(playerModelHandle, attachFrameIndex_);

    // フレームの位置を取得
    VECTOR framePos;
    framePos.x = frameMatrix.m[3][0];
    framePos.y = frameMatrix.m[3][1];
    framePos.z = frameMatrix.m[3][2];

    // オフセットを適用（フレームのローカル座標系で適用）
    VECTOR offsetPos = VGet(
        positionOffset_.x * frameMatrix.m[0][0] + positionOffset_.y * frameMatrix.m[1][0] + positionOffset_.z * frameMatrix.m[2][0],
        positionOffset_.x * frameMatrix.m[0][1] + positionOffset_.y * frameMatrix.m[1][1] + positionOffset_.z * frameMatrix.m[2][1],
        positionOffset_.x * frameMatrix.m[0][2] + positionOffset_.y * frameMatrix.m[1][2] + positionOffset_.z * frameMatrix.m[2][2]
    );

    trans_.pos = VAdd(framePos, offsetPos);

    // フレームの回転を取得（行列からオイラー角へ変換）
    float rotY = atan2f(frameMatrix.m[2][0], frameMatrix.m[2][2]);
    float rotX = atan2f(-frameMatrix.m[2][1], sqrtf(frameMatrix.m[2][0] * frameMatrix.m[2][0] + frameMatrix.m[2][2] * frameMatrix.m[2][2]));
    float rotZ = atan2f(frameMatrix.m[0][1], frameMatrix.m[1][1]);

    // 回転オフセットを適用
    trans_.rot.x = rotX + rotationOffset_.x;
    trans_.rot.y = rotY + rotationOffset_.y;
    trans_.rot.z = rotZ + rotationOffset_.z;
}