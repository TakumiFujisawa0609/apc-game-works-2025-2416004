#include "CollisionManager.h"
#include "Collision.h"
#include "../../Utility/Utility.h"

// 静的インスタンス
CollisionManager* CollisionManager::instance_ = nullptr;

// シングルトンの生成
void CollisionManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new CollisionManager();
		instance_->Init();
	}
}

// シングルトンの取得
CollisionManager& CollisionManager::GetInstance(void)
{
	return *instance_;
}

// シングルトンの破棄
void CollisionManager::Destroy(void)
{
	delete instance_;
	instance_ = nullptr;
}

// 初期化
void CollisionManager::Init(void)
{
	objects_.clear();
}

// 球の登録
void CollisionManager::RegisterSphere(std::shared_ptr<void> owner, VECTOR* pos, float radius, TAG_TYPE tag, bool push)
{
	auto obj = std::make_shared<CollisionObject>();
	
	obj->owner = owner;
	
	obj->posPtr = pos;
	
	obj->radius = radius;
	
	obj->type = push ? COLLISION_TYPE::SPHERE_PUSH : COLLISION_TYPE::SPHERE;
	
	obj->pushEnabled = push;
	
	obj->tag = tag;
	
	objects_.push_back(obj);
}

// BOXの登録
void CollisionManager::RegisterBox(std::shared_ptr<void> owner, VECTOR* pos, VECTOR min, VECTOR max, TAG_TYPE tag, bool push)
{
	auto obj = std::make_shared<CollisionObject>();
	
	obj->owner = owner;
	
	obj->posPtr = pos;
	
	obj->min = min;
	
	obj->max = max;
	
	obj->type = push ? COLLISION_TYPE::BOX_PUSH : COLLISION_TYPE::BOX;
	
	obj->pushEnabled = push;
	
	obj->tag = tag;
	
	objects_.push_back(obj);
}

// カプセルの登録
void CollisionManager::RegistCapsule(std::shared_ptr<void> owner, VECTOR* pos, const VECTOR& start, const VECTOR& end, float radius, TAG_TYPE tag, bool push)
{
	auto obj = std::make_shared<CollisionObject>();

	obj->owner = owner;

	obj->posPtr = pos;

	obj->capStart = start;

	obj->capEnd = end;

	obj->capRadius = radius;

	obj->type = push ? COLLISION_TYPE::CAPSULE_PUSH : COLLISION_TYPE::CAPSULE;

	obj->pushEnabled = push;

	obj->tag = tag;

	objects_.push_back(obj);
}

// メッシュの登録
void CollisionManager::RegisterMesh(std::shared_ptr<void> owner, int modelId, TAG_TYPE tag, bool push)
{
	// 重複チェック
	for (auto& o : objects_)
	{
		if (o->type == COLLISION_TYPE::MESH && o->modelId == modelId)
			return;
	}

	auto obj = std::make_shared<CollisionObject>();
	
	obj->owner = owner;
	
	obj->modelId = modelId;
	
	obj->type = push ? COLLISION_TYPE::MESH_PUSH : COLLISION_TYPE::MESH;
	
	obj->pushEnabled = push;
	
	obj->tag = tag;
	
	objects_.push_back(obj);
}

// 地面用のメッシュ登録
void CollisionManager::RegisterMeshTile(std::shared_ptr<void> owner, int modelId, VECTOR pos, float tileSize)
{
	auto obj = std::make_shared<CollisionObject>();
	
	obj->owner = owner;
	
	obj->modelId = modelId;
	
	// 押し出し有効
	obj->type = COLLISION_TYPE::MESH_PUSH; 
	
	obj->pushEnabled = true;
	
	obj->tag = TAG_TYPE::GROUND;

	// モデルの中心座標を設定
	obj->center = pos;

	// タイル半径（正方形タイルを対角で近似）
	obj->radius = sqrtf((tileSize * 0.5f) * (tileSize * 0.5f) * 2.0f);

	objects_.push_back(obj);
}

// 全削除
void CollisionManager::Clear(void)
{
	objects_.clear();
}

