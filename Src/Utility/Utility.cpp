#include "Utility.h"

#include<DxLib.h>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<math.h>
#include <cstdlib>

#include "../Manager/Generic/SceneManager.h"

// 小数値を四捨五入して整数に変換する
int Utility::Round(float v)
{
	return static_cast<int>(roundf(v));
}

// 文字列を指定された区切り文字で分割する
std::vector<std::string> Utility::Split(std::string& line, char delimiter)
{
	std::istringstream stream(line);
	std::string field;
	std::vector<std::string> result;

	while (getline(stream, field, delimiter))
	{
		result.push_back(field);
	}

	return result;
}

// ラジアンを度に変換する（double）
double Utility::Rad2DegD(double rad)
{
	return rad * (180.0 / DX_PI);
}

// ラジアンを度に変換する（float）
float Utility::Rad2DegF(float rad)
{
	return rad * (180.0f / DX_PI_F);
}

// ラジアンを度に変換して整数にする
int Utility::Rad2DegI(int rad)
{
	return rad * Round(180.0f / DX_PI_F);
}

// 度をラジアンに変換する（double）
double Utility::Deg2RadD(double deg)
{
	return deg * (DX_PI / 180.0);
}

// 度をラジアンに変換する（float）
float Utility::Deg2RadF(float deg)
{
	return deg * (DX_PI_F / 180.0f);
}

// 度をラジアンに変換して整数にする
int Utility::Deg2RadI(int deg)
{
	return deg * Round(DX_PI_F / 180.0f);
}

// 角度を0〜360度の範囲に正規化する
double Utility::DegIn360(double deg)
{
	deg = fmod(deg, 360.0);
	if (deg < 0.0f)
	{
		deg += 360.0;
	}
	
	return deg;
}

// ラジアンを0〜2πの範囲に正規化する
double Utility::RadIn2PI(double rad)
{
	rad = fmod(rad, DX_TWO_PI);
	if (rad < 0.0)
	{
		rad += DX_TWO_PI;
	}

	return rad;
}

// ラジアン単位でどちら回りが近いかを判定する（-1=反時計回り、1=時計回り）
int Utility::DirNearAroundRad(float from, float to)
{
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{

		//比較元よりも時計回りに位置する

		if (diff > DX_PI_F)
		{

			//でも、180度以上離れているので、反時計回りの方が近い
			ret = -1.0f;
		}
		else
		{
			//時計回り
			ret = 1.0f;
		}

	}
	else
	{

		//比較元よりも反時計回りに位置する

		if (diff < -DX_PI_F)
		{

			//でも、180度以上離れているので、時計回りの方が近い
			ret = 1.0f;
		}
		else
		{

			//反時計回り
			ret = -1.0f;
		}
	}

	return static_cast<int>(ret);
}

// 度単位でどちら回りが近いかを判定する（-1=反時計回り、1=時計回り）
int Utility::DirNearAroundDeg(float from, float to)
{
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{


		//比較元よりも時計回りに位置する

		if (diff > 180.0f)
		{

			//でも、180度以上離れているので、反時計回りの方が近い
			ret = -1.0f;
		}
		else
		{
			//時計回り
			ret = 1.0f;
		}
	}
	else
	{

		//比較元よりも反時計回りに位置する

		if (diff < -180.0f)
		{
			//でも、180度以上離れているので。時計回りの方が近い
			ret = 1.0f;
		}
		else
		{
			//時計回り
			ret = -1.0f;
		}
	}
	
	return static_cast<int>(ret);
}

// 整数値を線形補間する
int Utility::Lerp(int start, int end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	int ret = start;

	ret += Round(t * static_cast<float>(end - start));

	return ret;
}

// 浮動小数点値を線形補間する
float Utility::Lerp(float start, float end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	float ret = start;

	ret += t * (end - start);

	return ret;
}

// 倍精度浮動小数点値を線形補間する
double Utility::Lerp(double start, double end, double t)
{
	//線形補間
	if (t >= 1.0)
	{
		return end;
	}

	double ret = start;

	ret += t * (end - start);

	return ret;
}

// 2Dベクトルを線形補間する
Vector2 Utility::Lerp(const Vector2& start, const Vector2& end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	Vector2 ret = start;

	ret.x += Round(t * static_cast<float>((end.x - start.x)));
	ret.y += Round(t * static_cast<float>((end.y - start.y)));

	return end;
}

// 3Dベクトルを線形補間する
VECTOR Utility::Lerp(const VECTOR& start, const VECTOR& end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	VECTOR ret = start;

	ret.x += t * (end.x - start.x);
	ret.y += t * (end.y - start.y);
	ret.z += t * (end.z - start.z);

	return ret;
}

// 度単位で角度を線形補間し、360度を考慮する
double Utility::LerpDeg(double start, double end, double t)
{
	double ret;

	double diff = end - start;

	if (diff < -180.0)
	{
		end += 360.0;

		ret = Lerp(start, end, t);
		
		if (ret >= 360.0)
		{
			ret -= 360.0;
		}
	}
	else if (diff > 180.0)
	{
		end -= 360.0;

		ret = Lerp(start, end, t);

		if (ret < 0.0)
		{
			ret += 360.0;
		}
	}
	else
	{
		ret = Lerp(start, end, t);
	}

	return ret;
}

// 色（COLOR_F）を線形補間する
COLOR_F Utility::Lerp(const COLOR_F& start, const COLOR_F& end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	COLOR_F ret = start;

	ret.r += t * (end.r - start.r);
	ret.g += t * (end.g - start.g);
	ret.b += t * (end.b - start.b);
	ret.a += t * (end.a - start.a);

	return ret;
}

