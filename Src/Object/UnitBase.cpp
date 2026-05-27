#include "../Pch.h"
#include "UnitBase.h"
#include "../Utility/Utility.h"
#include "../Common/Quaternion.h"
#include "Common/AnimationController.h"
#include "../Collider/ColliderBase.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderModel.h"
#include "../Manager/System/CollisionController.h"
#include "../Collider/ColliderCapsule.h"

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

UnitBase::~UnitBase(void)
{
}

void UnitBase::Load(void)
{
}

void UnitBase::Initialize(void)
{

}

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

Transform& UnitBase::GetTransform(void)
{
	return trans_;
}

const Transform& UnitBase::GetTransform(void) const
{
	return trans_;
}

const VECTOR& UnitBase::GetPos(void) const
{
	return trans_.pos;
}

void UnitBase::SetPos(const VECTOR& pos)
{
	trans_.pos = pos;
}

const VECTOR& UnitBase::GetRot(void) const
{
	return trans_.rot;
}

void UnitBase::SetRot(const VECTOR& rot)
{
	trans_.rot = rot;
}

const VECTOR& UnitBase::GetScl(void) const
{
	return trans_.scl;
}

void UnitBase::SetScl(const VECTOR& scl)
{
	trans_.scl = scl;
}

const VECTOR& UnitBase::GetPrePos(void) const
{
	return prePos_;
}

float UnitBase::GetRadius(void) const
{
	return radius_;
}

void UnitBase::SetRadius(float r)
{
	radius_ = r;
}

void UnitBase::SetMovePow(const VECTOR& pow)
{
	movePow_ = pow;
}

const VECTOR& UnitBase::GetMovePow(void) const
{
	return movePow_;
}

void UnitBase::Turn(float deg, const VECTOR& axis)
{
	trans_.quaRot = trans_.quaRot.Mult(trans_.quaRot, Quaternion::AngleAxis(Utility::Deg2RadF(deg), axis));
}

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
		VECTOR colliderPos = collider->GetFollowTarget()->pos;

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
	// 移動処理
	trans_.pos = VAdd(trans_.pos, movePow_);

	// ジャンプ量を加算
	trans_.pos = VAdd(trans_.pos, jumpPow_);

	// 球体同士の押し出し（敵同士・プレイヤーとの押し合い）
	CollisionSphereVsSphere();

	// 敵との衝突（ダメージ判定など）
	CollisionWithEnemy();

	// カプセルとの衝突（剣の攻撃判定など）
	CollisionWithCapsule();

	// 地面との衝突
	CollisionGravity();
}

void UnitBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = Utility::DIR_DOWN;

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
	int lineType = static_cast<int>(COLLIDER_TYPE::LINE);
	if (ownColliders_.count(lineType) == 0) return;

	ColliderLine* colliderLine = dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));
	if (!colliderLine) return;

	VECTOR s = colliderLine->GetWorldStartPos();
	VECTOR e = colliderLine->GetWorldEndPos();

	bool isGrounded = false;
	float maxY = -FLT_MAX;

	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol->GetCollisionTag() != ColliderBase::TAG::STAGE &&
			hitCol->GetCollisionTag() != ColliderBase::TAG::GROUND) continue;

		const ColliderModel* colliderModel = dynamic_cast<const ColliderModel*>(hitCol);
		if (!colliderModel) continue;

		// 線分 vs モデル（地面）
		auto hits = MV1CollCheck_LineDim(colliderModel->GetFollowTarget()->modelId, -1, s, e);

		for (int i = 0; i < hits.HitNum; i++)
		{
			if (colliderModel->IsExcludedFrame(hits.Dim[i].FrameIndex)) continue;

			if (hits.Dim[i].HitPosition.y > maxY)
			{
				maxY = hits.Dim[i].HitPosition.y;
				isGrounded = true;
			}
		}
		MV1CollResultPolyDimTerminate(hits);
	}

	if (isGrounded)
	{
		// 落下中のみ着地判定
		if (jumpPow_.y <= 0.1f)
		{
			trans_.pos.y = maxY;
			jumpPow_ = VGet(0, 0, 0);
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
		if (hitCol->GetShapeType() != ColliderBase::SHAPE::SPHERE) continue;

		const ColliderSphere* hitSphere =
			dynamic_cast<const ColliderSphere*>(hitCol);

		if (hitSphere == nullptr) continue;

		// タグを取得
		ColliderBase::TAG myTag = mySphere->GetCollisionTag();
		ColliderBase::TAG hitTag = hitSphere->GetCollisionTag();

		// 攻撃判定かどうかを先にチェック
		bool isAttackTag = (hitTag == ColliderBase::TAG::FIRE_ATTACK ||
			hitTag == ColliderBase::TAG::WATER_ATTACK ||
			hitTag == ColliderBase::TAG::SWORD);

		// 球体同士の衝突判定
		VECTOR myPos = mySphere->GetLocalPosition();
		VECTOR hitPos = hitSphere->GetLocalPosition();
		float myRadius = mySphere->GetRadius();
		float hitRadius = hitSphere->GetRadius();

		VECTOR diff = VSub(hitPos, myPos);
		float distSq = VDot(diff, diff);
		float radiusSum = myRadius + hitRadius;

		// 衝突判定
		if (distSq < radiusSum * radiusSum && distSq > 0.0001f)
		{
			// 衝突している
			float dist = sqrtf(distSq);
			VECTOR normal = VScale(diff, 1.0f / dist);
			float penetration = radiusSum - dist;

			// 衝突情報の作成
			CollisionInfo info;
			info.myCollider = mySphere;
			info.hitCollider = hitSphere;
			info.hitPosition = VAdd(myPos, VScale(diff, myRadius / dist));
			info.hitNormal = normal;
			info.penetration = penetration;
			info.isValid = true;

			// 攻撃判定の場合
			if (isAttackTag)
			{
				OnCollisionEnter(info);
			}
			// 物理押し出し処理
			else
			{
				// 同じタグの場合
				bool isSameTag = (myTag == hitTag);

				if (isSameTag)
				{
					// ENEMY同士のみ処理（水平方向のみ押し出し）
					if (myTag == ColliderBase::TAG::ENEMY)
					{
						VECTOR pushVec = VGet(normal.x, 0.0f, normal.z);
						float pushLen = sqrtf(pushVec.x * pushVec.x + pushVec.z * pushVec.z);

						if (pushLen > 0.0001f)
						{
							pushVec = VScale(pushVec, (penetration * 0.5f) / pushLen);
							trans_.pos = VSub(trans_.pos, pushVec);
						}

						OnCollisionStay(info);
					}
					// それ以外の同タグはスキップ
				}
				// 異なるタグの場合
				else
				{
					// プレイヤー vs 敵の押し出し（水平方向のみ）
					bool isPlayerVsEnemy =
						(myTag == ColliderBase::TAG::PLAYER && hitTag == ColliderBase::TAG::ENEMY) ||
						(myTag == ColliderBase::TAG::ENEMY && hitTag == ColliderBase::TAG::PLAYER);

					if (isPlayerVsEnemy)
					{
						// 水平方向のみ押し出し
						VECTOR pushVec = VGet(normal.x, 0.0f, normal.z);
						float pushLen = sqrtf(pushVec.x * pushVec.x + pushVec.z * pushVec.z);

						if (pushLen > 0.0001f)
						{
							pushVec = VScale(pushVec, (penetration * 0.5f) / pushLen);
							trans_.pos = VSub(trans_.pos, pushVec);
						}

						OnCollisionStay(info);
					}
				}
			}
		}
	}
}

