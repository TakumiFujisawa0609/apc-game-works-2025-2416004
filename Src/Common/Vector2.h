#pragma once

/// @brief 整数（int）型の2次元ベクトルクラス
class Vector2
{
public:
	int x;		/// x座標
	int y;		/// y座標

	/// @brief デフォルトコンストラクタ（x, y を 0 に初期化）
	/// @param void 
	Vector2(void);

	/// @brief x, y を指定するコンストラクタ
	/// @param vX x座標
	/// @param vY y座標
	Vector2(int vX, int vY);

	/// @brief デストラクタ
	/// @param void 
	~Vector2(void);

	// --- 演算子オーバーロード ---

	/// @brief 加算演算子オーバーロード
	/// @param value 加算するベクトル
	/// @return 加算後のベクトル
	Vector2 operator+(const Vector2 value) const;

	/// @brief 加算代入演算子オーバーロード
	/// @param value 加算するベクトル
	void operator+=(const Vector2 value);

	/// @brief 減算演算子オーバーロード
	/// @param value 減算するベクトル
	/// @return 減算後のベクトル
	Vector2 operator-(const Vector2 value) const;

	/// @brief 減算代入演算子オーバーロード
	/// @param value 減算するベクトル
	void operator-=(const Vector2 value);

	/// @brief 乗算演算子オーバーロード
	/// @param value 乗算するスカラー値
	/// @return 乗算後のベクトル
	Vector2 operator*(const int value) const;

	/// @brief 乗算代入演算子オーバーロード
	/// @param value 乗算するスカラー値
	void operator*=(const int value);

	/// @brief 除算演算子オーバーロード
	/// @param value 除算するスカラー値
	/// @return 除算後のベクトル
	Vector2 operator/(const int value) const;

	/// @brief 除算代入演算子オーバーロード
	/// @param value 除算するスカラー値
	void operator/=(const int value);
};

/// @brief 少数（float）型の2次元ベクトルクラス
class Vector2F
{
public:
	float x;	/// x座標
	float y;	/// y座標

	/// @brief デフォルトコンストラクタ（x, y を 0.0f に初期化）
	/// @param void 
	Vector2F(void);

	/// @brief 指定された x, y で初期化するコンストラクタ
	/// @param vX x座標
	/// @param vY y座標
	Vector2F(float vX, float vY);

	/// @brief デストラクタ
	/// @param void 
	~Vector2F(void);

	// --- 演算子オーバーロード ---

	/// @brief ベクトル同士の加算演算子
	/// @param value 加算するベクトル
	/// @return 加算結果のベクトル
	Vector2F operator+(const Vector2F value) const;

	/// @brief ベクトル同士の加算代入演算子
	/// @param value 加算するベクトル
	void operator+=(const Vector2F value);

	/// @brief ベクトル同士の減算演算子
	/// @param value 減算するベクトル
	/// @return 減算結果のベクトル
	Vector2F operator-(const Vector2F value) const;

	/// @brief ベクトル同士の減算代入演算子
	/// @param value 減算するベクトル
	void operator-=(const Vector2F value);

	/// @brief スカラーとの乗算演算子
	/// @param value 乗算するスカラー値
	/// @return 乗算結果のベクトル
	Vector2F operator*(const float value) const;

	/// @brief スカラーとの乗算代入演算子
	/// @param value 乗算するスカラー値
	void operator*=(const float value);

	/// @brief スカラーとの除算演算子
	/// @param value 除算するスカラー値
	/// @return 除算結果のベクトル
	Vector2F operator/(const float value) const;

	/// @brief スカラーとの除算代入演算子
	/// @param value 除算するスカラー値
	void operator/=(const float value);
};