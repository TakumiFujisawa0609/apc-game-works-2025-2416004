#pragma once
#include<DxLib.h>

/// <summary>
/// 整数の3次元ベクトルを表すクラス
/// </summary>
class IntVector3
{
public:

	int x;			///x座標
	int y;			///y座標
	int z;			///z座標

	/// <summary>
	/// ゼロベクトルで初期化するコンストラクタ
	/// </summary>
	IntVector3();

	/// <summary>
	/// 指定した3つの値で初期化するコンストラクタ
	/// </summary>
	/// <param name="vX">X成分</param>
	/// <param name="vY">Y成分</param>
	/// <param name="vZ">Z成分</param>
	IntVector3(int vX, int vY, int vZ);

	/// <summary>
	/// DxLibのVECTOR構造体から初期化するコンストラクタ
	/// </summary>
	/// <param name="v">VECTOR型のベクトル</param>
	IntVector3(VECTOR v);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~IntVector3(void);

	/// <summary>
	/// 辞書順比較を行う演算子。主にstd::setなどでの順序付けに使用。
	/// </summary>
	/// <param name="value">比較対象のIntVector3</param>
	/// <returns>this が value より小さい場合 true</returns>
	bool operator<(const IntVector3& value) const;
	
	/// <summary>
	/// 全ての要素に整数値を加算する
	/// </summary>
	/// <param name="v">加算する整数値</param>
	void Add(int v);

	/// <summary>
	/// 全ての要素から整数値を減算する
	/// </summary>
	/// <param name="v">減算する整数値</param>
	void Sub(int v);

	/// <summary>
	/// 全ての要素を整数値でスケーリング（乗算）する
	/// </summary>
	/// <param name="v">スケーリング係数</param>
	void Scale(int v);
};

