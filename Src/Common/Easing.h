#pragma once

//.@brief イージング計算群
namespace Easing
{
    ///@brief 二次関数イージング（加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuadIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 二次関数イージング（減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値 
    ///@return            補間後の値
    float QuadOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 二次関数イージング（加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuadInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 三次関数イージング（非常にゆっくり加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float CubicIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 三次関数イージング（減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float CubicOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 三次関数イージング（加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float CubicInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 四次関数イージング（強い加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuartIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 四次関数イージング（強い減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuartOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 四次関数イージング（加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuartInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 五次関数イージング（非常に強い加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuintIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 五次関数イージング（非常に強い減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuintOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 五次関数イージング（加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float QuintInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief サインイージング（加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float SineIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief サインイージング（減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float SineOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief サインイージング（加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float SineInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 指数イージング（急加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float ExpIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 指数イージング（急減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float ExpOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 指数イージング（急加速→急減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float ExpInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 円イージング（加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float CircIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 円イージング（減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float CircOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 円イージング（加速→減速）
    ///@param currentTime 現在の時間 
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float CircInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 弾性イージング（バネ加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float ElasticIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 弾性イージング（バネ減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float ElasticOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 弾性イージング（バネ加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float ElasticInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 逆方向に動いてから加速
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@param overshoot   助走量
    ///@return            補間後の値
    float BackIn(float currentTime, float totalTime, float startValue, float endValue, float overshoot);

    ///@brief 減速時に少し逆方向へ動く
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@param overshoot   助走量
    ///@return            補間後の値
    float BackOut(float currentTime, float totalTime, float startValue, float end, float overshoot);

    ///@brief 加速減速＋逆方向挙動
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@param overshoot   助走量
    ///@return            補間後の値
    float BackInOut(float currentTime, float totalTime, float startValue, float end, float overshoot);

    ///@brief バウンド（加速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float BounceIn(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief バウンド（減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float BounceOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief バウンド（加速→減速）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float BounceInOut(float currentTime, float totalTime, float startValue, float endValue);

    ///@brief 線形補間（一定速度）
    ///@param currentTime 現在の時間
    ///@param totalTime   全体の時間
    ///@param startValue  開始値
    ///@param endValue    終了値
    ///@return            補間後の値
    float Linear(float currentTime, float totalTime, float startValue, float endValue);
}






