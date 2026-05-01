#include "Transform.h"

#include<DxLib.h>

#include"../../Utility/Utility.h"

Transform::Transform(void)
{
	modelId = -1;

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
	//大きさ
	matScl = MGetScale(scl);

	//回転
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	//位置
	matPos = MGetTranslate(pos);

	//行列の合成
	MATRIX mat = MGetIdent();

	//ローカル回転合成
	Quaternion  q = quaRot.Mult(quaRotLocal);

	//スケール適用
	mat = MMult(mat, matScl);

	//回転適用
	mat = MMult(mat, q.ToMatrix());

	//位置適用
	mat = MMult(mat, matPos);

	//行列をモデルに判定
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
