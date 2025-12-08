#pragma once
#include "Common/Transform.h"

class AnimationController;
class ColliderBase;
struct CollisionInfo;

class UnitBase
{
public:
	// アニメーション別
	enum class ANIM
	{
		NONE,
		IDEL,
		WALK,
		ATTACK,
	};

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		NONE = -1,
		LINE = 0,      // 線分（地面判定用）
		SPHERE,        // 球体（本体の当たり判定）
		CAPSULE,       // カプセル（体全体）
		MODEL,         // モデル（地形）
		MAX,
	};

	// コンストラクタ
	UnitBase(void);

	// デストラクタ
	virtual ~UnitBase(void);

	// リソースの読み込み
	virtual void Load(void);

	// 初期化
	virtual void Init(void);

	// 更新処理
	virtual void Update(void);

	// 描画処理
	virtual void Draw(void) const;

	// 解放処理
	virtual void Release(void);

	// モデル情報（非const版）
	Transform& GetTransform(void);

	// モデル情報（const版）
	const Transform& GetTransform(void) const;

	// 座標を取得
	const VECTOR& GetPos(void) const;

	// 座標を設定
	void SetPos(const VECTOR& pos);

	// 回転を取得
	const VECTOR& GetRot(void) const;

	// 回転を設定
	void SetRot(const VECTOR& rot);

	// スケールを取得
	const VECTOR& GetScl(void) const;

	// スケールを設定
	void SetScl(const VECTOR& scl);

	// 前座標
	const VECTOR& GetPrePos(void) const;

	// 半径の取得
	float GetRadius(void) const;

	// 半径の設定
	void SetRadius(float r);

	// 移動ベクトルの設定
	void SetMovePow(const VECTOR& pow);

	// 移動ベクトルの取得
	const VECTOR& GetMovePow(void) const;

	// 回転(クォータニオン)
	void Turn(float deg, const VECTOR& axis);

	// アニメーション制御
	void PlayAnim(ANIM aanimType, bool loop, float blendTime);

	// アニメーションの初期化
	void InitAnimaiton(void);

	// 自身の衝突情報取得
	const std::map<int, ColliderBase*>& GetOwnColliders(void) const
	{
		return ownColliders_;
	}

	// 特定の自身の衝突情報取得
	const ColliderBase* GetOwnCollider(int key) const;

	// 衝突対象となるコライダを登録
	void AddHitCollider(const ColliderBase* hitCollider);

	// 衝突対象となるコライダをクリア
	void ClearHitCollider(void);

	// 距離カリング用：有効範囲内の衝突相手のみを登録
	void AddHitCollidersInRange(const std::vector<const ColliderBase*>& colliders, float maxDistance);

	// 衝突時のコールバック（派生クラスでオーバーライド可能）
	virtual void OnCollisionEnter(const CollisionInfo& info) {}
	virtual void OnCollisionStay(const CollisionInfo& info) {}
	virtual void OnCollisionExit(const CollisionInfo& info) {}

protected:
	// 重力
	static constexpr float GRAVITY_POW = 1.0f;

	// 最大落下速度
	static constexpr float SPEED_MAX_JUMP_DOWN = 30.0f;

	// モデル情報
	Transform trans_;

	// 座標
	VECTOR prePos_;

	// 半径
	float radius_;

	// 移動速度
	float speed_;

	// 移動力
	VECTOR movePow_;

	// アニメーション
	std::unique_ptr<AnimationController> anim_;

	// アニメーション別
	ANIM currentAnim_;

	// 自身の衝突情報
	std::map<int, ColliderBase*> ownColliders_;

	// 衝突相手の情報
	std::vector<const ColliderBase*> hitColliders_;

	// ジャンプ量
	VECTOR jumpPow_;

	// 衝突判定
	virtual void Collision(void);

	// 重力計算
	virtual void CalcGravityPow(void);

	// 地面との衝突判定
	void CollisionGravity(void);

	// 球体同士の押し出し処理
	void CollisionSphereVsSphere(void);

	// 敵との衝突処理（ダメージ判定）
	void CollisionWithEnemy(void);

	// コライダの初期化（派生クラスで実装）
	virtual void InitCollider(void) = 0;

	// カプセルとの衝突処理（剣の攻撃判定）
	void CollisionWithCapsule(void);
};