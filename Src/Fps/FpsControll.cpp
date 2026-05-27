#include "FpsControll.h"
#include <DxLib.h>
#include <math.h>

// デフォルトコンストラクタ
Fps::Fps(void)
{
    mStartTime_ = 0;
    mCount_ = 0;
    mFps_ = 0.0f;
    mFrameStartTime_ = 0;
}

// デストラクタ
Fps::~Fps(void) {}

// 初期化
void Fps::FpsControll_Initialize(void)
{
    mStartTime_ = GetNowCount();
    mFrameStartTime_ = GetNowCount();
    mCount_ = 0;
    mFps_ = 0.0f;
}

// FPS計算（Nフレームごと）
bool Fps::FpsControll_Update(void)
{
    if (mCount_ == 0)
    {
        mStartTime_ = GetNowCount();
    }

    if (mCount_ == N)
    {
        int t = GetNowCount();
        mFps_ = 1000.0f / ((t - mStartTime_) / (float)N);
        mCount_ = 0;
        mStartTime_ = t;
    }

    mCount_++;
    return true;
}

// FPS表示
void Fps::FpsControll_Draw(void)
{
    DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS: %.1f", mFps_);
}

// フレーム待機（高精度ポーリング + Sleep）
void Fps::FpsControll_Wait(void)
{
    const int targetFrameTime = 1000 / FPS; // 目標フレーム時間(ms)
    int elapsed = GetNowCount() - mFrameStartTime_;

    int sleepTime = targetFrameTime - elapsed - 1; // 1ms手前までSleep
    if (sleepTime > 0)
    {
        Sleep(sleepTime);
    }

    // 1ms未満の微調整
    while (GetNowCount() - mFrameStartTime_ < targetFrameTime)
    {
        // ポーリングで待機
    }

    // 次フレームの基準時間更新
    mFrameStartTime_ = GetNowCount();
}
