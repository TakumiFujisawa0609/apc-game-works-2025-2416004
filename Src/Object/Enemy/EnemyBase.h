#pragma once

#include <DxLib.h>

#include "../UnitBase.h"


//エネミーのベース
class EnemyBase : public UnitBase
{
public:
	//最大体力
	static constexpr float MAX_HP = 100.0f;

	//移動速度
	static constexpr float DEFAULT_SPEED = 10.0f;

	//コンストラクタ
	EnemyBase(void);

	//デストラクタ
	~EnemyBase(void) override = default;

	//読み込み
	virtual void Load(int modelId);

	//初期化
	virtual void Init(const VECTOR& startPos);

	//更新処理
	void Update(void) override;

	//描画処理
	void Draw(void) const override;

	//HPの設定
	void SetHp(float hp);

	//HPの取得
	float GetHp(void) const;

	//ダメージ処理
	virtual void TakeDamage(float damage);

	//パラメータ
	virtual void SetParam(void) = 0;

protected:

	//体力
	float hp_;

	//最大体力
	float maxHp_;

	//基本移動速度
	float moveSpeed_;
};

