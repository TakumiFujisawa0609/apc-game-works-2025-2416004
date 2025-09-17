#pragma once

//FPSクラス(PFSオブジェクトの設計図)
class Fps
{
public:

	//定数宣言
	static constexpr int N = 60;         //平均をとるサンプル数
	static constexpr int FPS = 60;       //設定下fps

private:
	
	//変数宣言
	int _mStartTime;           //測定開始時刻
	int _mConut;               //カウンタ
	float _mFps;               //fps

public:
	//デフォルトコンストラクタ
	Fps(void);

	//デストラクタ
	~Fps(void);

	// 初期処理(最初の１回のみ実行)
	void FpsControll_Initialize();

	// 更新処理(毎フレーム実行)
	bool FpsControll_Update();

	// 描画処理(毎フレーム実行)
	void FpsControll_Draw();

	// 解放処理(最後の１回のみ実行)
	void FpsControll_Wait();

};
