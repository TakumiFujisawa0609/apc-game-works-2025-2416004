#include "Easing.h"

#include<cmath>
#include<DxLib.h>

//time			= 進行度
//totalTime		= 目標時間
//start			= 開始値
//end			= 目標値

//↓BackIn BackOut BackInOutのみ
//s				= 助走量

// 加速しながら値を変化させる（二次関数）
float QuadIn(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime;

	return -end * time * time + start;
}

// 減速しながら値を変化させる（二次関数）
float QuadQut(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime;

	return -end * time * (time - 2) + start;
}

// 加速してから減速する（二次関数）
float QuadInOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime / 2;
	if (time < 1) return end / 2 * time * time + start;

	time = time - 1;

	return -end / 2 * (time * (time - 2) - 1) + start;
}

// 非常にゆっくり加速（三次関数）
float CubicIn(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime;

	return end * time * time * time * time * time + start;
}

// 減速しながら終わる（三次関数）
float CubicOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time = time / totalTime - 1;

	return end * (time * time * time + 1) + start;
}

// 加速してから減速する（三次関数）
float CubicInOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime / 2;
	if (time < 1) return end / 2 * time * time * time + start;

	time = time - 2;

	return end / 2 * (time * time * time + 2) + start;
}

// 非常にゆっくり加速（四次関数）
float QuartIn(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime;

	return end * time * time * time * time + start;
}

// 急激に減速（四次関数）
float QuartOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time = time / totalTime - 1;

	return -end * (time * time * time * time - 1) + start;
}

// 加速してから減速する（四次関数）
float QuartInOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime / 2;
	if (time < 1) return end / 2 * time * time * time * time + start;

	time = time - 2;

	return -end / 2 * (time * time * time * time - 2) + start;
}

// 非常にゆっくり加速（五次関数）
float QuintIn(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime;

	return end * time * time * time * time * time + start;
}

// 急激に減速（五次関数）
float QuintOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time = time / totalTime - 1;

	return end * (time * time * time * time * time + 1) + start;
}

// 加速してから減速する（五次関数）
float QuintInOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime / 2;
	if (time < 1) return end / 2 * time * time * time * time * time + start;

	time = time - 2;

	return end / 2 * (time * time * time * time * time + 2) + start;
}

// ゆっくり始まり滑らかに加速（サイン関数）
float SineIn(float time, float totalTime, float start, float end)
{
	end -= start;

	return -end * cos(time * (DX_PI_F * 90 / 180) / totalTime) + end + start;
}

// 最初速く、緩やかに停止（サイン関数）
float SineOut(float time, float totalTime, float start, float end)
{
	end -= start;

	return end * sin(time * (DX_PI_F * 90 / 180) / totalTime) + start;
}

// 加速・減速を滑らかに（サイン関数）
float SineInOut(float time, float totalTime, float start, float end)
{
	end -= start;

	return -end / 2 * (cos(time * DX_PI_F / totalTime) - 1) + start;
}

// 急激に加速（指数関数）
float ExpIn(float time, float totalTime, float start, float end)
{
    end -= start;
 
	return time == 0.0f ? start : end * powf(2, 10 * (time / totalTime - 1)) + start;
}

// 急激に減速（指数関数）
float ExpOut(float time, float totalTime, float start, float end)
{
    end -= start;
  
	return time == totalTime ? end + start : end * (-powf(2, -10 * time / totalTime) + 1) + start;
}

// 加速・減速が非常に急（指数関数）
float ExpInOut(float time, float totalTime, float start, float end)
{
    if (time == 0.0f) return start;
    if (time == totalTime) return end;
    end -= start;
    time /= totalTime / 2;

    if (time < 1) return end / 2 * powf(2, 10 * (time - 1)) + start;

    time = time - 1;

    return end / 2 * (-powf(2, -10 * time) + 2) + start;

}

// 徐々に加速（円関数）
float CircIn(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime;

	return -end * (sqrt(1 - time * time) - 1) + start;
}

// 徐々に減速（円関数）
float CircOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time = time / totalTime - 1;

	return end * sqrt(1 - time * time) + start;
}

// 加速してから減速（円関数）
float CircInOut(float time, float totalTime, float start, float end)
{
	end -= start;
	time /= totalTime / 2;
	if (time < 1) return -end / 2 * (sqrt(1 - time * time) - 1) + start;

	time = time - 2;

	return end / 2 * (sqrt(1 - time * time) + 1) + start;
}

