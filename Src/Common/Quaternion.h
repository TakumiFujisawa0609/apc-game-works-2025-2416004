#pragma once
#include <DxLib.h>

/// @brief 回転を表現するためのクォータニオンクラス
class Quaternion
{
public:
    // 定数関連
    static constexpr float kEpsilonNormalSqrt = 1e-15F; // 正規化時の極小値（ゼロ割防止）

    // 成分関連
    double w; // スカラー成分（回転量の余弦）
    double x; // X成分（ベクトル部）
    double y; // Y成分（ベクトル部）
    double z; // Z成分（ベクトル部）

public:
    /// @brief デフォルトコンストラクタ（単位クォータニオンで初期化）
    Quaternion(void);

    /// @brief オイラー角（ラジアン）から初期化
    /// @param eulerRad X, Y, Z軸の回転角(ラジアン)
    Quaternion(const VECTOR& eulerRad);

    /// @brief 各成分から初期化
    /// @param initialW スカラー成分
    /// @param initialX X軸成分
    /// @param initialY Y軸成分
    /// @param initialZ Z軸成分
    Quaternion(double initialW, double initialX, double initialY, double initialZ);

    /// @brief デストラクタ
    ~Quaternion(void);

    /// @brief オイラー角からクォータニオンを生成
    /// @param eulerRad 各軸の回転角(ラジアン)
    /// @return 生成されたクォータニオン
    static Quaternion Euler(const VECTOR& eulerRad);

    /// @brief XYZ各軸からクォータニオンを生成
    /// @param radX X軸角度(ラジアン)
    /// @param radY Y軸角度(ラジアン)
    /// @param radZ Z軸角度(ラジアン)
    /// @return 生成されたクォータニオン
    static Quaternion Euler(double radX, double radY, double radZ);

    /// @brief 2つのクォータニオンを合成（掛け算）
    /// @param left 左側のクォータニオン
    /// @param right 右側のクォータニオン
    /// @return 合成されたクォータニオン
    static Quaternion Mult(const Quaternion& left, const Quaternion& right);

    /// @brief 現在のクォータニオンと指定クォータニオンを合成
    /// @param other 合成相手のクォータニオン
    /// @return 合成されたクォータニオン
    Quaternion Mult(const Quaternion& other) const;

    /// @brief 指定軸・角度の回転を表すクォータニオンを生成
    /// @param angleRad 回転角(ラジアン)
    /// @param axis 回転軸ベクトル
    /// @return 生成されたクォータニオン
    static Quaternion AngleAxis(double angleRad, VECTOR axis);

    /// @brief ベクトルを回転させる（静的）
    /// @param rotation 回転クォータニオン
    /// @param targetPos 回転対象ベクトル
    /// @return 回転後のベクトル
    static VECTOR PosAxis(const Quaternion& rotation, VECTOR targetPos);

    /// @brief ベクトルを回転させる（メンバ）
    /// @param targetPos 回転対象ベクトル
    /// @return 回転後のベクトル
    VECTOR PosAxis(VECTOR targetPos) const;

    /// @brief クォータニオンからオイラー角に変換（静的）
    /// @param target 変換対象のクォータニオン
    /// @return オイラー角(ラジアン)
    static VECTOR ToEuler(const Quaternion& target);

    /// @brief クォータニオンからオイラー角に変換（メンバ）
    /// @return オイラー角(ラジアン)
    VECTOR ToEuler(void) const;

    /// @brief クォータニオンを回転行列に変換（静的）
    /// @param target 変換対象のクォータニオン
    /// @return 回転行列
    static MATRIX ToMatrix(const Quaternion& target);

    /// @brief クォータニオンを回転行列に変換（メンバ）
    /// @return 回転行列
    MATRIX ToMatrix(void) const;

    /// @brief 方向ベクトルからクォータニオンを生成
    /// @param forwardDir 向く方向（前方向ベクトル）
    /// @return 生成されたクォータニオン
    static Quaternion LookRotation(VECTOR forwardDir);

    /// @brief 方向ベクトルとアップベクトルからクォータニオンを生成
    /// @param forwardDir 向く方向（前方向ベクトル）
    /// @param upDir 上方向ベクトル
    /// @return 生成されたクォータニオン
    static Quaternion LookRotation(VECTOR forwardDir, VECTOR upDir);

    /// @brief 行列から回転クォータニオンを抽出
    /// @param rotationMatrix 回転行列
    /// @return 抽出されたクォータニオン
    static Quaternion GetRotation(MATRIX rotationMatrix);

    /// @brief 前方向ベクトルを取得
    /// @return 前方向(Z+)ベクトル
    VECTOR GetForward(void) const;

    /// @brief 後方向ベクトルを取得
    /// @return 後方向(Z-)ベクトル
    VECTOR GetBack(void) const;

    /// @brief 右方向ベクトルを取得
    /// @return 右方向(X+)ベクトル
    VECTOR GetRight(void) const;

