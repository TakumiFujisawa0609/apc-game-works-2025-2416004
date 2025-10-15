#include "EnemyBase.h"

#include "../../Utility/Utility.h"


//コンストラクタ
EnemyBase::EnemyBase(void)
{
	hp_ = 0.0f;

	maxHp_ = 0.0f;

	moveSpeed_ = 0.0f;

	type_ = "";
}

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

	maxHp_ = info.hp;
	
	hp_ = info.hp;
	
	moveSpeed_ = info.speed;

	radius_ = info.radius;
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
