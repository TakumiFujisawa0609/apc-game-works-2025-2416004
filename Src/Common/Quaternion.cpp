#include "../Pch.h"
#include "Quaternion.h"
#include "../Utility/Utility.h"

Quaternion::Quaternion(void)
	: w(1.0), x(0.0), y(0.0), z(0.0) // 単位クォータニオン（回転なし）
{
}

Quaternion::Quaternion(const VECTOR& eulerRad)
{
	*this = Euler(eulerRad);
}

Quaternion::Quaternion(double initialW, double initialX, double initialY, double initialZ)
	: w(initialW), x(initialX), y(initialY), z(initialZ)
{
}

Quaternion::~Quaternion(void)
{
}

Quaternion Quaternion::Euler(const VECTOR& eulerRad)
{
	return Euler(eulerRad.x, eulerRad.y, eulerRad.z);
}

Quaternion Quaternion::Euler(double radX, double radY, double radZ)
{
	// 角度を 0～2π 範囲に正規化
	double normalizedX = Utility::RadIn2PI(radX);
	double normalizedY = Utility::RadIn2PI(radY);
	double normalizedZ = Utility::RadIn2PI(radZ);

	// ハーフアングル公式に基づき、各軸の sin/cos を計算
	double halfX = normalizedX * 0.5;
	double halfY = normalizedY * 0.5;
	double halfZ = normalizedZ * 0.5;

	double cosX = cos(halfX);
	double sinX = sin(halfX);
	double cosY = cos(halfY);
	double sinY = sin(halfY);
	double cosZ = cos(halfZ);
	double sinZ = sin(halfZ);

	// ZYX順（ロール・ピッチ・ヨー）での回転合成
	return Quaternion(
		cosX * cosY * cosZ + sinX * sinY * sinZ, // w
		sinX * cosY * cosZ + cosX * sinY * sinZ, // x
		cosX * sinY * cosZ - sinX * cosY * sinZ, // y
		cosX * cosY * sinZ - sinX * sinY * cosZ  // z
	);
}

Quaternion Quaternion::AngleAxis(double angleRad, VECTOR axis)
{
	// 軸ベクトルの長さの2乗を計算
	double sqMagnitude = static_cast<double>(axis.x) * axis.x +
		static_cast<double>(axis.y) * axis.y +
		static_cast<double>(axis.z) * axis.z;

	// 軸がゼロベクトルの場合は単位クォータニオンを返す
	if (sqMagnitude < kEpsilonNormalSqrt)
	{
		return Identity();
	}

	double inverseMagnitude = 1.0 / sqrt(sqMagnitude);
	double halfAngle = angleRad * 0.5;
	double sinHalf = sin(halfAngle);

	// 回転軸を正規化しつつ各成分を計算
	return Quaternion(
		cos(halfAngle),
		sinHalf * (axis.x * inverseMagnitude),
		sinHalf * (axis.y * inverseMagnitude),
		sinHalf * (axis.z * inverseMagnitude)
	);
}

Quaternion Quaternion::Mult(const Quaternion& left, const Quaternion& right)
{
	return left * right;
}

Quaternion Quaternion::Mult(const Quaternion& other) const
{
	return (*this) * other;
}

VECTOR Quaternion::PosAxis(const Quaternion& rotation, VECTOR targetPos)
{
	// 回転公式: p' = q * p * q^-1 を利用してベクトルを回転
	const double pureQuaternionW = 0.0;
	Quaternion point(pureQuaternionW, targetPos.x, targetPos.y, targetPos.z);
	Quaternion rotatedPoint = rotation * point * rotation.Inverse();

	return VGet(
		static_cast<float>(rotatedPoint.x),
		static_cast<float>(rotatedPoint.y),
		static_cast<float>(rotatedPoint.z)
	);
}

VECTOR Quaternion::PosAxis(VECTOR targetPos) const
{
	return PosAxis(*this, targetPos);
}

