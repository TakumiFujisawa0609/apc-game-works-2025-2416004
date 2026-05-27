#pragma once

#include <DxLib.h>

#include "../UnitBase.h"

/// @brief プレイヤーの武器（剣）を制御するクラス
class Sword : public UnitBase
{
public:
    /// @brief コンストラクタ
    Sword(void);

    /// @brief デストラクタ
    ~Sword(void);

    /// @brief リソースの読み込み
    void Load(void);

    /// @brief 初期化
    void Initialize(void);

    /// @brief 更新処理
    void Update(void);

    /// @brief 描画処理
    void Draw(void) const;

    /// @brief 解放処理
    void Release(void);

    /// @brief フレームの座標の設定
    /// @param pos 設定する座標
    void SetFramePos(const VECTOR& pos);

    /// @brief プレイヤーの回転を設定
    /// @param playerRot プレイヤーのクォータニオン
    void SetPlayerRotation(const Quaternion& playerRot);

    /// @brief 攻撃状態の設定
    /// @param attacking 攻撃中ならtrue
    void SetAttacking(bool attacking);

    /// @brief 攻撃中かどうか
    /// @return 攻撃中ならtrue
    bool IsAttacking(void) const;

    /// @brief 手の行列を設定する
    /// @param mat モデルの手のフレーム行列
    void SetHandMatrix(const MATRIX& mat) { handMatrix_ = mat; }

private:
    // 定数関連
    static constexpr float CAPSULE_RADIUS = 15.0f;                       // 当たり判定の半径
    static constexpr float SWORD_TILT_ANGLE = -90.0f;                    // 通常時の傾き
    static constexpr float SWORD_ATTACK_ANGLE = -90.0f;                  // 攻撃時の傾き
    static constexpr VECTOR POSITION_OFFSET_Y = { 0.0f, 0.0f, 0.0f };    // 座標調整オフセット
    static constexpr VECTOR CAPSULE_STATE_POS = { 0.0f, 0.0f, 0.0f };    // 判定開始位置
    static constexpr VECTOR CAPSULE_END_POS = { 0.0f, 0.0f, 140.0f };    // 判定終了位置
    static constexpr VECTOR SOWRD_SCALE = { 80.0f, 80.0f, 80.0f };       // モデルのスケール

    // トランスフォーム関連
    VECTOR framePos_;       // 剣の基準フレーム座標
    Quaternion playerRot_;  // プレイヤーの回転情報
    MATRIX handMatrix_;     // 手のボーン行列

    // 攻撃中フラグ
    bool isAttacking_;      

    /// @brief コライダの初期化
    void InitCollider(void);
};