// 全オブジェクトの当たり判定・押し出し処理
void CollisionManager::Update(void)
{
	size_t n = objects_.size();

	// 距離カリング用
	const float CULL_DISTANCE = 1000.0f; 
	const float CULL_DISTANCE_SQ = CULL_DISTANCE * CULL_DISTANCE;

	for (size_t i = 0; i < n; ++i)
	{
		auto& obj1 = objects_[i];
		VECTOR* pos1 = obj1->posPtr;

		bool isObj1Mesh = (obj1->type == COLLISION_TYPE::MESH || obj1->type == COLLISION_TYPE::MESH_PUSH);

		for (size_t j = i + 1; j < n; ++j)
		{
			auto& obj2 = objects_[j];
			VECTOR* pos2 = obj2->posPtr;

			// タグ的に衝突不要ならスキップ
			if (!CanCollide(obj1->tag, obj2->tag)) { continue; }

			bool isObj2Mesh = (obj2->type == COLLISION_TYPE::MESH || obj2->type == COLLISION_TYPE::MESH_PUSH);

			// 距離カリング用の座標取得
			VECTOR center1 = isObj1Mesh ? obj1->center : (pos1 ? *pos1 : Utility::VECTOR_ZERO);
			VECTOR center2 = isObj2Mesh ? obj2->center : (pos2 ? *pos2 : Utility::VECTOR_ZERO);

			VECTOR diff = VSub(center2, center1);
			float distSq = VDot(diff, diff);

			if (distSq > CULL_DISTANCE_SQ) { continue; }

			// 球同士の当たり判定
			if ((obj1->type == COLLISION_TYPE::SPHERE || obj1->type == COLLISION_TYPE::SPHERE_PUSH) &&
				(obj2->type == COLLISION_TYPE::SPHERE || obj2->type == COLLISION_TYPE::SPHERE_PUSH))
			{
				if (!pos1 || !pos2) continue;

				if (Collision::GetInstance().IsHitSpheres(*pos1, obj1->radius, *pos2, obj2->radius))
				{
					if (obj1->pushEnabled && obj2->pushEnabled)
					{
						VECTOR diff = VSub(*pos2, *pos1);
						float distSq = VDot(diff, diff);

						if (distSq > 0.0001f)
						{
							float dist = sqrtf(distSq);
							VECTOR normal = VScale(diff, 1.0f / dist);
							float overlap = (obj1->radius + obj2->radius) - dist;
							VECTOR pushVec = VScale(normal, overlap * 0.5f);

							*pos1 = VSub(*pos1, pushVec);
							*pos2 = VAdd(*pos2, pushVec);
						}
					}
				}
			}

			// 球とカプセルの当たり判定
			if ((obj1->type == COLLISION_TYPE::SPHERE || obj1->type == COLLISION_TYPE::SPHERE_PUSH) &&
				(obj2->type == COLLISION_TYPE::CAPSULE || obj2->type == COLLISION_TYPE::CAPSULE_PUSH))
			{
				if (!pos1) { continue; }

				// 衝突判定
				if (Collision::GetInstance().IsHitSphereCapsule(*pos1, obj1->radius, obj2->capStart, obj2->capEnd, obj2->capRadius))
				{
					//押し出し処理
					if (obj1->pushEnabled && obj2->pushEnabled)
					{
						// カプセル線分上で球に最も近い点を求める
						VECTOR capVac = VSub(obj1->capEnd, obj2->capStart);

						VECTOR pVec = VSub(*pos1, obj2->capStart);

						float t = VDot(pVec, VNorm(capVac));

						float capLen = Utility::MagnitudeF(capVac);

						float rate = t / capLen;

						VECTOR nearest;

						if (rate <= 0.0f)
						{
							nearest = obj2->capStart; 
						}
						else if (rate >= 1.0f) 
						{
							nearest = obj2->capEnd; 
						}
						else 
						{
							nearest = VAdd(obj2->capStart, VScale(VNorm(capVac), t));
						}

						VECTOR diff = VSub(*pos1, nearest);

						float dist = Utility::MagnitudeF(diff);

						float overlap = (obj1->radius + obj2->capRadius) - dist;

						if (overlap > 0.0f && dist > 0.00001f)
						{
							VECTOR pushDir = VScale(diff, 1.0f / dist);

							VECTOR pushVec = VScale(pushDir, overlap);

							*pos1 = VAdd(*pos1, pushVec);
						}


					}
				}
			}

			// BOX同士の当たり判定
			if ((obj1->type == COLLISION_TYPE::BOX || obj1->type == COLLISION_TYPE::BOX_PUSH) &&
				(obj2->type == COLLISION_TYPE::BOX || obj2->type == COLLISION_TYPE::BOX_PUSH))
			{
				if (!pos1 || !pos2) continue;

				// AABBの衝突を判定
				bool hit =
					(obj1->min.x <= obj2->max.x && obj1->max.x >= obj2->min.x) &&
					(obj1->min.y <= obj2->max.y && obj1->max.y >= obj2->min.y) &&
					(obj1->min.z <= obj2->max.z && obj1->max.z >= obj2->min.z);

				if (hit && obj1->pushEnabled && obj2->pushEnabled)
				{
					// 押し出し量を計算
					float dx = std::min(obj1->max.x, obj2->max.x) - std::max(obj1->min.x, obj2->min.x);
					float dy = std::min(obj1->max.y, obj2->max.y) - std::max(obj1->min.y, obj2->min.y);
					float dz = std::min(obj1->max.z, obj2->max.z) - std::max(obj1->min.z, obj2->min.z);

					// 最小の押し出し量を求める
					if (dx <= dy && dx < dz)
					{
						float push = (obj1->max.x + obj1->min.x > obj2->max.x + obj2->min.x) ? dx * 0.5f : -dx * 0.5f;
						pos1->x += push;
						pos2->x -= push;
					}
					else if (dy < dz)
					{
						float push = (obj1->max.y + obj1->min.y > obj2->max.y + obj2->min.y) ? dy * 0.5f : -dy * 0.5f;
						pos1->y += push;
						pos2->y -= push;
					}
					else
					{
						float push = (obj1->max.z + obj1->min.z > obj2->max.z + obj2->min.z) ? dz * 0.5f : -dz * 0.5f;
						pos1->z += push;
						pos2->z -= push;
					}
				}
			}

			// メッシュと球の当たり判定（カメラと地面は特別処理）
			// obj1 = メッシュ、obj2 = 球
			if (isObj1Mesh && !isObj2Mesh)
			{
				if (!pos2) continue;

				VECTOR hitPos, hitNormal;
				if (Collision::IsHitMeshSphere(obj1->modelId, *pos2, obj2->radius, &hitPos, &hitNormal))
				{
					if (obj1->pushEnabled && obj2->pushEnabled)
					{
						// カメラと地面の場合は最小限の押し出しのみ（判定用）
						if (obj2->tag == TAG_TYPE::CAMERA && obj1->tag == TAG_TYPE::GROUND)
						{
							// 法線が下向きの場合は反転
							if (hitNormal.y < 0.0f)
							{
								hitNormal = VScale(hitNormal, -1.0f);
							}

							// 球の中心から衝突点までのベクトル
							VECTOR toSphere = VSub(*pos2, hitPos);
							float dist = Utility::MagnitudeF(toSphere);

							// めり込み量 = 半径 - 距離
							float penetration = obj2->radius - dist;

							if (penetration > 0.0f)
							{
								// 最小限の押し出し（Y方向のみ、わずかに）
								// これにより isGroundHit 判定ができる
								VECTOR minimalPush = VGet(0.0f, 0.1f, 0.0f);
								*pos2 = VAdd(*pos2, minimalPush);
							}
						}
						else
						{
							// 通常の押し出し処理
							VECTOR toSphere = VSub(*pos2, hitPos);
							float dist = Utility::MagnitudeF(toSphere);
							float penetration = obj2->radius - dist;

							if (penetration > 0.0f)
							{
								if (hitNormal.y < 0.0f)
								{
									hitNormal = VScale(hitNormal, -1.0f);
								}

								VECTOR pushVec = VScale(hitNormal, penetration);
								*pos2 = VAdd(*pos2, pushVec);
							}
						}
					}
				}
			}
			// obj2 = メッシュ、obj1 = 球
			else if (isObj2Mesh && !isObj1Mesh)
			{
				if (!pos1) continue;

				VECTOR hitPos, hitNormal;
				if (Collision::IsHitMeshSphere(obj2->modelId, *pos1, obj1->radius, &hitPos, &hitNormal))
				{
					if (obj1->pushEnabled && obj2->pushEnabled)
					{
						// カメラと地面の場合は最小限の押し出しのみ（判定用）
						if (obj1->tag == TAG_TYPE::CAMERA && obj2->tag == TAG_TYPE::GROUND)
						{
							// 法線が下向きの場合は反転
							if (hitNormal.y < 0.0f)
							{
								hitNormal = VScale(hitNormal, -1.0f);
							}

							// 球の中心から衝突点までのベクトル
							VECTOR toSphere = VSub(*pos1, hitPos);
							float dist = Utility::MagnitudeF(toSphere);

							// めり込み量 = 半径 - 距離
							float penetration = obj1->radius - dist;

							if (penetration > 0.0f)
							{
								// 最小限の押し出し（Y方向のみ、わずかに）
								VECTOR minimalPush = VGet(0.0f, 0.1f, 0.0f);
								*pos1 = VAdd(*pos1, minimalPush);
							}
						}
						else
						{
							// 通常の押し出し処理
							VECTOR toSphere = VSub(*pos1, hitPos);
							float dist = Utility::MagnitudeF(toSphere);
							float penetration = obj1->radius - dist;

							if (penetration > 0.0f)
							{
								if (hitNormal.y < 0.0f)
								{
									hitNormal = VScale(hitNormal, -1.0f);
								}

								VECTOR pushVec = VScale(hitNormal, penetration);
								*pos1 = VAdd(*pos1, pushVec);
							}
						}
					}
				}
			}
		}
	}
}

bool CollisionManager::CanCollide(TAG_TYPE tagA, TAG_TYPE tagB) const
{
	// カメラと壁の衝突
	if ((tagA == TAG_TYPE::CAMERA && tagB == TAG_TYPE::WALL) ||
		(tagA == TAG_TYPE::WALL && tagB == TAG_TYPE::CAMERA))
	{
		return true;
	}

	// カメラと地面の衝突
	if ((tagA == TAG_TYPE::CAMERA && tagB == TAG_TYPE::GROUND) ||
		(tagA == TAG_TYPE::GROUND && tagB == TAG_TYPE::CAMERA))
	{
		return true;
	}

	// 敵同士は当たる
	if (tagA == TAG_TYPE::ENEMY && tagB == TAG_TYPE::ENEMY)
	{
		return true;
	}

	return false;
}