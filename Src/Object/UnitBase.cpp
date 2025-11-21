#include "UnitBase.h"
#include "../Utility/Utility.h"
#include "../Common/Quaternion.h"
#include "Common/AnimationController.h"
#include "../Collider/ColliderBase.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderModel.h"
#include "../Manager/System/CollisionController.h"

// コンストラクタ
UnitBase::UnitBase(void)
{
	// 半径の初期化
	radius_ = 0.0f;

	// 移動速度の初期化
	speed_ = 0.0f;

	// 移動量の初期化
	movePow_ = Utility::VECTOR_ZERO;

	// 前座標の初期化
	prePos_ = Utility::VECTOR_ZERO;

	// アニメーションの初期化
	currentAnim_ = ANIM::NONE;

	// モデルの初期化
	trans_.modelId = -1;

	// 座標の初期化
	trans_.pos = Utility::VECTOR_ZERO;

	// スケールの初期化
	trans_.scl = Utility::VECTOR_ONE;

	// 回転の初期化
	trans_.rot = Utility::VECTOR_ZERO;

	// 回転(クォータニオン)の初期化
	trans_.quaRot = Quaternion();

	// ジャンプ量の初期化
	jumpPow_ = Utility::VECTOR_ZERO;
}

// デストラクタ
UnitBase::~UnitBase(void)
{
}

void UnitBase::Load(void)
{
}

void UnitBase::Init(void)
{
}

// 更新処理
void UnitBase::Update(void)
{
	// 前座標の挿入
	prePos_ = trans_.pos;

	// 重力計算
	CalcGravityPow();

	// 衝突判定
	Collision();

	// モデル情報の設定
	if (trans_.modelId >= 0)
	{
		MV1SetPosition(trans_.modelId, trans_.pos);
		MV1SetRotationXYZ(trans_.modelId, trans_.rot);
		MV1SetScale(trans_.modelId, trans_.scl);
	}

	// アニメーションの更新
	if (anim_)
	{
		anim_->Update();
	}

	trans_.Update();
}

// 描画処理
void UnitBase::Draw(void) const
{
	// モデルの描画
	if (trans_.modelId >= 0)
	{
		MV1DrawModel(trans_.modelId);
	}

#ifdef _DEBUG
	// 所有しているコライダの描画
	for (const auto& own : ownColliders_)
	{
		own.second->Draw();
	}
#endif // _DEBUG
}

void UnitBase::Release(void)
{
	// 自身のコライダ解放
	for (auto& own : ownColliders_)
	{
		delete own.second;
	}
}

// モデル情報（非const版）
Transform& UnitBase::GetTransform(void)
{
	return trans_;
}

// モデル情報（const版）
const Transform& UnitBase::GetTransform(void) const
{
	return trans_;
}

// 座標の取得
const VECTOR& UnitBase::GetPos(void) const
{
	return trans_.pos;
}

// 座標の設定
void UnitBase::SetPos(const VECTOR& pos)
{
	trans_.pos = pos;
}

// 回転の取得
const VECTOR& UnitBase::GetRot(void) const
{
	return trans_.rot;
}

// 回転の設定
void UnitBase::SetRot(const VECTOR& rot)
{
	trans_.rot = rot;
}

// スケールの取得
const VECTOR& UnitBase::GetScl(void) const
{
	return trans_.scl;
}

// スケールの設定
void UnitBase::SetScl(const VECTOR& scl)
{
	trans_.scl = scl;
}

// 前座標の取得
const VECTOR& UnitBase::GetPrePos(void) const
{
	return prePos_;
}

// 半径の取得
float UnitBase::GetRadius(void) const
{
	return radius_;
}

// 半径の設定
void UnitBase::SetRadius(float r)
{
	radius_ = r;
}

// 移動ベクトルの設定
void UnitBase::SetMovePow(const VECTOR& pow)
{
	movePow_ = pow;
}

// 移動ベクトルの取得
const VECTOR& UnitBase::GetMovePow(void) const
{
	return movePow_;
}

// 回転(クォータニオン)
void UnitBase::Turn(float deg, const VECTOR& axis)
{
	trans_.quaRot = trans_.quaRot.Mult(trans_.quaRot, Quaternion::AngleAxis(Utility::Deg2RadF(deg), axis));
}

// アニメーションの制御
void UnitBase::PlayAnim(ANIM aanimType, bool loop, float blendTime)
{
	// アニメーションコントローラがない場合停止
	if (!anim_) return;

	// アニメーションインデックスを設定
	int animIndex = static_cast<int>(aanimType);

	// アニメーション再生
	if (currentAnim_ != aanimType)
	{
		anim_->Play(animIndex, loop, blendTime);
		currentAnim_ = aanimType;
	}
}

