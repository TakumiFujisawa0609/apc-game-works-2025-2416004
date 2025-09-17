#pragma once

#include <unordered_map>
#include <memory>

#include "Common/Transform.h"
#include "Common/AnimationController.h"

class UnitBase
{
public:
	//列挙型
	//アニメーション
	enum class ANIM
	{
		NONE,
		IDLE,	//待機
		WALK,	//歩き
	};

	//コンストラクタ
	UnitBase();
	//デストラクタ
	virtual ~UnitBase() = 0;

	//初期化
	virtual void Init(void) = 0;

	//更新
	virtual void Update(void) = 0;

	//描画
	virtual void Draw(void) = 0;

	//解放
	virtual void Release(void) = 0;

	//モデル情報
	const Transform& GetTransform(void) { return trans_; }

	//位置
	const VECTOR& GetPos(void)const { return trans_.pos; }
	//角度
	const VECTOR& GetRot(void)const { return trans_.rot; }
	//大きさ
	const VECTOR& GetScl(void)const { return trans_.scl; }
	//前座標
	const VECTOR& GetPrePos(void)const { return prePos_; }

	//半径
	const float GetRadius(void)const { return radius_; }

	//位置
	void SetPos(const VECTOR& _pos) { trans_.pos = _pos; }

	/// <summary>
	/// 回転処理
	/// </summary>
	/// <param name="_deg">回転する角度(デグリー角)</param>
	/// <param name="_axis">回転方向</param>
	void Turn(const float _deg, const VECTOR& _axis);

protected:

	//モデルの情報
	Transform trans_;

	//前座標
	VECTOR prePos_;

	//半径
	float radius_;

	//速度
	float speed_;

	//移動量
	VECTOR movePow_;

	//アニメーション関係
	std::unique_ptr<AnimationController> anim_;		//アニメーション制御

	//アニメーションリセット
	virtual void InitAnimation(void);
};

