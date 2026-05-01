#include "../Pch.h"
#include "Easing.h"


//time        = 進行度
//totalTime   = 目標時間
//start       = 開始値
//end         = 目標値
//s           = 助走量（Back系のみ）

namespace
{
    // Back (行き過ぎ・助走)
    static constexpr float BACK_OVERSHOOT = 1.70158f;         // 助走・超過の振幅
    static constexpr float BACK_INOUT_SCALE = 1.525f;         // InOut時の振幅補正

    // Elastic (弾性)
    static constexpr float ELASTIC_PERIOD_RATIO = 0.3f;       // 振動の基本周期
    static constexpr float ELASTIC_INOUT_PERIOD_SCALE = 1.5f; // InOut時の周期補正
    static constexpr float ELASTIC_PHASE_DIVISOR = 4.0f;      // 位相計算の補正値

    // Exponential (指数)
    static constexpr float EXPONENTIAL_POWER = 10.0f;         // 指数カーブの強さ

    // Bounce (跳ね返り)
    static constexpr float BOUNCE_COEFFICIENT = 7.5625f;      // 落下加速度係数
    static constexpr float BOUNCE_DIVISOR = 2.75f;            // 段階判定の基準時間

    // バウンド各段階の接地時間オフセット
    static constexpr float BOUNCE_STEP2_OFFSET = 0.75f;       // 第2段階の接地タイミング
    static constexpr float BOUNCE_STEP3_OFFSET = 0.9375f;     // 第3段階の接地タイミング
    static constexpr float BOUNCE_STEP4_OFFSET = 0.984375f;   // 第4段階の接地タイミング
}

float Easing::QuadIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化
    const float changeAmount = endValue - startValue;         // 全体の変化量

    // 二次関数による加速
    return changeAmount * normalizedTime * normalizedTime + startValue;
}

float Easing::QuadOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化
    const float changeAmount = endValue - startValue;         // 全体の変化量

    // 二次関数による減速
    return -changeAmount * normalizedTime * (normalizedTime - 2.0f) + startValue;
}

float Easing::QuadInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    
    float normalizedTime = currentTime / (totalTime / 2.0f);  // 前半・後半判定用の正規化

    // 前半：加速
    if (normalizedTime < 1.0f)
    {
        return halfChange * normalizedTime * normalizedTime + startValue;
    }

    normalizedTime -= 1.0f;

    // 後半：減速
    return -halfChange * (normalizedTime * (normalizedTime - 2.0f) - 1.0f) + startValue;
}

float Easing::CubicIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化
    const float changeAmount = endValue - startValue;         // 全体の変化量

    // 三次関数による加速
    return changeAmount * normalizedTime * normalizedTime * normalizedTime + startValue;
}

float Easing::CubicOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;                // 全体の変化量
    const float normalizedTime = (currentTime / totalTime) - 1.0f;   // 時間を0～1へ正規化し、-1～0へシフト

    // 三次関数による減速
    return changeAmount * (normalizedTime * normalizedTime * normalizedTime + 1.0f) + startValue;
}

float Easing::CubicInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    float normalizedTime = currentTime / (totalTime / 2.0f);  // 前半・後半判定用の正規化

    // 前半：三次加速
    if (normalizedTime < 1.0f)
    {
        return halfChange * normalizedTime * normalizedTime * normalizedTime + startValue;
    }

    normalizedTime -= 2.0f;

    // 後半：三次減速
    return halfChange * (normalizedTime * normalizedTime * normalizedTime + 2.0f) + startValue;
}

float Easing::QuartIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化
    const float changeAmount = endValue - startValue;         // 全体の変化量

    // 四次関数による加速
    return changeAmount * (normalizedTime * normalizedTime * 
           normalizedTime * normalizedTime) + startValue;
}

float Easing::QuartOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;                // 全体の変化量
    const float normalizedTime = (currentTime / totalTime) - 1.0f;   // 時間を0～1へ正規化し、-1～0へシフト

    // 四次関数による減速
    return -changeAmount * (normalizedTime * normalizedTime * 
           normalizedTime * normalizedTime - 1.0f) + startValue;
}

