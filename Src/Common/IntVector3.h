#pragma once
#include <DxLib.h>

/// @brief 整数の3次元ベクトルを表すクラス
class IntVector3
{
public:
    // 座標関連
    int x; // X座標
    int y; // Y座標
    int z; // Z座標

public:
    /// @brief ゼロベクトルで初期化
    IntVector3(void);

    /// @brief デストラクタ
    ~IntVector3(void) = default;

    /// @brief 指定した座標値で初期化
    /// @param initialX 初期X座標
    /// @param initialY 初期Y座標
    /// @param initialZ 初期Z座標
    IntVector3(int initialX, int initialY, int initialZ);

    /// @brief DxLibのVECTOR(float)から四捨五入して初期化
    /// @param floatVector 変換元の浮動小数点ベクトル
    IntVector3(const VECTOR& floatVector);

    /// @brief 未満比較
    /// @param other 比較対象のベクトル
    /// @return 自身が小さい場合 true
    bool operator<(const IntVector3& other) const;

    /// @brief 等価比較
    /// @param other 比較対象のベクトル
    /// @return 同値の場合 true
    bool operator==(const IntVector3& other) const;

    /// @brief 不等価比較
    /// @param other 比較対象のベクトル
    /// @return 異なる値の場合 true
    bool operator!=(const IntVector3& other) const;

    /// @brief 加算代入
    /// @param addVector 加算するベクトル
    /// @return 加算後の自身の参照
    IntVector3& operator+=(const IntVector3& addVector);

    /// @brief 減算代入
    /// @param subVector 減算するベクトル
    /// @return 減算後の自身の参照
    IntVector3& operator-=(const IntVector3& subVector);

    /// @brief スカラー乗算代入
    /// @param scaleValue 乗算する値
    /// @return 乗算後の自身の参照
    IntVector3& operator*=(int scaleValue);

    /// @brief 加算
    /// @param addVector 加算するベクトル
    /// @return 加算結果
    IntVector3 operator+(const IntVector3& addVector) const;

    /// @brief 減算
    /// @param subVector 減算するベクトル
    /// @return 減算結果
    IntVector3 operator-(const IntVector3& subVector) const;

    /// @brief スカラー乗算
    /// @param scaleValue 乗算する値
    /// @return 乗算結果
    IntVector3 operator*(int scaleValue) const;
};