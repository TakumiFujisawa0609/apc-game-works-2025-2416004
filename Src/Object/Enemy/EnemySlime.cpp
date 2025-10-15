#include "EnemySlime.h"

#include "../../Utility/Utility.h"

//コンストラクタ
EnemySlime::EnemySlime(void)
{

	targetPos_ = Utility::VECTOR_ZERO;
}

//読み込み
void EnemySlime::Load(int modelId)
{
	//モデルの読み込み
	trans_.modelId = modelId;
	trans_.SetModel(trans_.modelId);

}

//初期化
void EnemySlime::Init(const VECTOR& startPos)
{
	//基底クラスの初期化
	EnemyBase::Init(startPos);

	trans_.scl = Utility::VECTOR_ONE;


}

//追従対象
void EnemySlime::SetTargetPos(const VECTOR& pos)
{
	targetPos_ = pos;
}

//更新処理
void EnemySlime::Update(void)
{
	//ターゲット方向へ移動
	VECTOR dir = VSub(targetPos_, trans_.pos);

	float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (len > 1.0f)
	{
		//正規化
		dir = VScale(dir, 1.0f / len);

		movePow_ = VScale(dir, moveSpeed_);
	}
	else
	{
		movePow_ = Utility::VECTOR_ZERO;
	}

	//共通更新処理
	EnemyBase::Update();
}

void EnemySlime::Draw(void) const
{
	//共通描画処理
	EnemyBase::Draw();

#ifdef _DEBUG
	DrawCapsule3D(trans_.pos, trans_.pos, radius_, 12, 0x0000ff, 0x0000ff, false);
#endif // _DEBUG

}

void EnemySlime::ApplyData(const EnemyInfo& info)
{
	EnemyBase::ApplyData(info);
}
