#include "Loading.h"

#include <DxLib.h>
#include <iostream>

#include "../../Application.h"

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
	loadingThread_ = std::thread([this, loadFunc]()
		{
			if (loadFunc) loadFunc();
			for (int i = 0; i < 100; i++)
			{
				progress_ = i;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			EndAsyncLoad();
		});
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
	// 画面サイズを Application から取得
	const int screenW = Application::SCREEN_SIZE_X;
	const int screenH = Application::SCREEN_SIZE_Y;

	// 背景黒
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);

	// 進行バーサイズ
	const int barW = 400;  // 緑バーの幅100%で400px
	const int barH = 40;

	// 中央位置
	const int centerX = screenW / 2;
	const int centerY = screenH / 2;

	// 進行バーの枠
	DrawBox(centerX - barW / 2, centerY - barH / 2, centerX + barW / 2, centerY + barH / 2, GetColor(255, 255, 255), FALSE);

	// 進捗バー（緑）
	int progressWidth = static_cast<int>(barW * progress_.load() / 100.0f);
	DrawBox(centerX - barW / 2, centerY - barH / 2, centerX - barW / 2 + progressWidth, centerY + barH / 2, GetColor(0, 255, 0), TRUE);

	// 進捗テキスト（バーの上に表示）
	DrawFormatString(centerX - 80, centerY - 10, GetColor(255, 255, 255), "Loading... %d%%", static_cast<int>(progress_.load()));
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




