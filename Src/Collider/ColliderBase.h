#pragma once

#include "../Object/Common/Transform.h"

class ColliderBase
{
public:

	// 形状
	enum class SHAPE
	{
		NONE,
		LINE,
		SPHERE,
		CAPSULE,
		MODEL,
	};

	// 衝突種別
	enum class TAG
	{
		STAGE,
		PLAYER,
		ENEMY,
		CAMERA,
		WALL,
		GROUND,
		SWORD,
	};

	// コンストラクタ
	ColliderBase(SHAPE shape, TAG tag, const Transform* follow);

	// デストラクタ
	virtual ~ColliderBase(void);

	// 描画
	void Draw(void);

	// 追従先の取得
	const Transform* GetFollow(void) const;

	// 追従先の再設定
	void SetFollow(Transform* follow);

	// 形状の取得
	SHAPE GetShape(void) const;

	// 衝突種別の取得
	TAG GetTag(void) const;

	// 有効フラグの取得
	bool IsValid(void) const;

	// 有効フラグの設定
	void SetValid(bool valid);

protected:

	// デバック表示の色
	static constexpr int COLOR_VALID = 0xff0000;
	static constexpr int COLOR_INVALID = 0xaaaaaa;

	// 形状
	SHAPE shape_;

	// 衝突種別
	TAG tag_;

	// 追従先
	const Transform* follow_;

	// 有効フラグ
	bool isValid_;

	// ローカル座標をワールド座標に変換
	VECTOR GetRotPos(const VECTOR& localPos) const;

	// デバック用描画
	virtual void DrawDebug(int color) = 0;
};

