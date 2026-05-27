#include "../../Pch.h"
#include "Transform.h"
#include"../../Utility/Utility.h"

Transform::Transform(void)
{
	// モデルハンドルを無効値(-1)で初期化
	modelId = -1;

	// 各要素を初期値(1.0 または 0.0)に設定
	scl = Utility::VECTOR_ONE;
	rot = Utility::VECTOR_ZERO;
	pos = Utility::VECTOR_ZERO;
	localPos = Utility::VECTOR_ZERO;

	// 行列を単位行列で初期化
	matScl = MGetIdent();
	matRot = MGetIdent();
	matPos = MGetIdent();

	// 回転を無回転で初期化
	quaRot = Quaternion();
	quaRotLocal = Quaternion();
}

Transform::Transform(int model)
{
	modelId = model;

	scl = Utility::VECTOR_ONE;
	rot = Utility::VECTOR_ZERO;
	pos = Utility::VECTOR_ZERO;
	localPos = Utility::VECTOR_ZERO;

	matScl = MGetIdent();
	matRot = MGetIdent();
	matPos = MGetIdent();
	quaRot = Quaternion();
	quaRotLocal = Quaternion();
}

Transform::~Transform(void)
{

}

void Transform::Update(void)
{
	// スケール行列の作成
	matScl = MGetScale(scl);

	// 回転情報の更新（クォータニオンからオイラー角と行列を算出）
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	// 位置行列の作成
	matPos = MGetTranslate(pos);

	// 合成用行列の初期化（単位行列）
	MATRIX mat = MGetIdent();

	// ワールド回転とローカル回転を合成
	Quaternion q = quaRot.Mult(quaRotLocal);

	// 行列の合成：スケール -> 回転 -> 位置 の順で適用
	mat = MMult(mat, matScl);
	mat = MMult(mat, q.ToMatrix());
	mat = MMult(mat, matPos);

	// モデルハンドルが有効な場合のみ、計算した行列をモデルに反映
	if (modelId != -1)
	{
		MV1SetMatrix(modelId, mat);
	}
}

void Transform::Release(void)
{

}

void Transform::SetModel(int model)
{
	modelId = model;
}

VECTOR Transform::GetForward(void) const
{
	return GetDir(Utility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(Utility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(Utility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(Utility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(Utility::DIR_UP);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(Utility::DIR_DOWN);
}

VECTOR Transform::GetDir(const VECTOR& vec) const
{
	return quaRot.PosAxis(vec);
}

VECTOR* Transform::GetPosPtr(void)
{
	return &pos;
}

const VECTOR* Transform::GetPosPtr(void) const
{
	return &pos;
}
