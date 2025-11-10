#include "EnemyBase.h"

#include "../../Utility/Utility.h"


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

	// 当たり判定用座標の初期化
	collisionPos_ = VAdd(trans_.pos, GetCollisionOffset());
}

//更新処理
void EnemyBase::Update(void)
{
	UnitBase::Update();

	// 当たり判定用の座標を更新
	collisionPos_ = VAdd(trans_.pos, GetCollisionOffset());
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

// 当たり判定用の座標ポインタを取得
VECTOR* EnemyBase::GetCollisionPosPtr(void)
{
	return &collisionPos_;
}

// 当たり判定のオフセット
VECTOR EnemyBase::GetCollisionOffset(void) const
{
	return Utility::VECTOR_ZERO;
}
