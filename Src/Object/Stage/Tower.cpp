// Tower.cpp
#include "Tower.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Collider/ColliderLine.h"

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
    trans_.scl = VGet(15.0f, 15.0f, 15.0f); // モデルの高さは5.0fと仮定

    trans_.Update();

    InitCollider(); // コライダーの初期化
    // ... その他の初期化
}

// 更新処理
void Tower::Update(void)
{
    // ★ UnitBase::Update() のみ。重力落下ロジックは削除し、地面に固定された状態とする。
    // UnitBase::Update() の中でコライダーの位置更新と衝突判定が走ることを期待する。

    trans_.pos.y = 0.0f;
    UnitBase::Update();

    // タワーのロジック（例：攻撃タイマーを進める）
    // attackTimer_ += SceneManager::GetInstance().GetDeltaTime();
}

// 描画処理
void Tower::Draw(void) const
{
    UnitBase::Draw();

#ifdef _DEBUG
#endif
}

// 解放処理
void Tower::Release(void)
{
    // モデルを解放
    if (trans_.modelId >= 0)
    {
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

// コライダー初期化
void Tower::InitCollider(void)
{
    // タワーのスケールは Y=5.0f なので、中心を基準に底面は -2.5f の位置にある
    const float HALF_HEIGHT = trans_.scl.y / 2.0f; // 2.5f

    // 地面判定用のラインは、タワーの底面より少し上から下に伸ばす
    // 始点: Y = -2.0f (底面 -2.5f より 0.5f 上)
    // 終点: Y = -2.0f - 50.0f (下方向に十分な長さ)
    const float GROUND_CHECK_OFFSET = 0.5f;
    const float RAY_LENGTH = 50.0f;

    // ローカル座標で線分の始点と終点を定義
    const VECTOR localStart = VGet(0.0f, -HALF_HEIGHT + GROUND_CHECK_OFFSET, 0.0f); // Y: -2.0f
    const VECTOR localEnd = VGet(0.0f, -HALF_HEIGHT - RAY_LENGTH, 0.0f);          // Y: -52.0f

    ColliderLine* colLine = new ColliderLine(ColliderBase::TAG::STAGE, &trans_, COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);
}

// 衝突イベント
void Tower::OnCollisionEnter(const CollisionInfo& info)
{

}

void Tower::OnCollisionStay(const CollisionInfo& info)
{
}