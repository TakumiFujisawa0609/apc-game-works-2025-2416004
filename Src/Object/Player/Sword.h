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

    // 攻撃状態の設定
    void SetAttacking(bool attacking);

    // 攻撃中かどうか
    bool IsAttacking(void) const;

private:
    // カプセルコライダの半径
    static constexpr float CAPSULE_RADIUS = 15.0f;

    // フレームの座標
    VECTOR framePos_;

    // 攻撃中フラグ
    bool isAttacking_;

    // コライダーの初期化
    void InitCollider(void) override;
};