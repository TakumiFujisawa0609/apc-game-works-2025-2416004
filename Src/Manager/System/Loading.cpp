#include "Loading.h"

#include <DxLib.h>
#include <iostream>

//静的インスタンス
Loading* Loading::instance_ = nullptr;

//インスタンスの生成
void Loading::CreateInstance(void)
{
	if (!instance_)
	{
		instance_ = new Loading();
		instance_->Init();
	}
}

//インスタンスの取得
Loading* Loading::GetInstance(void)
{
	return instance_;
}

//インスタンスの破棄
void Loading::DestroyInstance(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

//デストラクタ
Loading::~Loading(void)
{
	if (loadingThread_.joinable())
	{
		loadingThread_.join();
	}
}

//初期化処理
void Loading::Init(void)
{
	isLoading_ = false;
	progress_ = 0.0f;
}

//非同期ロード開始
void Loading::StartAsyncLoad(std::function<void()> loadFunc)
{
	if (isLoading_) return;

	Init();
	isLoading_ = true;

	//別スロットでロード処理
	loadingThread_ = std::thread(&Loading::ThreadFunc, this, loadFunc);
	loadingThread_.detach();
}

//非同期ロード用のスレッド関数。
void Loading::ThreadFunc(std::function<void()> loadFunc)
{
	try
	{
		if (loadFunc)
		{
			// 実際のロード処理
			loadFunc();
		}

		// サンプルとして疑似進捗を増やす
		for (int i = 0; i <= 100; i++)
		{
			progress_ = i;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	catch (...)
	{
		std::cerr << "ロード中に例外発生！" << std::endl;
	}

	EndAsyncLoad();
}

//更新処理
void Loading::Update(void)
{

}

//描画処理
void Loading::Draw(void)
{
	//黒背景 + 進行バー
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	//背景
	DrawBox(100, 300, 300, 320, GetColor(255, 255, 255), true);

	//進捗バー
	DrawBox(100, 300, 100 + 2 * progress_, 320, GetColor(0, 255, 0), true);
}

//ロード完了
void Loading::EndAsyncLoad(void)
{
	isLoading_ = false;
	progress_ = 100;
}

bool Loading::IsLoading(void) const
{
	return isLoading_;
}

int Loading::GetProgress(void) const
{
	return progress_;
}




