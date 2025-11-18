#pragma once
#include "ColliderBase.h"

class Transform;

class ColliderSphere : public ColliderBase
{
public:
   
    // コンストラクタ
    ColliderSphere(TAG tag, const Transform* follow, const VECTOR& localPos, float radius);

    // デストラクタ
    ~ColliderSphere(void) override;

    // ローカル座標の設定
    void SetLocalPos(const VECTOR& pos);

    // 衝突半径の設定
    void SetRadius(float radius);

    // ローカル座標の取得
    const VECTOR& GetLocalPos(void) const;

    // ワールド座標の取得
    VECTOR GetPos(void) const;

    // 半径の取得
    float GetRadius(void) const;

protected:
    // デバッグ用描画
    void DrawDebug(int color) override;

private:
    // デバッグ表示の球体ポリゴン分割数
    static constexpr int DIV_NUM = 16;

    // 球体の中心座標(ローカル)
    VECTOR localPos_;

    // 球体の半径
    float radius_;
};