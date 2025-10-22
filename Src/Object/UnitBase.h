#pragma once

#include <DxLib.h>
#include <unordered_map>
#include <memory>

#include "Common/Transform.h"

class AnimationController;


class UnitBase
{
public:

	//アニメーション別
	enum class ANIM
	{
		NONE,
		IDEL,
		WALK,
	};

	//コンストラクタ
	UnitBase(void);

	//デストラクタ
	virtual ~UnitBase(void);

	//リソースの読み込み
	virtual void Load(void);

	//初期化
	virtual void Init(void);

	//更新処理
	virtual void Update(void);

	//描画処理
	virtual void Draw(void) const;

	//解放処理
	virtual void Release(void);

	//モデル情報（非const版）
	Transform& GetTransform(void);

	//モデル情報（const版）
	const Transform& GetTransform(void) const;

	//座標を取得
	const VECTOR& GetPos(void) const;

	//座標を設定
	void SetPos(const VECTOR& pos);

	//回転を取得
	const VECTOR& GetRot(void) const;

	//回転を設定
	void SetRot(const VECTOR& rot);

	//スケールを取得
	const VECTOR& GetScl(void) const;

	//スケールを設定
	void SetScl(const VECTOR& scl);

	//前座標
	const VECTOR& GetPrePos(void) const;

	//半径の取得
	float GetRadius(void) const;

	//半径の設定
	void SetRadius(float r);

	//移動ベクトルの設定
	void SetMovePow(const VECTOR& pow);

	//移動ベクトルの取得
	const VECTOR& GetMovePow(void) const;

	//回転(クォータニオン)
	void Turn(float deg, const VECTOR& axis);

	//アニメーション制御
	void PlayAnim(ANIM aanimType, bool loop, float blendTime);

	//アニメーションの初期化
	void InitAnimaiton(void);

protected:

	//モデル情報
	Transform trans_;

	//座標
	VECTOR prePos_;

	//半径
	float radius_;

	//移動速度
	float speed_;

	//移動力
	VECTOR movePow_;

	//アニメーション
	std::unique_ptr<AnimationController> anim_;

	//アニメーション別
	ANIM currentAnim_;
};
