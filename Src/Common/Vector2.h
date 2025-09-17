#pragma once

/// <summary>
/// 整数（int）型の2次元ベクトルクラス
/// </summary>
class Vector2
{
public:
	int x;		/// x座標
	int y;		/// y座標

	/// <summary>
	/// デフォルトコンストラクタ（x, y を 0 に初期化）
	/// </summary>
	Vector2(void);

	/// <summary>
	/// x, y を指定するコンストラクタ
	/// </summary>
	/// <param name="x">x座標</param>
	/// <param name="y">y座標</param>
	Vector2(int vX, int vY);

	///<summary>
	///デストラクタ
	///</summary>
	~Vector2(void);

	// 演算子オーバーロード（ベクトル間の演算を簡潔に記述）

    /// <summary>
	/// 加算演算子オーバーロード
	/// </summary>
	/// <param name="value">加算するベクトル</param>
	/// <returns>加算後のベクトル</returns>
	Vector2 operator+(const Vector2 value)const;

	/// <summary>
	/// 加算代入演算子オーバーロード
	/// </summary>
	/// <param name="value">加算するベクトル</param>
	void operator+=(const Vector2 value);

	/// <summary>
	/// 減算演算子オーバーロード
	/// </summary>
	/// <param name="value">減算するベクトル</param>
	/// <returns>減算後のベクトル</returns>
	Vector2 operator-(const Vector2 value)const;

	/// <summary>
	/// 減算代入演算子オーバーロード
	/// </summary>
	/// <param name="value">減算するベクトル</param>
	void operator-=(const Vector2 value);

	/// <summary>
	/// 乗算演算子オーバーロード
	/// </summary>
	/// <param name="value">乗算するベクトル</param>
	/// <returns>乗算後のベクトル</returns>
	Vector2 operator*(const int value)const;

	/// <summary>
	/// 乗算代入演算子オーバーロード
	/// </summary>
	/// <param name="value">乗算するベクトル</param>
	void operator*=(const int value);

	/// <summary>
	/// 除算演算子オーバーロード
	/// </summary>
	/// <param name="value">除算するベクトル</param>
	/// <returns>除算後のベクトル</returns>
	Vector2 operator/(const int value)const;

	/// <summary>
	/// 除算代入演算子オーバーロード
	/// </summary>
	/// <param name="value">除算するベクトル</param>
	void operator/=(const int value);
};

/// <summary>
/// 少数（float）型の2次元ベクトルクラス
/// </summary>
class Vector2F
{
public:

	float x;	/// x座標
	float y;	/// y座標

	/// <summary>
	/// デフォルトコンストラクタ（x, y を 0.0f に初期化）
	/// </summary>
	Vector2F(void);

	/// <summary>
	/// 指定された x, y で初期化するコンストラクタ
	/// </summary>
	/// <param name="vX">x座標</param>
	/// <param name="vY">y座標</param>
	Vector2F(float vX, float vY);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Vector2F(void);

	// 演算子オーバーロード（ベクトル間の演算を簡潔に記述）

	/// <summary>
	/// ベクトル同士の加算演算子
	/// </summary>
	/// <param name="value">加算するベクトル</param>
	/// <returns>加算結果のベクトル</returns>
	Vector2F operator+(const Vector2F value)const;

	/// <summary>
	/// ベクトル同士の加算代入演算子
	/// </summary>
	/// <param name="value">加算するベクトル</param>
	void operator+=(const Vector2F value);


	/// <summary>
	/// ベクトル同士の減算演算子
	/// </summary>
	/// <param name="value">減算するベクトル</param>
	/// <returns>減算結果のベクトル</returns>
	Vector2F operator-(const Vector2F value)const;

	/// <summary>
	/// ベクトル同士の減算代入演算子
	/// </summary>
	/// <param name="value">減算するベクトル</param>
	void operator-=(const Vector2F value);

	/// <summary>
	/// スカラーとの乗算演算子
	/// </summary>
	/// <param name="value">乗算するスカラー値</param>
	/// <returns>乗算結果のベクトル</returns>
	Vector2F operator*(const float value)const;

	/// <summary>
	/// スカラーとの乗算代入演算子
	/// </summary>
	/// <param name="value">乗算するスカラー値</param>
	void operator*=(const float value);

	/// <summary>
	/// スカラーとの除算演算子
	/// </summary>
	/// <param name="value">除算するスカラー値</param>
	/// <returns>除算結果のベクトル</returns>
	Vector2F operator/(const float value)const;

	/// <summary>
	/// スカラーとの除算代入演算子
	/// </summary>
	/// <param name="value">除算するスカラー値</param>
	void operator/=(const float value);
};

