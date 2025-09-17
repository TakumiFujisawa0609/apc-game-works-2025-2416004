#include"FpsControll.h"

#include <math.h>
#include <DxLib.h>

//デフォルトコンストラクタ
Fps::Fps(void)
{
	_mStartTime = 0;
	_mConut = 0;
	_mFps = 0;
}

//デストラクタ
Fps::~Fps(void)
{

}

//初期化
void Fps::FpsControll_Initialize()
{
	_mStartTime = GetNowCount();
	_mConut = 0;
	_mFps = 0;
}


//FPS制御
bool Fps::FpsControll_Update()
{
	if (_mConut == 0)
	{
		_mStartTime = GetNowCount();
	}

	if (_mConut == N)
	{
		int t = GetNowCount();
		_mFps = 1000.f / ((t - _mStartTime) / (float)N);
		_mConut = 0;
		_mStartTime = t;
	}
	_mConut++;

	return true;
}

//FPS表示
void Fps::FpsControll_Draw()
{
	DrawFormatString(0, 0, GetColor(255, 255, 255), "%.1f", _mFps);
}

//時間測定
void Fps::FpsControll_Wait()
{
	int _tookTime = GetNowCount() - _mStartTime;         //かかった時間
	int _waitTime = _mConut * 1000 / FPS - _tookTime;    //待つべき時間

	if (_waitTime > 0)
	{
		Sleep(_waitTime);
	}
}