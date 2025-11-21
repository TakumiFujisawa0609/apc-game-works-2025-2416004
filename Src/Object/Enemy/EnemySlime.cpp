#include "EnemySlime.h"
#include "../../Utility/Utility.h"
#include "../../Collider/ColliderSphere.h"
#include "../../Collider/ColliderLine.h"

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

	// ★スライム専用のコライダオフセットを再設定
	UpdateColliderOffset();
}

// コライダのオフセットを更新
void EnemySlime::UpdateColliderOffset(void)
{
	// 球体コライダを取得
	auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
	if (it != ownColliders_.end())
	{
		ColliderSphere* sphere = dynamic_cast<ColliderSphere*>(it->second);
		if (sphere)
		{
			// 既存のコライダを削除して新しいオフセットで再作成
			delete sphere;
			ownColliders_.erase(it);

			// 新しいオフセット（Y座標を上げる）
			VECTOR newOffset = VGet(0.0f, 30.0f, 0.0f);  // 30単位上に

			ColliderSphere* newSphere = new ColliderSphere(
				ColliderBase::TAG::ENEMY,
				&trans_,
				newOffset,
				radius_
			);

			ownColliders_.emplace(
				static_cast<int>(COLLIDER_TYPE::SPHERE),
				newSphere
			);
		}
	}
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
	// 球体コライダのオフセット位置を可視化
	auto it = ownColliders_.find(static_cast<int>(COLLIDER_TYPE::SPHERE));
	if (it != ownColliders_.end())
	{
		const ColliderSphere* sphere = dynamic_cast<const ColliderSphere*>(it->second);
		if (sphere)
		{
			VECTOR colliderPos = sphere->GetPos();

			// コライダの中心を緑の球で表示
			DrawSphere3D(colliderPos, 10.0f, 8, GetColor(0, 255, 0), GetColor(0, 255, 0), TRUE);

			// エネミーの中心からコライダ中心への線
			DrawLine3D(trans_.pos, colliderPos, GetColor(0, 255, 255));

			// オフセット値を表示
			VECTOR screenPos = ConvWorldPosToScreenPos(VAdd(colliderPos, VGet(0, 50, 0)));
			DrawFormatString(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
				GetColor(0, 255, 255), "Offset Y: 30");
		}
	}
#endif // _DEBUG
}

void EnemySlime::ApplyData(const EnemyInfo& info)
{
	EnemyBase::ApplyData(info);
}