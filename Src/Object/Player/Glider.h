#pragma once
#include "../UnitBase.h"

class Glider : public UnitBase
{
public:
    // コンストラクタ
    Glider(void);

    // デストラクタ
    ~Glider(void);

    // リソースの読み込み
    void Load(void);

    // 初期化
    void Initialize(void);

    // 更新処理
    void Update(void);

    // 描画処理
    void Draw(void) const;

    // 解放処理
    void Release(void);

    // フレームの座標の設定
    void SetFramePos(const VECTOR& pos);

    // プレイヤーの回転を設定
    void SetPlayerRotation(const Quaternion& playerRot);

    // グライド状態の設定
    void SetGliding(bool gliding);

    // グライド中かどうか
    bool IsGliding(void) const;

private:
    // 定数
    static constexpr float POSITION_OFFSET_Y = -40.0f;        // Y座標オフセット
    static constexpr float POSITION_OFFSET_Z = 0.0f;       // Z座標オフセット（背中側）
    static constexpr VECTOR GLIDER_SCALE = { 0.4f, 0.4f, 0.4f };  // グライダーのスケール
    static constexpr float GLIDER_TILT_ANGLE = -15.0f;        // グライダーの傾き角度

    // グライダーの基準フレーム座標
    VECTOR framePos_;

    // グライド中フラグ
    bool isGliding_;

    // プレイヤーの回転情報
    Quaternion playerRot_;

    // コライダの初期化（不要だが継承元の要求に応じて）
    void InitCollider(void) override;
};