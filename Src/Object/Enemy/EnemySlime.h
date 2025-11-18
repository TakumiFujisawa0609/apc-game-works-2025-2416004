#pragma once

#include "EnemyBase.h"

//エネミースライムクラス
class EnemySlime : public EnemyBase
{
public:

	// コンストラクタ
	EnemySlime(void);

	// デストラクタ
	~EnemySlime(void) override = default;

	// 読み込み
	void Load(int modelId) override;

	// 初期化
	void Init(const VECTOR& startPos) override;

	// 追従対象を設定
	void SetTargetPos(const VECTOR& pos);

	// 更新処理
	void Update(void) override;

	// 描画処理
	void Draw(void) const override;

	// 全パラメータを適用
	void ApplyData(const EnemyInfo& info) override;

private:
	// 最大視野
	static constexpr float VIEW_RANGE = 600.0f;

	// 視野の解除距離
	static constexpr float LOST_RANGE = 30.0f;

	// 視野角
	static constexpr float VIEW_ANGLE = 60.0f;

	// 当たり判定の高さ
	static constexpr float COLLISION_HEIGHT_OFFSET = 40.0f;

	// 移動速度
	VECTOR targetPos_;

};

