#pragma once
#include <DxLib.h>

// FPS制御クラス
class Fps
{
public:
    // FPS計算周期（フレーム数）
    static constexpr int N = 60;    
    
    // 目標FPS
    static constexpr int FPS = 60;  

    // コンストラクタ
    Fps(void);

    // デストラクタ
    ~Fps(void);

    // 初期化
    void FpsControll_Initialize(void);
   
    // 更新処理
    bool FpsControll_Update(void);
    
    // 描画処理
    void FpsControll_Draw(void);
    
    // フレーム待機処理
    void FpsControll_Wait();

private:

    // FPS計算用の開始時間
    int mStartTime_;    

    // フレーム制御用の開始時間
    int mFrameStartTime_; 

    // FPS計算カウンタ
    int mCount_;   

    // 現在のFPS
    float mFps_;            
};
