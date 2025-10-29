#pragma once

#include<DxLib.h>

#include"../../Common/Vector2.h"

class Collision
{
public:
	//インスタンスの生成
	static void CreateInstance(void);

	//インスタンスの取得
	static Collision& GetInstance(void);

	//初期化処理
	void Init(void);

	// 正方形同士の当たり判定
	bool IsHitBoxes(const Vector2F box1, const float box1Range, const Vector2F box2, const float box2Range)const;

	// 球体同士の衝突判定
	static bool IsHitSpheres(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2);

	//球体とカプセルの衝突判定
	static bool IsHitSphereCapsule(const VECTOR& sphPos, float sphRadius, const VECTOR& capPos1, const VECTOR& capPos2, float capRadius);

	//メッシュと球の衝突判定
	static bool IsHitMeshSphere(int modelId, const VECTOR& sphPos, float sphRadius, VECTOR* hitPos = nullptr, VECTOR* hitNor = nullptr);

private:

	//コンストラクタ
	Collision(void) = default;
	
	//デストラクタ
	~Collision(void) = default;
	
	//インスタンス用
	static Collision* instance_;
};

