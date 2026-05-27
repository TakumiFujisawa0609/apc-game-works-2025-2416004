#pragma once
#include <DxLib.h>

#include "../UnitBase.h"

/// @brief 背景の空（スカイドーム）を制御するクラス
class SkyDome : public UnitBase
{
public:
    /// @brief コンストラクタ
    SkyDome(void);

    /// @brief デストラクタ
    ~SkyDome(void) override;

    /// @brief リソースの読み込み
    void Load(void) override;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) const override;

    /// @brief 追従対象（主にプレイヤー）の設定
    /// @param target 追従する座標へのポインタ
    void SetFollowTarget(const VECTOR* target);

private:
    // 定数関連
    static constexpr VECTOR SKY_DOME_SCL = { 45.0f, 45.0f, 45.0f };         // スカイドームの大きさ
    static constexpr VECTOR SKY_DOME_POS = { 0.0f, 0.0f, 0.0f };            // 基本座標
    static constexpr VECTOR SKY_DOME_LOC_ROT = { 0.0f, DX_PI_F / 2, 0.0f }; // 初期回転

    // 追従対象の座標ポインタ
    const VECTOR* followTarget_; 

    /// @brief 衝突判定の初期化（スカイドームには判定不要なため空実装想定）
    void InitCollider(void);
};