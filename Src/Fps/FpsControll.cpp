#include "FpsControll.h"

// デフォルトコンストラクタ
Fps::Fps(void)
{
	_mStartTime = 0;
	_mConut = 0;
	_mFps = 0.0f;
	_mFrameStartTime = 0;
}

// デストラクタ
Fps::~Fps(void)
{
}

// 初期化
void Fps::FpsControll_Initialize()
{
	_mStartTime = GetNowCount();
	_mFrameStartTime = GetNowCount();
	_mConut = 0;
	_mFps = 0.0f;
}

// FPS制御
bool Fps::FpsControll_Update()
{
	if (_mConut == 0)
	{
		_mStartTime = GetNowCount();
	}

	// N フレームごとにFPS計算
	if (_mConut == N)
	{
		int t = GetNowCount();
		_mFps = 1000.0f / ((t - _mStartTime) / (float)N);
		_mConut = 0;
		_mStartTime = t;
	}

	_mConut++;
	return true;
}

// FPS表示
void Fps::FpsControll_Draw()
{
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS: %.1f", _mFps);
}

// 時間測定・待機
void Fps::FpsControll_Wait()
{
	int frameTime = GetNowCount() - _mFrameStartTime;    // このフレームの処理にかかった時間
	int targetFrameTime = 1000 / FPS;                     // 目標フレーム時間（ミリ秒）
	int waitTime = targetFrameTime - frameTime;           // 待つべき時間

	if (waitTime > 0)
	{
		Sleep(waitTime);
	}

	// 次フレームの計測開始
	_mFrameStartTime = GetNowCount();
}