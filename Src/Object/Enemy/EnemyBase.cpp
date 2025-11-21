#include "EnemyBase.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderLine.h"
#include "../../Manager/System/CollisionController.h"

// コンストラクタ
EnemyBase::EnemyBase(void) :
    hp_(0.0f),
    maxHp_(0.0f),
    moveSpeed_(0.0f),
    type_(""),
    viewRange_(0.0f),
    lostRange_(0.0f),
    forward_(Utility::DIR_F),
    viewAngle_(0.0f),
    isChasing_(false),
    isInView_(false)
{
}

// 読み込み
void EnemyBase::Load(int modelId)
{
    trans_.modelId = modelId;
    trans_.SetModel(trans_.modelId);
}

// 初期化
void EnemyBase::Init(const VECTOR& startPos)
{
    trans_.pos = startPos;
    prePos_ = startPos;
    trans_.scl = Utility::VECTOR_ONE;
    movePow_ = Utility::VECTOR_ZERO;
    jumpPow_ = Utility::VECTOR_ZERO;
    currentAnim_ = ANIM::NONE;

    // コライダ初期化
    InitCollider();

    // CollisionControllerに登録
    CollisionController::GetInstance().RegisterUnit(this);
}

// コライダ初期化
void EnemyBase::InitCollider(void)
{
    // 球体コライダの作成（敵同士・プレイヤーとの衝突用）
    ColliderSphere* colSphere = new ColliderSphere(
        ColliderBase::TAG::ENEMY,
        &trans_,
        Utility::VECTOR_ZERO,  // ローカル座標（中心）
        radius_
    );
    ownColliders_.emplace(
        static_cast<int>(UnitBase::COLLIDER_TYPE::SPHERE),
        colSphere
    );

    // 線分コライダの作成（地面判定用）
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::ENEMY,
        &trans_,
        COL_LINE_START_LOCAL_POS,
        COL_LINE_END_LOCAL_POS
    );
    ownColliders_.emplace(
        static_cast<int>(UnitBase::COLLIDER_TYPE::LINE),
        colLine
    );
}

// 更新処理
void EnemyBase::Update(void)
{
    // 押し出し前の位置を保存
    preCollisionPos_ = trans_.pos;

    // UnitBaseの更新（重力・衝突判定を含む）
    UnitBase::Update();

    // ステージ外に出た場合は元の位置に戻す
    if (!IsOnStage(trans_.pos))
    {
        // 前フレームの位置に戻す
        trans_.pos = preCollisionPos_;

        // 移動をキャンセル
        movePow_ = Utility::VECTOR_ZERO;
    }

#ifdef _DEBUG
    // デバッグ情報
    //printfDx("Enemy Y: %.2f, JumpPow.y: %.2f\n", trans_.pos.y, jumpPow_.y);
    //printfDx("Enemy HitColliders: %d\n", hitColliders_.size());
#endif
}

// ステージ上にいるかチェック
bool EnemyBase::IsOnStage(const VECTOR& pos) const
{
    // GroundManagerのタイル範囲を参照
    // TILE_COUNT = 10, TILE_SIZE = 1000.0f と仮定
    const float TILE_COUNT = 10.0f;
    const float TILE_SIZE = 1000.0f;
    const float HALF_STAGE_SIZE = (TILE_COUNT * TILE_SIZE) * 0.5f;

    // ステージの範囲内かチェック（マージンを持たせる）
    const float MARGIN = 100.0f;
    const float MAX_X = HALF_STAGE_SIZE - MARGIN;
    const float MIN_X = -HALF_STAGE_SIZE + MARGIN;
    const float MAX_Z = HALF_STAGE_SIZE - MARGIN;
    const float MIN_Z = -HALF_STAGE_SIZE + MARGIN;

    // 範囲チェック
    if (pos.x < MIN_X || pos.x > MAX_X ||
        pos.z < MIN_Z || pos.z > MAX_Z)
    {
        return false;
    }

    // Y座標が異常に低い場合もfalse
    if (pos.y < -500.0f)
    {
        return false;
    }

    return true;
}

// 描画処理
void EnemyBase::Draw(void) const
{
    // モデル描画（UnitBaseの描画）
    UnitBase::Draw();

#ifdef _DEBUG
    // 当たり判定可視化（球体）
    DrawCapsule3D(trans_.pos, trans_.pos, radius_, 12, 0xff0000, 0xff0000, false);

    // 線分コライダの可視化
    auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::LINE));
    if (it != ownColliders_.end())
    {
        const ColliderLine* line = dynamic_cast<const ColliderLine*>(it->second);
        if (line)
        {
            VECTOR start = line->GetPosStart();
            VECTOR end = line->GetPosEnd();
            DrawLine3D(start, end, GetColor(255, 255, 0));
            DrawSphere3D(start, 5.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);
            DrawSphere3D(end, 5.0f, 8, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
        }
    }

    // 衝突しているエネミー数の表示
    int enemyCollisionCount = 0;
    for (auto* col : hitColliders_)
    {
        if (col->GetTag() == ColliderBase::TAG::ENEMY)
        {
            enemyCollisionCount++;
        }
    }
    if (enemyCollisionCount > 0)
    {
        VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 100, 0)));
        DrawFormatString(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
            GetColor(255, 255, 0), "Colliding: %d", enemyCollisionCount);
    }

    // ステージ外警告
    if (!IsOnStage(trans_.pos))
    {
        VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 150, 0)));
        DrawFormatString(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
            GetColor(255, 0, 0), "OUT OF STAGE!");
    }
#endif // _DEBUG
}

// CSVデータを適用
void EnemyBase::ApplyData(const EnemyInfo& info)
{
    type_ = info.type;
    maxHp_ = info.param.maxHp;
    hp_ = info.param.hp;
    moveSpeed_ = info.param.speed;
    radius_ = info.param.radius;
}

// 前方向を設定
void EnemyBase::SetForward(const VECTOR& forward)
{
    forward_ = forward;
}

// 視野角を設定
void EnemyBase::SetViewAngle(float angle)
{
    viewAngle_ = angle;
}

// 体力を設定
void EnemyBase::SetHp(float hp)
{
    hp_ = std::clamp(hp, 0.0f, maxHp_);
}

// 体力を取得
float EnemyBase::GetHp(void) const
{
    return hp_;
}

// ダメージ処理
void EnemyBase::TakeDamage(float damage)
{
    hp_ -= damage;
    if (hp_ < 0.0f)
    {
        hp_ = 0.0f;
    }
}

// タイプを取得
const std::string& EnemyBase::GetType(void) const
{
    return type_;
}