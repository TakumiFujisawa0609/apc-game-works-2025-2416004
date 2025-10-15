#pragma once

#include <DxLib.h>
#include <string>

#include "../UnitBase.h"
#include "EnemyData.h"



//エネミーのベース
class EnemyBase : public UnitBase
{
public:

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

	//CSVデータを適用
	virtual void ApplyData(const EnemyInfo& info);;

	//HPの設定
	void SetHp(float hp);

	//HPの取得
	float GetHp(void) const;

	//ダメージ処理
	virtual void TakeDamage(float damage);

	//自身のタイプを返す
	const std::string& GetType(void) const;
protected:

	//体力
	float hp_;

	//最大体力
	float maxHp_;

	//基本移動速度
	float moveSpeed_;

	//タイプ
	std::string type_;
};

