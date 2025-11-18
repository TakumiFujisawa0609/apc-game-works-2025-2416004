#include "EnemyBase.h"

#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Manager/System/CollisionController.h"


//コンストラクタ
EnemyBase::EnemyBase(void) :
	hp_(0.0f),
	maxHp_(0.0f),
	moveSpeed_(0.0f),
	type_(""),
	viewRange_(0.0f),
	lostRange_(0.0f),
	forward_(0.0f),
	viewAngle_(0.0f),
	isChasing_(false),
	isInView_(false)
{

	forward_ = Utility::DIR_F;
}

// 読み込み
void EnemyBase::Load(int modelId)
{
	trans_.modelId = modelId;

	trans_.SetModel(trans_.modelId);
}

//初期化
void EnemyBase::Init(const VECTOR& startPos)
{
	trans_.pos = startPos;

	prePos_ = startPos;

	trans_.scl = Utility::VECTOR_ONE;

	movePow_ = Utility::VECTOR_ZERO;

	currentAnim_ = ANIM::NONE;

	InitCollider();

	// CollisionControllerに登録
	CollisionController::GetInstance().RegisterUnit(this);
}

void EnemyBase::InitCollider(void)
{
	// 球体コライダの作成
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
}

//更新処理
void EnemyBase::Update(void)
{
	UnitBase::Update();
}

//描画処理
void EnemyBase::Draw(void) const
{
	//モデル描画
	UnitBase::Draw();

	//デバック用当たり判定可視化
#ifdef _DEBUG
	DrawCapsule3D(trans_.pos, trans_.pos, radius_, 12, 0xff0000, 0xff0000, false);
#endif // _DEBUG

}

//CSVデータを適用
void EnemyBase::ApplyData(const EnemyInfo& info)
{
	type_ = info.type;

	maxHp_ = info.param.maxHp;
	
	hp_ = info.param.hp;
	
	moveSpeed_ = info.param.speed;

	radius_ = info.param.radius;
}

void EnemyBase::SetForward(const VECTOR& forward)
{
	forward_ = forward;
}

void EnemyBase::SetViewAngle(float angle)
{
	viewAngle_ = angle;
}

//体力を設定
void EnemyBase::SetHp(float hp)
{
	hp_ = std::clamp(hp, 0.0f, maxHp_);
}

//体力を取得
float EnemyBase::GetHp(void) const
{
	return hp_;
}

//ダメージ処理
void EnemyBase::TakeDamage(float damage)
{
	hp_ -= damage;

	if (hp_ < 0.0f) { hp_ = 0.0f; }
}

const std::string& EnemyBase::GetType(void) const
{
	return type_;
}