VECTOR Quaternion::ToEuler(const Quaternion& target)
{
	// 回転行列の各成分を抽出
	double r11 = 2.0 * (target.x * target.z + target.w * target.y);
	double r12 = target.w * target.w - target.x * target.x - target.y * target.y + target.z * target.z;
	double r21 = -2.0 * (target.y * target.z - target.w * target.x);
	double r31 = 2.0 * (target.x * target.y + target.w * target.z);
	double r32 = target.w * target.w - target.x * target.x + target.y * target.y - target.z * target.z;

	// asinの数値誤差を回避するためにクランプ
	double clampedR21 = std::max(-1.0, std::min(1.0, r21));

	return VGet(
		static_cast<float>(asin(clampedR21)),
		static_cast<float>(atan2(r11, r12)),
		static_cast<float>(atan2(r31, r32))
	);
}

VECTOR Quaternion::ToEuler(void) const
{
	return ToEuler(*this);
}

MATRIX Quaternion::ToMatrix(const Quaternion& target)
{
	MATRIX matrix = MGetIdent();

	// 回転行列への変換公式に基づき事前計算
	float x2 = static_cast<float>(target.x * target.x * 2.0);
	float y2 = static_cast<float>(target.y * target.y * 2.0);
	float z2 = static_cast<float>(target.z * target.z * 2.0);
	float xy = static_cast<float>(target.x * target.y * 2.0);
	float xz = static_cast<float>(target.x * target.z * 2.0);
	float yz = static_cast<float>(target.y * target.z * 2.0);
	float wx = static_cast<float>(target.w * target.x * 2.0);
	float wy = static_cast<float>(target.w * target.y * 2.0);
	float wz = static_cast<float>(target.w * target.z * 2.0);

	// 行列の各要素を設定
	matrix.m[0][0] = 1.0f - (y2 + z2); matrix.m[0][1] = xy + wz;          matrix.m[0][2] = xz - wy;
	matrix.m[1][0] = xy - wz;          matrix.m[1][1] = 1.0f - (x2 + z2); matrix.m[1][2] = yz + wx;
	matrix.m[2][0] = xz + wy;          matrix.m[2][1] = yz - wx;          matrix.m[2][2] = 1.0f - (x2 + y2);

	return matrix;
}

MATRIX Quaternion::ToMatrix(void) const
{
	return ToMatrix(*this);
}

Quaternion Quaternion::LookRotation(VECTOR forwardDir)
{
	return LookRotation(forwardDir, VGet(0.0f, 1.0f, 0.0f));
}

Quaternion Quaternion::LookRotation(VECTOR forwardDir, VECTOR upDir)
{
	// 向く方向を正規化し、右方向と上方向の基底ベクトルを算出
	forwardDir = Utility::VNormalize(forwardDir);
	VECTOR right = Utility::VNormalize(VCross(upDir, forwardDir));
	VECTOR orthoUp = VCross(forwardDir, right);

	float m00 = right.x;    float m01 = right.y;    float m02 = right.z;
	float m10 = orthoUp.x;  float m11 = orthoUp.y;  float m12 = orthoUp.z;
	float m20 = forwardDir.x; float m21 = forwardDir.y; float m22 = forwardDir.z;

	double trace = m00 + m11 + m22;
	Quaternion lookRotation;

	// トレースの正負によってクォータニオン成分の算出式を分岐
	if (trace > 0.0)
	{
		double s = sqrt(trace + 1.0);
		lookRotation.w = s * 0.5;
		s = 0.5 / s;
		lookRotation.x = (m12 - m21) * s;
		lookRotation.y = (m20 - m02) * s;
		lookRotation.z = (m01 - m10) * s;
	}
	else if ((m00 >= m11) && (m00 >= m22))
	{
		double s = sqrt(1.0 + m00 - m11 - m22);
		double invS = 0.5 / s;
		lookRotation.x = 0.5 * s;
		lookRotation.y = (m01 + m10) * invS;
		lookRotation.z = (m02 + m20) * invS;
		lookRotation.w = (m12 - m21) * invS;
	}
	else if (m11 > m22)
	{
		double s = sqrt(1.0 + m11 - m00 - m22);
		double invS = 0.5 / s;
		lookRotation.x = (m10 + m01) * invS;
		lookRotation.y = 0.5 * s;
		lookRotation.z = (m21 + m12) * invS;
		lookRotation.w = (m20 - m02) * invS;
	}
	else
	{
		double s = sqrt(1.0 + m22 - m00 - m11);
		double invS = 0.5 / s;
		lookRotation.x = (m20 + m02) * invS;
		lookRotation.y = (m21 + m12) * invS;
		lookRotation.z = 0.5 * s;
		lookRotation.w = (m01 - m10) * invS;
	}
	return lookRotation.Normalized();
}

