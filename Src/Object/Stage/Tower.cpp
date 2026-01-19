// Tower.cpp
#include "Tower.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderModel.h"

// コンストラクタ
Tower::Tower(const VECTOR& position)
    : UnitBase()
    , attackTimer_(0.0f)
    , isGround_(false)
{
    // 座標を初期設定
    trans_.pos = position;
}

// デストラクタ
Tower::~Tower()
{
}

// リソースの読み込み (モデルIDを設定)
void Tower::Load(int modelId)
{
    // モデルIDをメンバ変数に保持
    trans_.modelId = modelId;
}

// 初期化
void Tower::Init(void)
{
    UnitBase::Init();
    attackTimer_ = 0.0f;

    // スケールを初期設定
    trans_.scl = VGet(35.0f, 35.0f, 35.0f);
    trans_.Update();

    InitCollider(); // コライダーの初期化

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

// 更新処理
void Tower::Update(void)
{
    // 地面に固定
    UnitBase::Update();
}

// 描画処理
void Tower::Draw(void) const
{
    UnitBase::Draw();
#ifdef _DEBUG
    // モデルコライダーのデバッグ描画
    int modelType = static_cast<int>(COLLIDER_TYPE::MODEL);
    const ColliderBase* col = GetOwnCollider(modelType);
    if (col)
    {
        // タワーの位置に緑の球体を描画
        DrawSphere3D(trans_.pos, 100.0f, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), FALSE);
    }
#endif
}

// 解放処理
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

// タワーのパラメータを設定
void Tower::ApplyData(const TowerData& data)
{
    data_ = data;
}

// 座標の設定
void Tower::SetPosition(const VECTOR& position)
{
    trans_.pos = position;
}

void Tower::CalcGravityPow(void)
{
}

// コライダー初期化
void Tower::InitCollider(void)
{
    // 地面判定用のライン
    const float HALF_HEIGHT = trans_.scl.y / 2.0f; // 35.0 / 2 = 17.5
    const float GROUND_CHECK_OFFSET = 5.0f;       // 少し余裕を持たせてモデル内側から開始
    const float RAY_LENGTH = 30.0f;               // 地面を突き抜けるのに十分な長さ

    // 計算した値を座標として設定
    const VECTOR localStart = VGet(0.0f, -HALF_HEIGHT + GROUND_CHECK_OFFSET, 0.0f);
    const VECTOR localEnd = VGet(0.0f, -HALF_HEIGHT - RAY_LENGTH, 0.0f);

    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::STAGE,
        &trans_,
        localStart,
        localEnd
    );
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &trans_);

    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

// 衝突イベント
void Tower::OnCollisionEnter(const CollisionInfo& info)
{
 
}

void Tower::OnCollisionStay(const CollisionInfo& info)
{
    // 継続的な衝突処理が必要な場合はここに記述
}