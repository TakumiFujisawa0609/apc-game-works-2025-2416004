#include "UnitBase.h"

#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Common/Quaternion.h"
#include "Common/AnimationController.h"


//コンストラクタ
UnitBase::UnitBase(void)
{

	//半径の初期化
	radius_ = 0.0f;

	//移動速度の初期化
	speed_ = 0.0f;

	//移動量の初期化
	movePow_ = Utility::VECTOR_ZERO;

	//前座標の初期化
	prePos_ = Utility::VECTOR_ZERO;

	//アニメーションの初期化
	currentAnim_ = ANIM::NONE;

	//モデルの初期化
	trans_.modelId = -1;

	//座標の初期化
	trans_.pos = Utility::VECTOR_ZERO;

	//スケールの初期化
	trans_.scl = Utility::VECTOR_ONE;

	//回転の初期化
	trans_.rot = Utility::VECTOR_ZERO;

	//回転(クォータニオン)の初期化
	trans_.quaRot = Quaternion();
}

//デストラクタ
UnitBase::~UnitBase(void)
{

}

void UnitBase::Load(void)
{
}

void UnitBase::Init(void)
{
}

//更新処理
void UnitBase::Update(void)
{
	//前座標の挿入
	prePos_ = trans_.pos;

	//移動
	trans_.pos = VAdd(trans_.pos, movePow_);

	//モデル情報の設定
	if (trans_.modelId >= 0)
	{
		MV1SetPosition(trans_.modelId, trans_.pos);

		MV1SetRotationXYZ(trans_.modelId, trans_.rot);

		MV1SetScale(trans_.modelId, trans_.scl);
	}

	//アニメーションの更新
	if (anim_)
	{
		anim_->Update();
	}
}

//描画処理
void UnitBase::Draw(void) const
{
	//モデルの描画
	if (trans_.modelId >= 0)
	{
		MV1DrawModel(trans_.modelId);
	}
}

void UnitBase::Release(void)
{
}

//モデル情報（非const版）
Transform& UnitBase::GetTransform(void)
{
	return trans_;
}

//モデル情報（const版）
const Transform& UnitBase::GetTransform(void) const
{
   return trans_;
}

//座標の取得
const VECTOR& UnitBase::GetPos(void) const
{
	return trans_.pos;
}

//座標の設定
void UnitBase::SetPos(const VECTOR& pos)
{
	trans_.pos = pos;
}

//回転の取得
const VECTOR& UnitBase::GetRot(void) const
{
	return trans_.rot;
}

//回転の設定
void UnitBase::SetRot(const VECTOR& rot)
{
	trans_.rot;
}

//スケールの取得
const VECTOR& UnitBase::GetScl(void) const
{
	return trans_.scl;
}

//スケールの設定
void UnitBase::SetScl(const VECTOR& scl)
{
	trans_.scl = scl;
}

//前座標の取得
const VECTOR& UnitBase::GetPrePos(void) const
{
	return prePos_;
}

//半径の取得
float UnitBase::GetRadius(void) const
{
	return radius_;
}

//半径の設定
void UnitBase::SetRadius(float r)
{
	radius_ = r;
}

//移動ベクトルの設定
void UnitBase::SetMovePow(const VECTOR& pow)
{
	movePow_ = pow;
}

//移動ベクトルの取得
const VECTOR& UnitBase::GetMovePow(void) const
{
	return movePow_;
}

//回転(クォータニオン)
void UnitBase::Turn(float deg, const VECTOR& axis)
{
	trans_.quaRot = trans_.quaRot.Mult(trans_.quaRot, Quaternion::AngleAxis(Utility::Deg2RadF(deg), axis));
}

//アニメーションの制御
void UnitBase::PlayAnim(ANIM aanimType, bool loop, float blendTime)
{
	//アニメーションコントローラがない場合停止
	if (!anim_) return;

	//アニメーションインデックスを設定
	int animIndex = static_cast<int>(aanimType);

	//アニメーション再生
	if (currentAnim_ != aanimType)
	{
		anim_->Play(animIndex, loop, blendTime);

		currentAnim_ = aanimType;
	}
}

void UnitBase::InitAnimaiton(void)
{

}



