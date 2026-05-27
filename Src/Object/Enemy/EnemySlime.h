#pragma once

#include "EnemyBase.h"

/// @brief エネミー：スライムクラス
/// @details EnemyBaseを継承し、スライム固有の挙動（追従など）を管理する
class EnemySlime : public EnemyBase
{
public:

	/// @brief コンストラクタ
	EnemySlime(void);

	/// @brief デストラクタ
	~EnemySlime(void) override = default;

	/// @brief モデルの読み込み
	/// @param modelId 読み込むモデルのID
	void Load(int modelId) override;

	/// @brief 初期化処理
	/// @param startPos 初期出現座標
	void Initialize(const VECTOR& startPos) override;

	/// @brief ターゲット（プレイヤー等）の座標を設定
	/// @param pos 対象の座標
	void SetTargetPos(const VECTOR& pos);

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) const override;

	/// @brief 外部データからパラメータを一括適用する
	/// @param info 適用する敵データ情報
	void ApplyData(const EnemyInfo& info) override;

private:

	// 視野・索敵関連の定数
	static constexpr float VIEW_RANGE = 600.0f;               // 最大視野（発見できる距離）
	static constexpr float LOST_RANGE = 30.0f;                // 視野の解除距離（見失う距離）
	static constexpr float VIEW_ANGLE = 60.0f;                // 視野角（索敵範囲の角度）

	// 物理・判定関連の定数
	static constexpr float COLLISION_HEIGHT_OFFSET = 100.0f; // 当たり判定の高さオフセット

	// 移動・目標関連の変数
	VECTOR targetPos_;                                        // 追従対象の座標

	/// @brief 形状の変化等に合わせてコライダのオフセット位置を更新する
	void UpdateColliderOffset(void);

};