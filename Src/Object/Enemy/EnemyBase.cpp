#include "EnemyBase.h"

#include "../../Utility/Utility.h"

//コンストラクタ
EnemyBase::EnemyBase(void)
{
	hp_ = MAX_HP;

	maxHp_ = MAX_HP;

	moveSpeed_ = DEFAULT_SPEED;

	radius_ = 0.0f;
}

//初期化
void EnemyBase::Init(const VECTOR& startPos)
{
	trans_.pos = startPos;

	prePos_ = startPos;

	trans_.scl = Utility::VECTOR_ONE;

	movePow_ = Utility::VECTOR_ZERO;

	currentAnim_ = ANIM::NONE;

	SetParam();
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