void UnitBase::CollisionWithEnemy(void)
{
	// カプセルコライダー
	int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);
	int sphereType = static_cast<int>(COLLIDER_TYPE::SPHERE);

	// カプセルコライダーがあればそれを利用、なければスフィアを利用
	ColliderBase* myCol = nullptr;

	if (ownColliders_.count(capsuleType) > 0)
	{
		myCol = ownColliders_.at(capsuleType);
	}
	else if (ownColliders_.count(sphereType) > 0)
	{
		myCol = ownColliders_.at(sphereType);
	}

	if (myCol == nullptr) { return; }


	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// 火・水攻撃はENEMYタグのみチェック
		if (myCol->GetCollisionTag() == ColliderBase::TAG::FIRE_ATTACK ||
			myCol->GetCollisionTag() == ColliderBase::TAG::WATER_ATTACK)
		{
			// 火・水攻撃の場合はエネミー以外スキップ
			if (hitCol->GetCollisionTag() != ColliderBase::TAG::ENEMY) { continue; }
		}
		else
		{
			// それ以外（プレイヤー・エネミー）の既存ロジック
			if (hitCol->GetCollisionTag() != ColliderBase::TAG::ENEMY && 
				hitCol->GetCollisionTag() != ColliderBase::TAG::PLAYER) { continue; }
			if (hitCol->GetCollisionTag() == myCol->GetCollisionTag()) { continue; }
		}

		// 衝突判定
		CollisionInfo info;
		bool isHit = false;

		// カプセル同士の衝突判定
		if (myCol->GetShapeType() == ColliderBase::SHAPE::CAPSULE &&
			hitCol->GetShapeType() == ColliderBase::SHAPE::CAPSULE)
		{
			const ColliderCapsule* myCapsule = dynamic_cast<const ColliderCapsule*>(myCol);
			const ColliderCapsule* hitCapsule = dynamic_cast<const ColliderCapsule*>(hitCol);

			if (myCapsule && hitCapsule)
			{
				isHit = CollisionController::GetInstance().CheckCollision(myCapsule, hitCapsule, info);
			}
		}
		// カプセルと球体の衝突判定（自分がカプセル、相手が球体）
		else if (myCol->GetShapeType() == ColliderBase::SHAPE::CAPSULE &&
			hitCol->GetShapeType() == ColliderBase::SHAPE::SPHERE)
		{
			const ColliderCapsule* myCapsule = dynamic_cast<const ColliderCapsule*>(myCol);
			const ColliderSphere* hitSphere = dynamic_cast<const ColliderSphere*>(hitCol);

			if (myCapsule && hitSphere)
			{
				isHit = CollisionController::GetInstance().CheckCollision(hitSphere, myCapsule, info);

				if (isHit)
				{
					const ColliderBase* temp = info.myCollider;
					info.myCollider = info.hitCollider;
					info.hitCollider = temp;
					info.hitNormal = VScale(info.hitNormal, -1.0f);
				}
			}
		}
		// 球体とカプセルの衝突判定（自分が球体、相手がカプセル）
		else if (myCol->GetShapeType() == ColliderBase::SHAPE::SPHERE && 
			hitCol->GetShapeType() == ColliderBase::SHAPE::CAPSULE)
		{
			const ColliderSphere* mySphere = dynamic_cast<const ColliderSphere*>(myCol);
			const ColliderCapsule* hitCapsule = dynamic_cast<const ColliderCapsule*>(hitCol);

			if (mySphere && hitCapsule)
			{
				isHit = CollisionController::GetInstance().CheckCollision(mySphere, hitCapsule, info);
			}
		}
		// 球体同士の衝突判定
		else if (myCol->GetShapeType() == ColliderBase::SHAPE::SPHERE && 
			hitCol->GetShapeType() == ColliderBase::SHAPE::SPHERE)
		{
			const ColliderSphere* mySphere = dynamic_cast<const ColliderSphere*>(myCol);
			const ColliderSphere* hitSphere = dynamic_cast<const ColliderSphere*>(hitCol);

			if (mySphere && hitSphere)
			{
				VECTOR myPos = mySphere->GetLocalPosition();
				VECTOR hitPos = hitSphere->GetLocalPosition();
				VECTOR diff = VSub(hitPos, myPos);
				float distSq = VDot(diff, diff);
				float radiusSum = mySphere->GetRadius() + hitSphere->GetRadius();


				if (distSq < radiusSum * radiusSum)
				{
					float dist = sqrtf(distSq + 0.0001f);

					info.myCollider = mySphere;
					info.hitCollider = hitSphere;
					info.hitPosition = VAdd(mySphere->GetLocalPosition(), VScale(diff, mySphere->GetRadius() / dist));
					info.hitNormal = dist > 0.0001f ? VScale(diff, 1.0f / dist) : Utility::DIR_UP;
					info.penetration = radiusSum - dist;
					info.isValid = true;
					isHit = true;

				}

			}
		}

		if (isHit)
		{
			OnCollisionEnter(info);

			if (myCol->GetCollisionTag() == ColliderBase::TAG::ENEMY && hitCol->GetCollisionTag() == ColliderBase::TAG::PLAYER)
			{


				VECTOR pushVec = VGet(info.hitNormal.x, 0.0f, info.hitNormal.z);
				float pushLen = VSize(pushVec);

				if (pushLen > 0.0001f)
				{

					VECTOR finalPush = VScale(pushVec, info.penetration / pushLen);

					trans_.pos = VAdd(trans_.pos, finalPush);
				}
			}
		}
	}
}

void UnitBase::CollisionWithCapsule(void)
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
		// カプセル以外はスキップ
		if (hitCol->GetShapeType() != ColliderBase::SHAPE::CAPSULE) continue;

		const ColliderCapsule* hitCapsule =
			dynamic_cast<const ColliderCapsule*>(hitCol);

		if (hitCapsule == nullptr) continue;

		// 衝突判定
		CollisionInfo info;
		if (CollisionController::GetInstance().CheckCollision(mySphere, hitCapsule, info))
		{
			// コールバック呼び出し
			OnCollisionEnter(info);
		}
	}
}