void UnitBase::InitAnimaiton(void)
{
}

// コライダシステム

const ColliderBase* UnitBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return nullptr;
	}
	return ownColliders_.at(key);
}

void UnitBase::AddHitCollider(const ColliderBase* hitCollider)
{
	// 重複チェック
	for (const auto& c : hitColliders_)
	{
		if (c == hitCollider)
		{
			return;
		}
	}
	hitColliders_.emplace_back(hitCollider);
}

void UnitBase::ClearHitCollider(void)
{
	hitColliders_.clear();
}

void UnitBase::AddHitCollidersInRange(const std::vector<const ColliderBase*>& colliders, float maxDistance)
{
	// 距離の2乗で比較（sqrt回避で高速化）
	float maxDistSq = maxDistance * maxDistance;

	for (const auto& collider : colliders)
	{
		// コライダの追従先座標を取得
		VECTOR colliderPos = collider->GetFollow()->pos;

		// 自分との距離を計算（XZ平面のみ、Y軸は無視）
		float dx = colliderPos.x - trans_.pos.x;
		float dz = colliderPos.z - trans_.pos.z;
		float distSq = dx * dx + dz * dz;

		// 範囲内なら登録
		if (distSq <= maxDistSq)
		{
			AddHitCollider(collider);
		}
	}
}

void UnitBase::Collision(void)
{
	// 1. 移動処理
	trans_.pos = VAdd(trans_.pos, movePow_);

	// 2. ジャンプ量を加算
	trans_.pos = VAdd(trans_.pos, jumpPow_);

	// 3. 球体同士の押し出し（敵同士・プレイヤーとの押し合い）
	CollisionSphereVsSphere();

	// 4. 敵との衝突（ダメージ判定など）
	CollisionWithEnemy();

	// 5. 地面との衝突
	CollisionGravity();
}

void UnitBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = Utility::DIR_D;

	// 重力
	VECTOR gravity = VScale(dirGravity, GRAVITY_POW);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 最大速度を超えないようにする
	float dot = VDot(dirGravity, jumpPow_);
	if (dot > SPEED_MAX_JUMP_DOWN)
	{
		// 最大速度
		VECTOR vy = VScale(dirGravity, SPEED_MAX_JUMP_DOWN);

		// 重力方向以外の速度(X,Z)
		VECTOR vxz = VSub(jumpPow_, VScale(dirGravity, dot));

		jumpPow_ = VAdd(vy, vxz);
	}
}

void UnitBase::CollisionGravity(void)
{
	// 線分コライダ
	int lineType = static_cast<int>(COLLIDER_TYPE::LINE);

	// 線分コライダが無ければ処理を抜ける
	if (ownColliders_.count(lineType) == 0) { return; }

	// 線分コライダ情報
	ColliderLine* colliderLine =
		dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));

	if (colliderLine == nullptr) { return; }

	// 線分の始点と終点を取得
	VECTOR s = colliderLine->GetPosStart();
	VECTOR e = colliderLine->GetPosEnd();

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// ステージ・地面以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE &&
			hitCol->GetTag() != ColliderBase::TAG::GROUND) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) { continue; }

		// ステージモデル(地面)との衝突
		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollow()->modelId, -1, s, e);

		bool isGrounded = false;
		VECTOR bestHitPos = trans_.pos;
		float maxY = -FLT_MAX;

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			// 除外フレームは無視
			if (colliderModel->IsExcludeFrame(hit.FrameIndex))
			{
				continue;
			}

			// 最もY座標が高い衝突点を採用
			if (hit.HitPosition.y > maxY)
			{
				maxY = hit.HitPosition.y;
				bestHitPos = hit.HitPosition;
				isGrounded = true;
			}
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

		if (isGrounded)
		{
			// 衝突地点から少し上に移動
			trans_.pos = VAdd(bestHitPos, VScale(Utility::DIR_U, 2.0f));

			// ジャンプリセット
			jumpPow_ = Utility::VECTOR_ZERO;
		}
	}
}

