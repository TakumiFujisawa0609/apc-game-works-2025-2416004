#pragma once

#include "../UnitBase.h"

/// @brief プレイヤーが滑空する際に使用するグライダーの表示・制御クラス
class Glider : public UnitBase
{
public:
    /// @brief コンストラクタ
    Glider(void);

    /// @brief デストラクタ
    ~Glider(void);

    /// @brief グライダーモデルのリソース読み込み
    void Load(void);

    /// @brief 初期化処理
    void Initialize(void);

    /// @brief 更新処理（位置同期や回転計算）
    void Update(void);

    /// @brief 描画処理
    void Draw(void) const;

    /// @brief 解放処理
    void Release(void);

    /// @brief 基準となるフレーム（プレイヤーの背中など）の座標を設定
    /// @param pos 設定するワールド座標
    void SetFramePos(const VECTOR& pos);

    /// @brief プレイヤーの現在の回転情報を同期
    /// @param playerRot プレイヤーのクォータニオン
    void SetPlayerRotation(const Quaternion& playerRot);

    /// @brief グライド（滑空）状態の有効・無効を切り替え
    /// @param gliding 有効ならtrue
    void SetGliding(bool gliding);

    /// @brief 現在グライド中かどうかを取得
    /// @return グライド中ならtrue
    bool IsGliding(void) const;

private:
    // 配置・外見に関する定数
    static constexpr float POSITION_OFFSET_Y = -40.0f;           // プレイヤー基準位置からのY軸オフセット
    static constexpr float POSITION_OFFSET_Z = 0.0f;             // プレイヤー基準位置からのZ軸オフセット
    static constexpr VECTOR GLIDER_SCALE = { 0.4f, 0.4f, 0.4f }; // モデルの表示スケール
    static constexpr float GLIDER_TILT_ANGLE = -15.0f;           // 飛行時の自然な傾き角度

    // 状態管理変数
    VECTOR framePos_;             // 追従対象となるフレームの座標
    bool isGliding_;              // 現在滑空状態にあるかどうかのフラグ
    Quaternion playerRot_;        // プレイヤーの向き（モデルの回転に反映）

    /// @brief コライダの初期化（グライダー自体に判定は持たせないため空実装）
    void InitCollider(void) override;
};