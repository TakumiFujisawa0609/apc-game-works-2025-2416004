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

	/// <summary>
	/// 正方形同士の当たり判定
	/// </summary>
	/// <param name="_box1">四角形１の位置</param>
	/// <param name="_box1Range">四角形１の長さ</param>
	/// <param name="_box2">四角形２の位置</param>
	/// <param name="_box2Range">四角形２の長さ</param>
	/// <returns>当たっているか(true:当たっている)</returns>
	bool IsHitBoxes(const Vector2F box1, const float box1Range, const Vector2F box2, const float box2Range)const;

	// 球体同士の衝突判定
	static bool IsHitSpheres(const VECTOR& pos1, float radius1, const VECTOR& pos2, float radius2);

	//球体とカプセルの衝突判定
	static bool IsHitSphereCapsule(const VECTOR& sphPos, float sphRadius, const VECTOR& capPos1, const VECTOR& capPos2, float capRadius);

private:

	//コンストラクタ＆デストラクタ
	Collision(void) = default;
	~Collision(void) = default;
	//インスタンス用
	static Collision* instance_;
};

