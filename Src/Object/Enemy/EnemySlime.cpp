#include "EnemySlime.h"

#include "../../Utility/Utility.h"


//コンストラクタ
EnemySlime::EnemySlime(void)
{

	//ターゲットの初期化
	targetPos_ = Utility::VECTOR_ZERO;

	//視野の初期化
	viewRange_ = VIEW_RANGE;

	//視野解除距離の初期化
	lostRange_ = LOST_RANGE;

	//正面ベクトルの初期化
	forward_ = Utility::DIR_F;

	//視野角の初期化
	viewAngle_ = VIEW_ANGLE;

	//追跡中かどうかを初期化
	isChasing_ = false;

	//視野内かどうかを初期化
	isInView_ = false;
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

	trans_.scl = VGet(0.5f, 0.5f, 0.5f);

}

//追従対象
void EnemySlime::SetTargetPos(const VECTOR& pos)
{
	targetPos_ = pos;
}

//更新処理
void EnemySlime::Update(void)
{
	// プレイヤーへの方向ベクトル
	VECTOR dirPlayer = VSub(targetPos_, trans_.pos);
	float distance = static_cast<float>(Utility::MagnitudeF(dirPlayer));
	VECTOR dirNorm = Utility::VNormalize(dirPlayer);

	// プレイヤーが視野内にいるか判定
	double angle = Utility::AngleDeg(forward_, dirNorm);
	bool inView = (distance <= viewRange_ && angle <= viewAngle_ * 0.5);

	// --- 状態更新 ---
	if (inView)
	{
		// プレイヤーを発見 → 追跡開始
		isChasing_ = true;
	}
	else if (distance > lostRange_)
	{ 
		 // 完全に見失ったら追跡解除
		isChasing_ = false;
	}

	// --- 移動処理 ---
	if (isChasing_)
	{
		movePow_ = VScale(dirNorm, moveSpeed_);
	}
	else
	{
		movePow_ = Utility::VECTOR_ZERO;
	}

	// 正面方向更新
	if (!Utility::EqualsVZero(movePow_))
	{
		forward_ = Utility::VNormalize(movePow_);
	}

	//共通更新処理
	EnemyBase::Update();
}

void EnemySlime::Draw(void) const
{
	//共通描画処理
	EnemyBase::Draw();

#ifdef _DEBUG

#endif // _DEBUG

}

void EnemySlime::ApplyData(const EnemyInfo& info)
{
	EnemyBase::ApplyData(info);
}