Quaternion Quaternion::GetRotation(MATRIX rotationMatrix)
{
	float trace = rotationMatrix.m[0][0] + rotationMatrix.m[1][1] + rotationMatrix.m[2][2];
	Quaternion extractedRotation;

	// 行列の対角成分を利用して回転クォータニオンを復元
	if (trace > 0.0f)
	{
		float s = sqrtf(trace + 1.0f) * 2.0f;
		extractedRotation.w = 0.25f * s;
		extractedRotation.x = (rotationMatrix.m[1][2] - rotationMatrix.m[2][1]) / s;
		extractedRotation.y = (rotationMatrix.m[2][0] - rotationMatrix.m[0][2]) / s;
		extractedRotation.z = (rotationMatrix.m[0][1] - rotationMatrix.m[1][0]) / s;
	}
	else if ((rotationMatrix.m[0][0] > rotationMatrix.m[1][1]) && (rotationMatrix.m[0][0] > rotationMatrix.m[2][2]))
	{
		float s = sqrtf(1.0f + rotationMatrix.m[0][0] - rotationMatrix.m[1][1] - rotationMatrix.m[2][2]) * 2.0f;
		extractedRotation.w = (rotationMatrix.m[1][2] - rotationMatrix.m[2][1]) / s;
		extractedRotation.x = 0.25f * s;
		extractedRotation.y = (rotationMatrix.m[0][1] + rotationMatrix.m[1][0]) / s;
		extractedRotation.z = (rotationMatrix.m[0][2] + rotationMatrix.m[2][0]) / s;
	}
	else if (rotationMatrix.m[1][1] > rotationMatrix.m[2][2])
	{
		float s = sqrtf(1.0f + rotationMatrix.m[1][1] - rotationMatrix.m[0][0] - rotationMatrix.m[2][2]) * 2.0f;
		extractedRotation.w = (rotationMatrix.m[2][0] - rotationMatrix.m[0][2]) / s;
		extractedRotation.x = (rotationMatrix.m[0][1] + rotationMatrix.m[1][0]) / s;
		extractedRotation.y = 0.25f * s;
		extractedRotation.z = (rotationMatrix.m[1][2] + rotationMatrix.m[2][1]) / s;
	}
	else
	{
		float s = sqrtf(1.0f + rotationMatrix.m[2][2] - rotationMatrix.m[0][0] - rotationMatrix.m[1][1]) * 2.0f;
		extractedRotation.w = (rotationMatrix.m[0][1] - rotationMatrix.m[1][0]) / s;
		extractedRotation.x = (rotationMatrix.m[0][2] + rotationMatrix.m[2][0]) / s;
		extractedRotation.y = (rotationMatrix.m[1][2] + rotationMatrix.m[2][1]) / s;
		extractedRotation.z = 0.25f * s;
	}
	return extractedRotation.Normalized();
}

