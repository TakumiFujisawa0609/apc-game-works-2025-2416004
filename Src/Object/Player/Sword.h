#pragma once
#include "../UnitBase.h"

class Sword : public UnitBase
{
public:
    // コンストラクタ
    Sword(void);

    // デストラクタ
    ~Sword(void);

    // リソースの読み込み
    void Load(void);

    // 初期化
    void Init(void);

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

    // 攻撃状態の設定
    void SetAttacking(bool attacking);

    // 攻撃中かどうか
    bool IsAttacking(void) const;

private:

    // 定数
    static constexpr float CAPSULE_RADIUS = 15.0f;      // カプセル半径
    static constexpr float SWORD_TILT_ANGLE = -90.0f;   // 剣の傾き（通常時）
    static constexpr float SWORD_ATTACK_ANGLE = -90.0f;  // 剣の傾き（攻撃時）
    static constexpr float POSITION_OFFSET_Y = 10.0f;    // Y座標オフセット

    static constexpr VECTOR SOWRD_SCALE = { 80.0f, 80.0f, 80.0f };

    // 剣の基準フレーム座標
    VECTOR framePos_;

    // 攻撃中フラグ
    bool isAttacking_;

    // プレイヤーの回転情報
    Quaternion playerRot_;

    // コライダの初期化
    void InitCollider(void);

    
};