float Easing::QuartInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    float normalizedTime = currentTime / (totalTime / 2.0f);  // 前半・後半判定用の正規化

    // 前半：四次加速
    if (normalizedTime < 1.0f)
    {
        return halfChange * (normalizedTime * normalizedTime * 
               normalizedTime * normalizedTime) + startValue;
    }

    normalizedTime -= 2.0f;

    // 後半：四次減速
    return -halfChange * (normalizedTime * normalizedTime * 
           normalizedTime * normalizedTime - 2.0f) + startValue;
}

float Easing::QuintIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float normalizedTime = currentTime / totalTime;     // 全体の変化量
    const float changeAmount = endValue - startValue;         // 時間を0～1へ正規化し、-1～0へシフト

    // 五次関数による減速
    return changeAmount * (normalizedTime * normalizedTime * 
           normalizedTime * normalizedTime * normalizedTime) + startValue;
}

float Easing::QuintOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;                // 全体の変化量
    const float normalizedTime = (currentTime / totalTime) - 1.0f;   // 時間を0～1へ正規化し、-1～0へシフト

    // 五次関数による減速
    return changeAmount * (normalizedTime * normalizedTime * 
           normalizedTime * normalizedTime * normalizedTime + 1.0f) + startValue;
}

float Easing::QuintInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    float normalizedTime = currentTime / (totalTime / 2.0f);  // 前半・後半判定用の正規化

    // 前半：五次加速
    if (normalizedTime < 1.0f)
    {
        return halfChange * (normalizedTime * normalizedTime * 
               normalizedTime * normalizedTime * normalizedTime) + startValue;
    }

    normalizedTime -= 2.0f;

    // 後半：五次減速
    return halfChange * (normalizedTime * normalizedTime * 
           normalizedTime * normalizedTime * normalizedTime + 2.0f) + startValue;
}

float Easing::SineIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 正弦波（余弦）による加速
    return -changeAmount * cosf(normalizedTime * (DX_PI_F / 2.0f)) + changeAmount + startValue;
}

float Easing::SineOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 正弦波による減速
    return changeAmount * sinf(normalizedTime * (DX_PI_F / 2.0f)) + startValue;
}

float Easing::SineInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 正弦波による加速・減速
    return -halfChange * (cosf(normalizedTime * DX_PI_F) - 1.0f) + startValue;
}

float Easing::ExpIn(float currentTime, float totalTime, float startValue, float endValue)
{
    // 開始時間以前は開始値を返す
    if (currentTime <= 0.0f) { return startValue; }           

    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 指数関数による急激な加速
    return changeAmount * powf(2.0f, EXPONENTIAL_POWER * (normalizedTime - 1.0f)) + startValue;
}

float Easing::ExpOut(float currentTime, float totalTime, float startValue, float endValue)
{
    // 終了時間以降は終了値を返す
    if (currentTime >= totalTime) { return endValue; }

    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 指数関数による急激な減速
    return changeAmount * (-powf(2.0f, -EXPONENTIAL_POWER * normalizedTime) + 1.0f) + startValue;
}

float Easing::ExpInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    // 開始時間以前は開始値を返す
    if (currentTime <= 0.0f) { return startValue; }

    // 終了時間以降は終了値を返す
    if (currentTime >= totalTime) { return endValue; }

    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    float normalizedTime = currentTime / (totalTime / 2.0f);  // 前半・後半判定用の正規化

    // 前半：指数加速
    if (normalizedTime < 1.0f)
    {
        return halfChange * powf(2.0f, EXPONENTIAL_POWER * (normalizedTime - 1.0f)) + startValue;
    }

    normalizedTime -= 1.0f;

    // 後半：指数減速
    return halfChange * (-powf(2.0f, -EXPONENTIAL_POWER * normalizedTime) + 2.0f) + startValue;
}

float Easing::CircIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 円の公式を用いた加速
    return -changeAmount * (sqrtf(1.0f - normalizedTime * normalizedTime) - 1.0f) + startValue;
}

