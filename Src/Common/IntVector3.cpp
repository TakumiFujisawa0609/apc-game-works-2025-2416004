#include "../Pch.h"
#include "IntVector3.h"
#include "../Utility/Utility.h"

IntVector3::IntVector3(void)
    : x(0), y(0), z(0)
{
}

IntVector3::IntVector3(int vX, int vY, int vZ)
    : x(vX), y(vY), z(vZ)
{
}

IntVector3::IntVector3(const VECTOR& v)
    : x(Utility::Round(v.x)) // x成分を四捨五入して代入
    , y(Utility::Round(v.y)) // y成分を四捨五入して代入
    , z(Utility::Round(v.z)) // z成分を四捨五入して代入
{
}

bool IntVector3::operator<(const IntVector3& value) const
{
    // x, y, z の優先順位で一括比較
    return std::tie(x, y, z) < std::tie(value.x, value.y, value.z);
}

bool IntVector3::operator==(const IntVector3& value) const
{
    // 全ての成分が一致するか判定
    return (x == value.x && y == value.y && z == value.z);
}

bool IntVector3::operator!=(const IntVector3& value) const
{
    // == 演算子の結果を反転
    return !(*this == value);
}

IntVector3& IntVector3::operator+=(const IntVector3& v)
{
    x += v.x; // 各成分を加算
    y += v.y;
    z += v.z;
    return *this;
}

IntVector3& IntVector3::operator-=(const IntVector3& v)
{
    x -= v.x; // 各成分を減算
    y -= v.y;
    z -= v.z;
    return *this;
}

IntVector3& IntVector3::operator*=(int v)
{
    x *= v; // 各成分にスカラー値を乗算
    y *= v;
    z *= v;
    return *this;
}

IntVector3 IntVector3::operator+(const IntVector3& v) const
{
    // コピーを作成してから加算代入を利用
    return IntVector3(*this) += v;
}

IntVector3 IntVector3::operator-(const IntVector3& v) const
{
    // コピーを作成してから減算代入を利用
    return IntVector3(*this) -= v;
}

IntVector3 IntVector3::operator*(int v) const
{
    // コピーを作成してから乗算代入を利用
    return IntVector3(*this) *= v;
}