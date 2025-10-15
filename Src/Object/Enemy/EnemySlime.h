#pragma once

#include <DxLib.h>

#include "EnemyBase.h"

//エネミースライムクラス
class EnemySlime : public EnemyBase
{
public:
	//コンストラクタ
	EnemySlime(void);

	//デストラクタ
	~EnemySlime(void) override = default;

	//読み込み
	void Load(int modelId) override;

	//初期化
	void Init(const VECTOR& startPos) override;

	//追従対象を設定
	void SetTargetPos(const VECTOR& pos);

	//更新処理
	void Update(void) override;

	//描画処理
	void Draw(void) const override;

	//
	void ApplyData(const EnemyInfo& info) override;

private:

	//移動速度
	VECTOR targetPos_;

};

