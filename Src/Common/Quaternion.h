#pragma once

#include<DxLib.h>
#include<iostream>
#include<algorithm>

/// <summary>
/// 回転を表現するためのクォータニオンクラス
/// </summary>
class Quaternion
{
public:

	/// <summary>
	/// 正規化時の極小値（ゼロ割防止）
	/// </summary>
	static constexpr float kEpsilonNorMalSqrt = 1e-15F;

	/// <summary>
	/// クォータニオンのスカラー成分（回転量の余弦）
	/// </summary>
	double w;

	/// <summary>
	/// クォータニオンのX成分（ベクトル部）
	/// </summary>
	double x;

	/// <summary>
	/// クォータニオンのY成分（ベクトル部）
	/// </summary>
	double y;

	/// <summary>
	/// クォータニオンのZ成分（ベクトル部）
	/// </summary>
	double z;

	/// <summary>
	/// デフォルトコンストラクタ（すべて0）
	/// </summary>
	Quaternion(void);

	/// <summary>
	/// オイラー角（ラジアン）から初期化
	/// </summary>
	/// <param name="rad">X, Y, Z軸の回転角</param>
	Quaternion(const VECTOR& rad);
	
	/// <summary>
    /// 各成分から初期化
    /// </summary>
    /// <param name="w">スカラー成分</param>
    /// <param name="x">X軸成分</param>
    /// <param name="y">Y軸成分</param>
    /// <param name="z">Z軸成分</param>
	Quaternion(double w, double x, double y, double z);

	/// <summary>
    /// デストラクタ
    /// </summary>
	~Quaternion(void);

	/// <summary>
    /// オイラー角からクォータニオンを生成
    /// </summary>
    /// <param name="rad">各軸の回転角</param>
	static Quaternion Euler(const VECTOR& rad);

	/// <summary>
	/// XYZ各軸からクォータニオンを生成
	/// </summary>
	/// <param name="radX">X軸角度</param>
	/// <param name="radY">Y軸角度</param>
	/// <param name="radZ">Z軸角度</param>
	static Quaternion Euler(double radX, double radY, double radZ);

	/// <summary>
	/// 2つのクォータニオンを合成
	/// </summary>
	/// <param name="q1">第1クォータニオン</param>
	/// <param name="q2">第2クォータニオン</param>
	static Quaternion Mult(const Quaternion& q1, const Quaternion& q2);

	/// <summary>
    /// 現在のクォータニオンと指定クォータニオンを合成
    /// </summary>
    /// <param name="q">合成相手のクォータニオン</param>
	Quaternion Mult(const Quaternion& q) const;

	/// <summary>
	/// 指定軸・角度の回転を表すクォータニオンを生成
	/// </summary>
	/// <param name="rad">回転角</param>
	/// <param name="axis">回転軸ベクトル</param>
	static Quaternion AngleAxis(double rad, VECTOR axis);

	/// <summary>
	/// ベクトルを回転させる（静的）
	/// </summary>
	/// <param name="q">回転クォータニオン</param>
	/// <param name="axis">回転対象ベクトル</param>
	static VECTOR PosAxis(const Quaternion& q, VECTOR axis);

	/// <summary>
    /// ベクトルを回転させる（メンバ）
    /// </summary>
    /// <param name="pos">回転対象ベクトル</param>
	VECTOR PosAxis(VECTOR pos) const;

	/// <summary>
	/// クォータニオンからオイラー角に変換（静的）
	/// </summary>
	/// <param name="q">変換対象</param>
	static VECTOR ToEuler(const Quaternion& q);

	/// <summary>
	/// クォータニオンからオイラー角に変換（メンバ）
	/// </summary>
	/// <param name="pos">無視される（将来的な拡張？）</param>
	VECTOR ToEuler(void) const;

	/// <summary>
	/// クォータニオンを回転行列に変換（静的）
	/// </summary>
	/// <param name="q">変換対象</param>
	static MATRIX ToMatrix(const Quaternion& q);

	/// <summary>
	/// クォータニオンを回転行列に変換（メンバ）
	/// </summary>
	MATRIX ToMatrix(void) const;

	/// <summary>
	/// 方向ベクトルからクォータニオンを生成（前方向のみ）
	/// </summary>
	/// <param name="dir">向く方向</param>
	static Quaternion LookRotation(VECTOR dir);

	/// <summary>
	/// 方向ベクトルからクォータニオンを生成（前方向のみ）
	/// </summary>
	/// <param name="dir">向く方向</param>
	static Quaternion LookRotation(VECTOR dir, VECTOR up);

	/// <summary>
    /// 行列から回転クォータニオンを抽出
    /// </summary>
    /// <param name="mat">回転行列</param>
	static Quaternion GetRotation(MATRIX mat);

