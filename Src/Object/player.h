#pragma once

#include "UnitBase.h"

class Player : public UnitBase
{
public:

	//ジャンプ力
	static constexpr float JUMP_POWER = 20.0f;

	//重力加速度
	static constexpr float GRAVITY = -1.0f;

	//コンストラクタ
	Player(void);

	//デストラクタ
	~Player(void);

	//リソースの読み込み
	void Load(void) override;

	//初期化
	void Init(void) override;

	//更新処理
	void Update(void) override;

	//描画処理
	void Draw(void) const override;

	//解放処理
	void Release(void) override;

	//移動可能かの設定
	void SetMovementEndbled(bool enabled);

	//移動可能かを取得
	bool IsMovementEndbled(void) const;
private:

	//モデルハンドル
	int modelId_;

	//移動制限x
	int blockedDirX_;

	//移動制限z
	int blockedDirZ_;

	//移動可能か
	bool movementEnabled_;

	//ジャンプ力
	float jumpPower_;

	//重力加速
	float gravity_;

	//現在のY方向速度
	float velocityY_;

	//地面にいるかどうか
	bool isGround_;

	//移動入力
	void ProcessMove(void);

};