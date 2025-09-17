#include "Vector2.h"

// x_ と y_ を 0 に初期化するデフォルトコンストラクタ
Vector2::Vector2(void)
{
	x = 0;
	y = 0;
}

// 指定された値で初期化するコンストラクタ
Vector2::Vector2(int vX, int vY)
{
	x = vX;
	y = vY;
}

// デストラクタ（特に処理はなし）
Vector2::~Vector2(void)
{
}

// ベクトル同士の加算（新しいベクトルを返す）
Vector2 Vector2::operator+(const Vector2 value)const
{
	return Vector2(x + value.x, y + value.y);
}

// ベクトル同士の加算（自分自身に加算）
void Vector2::operator+=(const Vector2 value)
{
	x = value.x;
	y = value.y;
}

// ベクトル同士の減算（新しいベクトルを返す）
Vector2 Vector2::operator-(const Vector2 value)const
{
	return Vector2(x - value.x, y - value.y);
}

// ベクトル同士の減算（自分自身に減算）
void Vector2::operator-=(const Vector2 value)
{
	x = y - value.x;
	y = y - value.y;
}

// スカラーとの乗算（新しいベクトルを返す）
Vector2 Vector2::operator*(const int value)const
{
	return Vector2(x * value, y * value);
}

// スカラーとの乗算（自分自身に乗算）
void Vector2::operator*=(const int value)
{
	x = x * value;
	y = y * value;
}

// スカラーとの除算（新しいベクトルを返す）
Vector2 Vector2::operator/(const int value)const
{
	return Vector2(x / value, y / value);
}

// スカラーとの除算（自分自身に除算）
void Vector2::operator/=(const int value)
{
	x = x / value;
	y = y / value;
}

// x と y を 0.0f に初期化するデフォルトコンストラクタ
Vector2F::Vector2F(void)
{
	x = 0.0f;
	y = 0.0f;
}

// 指定された値で x と y を初期化するコンストラクタ
Vector2F::Vector2F(float vX, float vY)
{
	x = vX;
	y = vY;
}

// デストラクタ（特に処理はなし）
Vector2F::~Vector2F(void)
{
}

// ベクトル同士を加算した結果を新しいベクトルとして返す
Vector2F Vector2F::operator+(const Vector2F value)const
{
	return Vector2F(x + value.x, y + value.y);
}

// 自身に別のベクトルを加算する
void Vector2F::operator+=(const Vector2F value)
{
	x = x + value.x;
	y = y + value.y;
}

// ベクトル同士を減算した結果を新しいベクトルとして返す
Vector2F Vector2F::operator-(const Vector2F value)const
{
	return Vector2F(x - value.x, y - value.y);
}

// 自身から別のベクトルを減算する
void Vector2F::operator-=(const Vector2F value)
{
	x = x - value.x;
	y = y - value.y;
}

// 自身のベクトルにスカラーを掛けた結果を新しいベクトルとして返す
Vector2F Vector2F::operator*(const float value)const
{
	return Vector2F(x * value, y * value);
}

// 自身のベクトルにスカラーを掛ける
void Vector2F::operator*=(const float value)
{
	x = x * value;
	y = y * value;
}

// 自身のベクトルをスカラーで割った結果を新しいベクトルとして返す
Vector2F Vector2F::operator/(const float value)const
{
	return Vector2F(x / value, y / value);
}

// 自身のベクトルをスカラーで割る
void Vector2F::operator/=(const float  value)
{
	x = x / value;
	y = y / value;
}