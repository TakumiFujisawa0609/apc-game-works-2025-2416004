#include "../../Pch.h"
#include "Tower.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderModel.h"

Tower::Tower(const VECTOR& position)
    : UnitBase()
    , attackTimer_(0.0f)
    , isGround_(false)
{
    // 座標を初期設定
    trans_.pos = position;
}

Tower::~Tower(void)
{
}

void Tower::Load(int modelId)
{
    // モデルIDをメンバ変数に保持
    trans_.modelId = modelId;
}

void Tower::Initialize(void)
{
    UnitBase::Initialize();
    attackTimer_ = 0.0f;

    // スケールを初期設定
    trans_.scl = VGet(35.0f, 35.0f, 35.0f);
    trans_.Update();

    // コライダーの初期化
    InitCollider(); 

    MV1SetScale(trans_.modelId, trans_.scl);

    // その後でセットアップ
    MV1SetupCollInfo(trans_.modelId, -1);

    CollisionController::GetInstance().RegisterUnit(this);
}

void Tower::Update(void)
{
    // 地面に固定
    UnitBase::Update();
}

void Tower::Draw(void) const
{
    UnitBase::Draw();
#ifdef _DEBUG
    //// モデルが持つ全フレーム数を取得
    //int frameNum = MV1GetFrameNum(trans_.modelId);

    //// 画面に表示 (座標 20, 20 に白文字で)
    //DrawFormatString(20, 400, GetColor(255, 255, 255), "Tower Total Frames: % d", frameNum);

    //// ついでに各フレームの名前と判定があるかをコンソール(出力ウィンドウ)に出す
    //for (int i = 0; i < frameNum; i++) 
    //{
    //    const char* name = MV1GetFrameName(trans_.modelId, i);
    //    // そのフレームにポリゴンが含まれているか
    //    int triangleNum = MV1GetFrameTriangleNum(trans_.modelId, i);
    //}
#endif
}

void Tower::Release(void)
{
    // CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    if (trans_.modelId >= 0) {
        MV1DeleteModel(trans_.modelId);
        trans_.modelId = -1;
    }

    UnitBase::Release();
}

void Tower::ApplyData(const TowerData& data)
{
    data_ = data;
}

void Tower::SetPosition(const VECTOR& position)
{
    trans_.pos = position;
}

void Tower::CalcGravityPow(void)
{
}

void Tower::InitCollider(void)
{
    const VECTOR localStart = VGet(0.0f, 50.0f, 0.0f);
    const VECTOR localEnd = VGet(0.0f, -100.0f, 0.0f);

    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::STAGE,
        &trans_,
        localStart,
        localEnd
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    // モデルコライダーを作成（除外フレームなしで全フレーム有効化）
    ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &trans_);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void Tower::OnCollisionEnter(const CollisionInfo& info)
{
 
}

void Tower::OnCollisionStay(const CollisionInfo& info)
{
}