#pragma once
#include "ColliderBase.h"

class Transform;

class ColliderCapsule : public ColliderBase
{
public:
    // コンストラクタ
    ColliderCapsule(
        TAG tag, const Transform* follow,
        const VECTOR& localPosStart, const VECTOR& localPosEnd, float radius);

    // デストラクタ
    ~ColliderCapsule(void) override;

    // ローカル座標での設定
    void SetLocalPosStart(const VECTOR& pos);
    void SetLocalPosEnd(const VECTOR& pos);
    void SetRadius(float radius);

    // ローカル座標の取得
    const VECTOR& GetLocalPosStart(void) const;
    const VECTOR& GetLocalPosEnd(void) const;

    // ワールド座標の取得
    VECTOR GetPosStart(void) const;
    VECTOR GetPosEnd(void) const;

    // 半径の取得
    float GetRadius(void) const;

protected:
    // デバッグ用描画
    void DrawDebug(int color) override;

private:
    // デバッグ表示の球体・カプセルポリゴン分割数
    static constexpr int DIV_NUM = 8;

    // カプセルの開始座標(ローカル)
    VECTOR localPosStart_;

    // カプセルの終了座標(ローカル)
    VECTOR localPosEnd_;

    // カプセルの半径
    float radius_;
};