#pragma once

/// <summary>
/// 二次関数的なイージング。開始時に遅く、後半で速くなる加速
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuadIn(float time, float totalTime, float start, float end);

/// <summary>
/// 二次関数的なイージング。最初は速く、終わりに向かって減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuadQut(float time, float totalTime, float start, float end);

/// <summary>
/// 二次関数的なイージング。最初に加速し、最後に減速する（加速と減速の両方を含む）
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuadInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 三次関数的なイージング。開始時に非常にゆっくり、後半で速くなる
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float CubicIn(float time, float totalTime, float start, float end);

/// <summary>
/// 三次関数的なイージング。最初は速く、後で減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float CubicOut(float time, float totalTime, float start, float end);

/// <summary>
/// 三次関数的なイージング。加速と減速の両方がある
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float CubicInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 四次関数的なイージング。開始時に非常にゆっくり、後半で急激に加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuartIn(float time, float totalTime, float start, float end);

/// <summary>
/// 四次関数的なイージング。最初は速く、終わりに向かって急激に減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuartOut(float time, float totalTime, float start, float end);

/// <summary>
/// 四次関数的なイージング。加速と減速が両方ある
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuartInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 五次関数的なイージングで最初は非常にゆっくり始まり後半で急激に加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuintIn(float time, float totalTime, float start, float end);

/// <summary>
/// 五次関数的なイージングで最初は速く後半に向かって滑らかに減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuintOut(float time, float totalTime, float start, float end);

/// <summary>
/// 五次関数的なイージングで最初は滑らかに加速し中盤で最速になり後半に向かって滑らかに減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float QuintInOut(float time, float totalTime, float start, float end);


/// <summary>
/// サイン関数的なイージング。最初に非常に遅く始まり、後半で急速に加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float SineIn(float time, float totalTime, float start, float end);

/// <summary>
/// サイン関数的なイージング。最初は速く、終わりに向かって急激に減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float SineOut(float time, float totalTime, float start, float end);

/// <summary>
/// サイン関数的なイージング。加速と減速が両方含まれており、サイン波のように滑らかに変化する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float SineInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 指数関数的なイージング。最初は非常に遅く、後半で急激に加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float ExpIn(float time, float totalTime, float start, float end);

/// <summary>
/// 指数関数的なイージング。最初は速く、後半で急激に減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float ExpOut(float time, float totalTime, float start, float end);

/// <summary>
/// 指数関数的なイージング。加速と減速が両方含まれ、急激に変化する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float ExpInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 円関数的なイージング。最初は遅く、後半で急激に加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float CircIn(float time, float totalTime, float start, float end);

/// <summary>
/// 円関数的なイージング。最初は速く、終わりに向かって減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float CircOut(float time, float totalTime, float start, float end);

/// <summary>
/// 円関数的なイージング。加速と減速の両方があり、円形のようにスムーズな動きを持つ
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float CircInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 弾性関数的なイージング。最初はゆっくり始まり、途中で反発して加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float ElasticIn(float time, float totalTime, float start, float end);

/// <summary>
/// 弾性関数的なイージング。最初は速く、後半に反発しながら減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値< / param>
/// <param name="end">終了時の値</param>
float ElasticOut(float time, float totalTime, float start, float end);

/// <summary>
/// 弾性関数的なイージング。加速と減速の間に弾性効果を含んだ動き
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float ElasticInOut(float time, float totalTime, float start, float end);

/// <summary>
/// バック関数的なイージング。最初に少し逆方向に動き、その後加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float BackIn(float time, float totalTime, float start, float end, float s);

/// <summary>
/// バック関数的なイージング。最初は速く、後半に少し逆方向に動いて減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float BackOut(float time, float totalTime, float start, float end, float s);

/// <summary>
/// バック関数的なイージング。加速と減速の両方があり、バック関数の特徴を含んでいる
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float BackInOut(float time, float totalTime, float start, float end, float s);

/// <summary>
/// バウンド関数的なイージング。最初に跳ねるような動きで加速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float BounceIn(float time, float totalTime, float start, float end);

/// <summary>
/// バウンド関数的なイージング。最初は速く、跳ねるように減速する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float BounceOut(float time, float totalTime, float start, float end);

/// <summary>
/// バウンド関数的なイージング。加速と減速が両方あり、跳ねる動きがある
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float BounceInOut(float time, float totalTime, float start, float end);

/// <summary>
/// 線形補間。加速も減速もなく、一定の速度で直線的に変化する
/// </summary>
/// <param name="time">現在の時間（開始からの経過時間）</param>
/// <param name="totalTime">全体の時間（アニメーションの総時間）</param>
/// <param name="start">開始時の値</param>
/// <param name="end">終了時の値</param>
float Linear(float time, float totalTime, float start, float end);