void UnitBase::CollisionSphereVsSphere(void)
{
	// 球体コライダ
	int sphereType = static_cast<int>(COLLIDER_TYPE::SPHERE);

	// 球体コライダが無ければ処理を抜ける
	if (ownColliders_.count(sphereType) == 0) return;

	ColliderSphere* mySphere =
		dynamic_cast<ColliderSphere*>(ownColliders_.at(sphereType));

	if (mySphere == nullptr) return;

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// 球体以外はスキップ
		if (hitCol->GetShape() != ColliderBase::SHAPE::SPHERE) continue;

		const ColliderSphere* hitSphere =
			dynamic_cast<const ColliderSphere*>(hitCol);

		if (hitSphere == nullptr) continue;

		// 同じタグ同士の判定条件を変更
		// ENEMYタグ同士は押し出しを行う、それ以外の同じタグはスキップ
		bool isSameTag = (hitCol->GetTag() == mySphere->GetTag());
		bool isEnemyVsEnemy = (mySphere->GetTag() == ColliderBase::TAG::ENEMY &&
			hitSphere->GetTag() == ColliderBase::TAG::ENEMY);

		// 同じタグかつENEMY同士でない場合はスキップ
		if (isSameTag && !isEnemyVsEnemy) continue;

		// 球体同士の衝突判定
		VECTOR myPos = mySphere->GetPos();
		VECTOR hitPos = hitSphere->GetPos();
		float myRadius = mySphere->GetRadius();
		float hitRadius = hitSphere->GetRadius();

		VECTOR diff = VSub(hitPos, myPos);
		float distSq = VDot(diff, diff);
		float radiusSum = myRadius + hitRadius;

		if (distSq < radiusSum * radiusSum && distSq > 0.0001f)
		{
			// 衝突している
			float dist = sqrtf(distSq);
			VECTOR normal = VScale(diff, 1.0f / dist);
			float penetration = radiusSum - dist;

			// ENEMY同士の場合は水平方向のみ押し出し
			if (isEnemyVsEnemy)
			{
				// 水平方向の押し出しベクトル
				VECTOR pushVec = VGet(normal.x, 0.0f, normal.z);
				float pushLen = sqrtf(pushVec.x * pushVec.x + pushVec.z * pushVec.z);

				if (pushLen > 0.0001f)
				{
					// 正規化して押し出し量を適用
					pushVec = VScale(pushVec, (penetration * 0.5f) / pushLen);
					trans_.pos = VSub(trans_.pos, pushVec);
				}
			}
			else
			{
				// それ以外は通常の押し出し（全方向）
				VECTOR pushVec = VScale(normal, penetration * 0.5f);
				trans_.pos = VSub(trans_.pos, pushVec);
			}

			// コールバック呼び出し
			CollisionInfo info;
			info.myCollider = mySphere;
			info.hitCollider = hitSphere;
			info.hitPosition = VAdd(myPos, VScale(diff, myRadius / dist));
			info.hitNormal = normal;
			info.penetration = penetration;
			info.isValid = true;

			OnCollisionStay(info);
		}
	}
}

void UnitBase::CollisionWithEnemy(void)
{
	// 球体コライダ
	int sphereType = static_cast<int>(COLLIDER_TYPE::SPHERE);

	if (ownColliders_.count(sphereType) == 0) return;

	ColliderSphere* mySphere =
		dynamic_cast<ColliderSphere*>(ownColliders_.at(sphereType));

	if (mySphere == nullptr) return;

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// 敵タグ以外はスキップ（自分も敵の場合は別のタグと判定）
		if (hitCol->GetTag() != ColliderBase::TAG::ENEMY &&
			hitCol->GetTag() != ColliderBase::TAG::PLAYER) continue;

		// 自分と同じタグはスキップ
		if (hitCol->GetTag() == mySphere->GetTag()) continue;

		// 球体以外はスキップ
		if (hitCol->GetShape() != ColliderBase::SHAPE::SPHERE) continue;

		const ColliderSphere* hitSphere =
			dynamic_cast<const ColliderSphere*>(hitCol);

		if (hitSphere == nullptr) continue;

		// 衝突判定
		VECTOR myPos = mySphere->GetPos();
		VECTOR hitPos = hitSphere->GetPos();
		float myRadius = mySphere->GetRadius();
		float hitRadius = hitSphere->GetRadius();

		VECTOR diff = VSub(hitPos, myPos);
		float distSq = VDot(diff, diff);
		float radiusSum = myRadius + hitRadius;

		if (distSq < radiusSum * radiusSum)
		{
			// 衝突している（ダメージ処理など）
			float dist = sqrtf(distSq + 0.0001f);

			CollisionInfo info;
			info.myCollider = mySphere;
			info.hitCollider = hitSphere;
			info.hitPosition = VAdd(myPos, VScale(diff, myRadius / dist));
			info.hitNormal = dist > 0.0001f ? VScale(diff, 1.0f / dist) : VGet(0, 1, 0);
			info.penetration = radiusSum - dist;
			info.isValid = true;

			OnCollisionEnter(info);
		}
	}
}