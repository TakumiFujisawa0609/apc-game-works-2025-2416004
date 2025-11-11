#pragma once
#include "../UnitBase.h"


class Player;

class Sword : public UnitBase
{
public:
    // コンストラクタ
    Sword(void);

    // デストラクタ
    ~Sword(void) override;

    // リソースの読み込み
    void Load(void) override;

    // 初期化
    void Init(void) override;

    // 更新
    void Update(void) override;

    // 描画
    void Draw(void) const override;

    // 解放
    void Release(void) override;

    // プレイヤーの設定
    void SetPlayer(std::weak_ptr<Player> player);

    // フレーム番号の設定
    void SetAttachFrame(int frameIndex);

    // 表示/非表示設定
    void SetVisible(bool visible);

    // 表示中か
    bool IsVisible(void) const;

    // 座標の微調整
    void SetPositionOffset(const VECTOR& offset);

    // 回転の微調整
    void SetRotationOffset(const VECTOR& offset);

    // 当たり半径
    float GetCapsuleRadius(void);

    // カプセル開始地点
    VECTOR GetCapsuleStart(void);

    // カプセル終了地点
    VECTOR GetCapsuleEnd(void);

private:
    // プレイヤーへの参照
    std::weak_ptr<Player> player_;

    // 装着するフレーム番号
    int attachFrameIndex_;

    // 位置オフセット（調整用）
    VECTOR positionOffset_;

    // 回転オフセット（調整用）
    VECTOR rotationOffset_;

    // 表示フラグ
    bool isVisible_;

    // 当たり半径
    float capsuleRadius_;

    // カプセル開始地点
    VECTOR capsuleStart_;

    // カプセル終了地点
    VECTOR capsuleEnd_;

    // プレイヤーのフレームに追従
    void FollowPlayerFrame(void);
};