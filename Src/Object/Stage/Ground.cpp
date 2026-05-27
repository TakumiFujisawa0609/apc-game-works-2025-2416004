#include "../../Pch.h"
#include "Ground.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderModel.h"
#include "../../Utility/Utility.h"

Ground::Ground(void)
    : isRegistered_(false)  
{
}

Ground::~Ground(void)
{
}

void Ground::Initialize(const VECTOR& pos, int modelId)
{
    // 座標を設定
    trans_.pos = pos;

    // モデルIDを設定
    trans_.modelId = modelId;

    // スケールを設定
    trans_.scl = VGet(TILE_SCALE, 1.0f, TILE_SCALE);

    // 回転を初期化
    trans_.rot = Utility::VECTOR_ZERO;

    // モデル位置設定
    if (trans_.modelId != -1)
    {
        MV1SetPosition(trans_.modelId, trans_.pos);
        MV1SetScale(trans_.modelId, trans_.scl);

        // コリジョン情報を構築
        MV1SetupCollInfo(trans_.modelId, -1);
    }

    // 衝突判定の初期化
    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
    isRegistered_ = true;
}

void Ground::InitWithoutRegister(const VECTOR& pos, int modelId)
{
    // 座標を設定
    trans_.pos = pos;

    // モデルIDを設定
    trans_.modelId = modelId;

    // スケールを設定
    trans_.scl = VGet(TILE_SCALE, 1.0f, TILE_SCALE);

    // 回転を初期化
    trans_.rot = Utility::VECTOR_ZERO;

    // モデル位置設定
    if (trans_.modelId != -1)
    {
        MV1SetPosition(trans_.modelId, trans_.pos);
        MV1SetScale(trans_.modelId, trans_.scl);

        // コリジョン情報を構築
        MV1SetupCollInfo(trans_.modelId, -1);
    }

    // 衝突判定の初期化
    InitCollider();

    // CollisionControllerには登録しない
    isRegistered_ = false;
}

void Ground::InitCollider(void)
{
    if (trans_.modelId == -1) return;

    // モデルコライダの作成
    ColliderModel* colModel = new ColliderModel(
        ColliderBase::TAG::GROUND,
        &trans_
    );

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::MODEL),
        colModel
    );

}

void Ground::Update(void)
{
}

void Ground::Draw(void) const
{
    if (trans_.modelId == -1) return;

    // モデルの描画
    MV1SetPosition(trans_.modelId, trans_.pos);
    MV1SetScale(trans_.modelId, trans_.scl);
    MV1DrawModel(trans_.modelId);

    // デバッグ表示
#ifdef _DEBUG
    // 基底クラスのコライダ描画（重いのでコメントアウト推奨）
    // UnitBase::Draw();
#endif
}

void Ground::Release(void)
{
    // CollisionControllerから登録解除
    if (isRegistered_)
    {
        CollisionController::GetInstance().UnregisterUnit(this);
        isRegistered_ = false;
    }

    // 基底クラスの解放（コライダの削除）
    UnitBase::Release();

    if (trans_.modelId != -1)
    {
        MV1DeleteModel(trans_.modelId);
        trans_.modelId = -1;
    }
}

float Ground::GetTileSize(void) const
{
    return BASE_TILE_SIZE * TILE_SCALE;
}