#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "../Enemy/EnemyData.h"

class EnemyBase;

class EnemyManager
{
public:
	// ランダム生成範囲
	static constexpr float RANDOM_RANGE = 500.0f;

	// コンストラクタ
	EnemyManager(void);
	
	// デストラクタ
	~EnemyManager(void) = default;

	// モデル読み込み
	void Load(void);
	
	// 初期化
	void Init(void);
	
	// ランダム生成
	void RandomSpawn(const std::string& type, const EnemyData& data);
	
	// ランダム座標の生成
	VECTOR RandomSpawnPos(void) const;
	
	// 更新処理
	void Update(void);
	
	// 描画処理
	void Draw(void);
	
	// 解放処理
	void Release(void);

	// 追従対象の設定
	void SetTargetPos(const VECTOR& pos);

	// 衝突判定用の再登録
	void RegisterCollisions(void);

private:

	// 敵リスト（shared_ptr に変更）
	std::vector<std::shared_ptr<EnemyBase>> enemies_;
	
	// ターゲット座標
	VECTOR targetPos_;
	
	// モデルID
	std::unordered_map<std::string, int> modelIds_;
	
	// 生成上限
	std::unordered_map<std::string, int> maxSpawns_;
	
	// 現在の生成数
	std::unordered_map<std::string, int> curSpawns_;

	// 敵の生成
	std::shared_ptr<EnemyBase> CreateEnemy(const EnemyInfo& info);
};