float Easing::CircOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;                // 全体の変化量
    const float normalizedTime = (currentTime / totalTime) - 1.0f;   // 時間を0～1へ正規化し、-1～0へシフト

    // 円の公式を用いた減速
    return changeAmount * sqrtf(1.0f - normalizedTime * normalizedTime) + startValue;
}

float Easing::CircInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount / 2.0f;             // 変化量の半分
    float normalizedTime = currentTime / (totalTime / 2.0f);  // 前半・後半判定用の正規化

    // 前半：円の加速
    if (normalizedTime < 1.0f)
    {
        return -halfChange * (sqrtf(1.0f - normalizedTime * normalizedTime) - 1.0f) + startValue;
    }

    normalizedTime -= 2.0f;

    // 後半：円の減速
    return halfChange * (sqrtf(1.0f - normalizedTime * normalizedTime) + 1.0f) + startValue;
}

float Easing::ElasticIn(float currentTime, float totalTime, float startValue, float endValue)
{
    // 開始時間以前は開始値を返す
    if (currentTime <= 0.0f) { return startValue; }

    // 終了時間以降は終了値を返す
    if (currentTime >= totalTime) { return endValue; }

    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化
    const float period = totalTime * ELASTIC_PERIOD_RATIO;    // 振動周期
    const float phaseShift = period / ELASTIC_PHASE_DIVISOR;  // 位相の補正
    const float shiftedTime = normalizedTime - 1.0f;          // 計算用に時間をシフト

    // 指数加速と正弦波による弾性的な動き（入り）
    return -(changeAmount * powf(2.0f, EXPONENTIAL_POWER * shiftedTime) * 
           sinf((shiftedTime * totalTime - phaseShift) * (2.0f * DX_PI_F) / period)) + startValue;
}

float Easing::ElasticOut(float currentTime, float totalTime, float startValue, float endValue)
{
    // 開始時間以前は開始値を返す
    if (currentTime <= 0.0f) { return startValue; }

    // 終了時間以降は終了値を返す
    if (currentTime >= totalTime) { return endValue; }

    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化
    const float period = totalTime * ELASTIC_PERIOD_RATIO;    // 振動周期
    const float phaseShift = period / ELASTIC_PHASE_DIVISOR;  // 位相の補正

    // 指数減速と正弦波による弾性的な動き（戻り）
    return changeAmount * powf(2.0f, -EXPONENTIAL_POWER * normalizedTime) * 
        sinf((normalizedTime * totalTime - phaseShift) * (2.0f * DX_PI_F) / period) +
        changeAmount + startValue;
}

float Easing::ElasticInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    // 開始時間以前は開始値を返す
    if (currentTime <= 0.0f) { return startValue; }

    // 終了時間以降は終了値を返す
    if (currentTime >= totalTime) { return endValue; }

    const float changeAmount = endValue - startValue;                                      // 全体の変化量
    const float halfChange = changeAmount * 0.5f;                                          // 変化量の半分
    const float halfTotalTime = totalTime * 0.5f;                                          // 所要時間の半分
    float normalizedTime = currentTime / halfTotalTime;                                    // 前半・後半判定用の正規化
    const float period = totalTime * (ELASTIC_PERIOD_RATIO * ELASTIC_INOUT_PERIOD_SCALE);  // 補正後の周期
    const float phaseShift = period / ELASTIC_PHASE_DIVISOR;                               // 位相の補正

    // 前半：弾性的な加速
    if (normalizedTime < 1.0f)
    {
        normalizedTime -= 1.0f;

        return -halfChange * (powf(2.0f, EXPONENTIAL_POWER * normalizedTime) * 
               sinf((normalizedTime * totalTime - phaseShift) * 
               (2.0f * DX_PI_F) / period)) + startValue;
    }

    normalizedTime -= 1.0f;

    // 後半：弾性的な減速
    return (halfChange * powf(2.0f, -EXPONENTIAL_POWER * normalizedTime) * 
           sinf((normalizedTime * totalTime - phaseShift) * (2.0f * DX_PI_F) / period)) +
           changeAmount + startValue;
}

