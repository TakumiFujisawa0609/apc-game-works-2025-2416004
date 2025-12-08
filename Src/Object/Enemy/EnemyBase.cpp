#include "EnemyBase.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderLine.h"
#include "../../Manager/System/CollisionController.h"
#include "../../Manager/Generic/SceneManager.h"

// コンストラクタ
EnemyBase::EnemyBase(void) 
    : hp_(0.0f)
    , lastHitTime_(0.0f)
    , maxHp_(0.0f)
    , attack_(0.0f)
    , defense_(0.0f)
    , level_(1)
    , moveSpeed_(0.0f)
    , viewRange_(0.0f)
    , lostRange_(0.0f)
    , forward_(Utility::DIR_F)
    , viewAngle_(0.0f)
    , type_("")
    , isChasing_(false)
    , isInView_(false)
    
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
    // ヒット判定のクールタイム更新
    if (lastHitTime_ > 0.0f)
    {
        lastHitTime_ -= SceneManager::GetInstance().GetDeltaTime();
        if (lastHitTime_ < 0.0f)
        {
            lastHitTime_ = 0.0f;
        }
    }

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
    const float TILE_COUNT = 100.0f;
    const float TILE_SIZE = 100.0f;
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

    // HPバーの表示
    VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(trans_.pos, VGet(0, 80, 0)));
    int barWidth = 60;
    int barHeight = 8;
    int barX = static_cast<int>(screenPos.x) - barWidth / 2;
    int barY = static_cast<int>(screenPos.y);

    float hpRate = hp_ / maxHp_;
    int hpBarWidth = static_cast<int>(barWidth * hpRate);

    // 枠
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(0, 0, 0), false);
    // HP
    DrawBox(barX, barY, barX + hpBarWidth, barY + barHeight, GetColor(0, 255, 0), true);

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
#endif // _DEBUG
}

// EnemyBase.cpp
void EnemyBase::Release(void)
{
    // 1. コライダを無効化
    for (auto& pair : ownColliders_)
    {
        if (pair.second != nullptr)
        {
            pair.second->SetValid(false);
        }
    }

    // 2. CollisionControllerから登録解除
    CollisionController::GetInstance().UnregisterUnit(this);

    // 3. UnitBaseの解放（コライダ削除）
    UnitBase::Release();
}

// CSVデータを適用
void EnemyBase::ApplyData(const EnemyInfo& info)
{
    type_ = info.type;
    maxHp_ = info.param.maxHp;
    hp_ = info.param.hp;
    attack_ = info.param.attack;
    defense_ = info.param.defense;
    moveSpeed_ = info.param.speed;
    radius_ = info.param.radius;
    level_ = info.param.level;
}

// 衝突時のコールバック
void EnemyBase::OnCollisionEnter(const CollisionInfo& info)
{
    // 剣との衝突（SWORDタグ、カプセルコライダ）
    if (info.hitCollider->GetTag() == ColliderBase::TAG::SWORD &&
        info.hitCollider->GetShape() == ColliderBase::SHAPE::CAPSULE)
    {

        // ヒット判定のクールタイム中はダメージを受けない
        if (lastHitTime_ > 0.0f)
        {
            return;
        }

        // ダメージを受ける
        float oldHp = hp_;
        TakeDamage(SWORD_DAMAGE);

        // ヒット判定のクールタイムを設定
        lastHitTime_ = HIT_COOLDOWN;

    }
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

// レベルを設定
void EnemyBase::SetLevel(int level)
{
    level_ = level;

    ApplyLevelParams();
}

// レベルを取得
int EnemyBase::GetLevel(void) const
{
    return level_;
}

// レベルボーナスを適用
void EnemyBase::ApplyLevelParams(void)
{
    if (level_ <= 1) { return; }

    int levelDiff = level_ - 1;

    // パラメータ上昇
    maxHp_ += LEVEL_UP_STAT * levelDiff;

    hp_ = maxHp_;

    attack_ += LEVEL_UP_STAT * levelDiff;

    defense_ += LEVEL_UP_STAT * levelDiff;
}

// 撃破時の経験値報酬
int EnemyBase::GetExpReward(void) const
{
    return BASE_EXP_REWARD * level_;
}

// 攻撃力を取得
float EnemyBase::GetAttack(void) const
{
    return attack_;
}

// 防御力を取得
float EnemyBase::GetDefense(void) const
{
    return defense_;
}