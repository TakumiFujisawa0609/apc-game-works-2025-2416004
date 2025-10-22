#include "Loading.h"

#include <DxLib.h>
#include <iostream>
#include "../../Application.h"

// インスタンスを初期化する
Loading* Loading::instance_ = nullptr;

// インスタンスを生成する
void Loading::CreateInstance(void)
{
    if (!instance_)
    {
        instance_ = new Loading();
        instance_->Init();
    }
}

// インスタンスを取得する
Loading* Loading::GetInstance(void)
{
    return instance_;
}

// インスタンスを破棄する
void Loading::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

// デストラクタ
Loading::~Loading(void)
{
    // スレッドが実行中なら待機する
    if (loadingThread_.joinable())
    {
        loadingThread_.join();
    }
}

// 初期化する
void Loading::Init(void)
{
    isLoading_ = false;
    progress_ = 0.0f;
}

// 非同期ロードを開始する
void Loading::StartAsyncLoad(std::function<void()> loadFunc)
{
    // 既にロード中なら無視する
    if (isLoading_) return;

    // 初期化してロード開始フラグを立てる
    Init();
    isLoading_ = true;

    // スレッドを開始する
    loadingThread_ = std::thread(&Loading::ThreadFunc, this, loadFunc);

    // スレッドを切り離す（デタッチする）
    loadingThread_.detach();
}

// 非同期ロード処理を行う
void Loading::ThreadFunc(std::function<void()> loadFunc)
{
    try
    {
        // 実際のロード処理を行う
        if (loadFunc)
        {
            loadFunc();
        }

        // 疑似的に進捗を増加させる
        for (int i = 0; i <= 100; i++)
        {
            progress_ = static_cast<float>(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (...)
    {
        std::cerr << "ロード中に例外が発生しました。" << std::endl;
    }

    // ロード完了処理を行う
    EndAsyncLoad();
}

// 更新する
void Loading::Update(void)
{
    // ここでは特に処理を行わないが、将来的な拡張用
}

// 描画する
void Loading::Draw(void)
{
    // 画面サイズを取得する
    const int screenW = Application::SCREEN_SIZE_X;
    const int screenH = Application::SCREEN_SIZE_Y;

    // 背景を黒で塗りつぶす
    SetDrawScreen(DX_SCREEN_BACK);
    ClearDrawScreen();
    DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);

    // 進行バーのサイズを設定する
    const int barW = 400;
    const int barH = 40;

    // 画面中央座標を求める
    const int centerX = screenW / 2;
    const int centerY = screenH / 2;

    // 枠を描画する
    DrawBox(centerX - barW / 2, centerY - barH / 2,
        centerX + barW / 2, centerY + barH / 2,
        GetColor(255, 255, 255), FALSE);

    // 進捗バーを描画する
    int progressWidth = static_cast<int>(barW * progress_.load() / 100.0f);
    DrawBox(centerX - barW / 2, centerY - barH / 2,
        centerX - barW / 2 + progressWidth, centerY + barH / 2,
        GetColor(0, 255, 0), TRUE);

    // テキストを描画する
    DrawFormatString(centerX - 80, centerY - 10,
        GetColor(255, 255, 255), "Loading... %d%%", static_cast<int>(progress_.load()));
}

// ロード完了処理を行う
void Loading::EndAsyncLoad(void)
{
    isLoading_ = false;
    progress_ = 100.0f;
}

// ロード中か確認する
bool Loading::IsLoading(void) const
{
    return isLoading_;
}

// 進捗率を取得する
int Loading::GetProgress(void) const
{
    return static_cast<int>(progress_.load());
}
