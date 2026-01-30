#include"Resource.h"

#include<DxLib.h>
#include<EffekseerForDXLib.h>



//コンストラクタ
Resource::Resource()
{
	resType_ = TYPE::NONE;

	path_= "";

	numX_ = -1;

	numY_ = -1;

	sizeX_ = -1;

	sizeY_ = -1;

	handleId_ = -1;

	handleIds_ = nullptr;
}

//コンストラクタ
Resource::Resource(TYPE type, const std::string& path)
{
	resType_ = type;
	path_ = path;

	numX_ = -1;
	numY_ = -1;
	sizeX_ = -1;
	sizeY_ = -1;

	handleId_ = -1;
	handleIds_ = nullptr;
}

//コンストラクタ(IMGS用)
Resource::Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY)
{
	resType_ = type;
	path_ = path;
	numX_ = numX;
	numY_ = numY;
	sizeX_ = sizeX;
	sizeY_ = sizeY;

	handleId_ = -1;
	handleIds_ = nullptr;
}

//デストラクタ
Resource::~Resource()
{

}

//読み込み
void Resource::Load(void)
{
	int prevASync = 0;

	switch (resType_)
	{
	case Resource::TYPE::IMG:
		//画像
		handleId_ = LoadGraph(path_.c_str());
		break;

	case Resource::TYPE::IMGS:
		//画像分割
		handleId_ = LoadDivGraph(path_.c_str(), numX_ * numY_, numX_, numY_, sizeX_, sizeY_, &handleIds_[0]);
		break;
		
	case Resource::TYPE::MASK:
		//マスクがぞ像
		handleId_ = LoadGraph(path_.c_str());
		break;

	case Resource::TYPE::MODEL:
		//モデル
		handleId_ = MV1LoadModel(path_.c_str());
		break;

	case Resource::TYPE::ANIM:
		// アニメーション
		handleId_ = MV1LoadModel(path_.c_str());
		break;

	case Resource::TYPE::EFFEKSEER:
	{ // ← 中括弧を開始
		int prevASync = GetUseASyncLoadFlag();
		SetUseASyncLoadFlag(FALSE);

		handleId_ = LoadEffekseerEffect(path_.c_str());

		SetUseASyncLoadFlag(prevASync);
	} // ← 中括弧を終了
		break;

	case Resource::TYPE::SOUND:
		//サウンド
		handleId_ = LoadSoundMem(path_.c_str());
		break;

	}
}

void Resource::Release(void)
{
	switch (resType_)
	{
	case Resource::TYPE::IMG:
		//画像
		DeleteGraph(handleId_);
		break;

	case Resource::TYPE::IMGS:
	{
		//画像分割
		int num = numX_ * numY_;
		for (int i = 0; i < num; i++)
		{
			DeleteGraph(handleIds_[i]);
		}
		delete[] handleIds_;
		break;
	}

	case Resource::TYPE::MASK:
		//マスク画像
		DeleteGraph(handleId_);
		break;

	case Resource::TYPE::MODEL:
	{
		//モデル
		MV1DeleteModel(handleId_);
		auto ids = duplicateModelIds_;
		for (auto id : ids)
		{
			MV1DeleteModel(id);
		}
	}
		break;

	case Resource::TYPE::ANIM:
	{
		//モデル
		MV1DeleteModel(handleId_);
		auto ids = duplicateModelIds_;
		for (auto id : ids)
		{
			MV1DeleteModel(id);
		}
	}
	break;

	case Resource::TYPE::EFFEKSEER:
		DeleteEffekseerEffect(handleId_);
		break;

	case Resource::TYPE::SOUND:
		//サウンド
		DeleteSoundMem(handleId_);
		break;
	}
}

void Resource::CoopyHandle(int* imgs)
{
	if (handleIds_ == nullptr)
	{
		return;
	}

	int num = numX_ * numY_;
	for (int i = 0; i < num; i++)
	{
		imgs[i] = handleIds_[i];
	}
}

int Resource::GetHandle(void) const
{
	return handleId_;
}
