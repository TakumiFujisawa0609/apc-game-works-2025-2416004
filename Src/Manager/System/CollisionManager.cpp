#define NOMINMAX

#include "CollisionManager.h"
#include "Collision.h"

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
void CollisionManager::RegisterSphere(std::shared_ptr<void> owner, std::shared_ptr<VECTOR> pos, float radius, TAG_TYPE tag, bool push)
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
void CollisionManager::RegisterBox(std::shared_ptr<void> owner, std::shared_ptr<VECTOR> pos, VECTOR min, VECTOR max, TAG_TYPE tag, bool push)
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

// 全削除
void CollisionManager::Clear(void)
{
	objects_.clear();
}

// 全オブジェクトの当たり判定・押し出し処理
void CollisionManager::Update(void)
{
	size_t n = objects_.size();

	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = i + 1; j < n; ++j)
		{
			auto& obj1 = objects_[i];

			auto& obj2 = objects_[j];

			// タグ的に衝突不要ならスキップ
			if (!CanCollide(obj1->tag, obj2->tag))
				continue;

			//参照ロック
			std::shared_ptr<VECTOR> pos1 = obj1->posPtr.lock();

			std::shared_ptr<VECTOR> pos2 = obj2->posPtr.lock();
			
			if (!pos1 || !pos2) continue;

			// 球同士の当たり判定
			if ((obj1->type == COLLISION_TYPE::SPHERE || obj1->type == COLLISION_TYPE::SPHERE_PUSH) &&
				(obj2->type == COLLISION_TYPE::SPHERE || obj2->type == COLLISION_TYPE::SPHERE_PUSH))
			{
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

			// BOX同士の当たり判定
			if ((obj1->type == COLLISION_TYPE::BOX || obj1->type == COLLISION_TYPE::BOX_PUSH) &&
				obj2->type == COLLISION_TYPE::BOX || obj2->type == COLLISION_TYPE::BOX_PUSH)
			{

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
						float push = (obj1->max.y + obj1->min.y > obj2->max.y + obj2->min.y) ? dy + 0.5f : -dy * 0.5f;

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
		}
	}
}

bool CollisionManager::CanCollide(TAG_TYPE tagA, TAG_TYPE tagB) const
{
	// 敵同士は当たる
	return (tagA == TAG_TYPE::ENEMY && tagB == TAG_TYPE::ENEMY);
}