VECTOR Quaternion::GetForward(void) const { return GetDir(Utility::DIR_F); }
VECTOR Quaternion::GetBack(void) const { return GetDir(Utility::DIR_B); }
VECTOR Quaternion::GetRight(void) const { return GetDir(Utility::DIR_R); }
VECTOR Quaternion::GetLeft(void) const { return GetDir(Utility::DIR_L); }
VECTOR Quaternion::GetUp(void) const { return GetDir(Utility::DIR_UP); }
VECTOR Quaternion::GetDown(void) const { return GetDir(Utility::DIR_DOWN); }

double Quaternion::Dot(const Quaternion& left, const Quaternion& right)
{
	// 4次元ベクトルのドット積
	return left.w * right.w + left.x * right.x + left.y * right.y + left.z * right.z;
}

double Quaternion::Dot(const Quaternion& other) const
{
	return Dot(*this, other);
}

Quaternion Quaternion::Normalize(const Quaternion& target)
{
	return target.Normalized();
}

Quaternion Quaternion::Normalized(void) const
{
	double magnitude = Length();
	if (magnitude < kEpsilonNormalSqrt)
	{
		return Identity();
	}
	double inverseMagnitude = 1.0 / magnitude;
	return Quaternion(w * inverseMagnitude, x * inverseMagnitude, y * inverseMagnitude, z * inverseMagnitude);
}

void Quaternion::Normalize(void)
{
	*this = Normalized();
}

Quaternion Quaternion::Inverse(void) const
{
	double sqMagnitude = LengthSquared();
	if (sqMagnitude < kEpsilonNormalSqrt)
	{
		return Identity();
	}
	double inverseSqMagnitude = 1.0 / sqMagnitude;
	// 逆回転は共役クォータニオン(ベクトル部反転)をノルムの2乗で割る
	return Quaternion(w * inverseSqMagnitude, -x * inverseSqMagnitude, -y * inverseSqMagnitude, -z * inverseSqMagnitude);
}

Quaternion Quaternion::Slerp(Quaternion start, Quaternion end, double interpolationFactor)
{
	// 補間係数を 0.0 ～ 1.0 に制限
	return SlerpUnclamped(start, end, static_cast<float>(std::clamp(interpolationFactor, 0.0, 1.0)));
}

Quaternion Quaternion::FromToRotation(VECTOR startDir, VECTOR toDir)
{
	VECTOR axis = VCross(startDir, toDir);
	double angleDeg = Utility::AngleDeg(startDir, toDir);

	const double oppositeAngleThreshold = 179.9;
	// 向きがほぼ反対(180度)の場合の特例処理（任意の直交軸を見つける）
	if (angleDeg >= oppositeAngleThreshold)
	{
		VECTOR rightVec = VCross(startDir, Utility::DIR_R);
		axis = VCross(rightVec, startDir);

		if (VDot(axis, axis) < 1e-6f)
		{
			axis = Utility::DIR_UP;
		}
	}
	return AngleAxis(Utility::Deg2RadD(angleDeg), Utility::VNormalize(axis));
}

Quaternion Quaternion::RotateTowards(const Quaternion& current, const Quaternion& target, float maxDegreesDelta)
{
	double angleDiff = Angle(current, target);
	if (angleDiff < 1e-6)
	{
		return target;
	}

	// 1フレームの回転量を制限しつつSlerpを実行
	float t = std::min(1.0f, maxDegreesDelta / static_cast<float>(angleDiff));
	return SlerpUnclamped(current, target, t);
}

double Quaternion::Angle(const Quaternion& from, const Quaternion& to)
{
	// 内積から角度差を算出（絶対値をとることで最短の回転角を得る）
	double dotProduct = std::abs(Dot(from.Normalized(), to.Normalized()));
	if (dotProduct >= 1.0)
	{
		return 0.0;
	}

	const double radToDeg = 180.0 / DX_PI;
	return acos(dotProduct) * 2.0 * radToDeg;
}

