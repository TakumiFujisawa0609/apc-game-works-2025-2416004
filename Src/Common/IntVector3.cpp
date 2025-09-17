#include "IntVector3.h"
#include<tuple>
#include"../Utility/Utility.h"

// デフォルトコンストラクタ
// 全ての座標を0に初期化する
IntVector3::IntVector3()
{
	x = 0;
	y = 0;
	z = 0;
}

// パラメータ付きコンストラクタ
// 指定された値でベクトルを初期化する
IntVector3::IntVector3(int vX, int vY, int vZ)
{
	x = vX;
	y = vY;
	z = vZ;
}

// VECTORからの変換コンストラクタ
// 浮動小数点ベクトルを整数ベクトルに変換する
// Utilityクラスのround関数を使用して四捨五入する
IntVector3::IntVector3(VECTOR v)
{
	x = Utility::Round(v.x);
	y = Utility::Round(v.y);
	z = Utility::Round(v.z);
}

// デストラクタ
// 特に処理は行わない
IntVector3::~IntVector3(void)
{
}

// 比較演算子のオーバーロード
// std::tieを使用して辞書式順序で比較する
// x値を最初に比較し、同じ場合はy値、さらに同じ場合はz値を比較する
bool IntVector3::operator<(const IntVector3& value) const
{
	return std::tie(x, y, z) < std::tie(value.x, value.y, value.z);
}

// ベクトルの各成分に値を加算するメソッド
void IntVector3::Add(int v)
{
	x += v;
	y += v;
	z += v;
}

// ベクトルの各成分から値を減算するメソッド
void IntVector3::Sub(int v)
{
	x -= v;
	y -= v;
	z -= v;
}

// ベクトルの各成分を指定された値でスケーリング（乗算）するメソッド
void IntVector3::Scale(int v)
{
	x *= v;
	y *= v;
	z *= v;
}