float Easing::BackIn(float currentTime, float totalTime, float startValue, float endValue, float overshoot)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float normalizedTime = currentTime / totalTime;     // 時間を0～1へ正規化

    // 少し逆方向に引いてから加速
    return changeAmount * normalizedTime * normalizedTime * 
           ((overshoot + 1.0f) * normalizedTime - overshoot) + startValue;
}

float Easing::BackOut(float currentTime, float totalTime, float startValue, float endValue, float overshoot)
{
    const float changeAmount = endValue - startValue;                // 全体の変化量
    const float normalizedTime = (currentTime / totalTime) - 1.0f;   // 時間を0～1へ正規化し、-1～0へシフト

    // 目標値を少し通り過ぎてから戻る
    return changeAmount * (normalizedTime * normalizedTime * 
           ((overshoot + 1.0f) * normalizedTime + overshoot) + 1.0f) + startValue;
}

float Easing::BackInOut(float currentTime, float totalTime, float startValue, float endValue, float overshoot)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount * 0.5f;             // 変化量の半分
    const float halfTotalTime = totalTime * 0.5f;             // 所要時間の半分
    float normalizedTime = currentTime / halfTotalTime;       // 前半・後半判定用の正規化
    const float overValue = overshoot * BACK_INOUT_SCALE;     // InOut用の超過量補正

    // 前半：加速（予備動作あり）
    if (normalizedTime < 1.0f)
    {
        return halfChange * (normalizedTime * normalizedTime * 
               ((overValue + 1.0f) * normalizedTime - overValue)) + startValue;
    }

    normalizedTime -= 2.0f;

    // 後半：減速（超過後の戻りあり）
    return halfChange * (normalizedTime * normalizedTime * 
           ((overValue + 1.0f) * normalizedTime + overValue) + 2.0f) + startValue;
}

float Easing::BounceIn(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量

    // BounceOutを時間的に反転させて計算
    return changeAmount - BounceOut(totalTime - currentTime, 
           totalTime, 0.0f, changeAmount) + startValue;
}

float Easing::BounceOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    float normalizedTime = currentTime / totalTime;           // 時間を0～1へ正規化

    // 1回目の落下
    if (normalizedTime < (1.0f / BOUNCE_DIVISOR))
    {
        return changeAmount * (BOUNCE_COEFFICIENT * normalizedTime * normalizedTime) + startValue;
    }

    // 2回目のバウンド
    if (normalizedTime < (2.0f / BOUNCE_DIVISOR))
    {
        normalizedTime -= (1.5f / BOUNCE_DIVISOR);
        return changeAmount * (BOUNCE_COEFFICIENT * normalizedTime * 
               normalizedTime + BOUNCE_STEP2_OFFSET) + startValue;
    }

    // 3回目のバウンド
    if (normalizedTime < (2.5f / BOUNCE_DIVISOR))
    {
        normalizedTime -= (2.25f / BOUNCE_DIVISOR);
        return changeAmount * (BOUNCE_COEFFICIENT * normalizedTime * 
               normalizedTime + BOUNCE_STEP3_OFFSET) + startValue;
    }

    // 最後の微小バウンド
    normalizedTime -= (2.625f / BOUNCE_DIVISOR);
    return changeAmount * (BOUNCE_COEFFICIENT * normalizedTime * 
           normalizedTime + BOUNCE_STEP4_OFFSET) + startValue;
}

float Easing::BounceInOut(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量
    const float halfChange = changeAmount * 0.5f;             // 変化量の半分
    const float halfTotalTime = totalTime * 0.5f;             // 所要時間の半分

    // 前半：バウンドしながら加速
    if (currentTime < halfTotalTime)
    {
        return BounceIn(currentTime * 2.0f, totalTime, 0.0f, changeAmount) * 0.5f + startValue;
    }

    // 後半：バウンドしながら減速
    return BounceOut(currentTime * 2.0f - totalTime, totalTime, 
           0.0f, changeAmount) * 0.5f + halfChange + startValue;
}

float Easing::Linear(float currentTime, float totalTime, float startValue, float endValue)
{
    const float changeAmount = endValue - startValue;         // 全体の変化量

    // 一定速度での変化（線形補間）
    return changeAmount * currentTime / totalTime + startValue;
}