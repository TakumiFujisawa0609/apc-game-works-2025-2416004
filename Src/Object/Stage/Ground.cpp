#include "Ground.h"

#include "../../Manager/Generic/ResourceManager.h"
#include "../../Utility/Utility.h"

//コンストラクタ
Ground::Ground(void)
{
	//モデルの初期化
	modelId_ = -1;

	//座標の初期化
	pos_ = Utility::VECTOR_ZERO;
	
}

//デストラクタ
Ground::~Ground(void)
{

}

//初期化
void Ground::Init(const VECTOR& pos, int modelId)
{
	//座標を初期化
	pos_ = pos;

	//モデル読み込み
	modelId_ = modelId_;

}

//描画処理
void Ground::Draw(void)
{
	if (modelId_ == -1) return;

	MV1SetPosition(modelId_, pos_);

	MV1DrawModel(modelId_);
}

int Ground::GetModelId(void) const
{
	return modelId_;
}