	/// <summary>
	/// 前方向ベクトルを取得
	/// </summary>
	VECTOR GetForward(void) const;

	/// <summary>
	/// 後方向ベクトルを取得
	/// </summary>
	VECTOR GetBack(void) const;

	/// <summary>
	/// 右方向ベクトルを取得
	/// </summary>
	VECTOR GetRight(void) const;

	/// <summary>
	/// 左方向ベクトルを取得
	/// </summary>
	VECTOR GetLeft(void) const;

	/// <summary>
	/// 上方向ベクトルを取得
	/// </summary>
	VECTOR GetUp(void) const;

	/// <summary>
	/// 下方向ベクトルを取得
	/// </summary>
	VECTOR GetDown(void) const;

	/// <summary>
	/// クォータニオンの内積を計算
	/// </summary>
	/// <param name="q1">第1クォータニオン</param>
	/// <param name="q2">第2クォータニオン</param>
	static double Dot(const Quaternion& q1, const Quaternion& q2);

	/// <summary>
    /// 現在のクォータニオンと別のクォータニオンとの内積
    /// </summary>
    /// <param name="b">比較対象</param>
	double Dot(const Quaternion& b) const;

	/// <summary>
	/// クォータニオンを正規化（静的）
	/// </summary>
	/// <param name="q">正規化対象</param>
	static Quaternion Normalize(const Quaternion& q);

	/// <summary>
    /// 正規化されたクォータニオンを返す（非破壊）
    /// </summary>
	Quaternion Normalized(void) const;

	/// <summary>
	/// クォータニオンを正規化（破壊的）
	/// </summary>
	void Normalize(void);

	/// <summary>
    /// 逆クォータニオン（共役）を返す
    /// </summary>
	Quaternion Inverse(void) const;

	/// <summary>
	/// 球面線形補間（Slerp）
	/// </summary>
	/// <param name="from">開始回転</param>
	/// <param name="to">終了回転</param>
	/// <param name="t">補間率（0.0～1.0）</param>
	static Quaternion Slerp(Quaternion from, Quaternion to, double t);

	/// <summary>
	/// from→to の回転を取得
	/// </summary>
	/// <param name="fromDir">開始方向</param>
	/// <param name="toDir">目標方向</param>
	static Quaternion FromToRotation(VECTOR fromDir, VECTOR toDir);
	
	/// <summary>
	/// 最大角度制限付きの回転補間
	/// </summary>
	/// <param name="from">現在の回転</param>
	/// <param name="to">目標回転</param>
	/// <param name="maxDegreesDelta">最大回転角（度）</param>
	static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta);
	
	/// <summary>
    /// 2つのクォータニオンの角度差を計算
    /// </summary>
    /// <param name="q1">クォータニオン1</param>
    /// <param name="q2">クォータニオン2</param>
	static double Angle(const Quaternion& q1, const Quaternion& q2);

	/// <summary>
	/// 補間率を制限しないSlerp
	/// </summary>
	/// <param name="a">開始クォータニオン</param>
	/// <param name="b">終了クォータニオン</param>
	/// <param name="t">補間係数</param>
	static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);

	/// <summary>
	/// 単位クォータニオン（回転なし）を取得
	/// </summary>
	static Quaternion Identity(void);

	/// <summary>
    /// クォータニオンの長さを取得
    /// </summary>
	double Length(void) const;

	/// <summary>
	/// クォータニオンの長さの2乗を取得
	/// </summary>
	double LengthSquared(void) const;

	/// <summary>
	/// x, y, z 成分をベクトルで取得
	/// </summary>
	VECTOR xyz(void) const;

	/// <summary>
	/// クォータニオンを角度と軸に分解（未実装の可能性あり）
	/// </summary>
	void ToAngleAxis(float* angle, VECTOR* axis);

private:

	/// <summary>
	/// 指定方向のベクトルを現在の回転で回す
	/// </summary>
	/// <param name="dir">方向ベクトル</param>
	VECTOR GetDir(VECTOR dir) const;

	/// <summary>
	/// スカラー乗算（破壊的）
	/// </summary>
	/// <param name="rhs">係数</param>
	Quaternion operator*(float& rhs);

	/// <summary>
	/// スカラー乗算（非破壊）
	/// </summary>
	/// <param name="rhs">係数</param>
	const Quaternion operator*(const float& rhs);

	/// <summary>
	/// 加算（破壊的）
	/// </summary>
	/// <param name="rhs">加算対象</param>
	Quaternion operator+(Quaternion& rhs);

	/// <summary>
	/// 加算（非破壊）
	/// </summary>
	/// <param name="rhs">加算対象</param>
	const Quaternion operator+(const Quaternion& rhs);
	
};

