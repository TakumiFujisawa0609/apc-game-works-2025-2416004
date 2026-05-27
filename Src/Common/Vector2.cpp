#include "Vector2.h"

Vector2::Vector2(void)
{
	// 初期値として原点を設定
	x = 0;
	y = 0;
}

Vector2::Vector2(int vX, int vY)
{
	// 指定された座標で初期化
	x = vX;
	y = vY;
}

Vector2::~Vector2(void)
{
}

Vector2 Vector2::operator+(const Vector2 value) const
{
	// 各成分を足し合わせた新しいベクトルを生成して返す
	return Vector2(x + value.x, y + value.y);
}

void Vector2::operator+=(const Vector2 value)
{
	// 引数のベクトルの値を自身の成分に直接加算する
	x += value.x;
	y += value.y;
}

Vector2 Vector2::operator-(const Vector2 value) const
{
	// 各成分の差を持つ新しいベクトルを生成して返す
	return Vector2(x - value.x, y - value.y);
}

void Vector2::operator-=(const Vector2 value)
{
	// 自身の成分から引数のベクトルの値を減算する
	x -= value.x;
	y -= value.y;
}

Vector2 Vector2::operator*(const int value) const
{
	// 全ての成分を指定された倍率でスケールさせたベクトルを返す
	return Vector2(x * value, y * value);
}

void Vector2::operator*=(const int value)
{
	// 自身の成分に倍率を掛けて更新する
	x *= value;
	y *= value;
}

Vector2 Vector2::operator/(const int value) const
{
	// 各成分を指定された値で割った結果を返す
	return Vector2(x / value, y / value);
}

void Vector2::operator/=(const int value)
{
	// 自身の成分を割った値で更新する
	x /= value;
	y /= value;
}

Vector2F::Vector2F(void)
{
	// 浮動小数点数で原点リセット
	x = 0.0f;
	y = 0.0f;
}

Vector2F::Vector2F(float vX, float vY)
{
	x = vX;
	y = vY;
}

Vector2F::~Vector2F(void)
{
}

Vector2F Vector2F::operator+(const Vector2F value) const
{
	return Vector2F(x + value.x, y + value.y);
}

void Vector2F::operator+=(const Vector2F value)
{
	x += value.x;
	y += value.y;
}

Vector2F Vector2F::operator-(const Vector2F value) const
{
	return Vector2F(x - value.x, y - value.y);
}

void Vector2F::operator-=(const Vector2F value)
{
	x -= value.x;
	y -= value.y;
}

Vector2F Vector2F::operator*(const float value) const
{
	return Vector2F(x * value, y * value);
}

void Vector2F::operator*=(const float value)
{
	x *= value;
	y *= value;
}

Vector2F Vector2F::operator/(const float value) const
{
	return Vector2F(x / value, y / value);
}

void Vector2F::operator/=(const float value)
{
	x /= value;
	y /= value;
}