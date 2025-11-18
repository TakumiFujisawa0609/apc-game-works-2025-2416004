#pragma once
#include "ColliderBase.h"

class Transform;

class ColliderLine : public ColliderBase
{
public:

	// コンストラクタ
	ColliderLine(TAG tag, const Transform* follow, const VECTOR& localPosStart, const VECTOR& localPosEnd);

	// デストラクタ
	~ColliderLine(void) override;

	// ローカル座標での設定(開始地点)
	void SetLocalPosStart(const VECTOR& pos);

	// ローカル座標での設定(終了地点)
	void SetLocalPosEnd(const VECTOR& pos);

	// ローカル座標の取得(開始地点)
	const VECTOR& GetLocalPosStart(void) const;

	// ローカル座標の取得(終了地点)
	const VECTOR& GetLocalPosEnd(void) const;

	// ワールド座標の取得(開始地点)
	VECTOR GetPosStart(void) const;

	// ワールド座標の取得(終了地点)
	VECTOR GetPosEnd(void) const;

protected:

	// デバック用描画
	void DrawDebug(int color) override;

private:

	// デバック表示の球体半径
	static constexpr float RADIUS = 5.0f;

	// デバック表示の球体ポリゴン分割数
	static constexpr int DIV_NUM = 6;

	// 線分の開始地点座標(ローカル)
	VECTOR localPosStart_;

	// 線分の終了座標(ローカル)
	VECTOR localPosEnd_;
	


};

