#include "Glider.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"

// コンストラクタ
Glider::Glider(void)
    : framePos_(Utility::VECTOR_ZERO)
    , isGliding_(false)
    , playerRot_(Quaternion::Identity())
    , UnitBase()
{
}

// デストラクタ
Glider::~Glider(void)
{
}

// リソースの読み込み
void Glider::Load(void)
{
    // グライダーのモデルを読み込み
    // ※ ResourceManager::SRC::MODEL_GLIDER を定義する必要があります
    trans_.modelId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_GLIDER).handleId_;
    trans_.SetModel(trans_.modelId);
}

// 初期化
void Glider::Init(void)
{
    // グライダーはコライダを持たないため、空実装
    InitCollider();

    trans_.Update();
}

// 更新処理
void Glider::Update(void)
{
    // スケール設定
    trans_.scl = GLIDER_SCALE;

    // プレイヤーの回転に基づいてオフセット位置を計算
    VECTOR localOffset = VGet(0.0f, POSITION_OFFSET_Y, POSITION_OFFSET_Z);

    // プレイヤーの回転を適用してワールド座標のオフセットに変換
    VECTOR worldOffset = playerRot_.PosAxis(localOffset);

    // フレーム位置にオフセットを加えた位置に配置
    trans_.pos = VAdd(framePos_, worldOffset);

    Quaternion offsetRot = Quaternion::Axis(VGet(0.0f, 1.0f, 0.0f), Utility::Deg2RadF(180.0f));

    if (isGliding_) // グライド中
    {
        // グライダーを少し傾ける（X軸周りの回転）
        Quaternion tiltRotation = Quaternion::Axis(VGet(1.0f, 0.0f, 0.0f), Utility::Deg2RadF(GLIDER_TILT_ANGLE));

        // プレイヤーの回転に傾きを合成
        trans_.quaRotLocal = playerRot_ * tiltRotation;
    }
    else // 非グライド時
    {
        // プレイヤーの回転をそのまま適用
        trans_.quaRotLocal = playerRot_;
    }

    // UnitBaseの更新
    UnitBase::Update();
}

// 描画処理
void Glider::Draw(void) const
{
    // グライド中でなければ描画しない
    if (!isGliding_)
    {
        return;
    }

    if (trans_.modelId != -1)
    {
        UnitBase::Draw();
    }

#ifdef _DEBUG
    // デバッグ情報の表示
    DrawFormatString(50, 100, GetColor(255, 255, 0),
        "Glider Active - Pos: (%.1f, %.1f, %.1f)",
        trans_.pos.x, trans_.pos.y, trans_.pos.z);
#endif // _DEBUG
}

// 解放処理
void Glider::Release(void)
{
    // UnitBaseの解放
    UnitBase::Release();
}

// フレームの座標の設定
void Glider::SetFramePos(const VECTOR& pos)
{
    framePos_ = pos;
}

// プレイヤーの回転を設定
void Glider::SetPlayerRotation(const Quaternion& playerRot)
{
    playerRot_ = playerRot;
}

// グライド状態の設定
void Glider::SetGliding(bool gliding)
{
    isGliding_ = gliding;
}

// グライド中かどうか
bool Glider::IsGliding(void) const
{
    return isGliding_;
}

// コライダの初期化（グライダーはコライダを持たない）
void Glider::InitCollider(void)
{
    // 空実装
}