    /// @brief 左方向ベクトルを取得
    /// @return 左方向(X-)ベクトル
    VECTOR GetLeft(void) const;

    /// @brief 上方向ベクトルを取得
    /// @return 上方向(Y+)ベクトル
    VECTOR GetUp(void) const;

    /// @brief 下方向ベクトルを取得
    /// @return 下方向(Y-)ベクトル
    VECTOR GetDown(void) const;

    /// @brief クォータニオンの内積を計算
    /// @param left 第1クォータニオン
    /// @param right 第2クォータニオン
    /// @return 内積値
    static double Dot(const Quaternion& left, const Quaternion& right);

    /// @brief 現在のクォータニオンと別のクォータニオンとの内積
    /// @param other 比較対象
    /// @return 内積値
    double Dot(const Quaternion& other) const;

    /// @brief クォータニオンを正規化（静的）
    /// @param target 正規化対象
    /// @return 正規化されたクォータニオン
    static Quaternion Normalize(const Quaternion& target);

    /// @brief 正規化されたクォータニオンを返す（非破壊）
    /// @return 正規化されたクォータニオン
    Quaternion Normalized(void) const;

    /// @brief クォータニオンを正規化（破壊的）
    void Normalize(void);

    /// @brief 逆クォータニオン（共役）を返す
    /// @return 逆クォータニオン
    Quaternion Inverse(void) const;

    /// @brief 球面線形補間（Slerp）
    /// @param start 開始回転
    /// @param end 終了回転
    /// @param interpolationFactor 補間率（0.0～1.0）
    /// @return 補間後のクォータニオン
    static Quaternion Slerp(Quaternion start, Quaternion end, double interpolationFactor);

    /// @brief startDirからtoDirへの回転を取得
    /// @param startDir 開始方向
    /// @param toDir 目標方向
    /// @return 2つの方向を繋ぐ回転クォータニオン
    static Quaternion FromToRotation(VECTOR startDir, VECTOR toDir);

    /// @brief 最大角度制限付きの回転補間
    /// @param current 現在の回転
    /// @param target 目標回転
    /// @param maxDegreesDelta 最大回転角（度）
    /// @return 補間後のクォータニオン
    static Quaternion RotateTowards(const Quaternion& current, const Quaternion& target, float maxDegreesDelta);

    /// @brief 2つのクォータニオンの角度差（度）を計算
    /// @param from クォータニオン1
    /// @param to クォータニオン2
    /// @return 角度差（度）
    static double Angle(const Quaternion& from, const Quaternion& to);

    /// @brief 補間率を制限しないSlerp
    /// @param start 開始クォータニオン
    /// @param end 終了クォータニオン
    /// @param interpolationFactor 補間係数
    /// @return 補間後のクォータニオン
    static Quaternion SlerpUnclamped(Quaternion start, Quaternion end, float interpolationFactor);

    /// @brief 単位クォータニオン（回転なし）を取得
    /// @return 単位クォータニオン
    static Quaternion Identity(void);

    /// @brief クォータニオンの長さ（ノルム）を取得
    /// @return 長さ
    double Length(void) const;

    /// @brief クォータニオンの長さの2乗を取得
    /// @return 長さの2乗
    double LengthSquared(void) const;

    /// @brief x, y, z 成分をベクトルで取得
    /// @return 成分ベクトル
    VECTOR xyz(void) const;

    /// @brief クォータニオンを角度と軸に分解
    /// @param outAngleRad 回転角（ラジアン）の出力先
    /// @param outAxis 回転軸ベクトルの出力先
    void ToAngleAxis(float* outAngleRad, VECTOR* outAxis);

    /// @brief 回転軸と角度（ラジアン）からクォータニオンを生成
    /// @param axis 回転軸ベクトル
    /// @param angleRad 回転角度（ラジアン）
    /// @return 生成されたクォータニオン
    static Quaternion Axis(const VECTOR& axis, float angleRad);

    /// @brief クォータニオン同士の乗算
    /// @param other 掛ける対象
    /// @return 合成後のクォータニオン
    Quaternion operator*(const Quaternion& other) const;

private:
    /// @brief 指定方向のベクトルを現在の回転で回す
    /// @param direction 方向ベクトル
    /// @return 回転後のベクトル
    VECTOR GetDir(VECTOR direction) const;

    /// @brief スカラー乗算（破壊的）
    /// @param scale 係数
    /// @return 乗算後の自身の参照
    Quaternion operator*(float& scale);

    /// @brief スカラー乗算（非破壊）
    /// @param scale 係数
    /// @return 乗算後のクォータニオン
    const Quaternion operator*(const float& scale);

    /// @brief 加算（破壊的）
    /// @param other 加算対象
    /// @return 加算後の自身の参照
    Quaternion operator+(Quaternion& other);

    /// @brief 加算（非破壊）
    /// @param other 加算対象
    /// @return 加算後のクォータニオン
    const Quaternion operator+(const Quaternion& other);
};