// 弾むように加速（弾性関数）
float ElasticIn(float time, float totalTime, float start, float end)
{
    end -= start;
    time /= totalTime;

    float s = 1.70158f;
    float p = totalTime * 0.3f;
    float a = end;

    if (time == 0) return start;
    if (time == 1) return start + end;

    if (a < abs(end))
    {
        a = end;
        s = p / 4;
    }
    else
    {
        s = p / (2 * DX_PI_F) * asin(end / a);
    }

    time = time - 1;
    return -(a * powf(2, 10 * time) * sin((time * totalTime - s) * (2 * DX_PI_F) / p)) + start;
}

// 弾むように減速（弾性関数）
float ElasticOut(float time, float totalTime, float start, float end)
{
    end -= start;
    time /= totalTime;

    float s = 1.70158f;
    float p = totalTime * 0.3f; ;
    float a = end;

    if (time == 0) return start;
    if (time == 1) return start + end;

    if (a < abs(end))
    {
        a = end;
        s = p / 4;
    }
    else
    {
        s = p / (2 * DX_PI_F) * asin(end / a);
    }

    return a * powf(2, -10 * time) * sin((time * totalTime - s) * (2 * DX_PI_F) / p) + end + start;
}

// 弾むように加速して減速（弾性関数）
float ElasticInOut(float time, float totalTime, float start, float end)
{
    end -= start;
    time /= totalTime / 2;

    float s = 1.70158f;
    float p = totalTime * (0.3f * 1.5f);
    float a = end;

    if (time == 0) return start;
    if (time == 2) return start + end;

    if (a < abs(end))
    {
        a = end;
        s = p / 4;
    }
    else
    {
        s = p / (2 * DX_PI_F) * asin(end / a);
    }

    if (time < 1)
    {
        return -0.5f * (a * powf(2, 10 * (time -= 1)) * sin((time * totalTime - s) * (2 * DX_PI_F) / p)) + start;
    }

    time = time - 1;

    return a * powf(2, -10 * time) * sin((time * totalTime - s) * (2 * DX_PI_F) / p) * 0.5f + end + start;
}

// 少し逆方向に動いてから加速（バック関数）
float BackIn(float time, float totalTime, float start, float end, float s)
{
    end -= start;
    time /= totalTime;

    return end * time * time * ((s + 1) * time - s) + start;
}

// 加速・減速しつつ逆方向にも動く（バック関数）
float BackOut(float time, float totalTime, float start, float end, float s)
{
    end -= start;
    time = time / totalTime - 1;

    return end * (time * time * ((s + 1) * time + s) + 1) + start;
}

// 跳ねるように加速（バウンド関数）
float BackInOut(float time, float totalTime, float start, float end, float s)
{
    end -= start;
    s *= 1.525f;
    time /= totalTime / 2;
    if (time < 1) return end / 2 * (time * time * ((s + 1) * time - s)) + start;

    time = time - 2;

    return end / 2 * (time * time * ((s + 1) * time + s) + 2) + start;
}

// 跳ねるように減速（バウンド関数）
float BounceIn(float time, float totalTime, float start, float end)
{
    end -= start;

    return end - BounceOut(totalTime - time, totalTime, 0, end) + start;
}

// 跳ねるように減速（バウンド関数）
float BounceOut(float time, float totalTime, float start, float end)
{
    end -= start;
    time /= totalTime;

    if (time < 1.0f / 2.75f)
    {
        return end * (7.5625f * time * time) + start;
    }
    else if (time < 2.0f / 2.75f)
    {
        time -= 1.5f / 2.75f;
        return end * (7.5625f * time * time + 0.75f) + start;
    }
    else if (time < 2.5f / 2.75f)
    {
        time -= 2.25f / 2.75f;
        return end * (7.5625f * time * time + 0.9375f) + start;
    }
    else
    {
        time -= 2.625f / 2.75f;
        return end * (7.5625f * time * time + 0.984375f) + start;
    }
}

// 跳ねながら加速・減速（バウンド関数）
float BounceInOut(float time, float totalTime, float start, float end)
{
    if (time < totalTime / 2)
    {
        return BounceIn(time * 2, totalTime, 0, end - start) * 0.5f + start;
    }
    else
    {
        return BounceOut(time * 2 - totalTime, totalTime, 0, end - start) * 0.5f + start + (end - start) * 0.5f;
    }
}

// 一定速度で直線的に変化（線形補間）
float Linear(float time, float totalTime, float start, float end)
{
    return (end - start) * time / totalTime + start;
}




