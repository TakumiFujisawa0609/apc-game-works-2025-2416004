#include "../../Pch.h"
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

void Glider::Load(void)
{
    trans_.modelId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_GLIDER).handleId_;
    trans_.SetModel(trans_.modelId);
}

void Glider::Initialize(void)
{
    // グライダー自体には当たり判定を持たせないため、内部は空実装を呼び出す
    InitCollider();

    // 初回のトランスフォーム更新
    trans_.Update();
}

void Glider::Update(void)
{
    // モデルのスケールを定数から設定
    trans_.scl = GLIDER_SCALE;

    // プレイヤーの回転に基づき、ローカルのオフセット位置をワールド座標系に変換
    VECTOR localOffset = VGet(0.0f, POSITION_OFFSET_Y, POSITION_OFFSET_Z);
    VECTOR worldOffset = playerRot_.PosAxis(localOffset);

    // プレイヤーの背中などの基準点にオフセットを加算して位置を確定
    trans_.pos = VAdd(framePos_, worldOffset);

    // モデルの初期向き調整用
    // ※モデルが背面を向いている場合などの補正用
    Quaternion offsetRot = Quaternion::Axis(VGet(0.0f, 1.0f, 0.0f), Utility::Deg2RadF(180.0f));

    if (isGliding_)
    {
        // 飛行中の自然な傾き（前傾など）を計算
        Quaternion tiltRotation = Quaternion::Axis(VGet(1.0f, 0.0f, 0.0f), Utility::Deg2RadF(GLIDER_TILT_ANGLE));

        // プレイヤーの現在の回転に、グライダー特有の傾きを合成して適用
        trans_.quaRotLocal = playerRot_ * tiltRotation;
    }
    else
    {
        // プレイヤーの向きと完全に同期
        trans_.quaRotLocal = playerRot_;
    }

    UnitBase::Update();
}

void Glider::Draw(void) const
{
    // 滑空中でなければモデルを表示しない
    if (!isGliding_)
    {
        return;
    }

    // モデルハンドルが正しく読み込まれている場合のみ描画
    if (trans_.modelId != -1)
    {
        UnitBase::Draw();
    }

#ifdef _DEBUG
    // デバッグ情報：グライダーの現在座標を画面に表示
    DrawFormatString(50, 100, GetColor(255, 255, 0),
        "Glider Active - Pos: (%.1f, %.1f, %.1f)",
        trans_.pos.x, trans_.pos.y, trans_.pos.z);
#endif // _DEBUG
}

void Glider::Release(void)
{
    UnitBase::Release();
}

void Glider::SetFramePos(const VECTOR& pos)
{
    framePos_ = pos;
}

void Glider::SetPlayerRotation(const Quaternion& playerRot)
{
    playerRot_ = playerRot;
}

void Glider::SetGliding(bool gliding)
{
    isGliding_ = gliding;
}

bool Glider::IsGliding(void) const
{
    return isGliding_;
}

void Glider::InitCollider(void)
{
}