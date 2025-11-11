#include "Sword.h"
#include "Player.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/System/CollisionManager.h"
#include "../../Utility/Utility.h"

// コンストラクタ
Sword::Sword(void)
    : attachFrameIndex_(26)
    , positionOffset_(Utility::VECTOR_ZERO)
    , rotationOffset_(Utility::VECTOR_ZERO)
    , isVisible_(true)
    , capsuleRadius_(10.0f)
    , capsuleStart_(Utility::VECTOR_ZERO)
    , capsuleEnd_({0.0f, 1.0f, 0.0f})
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
    // ResourceManager::SRC::MODEL_SWORD が定義されていると仮定
    // 未定義の場合は適切なリソースIDに変更してください
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
}

// 更新
void Sword::Update(void)
{
    // プレイヤーのフレームに追従
    FollowPlayerFrame();

    // 基底クラスの更新
    UnitBase::Update();
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

// 当たり半径
float Sword::GetCapsuleRadius(void)
{
    return capsuleRadius_;
}

// カプセル開始地点
VECTOR Sword::GetCapsuleStart(void)
{
    return capsuleStart_;
}

// カプセル終了地点
VECTOR Sword::GetCapsuleEnd(void)
{
    return capsuleEnd_;
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

    // 剣の forward 方向（行列のZ軸）
    VECTOR forward = VGet(frameMatrix.m[2][0], frameMatrix.m[2][1], frameMatrix.m[2][2]);
    forward = VNorm(forward);

    // 剣の長さ（モデルの大きさによって調整）
    float swordLength = 55.0f; // ← 必要なら調整してOK

    // カプセル開始地点（柄付近）
    capsuleStart_ = trans_.pos;

    // カプセル終了地点（先端）
    capsuleEnd_ = VAdd(trans_.pos, VScale(forward, swordLength));
}