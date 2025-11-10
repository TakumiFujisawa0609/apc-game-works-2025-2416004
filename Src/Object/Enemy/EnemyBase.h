#pragma once

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

	//前方向を設定
	void SetForward(const VECTOR& forward);

	//視野角を設定
	void SetViewAngle(float angle);

	//HPの設定
	void SetHp(float hp);

	//HPの取得
	float GetHp(void) const;

	//ダメージ処理
	virtual void TakeDamage(float damage);

	//自身のタイプを返す
	const std::string& GetType(void) const;

	// 当たり判定用の座標ポインタを取得
	VECTOR* GetCollisionPosPtr(void);

	// 当たり判定のオフセット（派生クラスでオーバーライド可能）
	virtual VECTOR GetCollisionOffset(void) const;
protected:

	//体力
	float hp_;

	//最大体力
	float maxHp_;

	//基本移動速度
	float moveSpeed_;

	//視野距離
	float viewRange_;

	//視野解除距離
	float lostRange_;

	//視野角
	float viewAngle_;

	//追跡中フラグ
	bool isChasing_;

	//視野ないかどうか
	bool isInView_;

	//前方向
	VECTOR forward_;

	//タイプ
	std::string type_;

	// 当たり判定用位置
	VECTOR collisionPos_;
};

