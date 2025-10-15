#pragma once

#include <DxLib.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyData.h"

class EnemySlime;

//エネミーマネージャークラス
class EnemyManager
{
public:

	//ランダム生成範囲
	static constexpr float RANDOM_RANGE = 500.0f;

	//コンストラクタ
	EnemyManager(void);

	// デストラクタ
	~EnemyManager(void) = default;

	//読み込み
	void Load(void);

	// 初期化
	void Init(void);

	// 生成
	void RandomSpawn(const std::string& type, const EnemyData& data);

	// 更新処理
	void Update(void);

	// 描画処理
	void Draw(void);

	// 解放処理
	void Release(void);

	//ターゲット座標を設定
	void SetTargetPos(const VECTOR& pos);

	//敵の生成
	std::unique_ptr<EnemyBase> CreateEnemy(const EnemyInfo& info);

private:
	
	// 敵ごとの最大生成数
	std::unordered_map<std::string, int> maxSpawns_;

	// 現在の生成数
	std::unordered_map<std::string, int> curSpawns_;

	// モデルの共有管理
	std::unordered_map<std::string, int> modelIds_;

	// 敵のリスト
	std::vector<std::unique_ptr<EnemyBase>> enemies_;

	// ターゲット座標
	VECTOR targetPos_;

	// ランダム座標を返す
	VECTOR RandomSpawnPos(void) const;
	

	
};

