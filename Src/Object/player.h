#pragma once

#include <DxLib.h>
#include <memory>

#include "UnitBase.h"

class Player : public UnitBase
{
public:

	// プレイヤー
	struct Param
	{
		int attack = 0;                // 攻撃力

		int defensse = 0;              // 防御力

		int hp = 0;                    // 現在の体力

		int maxHp = 0;                 // 最大体力

		int stamina = 0;               // スタミナ

		float collisionRadius = 1.0f;  // 当たり判定

		int level = 1;                 // 現在のレベル

		int maxLevel = 1;              // 最大レベル

		float jumpPower = 0.0f;        // ジャンプ力

	};

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void);

	// CSV読み込み
	void LoadParamCSV(const std::string& path);

	// リソースの読み込み
	void Load(void) override;

	// 初期化
	void Init(void) override;

	// 更新処理
	void Update(void) override;

	// 描画処理
	void Draw(void) const override;

	// 解放処理
	void Release(void) override;

	// 移動可能かの設定
	void SetMovementEndbled(bool enabled);

	// 移動可能かを取得
	bool IsMovementEndbled(void) const;

	// プレイヤーのパラメータ取得
	const Param& GetParam(void) const;
private:

	// 重力加速度
	static constexpr float GRAVITY = -1.0f;

	// プレイヤーの大きさ
	static constexpr VECTOR PLAYER_SCL = { 2.5f, 2.5f, 2.5f };

	// HPバーの長さ
	static constexpr int HP_BAR_WIDTH = 300;

	// HPバーの高さ
	static constexpr int HP_BAR_HEIGHT = 25;

	// HPの数値の文字幅の調整
	static constexpr int HP_TEXT_HALF_WIDTH = 30;

	// HPの数値の高さの調整
	static constexpr int HP_TEXT_HALF_HEIGHT = 8;

	// HPバーを下からちょっと上に
	static constexpr int BAR_Y = 60;

	// HPバーの枠カラー
	static constexpr VECTOR HP_BAR_FRAME = { 255, 255, 255 };

	// 遅延スピード
	static constexpr float DELAY_SPEED = 0.5f;

	// パラメータ
	Param param_;

	// モデルハンドル
	int modelId_;

	// 移動制限x
	int blockedDirX_;

	// 移動制限z
	int blockedDirZ_;

	// 移動可能か
	bool movementEnabled_;

	// ジャンプ力
	float jumpPower_;

	// 重力加速
	float gravity_;

	// 現在のY方向速度
	float velocityY_;

	// 地面にいるかどうか
	bool isGround_;

	// 動いているかどうか
	bool isMoving_;

	// 表示状のHP
	float hpDisplay_;

	// 追いつく速度
	float hpDelaySpeed_;

	// 移動入力
	void ProcessMove(void);

};