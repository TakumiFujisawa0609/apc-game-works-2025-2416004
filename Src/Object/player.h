#pragma once
#include<DxLib.h>
#include "UnitBase.h"

class AnimationController;

class Player : public UnitBase
{

public:

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void) override;

	//初期化
	void Init(void) override;

	//更新
	void Update(void) override;

	//描画
	void Draw(void) override;

	//解放
	void Release(void) override;

	//座標取得
	VECTOR GetPos(void) const;

	//座標設定
	void SetPos(VECTOR pos);

	// プレイヤーのAABB最小点を取得
	VECTOR GetHitMin(void) const;

	// プレイヤーのAABB最大点を取得
	VECTOR GetHitMax(void) const;

	//当たり判定の半径
	float GetRadius(void) const;

	//プレイヤーの所持金を取得
	int GetMoney(void) const;

	//プレイヤーの所持金を設定
	void AddMoney(int money);

	void SetBlockedDirX(int dir);

	void SetBlockedDirZ(int dir);

	void ResetBlockDirs(void);

	void SetMovementEnabled(bool enabled);
	
	bool IsMovementEnabled(void) const;



	// アニメーション種別
	enum class ANIM_TYPE
	{
		NOME,
		WALK,
		IDLE,
		MAX
	};

	// モデルの大きさ
	static constexpr VECTOR SCALES = { 1.0f, 1.0f, 1.0f };

	// 初期位置
	static constexpr VECTOR DEFAULT_POS = { 0.0f, 20.0f, -200.0f };

	// 標準の自己発光色
	static constexpr COLOR_F COLOR_EMI_DEFAULT = { 0.5f, 0.5f, 0.5f, 0.5f };

	// 移動スピード
	static constexpr float SPEED_MOVE = 5.0f;

	//当たり判定の半径
	static constexpr float RADIUS = 30.0f;

	void PlayAnim(ANIM_TYPE type, bool loop = true);


private:
	// モデルID
	int modelId_;
	VECTOR scales_;
	//カメラ
	VECTOR axis_;
	VECTOR angles_;

	// 地面にいるかどうか
	bool isOnGround_;

	//所持金
	int money_;

	int blockedDirX_;

	int blockedDirZ_;

	bool movementEnabled_;

	int currentAnimType_; 


	// アニメーション
	AnimationController* animationController_;

	// 行動制御
	void ProcessMove(void);

};