Quaternion Quaternion::SlerpUnclamped(Quaternion start, Quaternion end, float interpolationFactor)
{
	if (start.LengthSquared() < kEpsilonNormalSqrt) return end;
	if (end.LengthSquared() < kEpsilonNormalSqrt) return start;

	start.Normalize();
	end.Normalize();

	double dotProduct = Dot(start, end);

	// 逆向きの場合は経路を反転させて最短距離を選択
	if (dotProduct < 0.0)
	{
		end = Quaternion(-end.w, -end.x, -end.y, -end.z);
		dotProduct = -dotProduct;
	}

	// 角度が非常に小さい場合は線形補間で近似
	if (dotProduct > 0.9995)
	{
		return Quaternion(
			start.w + (end.w - start.w) * interpolationFactor,
			start.x + (end.x - start.x) * interpolationFactor,
			start.y + (end.y - start.y) * interpolationFactor,
			start.z + (end.z - start.z) * interpolationFactor
		).Normalized();
	}

	// 球面線形補間の重み計算
	double theta0 = acos(dotProduct);
	double theta = theta0 * interpolationFactor;
	double sinTheta = sin(theta);
	double sinTheta0 = sin(theta0);

	double weightStart = cos(theta) - dotProduct * sinTheta / sinTheta0;
	double weightEnd = sinTheta / sinTheta0;

	return Quaternion(
		(start.w * weightStart) + (end.w * weightEnd),
		(start.x * weightStart) + (end.x * weightEnd),
		(start.y * weightStart) + (end.y * weightEnd),
		(start.z * weightStart) + (end.z * weightEnd)
	).Normalized();
}

Quaternion Quaternion::Identity(void)
{
	return Quaternion(1.0, 0.0, 0.0, 0.0);
}

double Quaternion::Length(void) const
{
	return sqrt(LengthSquared());
}

double Quaternion::LengthSquared(void) const
{
	return w * w + x * x + y * y + z * z;
}

VECTOR Quaternion::xyz(void) const
{
	return VGet(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void Quaternion::ToAngleAxis(float* outAngleRad, VECTOR* outAxis)
{
	Quaternion normalizedSelf = Normalized();
	// w成分から角度を逆算
	*outAngleRad = static_cast<float>(2.0 * acos(std::clamp(normalizedSelf.w, -1.0, 1.0)));

	double sinHalfAngle = sqrt(1.0 - normalizedSelf.w * normalizedSelf.w);
	if (sinHalfAngle < 0.001)
	{
		*outAxis = VGet(1.0f, 0.0f, 0.0f); // 角度がゼロに近い場合の軸
	}
	else
	{
		*outAxis = VGet(
			static_cast<float>(normalizedSelf.x / sinHalfAngle),
			static_cast<float>(normalizedSelf.y / sinHalfAngle),
			static_cast<float>(normalizedSelf.z / sinHalfAngle)
		);
	}
}

Quaternion Quaternion::Axis(const VECTOR& axis, float angleRad)
{
	return AngleAxis(angleRad, axis);
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
	// クォータニオンの外積公式による回転合成
	return Quaternion(
		w * other.w - x * other.x - y * other.y - z * other.z,
		w * other.x + x * other.w + y * other.z - z * other.y,
		w * other.y - x * other.z + y * other.w + z * other.x,
		w * other.z + x * other.y - y * other.x + z * other.w
	);
}

VECTOR Quaternion::GetDir(VECTOR direction) const
{
	return PosAxis(direction);
}

Quaternion Quaternion::operator*(float& scale)
{
	w *= scale; x *= scale; y *= scale; z *= scale;
	return *this;
}

const Quaternion Quaternion::operator*(const float& scale)
{
	return Quaternion(w * scale, x * scale, y * scale, z * scale);
}

Quaternion Quaternion::operator+(Quaternion& other)
{
	w += other.w; x += other.x; y += other.y; z += other.z;
	return *this;
}

const Quaternion Quaternion::operator+(const Quaternion& other)
{
	return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
}