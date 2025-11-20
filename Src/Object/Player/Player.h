#pragma once
#include "../UnitBase.h"

class Sword;
struct CollisionInfo;

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

	// プレイヤーのhpバーの描画
	void DrawHpBar(void) const;

	// ダメージ処理
	void TakeDamage(int damage);

	VECTOR* GetPosPtr(void);

	// 攻撃中かどうか
	bool IsAttacking(void) const;



	// 衝突時のコールバック
	void OnCollisionEnter(const CollisionInfo& info) override;
	void OnCollisionStay(const CollisionInfo& info) override;

protected:
	// コライダ初期化（必須実装）
	void InitCollider(void) override;

private:

	// 重力加速度（削除：UnitBaseで管理）
	// static constexpr float GRAVITY = -1.0f;

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

	// 攻撃クールタイムの最大値
	static constexpr float ATTACK_COOL_TIME_MAX = 0.5f;

	// 衝突判定用線分開始（地面判定用）
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f };

	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 本体の球体コライダ（ローカル位置）
	static constexpr VECTOR COL_SPHERE_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// 無敵時間
	float invincibleTime_;
	static constexpr float INVINCIBLE_DURATION = 1.0f;

	// パラメータ
	Param param_;

	// モデルハンドル
	int modelId_;

	// 移動可能か
	bool movementEnabled_;

	// 地面にいるかどうか
	bool isGround_;

	// 動いているかどうか
	bool isMoving_;

	// 表示状のHP
	float hpDisplay_;

	// 追いつく速度
	float hpDelaySpeed_;

	// 攻撃状態
	bool isAttacking_;

	// 攻撃クールタイム
	float attackCoolTime_;

	// 移動入力
	void ProcessMove(void);

	// 攻撃処理
	void Attack(void);

	void DrawCollisionCapsuleDebug(void) const;
};