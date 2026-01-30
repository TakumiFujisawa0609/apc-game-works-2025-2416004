#pragma once

#include "../UnitBase.h"

class SkyDome: public UnitBase
{
public:
	// コンストラクタ
	SkyDome(void);

	// デストラクタ
	~SkyDome(void) override;

	// リソースの読み込み
	void Load(void) override;

	// 初期化
	void Init(void) override;

	// 更新処理
	void Update(void) override;

	// 描画処理
	void Draw(void) const override;

	// 追従対象の設定
	void SetFollowTarget(const VECTOR* target);

private:
	// スカイドームの大きさ
	static constexpr VECTOR SKY_DOME_SCL = { 45.0f, 45.0f, 45.0f };

	// スカイドームの基本座標
	static constexpr VECTOR SKY_DOME_POS = { 0.0f, 0.0f, 0.0f };

	// スカイドームのローカル回転
	static constexpr VECTOR SKY_DOME_LOC_ROT = { 0.0f, DX_PI_F / 2, 0.0f };

	// 追従対象
	const VECTOR* followTarget_;

	void InitCollider(void);
};