// 2Dベクトルの3点間でベジエ補間する
Vector2 Utility::Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
{
	Vector2 a = Lerp(p1, p2, t);
	Vector2 b = Lerp(p2, p3, t);

	return Lerp(a, b, t);
}

// 3Dベクトルの3点間でベジエ補間する
VECTOR Utility::Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t)
{
	VECTOR a = Lerp(p1, p2, t);
	VECTOR b = Lerp(p2, p3, t);

	return Lerp(a, b, t);
}

// XZ平面上で中心点を軸に回転後の座標を求める
VECTOR Utility::RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad)
{
	float x = ((radiusPos.x - centerPos.x) * cosf(rad)) - ((radiusPos.z - centerPos.z) * sinf(rad));
	float z = ((radiusPos.x - centerPos.x) * cosf(rad)) + ((radiusPos.z - centerPos.z) * cosf(rad));

	return VGet(centerPos.x + x, radiusPos.y, centerPos.z + z);
}

// 2Dベクトルの長さを求める
double Utility::Magnitude(const Vector2& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y));
}

// 3Dベクトルの長さを求める
double Utility::Magnitude(const VECTOR& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 3Dベクトルの長さをfloatで求める
float Utility::MagnitudeF(const VECTOR& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 2Dベクトルの長さの2乗を求める
int Utility::SqrMagnitude(const Vector2& v)
{
	return v.x * v.x + v.y * v.y;
}

// 3Dベクトルの長さの2乗をfloatで求める
float Utility::SqrMagnitudeF(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 3Dベクトルの長さの2乗を求める
double Utility::SqrMagnitude(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 2つの3Dベクトル間の距離の2乗を求める
double Utility::SqrMagnitude(const VECTOR& v1, const VECTOR& v2)
{
	return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2);
}

// 2つの2Dベクトル間の距離を求める
double Utility::Distance(const Vector2& v1, const Vector2& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

// 2つの3Dベクトル間の距離を求める
double Utility::Distance(const VECTOR& v1, const VECTOR& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

// 2つの3Dベクトルが等しいかどうかを判定する
bool Utility::Equals(const VECTOR& v1, const VECTOR& v2)
{
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}

	return false;
}

// ベクトルがゼロベクトルかどうかを判定する
bool Utility::EqualsVZero(const VECTOR& v1)
{
	const VECTOR& v2 = VECTOR_ZERO;
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
	{
		return true;
	}
	
	return false;
}

// 2Dベクトルを正規化して3Dベクトルで返す
VECTOR Utility::Normalize(const Vector2& v)
{
	VECTOR ret = VGet(
		static_cast<float>(v.x),
		static_cast<float>(v.y),
		0.0f
		);

	float len = static_cast<float>(Magnitude(v));

	ret.x /= len;
	ret.y /= len;
	ret.z /= len;

	return ret;
}

// 3Dベクトルを正規化する（ゼロベクトルのときはそのまま返す）
VECTOR Utility::VNormalize(const VECTOR& v)
{
	if (Utility::EqualsVZero(v))
	{
		//Quaternion計算でゼロを渡して、
		//エラー(-1, -1, -1)が返ってくると困る
		return v;
	}
	return VNorm(v);
}

// 2つの3Dベクトルのなす角（度）を求める
double Utility::AngleDeg(const VECTOR& from, const VECTOR& to)
{
	auto fLen = SqrMagnitude(from);
	auto tLen = SqrMagnitude(to);
	auto denominator = sqrt(fLen * tLen);
	if (denominator < kEpsilonNormalSqrt)
	{
		return 0.0f;
	}

	auto dot = VDot(from, to) / denominator;

	if (dot < -1.0f)
	{
		dot = -1.0f;
	}
	if (dot > 1.0f)
	{
		dot = 1.0f;
	}

	return acos(dot) * (180.0 / DX_PI);

}

// 指定方向に線と終点に球体を描画する
void Utility::DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len)
{
	auto nDir = Utility::VNormalize(dir);
	auto sPos = VAdd(pos, VScale(nDir, -len));
	auto ePos = VAdd(pos, VScale(nDir, len));

	DrawLine3D(sPos, ePos, color);
	DrawSphere3D(ePos, 5.0f, 5, color, color, true);
}

// 回転行列を基にXYZ軸方向の線を描画する
void Utility::DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len)
{
	VECTOR dir;

	// X
	dir = VTransform(Utility::DIR_R, rot);
	DrawLineDir(pos, dir, 0xff0000, len);

	// Y
	dir = VTransform(Utility::DIR_UP, rot);
	DrawLineDir(pos, dir, 0x00ff00, len);

	// Z
	dir = VTransform(Utility::DIR_F, rot);
	DrawLineDir(pos, dir, 0x0000ff, len);
}

// クォータニオンを基にXYZ軸方向の線を描画する
void Utility::DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len)
{
	VECTOR dir;

	// X
	dir = rot.GetRight();
	DrawLineDir(pos, dir, 0xff0000, len);

	// Y
	dir = rot.GetUp();
	DrawLineDir(pos, dir, 0x00ff00, len);

	// Z
	dir = rot.GetForward();
	DrawLineDir(pos, dir, 0x0000ff, len);
}

bool Utility::IsTimeOver(float& totalTime, const float& waitTime)
{
	//デルタタイム
	auto delta = SceneManager::GetInstance().GetDeltaTime();
	totalTime += delta;

	//待機時間を超過しているか判断
	if (totalTime >= waitTime)
	{
		return true;
	}

	return false;
}

// ランダムな数値を返す(float用)
float Utility::RandRangeF(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}
