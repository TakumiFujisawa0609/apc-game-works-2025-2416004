#include "EnemyManager.h"
#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Enemy/EnemySlime.h"


// コンストラクタ
EnemyManager::EnemyManager(void)
{
	//ターゲット座標を初期化
	targetPos_ = Utility::VECTOR_ZERO;
}

// モデル読み込み
void EnemyManager::Load(void)
{
	auto& res = ResourceManager::GetInstance();
	
	// スライムの生成上限を初期化
	maxSpawns_["SLIME"] = 10;
	
	//スライムの現在数を初期化
	curSpawns_["SLIME"] = 0;
}

// 初期化
void EnemyManager::Init(void)
{
	enemies_.clear();
	
	for (auto& [type, count] : curSpawns_)
	{
		count = 0;
	}
}

// ランダム生成
void EnemyManager::RandomSpawn(const std::string& type, const EnemyData& data)
{

	// 上限をチェック
	if (curSpawns_[type] >= maxSpawns_[type]) { return; }

	// エネミーのデータを取得
	const EnemyInfo* info = data.GetData(type);
	if (!info) { return; }

	// 座標をランダム生成
	VECTOR spawnPos = RandomSpawnPos();

	// 敵の生成
	auto enemy = CreateEnemy(*info);

	// 敵の初期化
	enemy->Init(spawnPos);

	// shared_ptr で管理
	auto enemy_sp = enemy;
	enemies_.push_back(enemy_sp);

	curSpawns_[type]++;
}

// ランダム座標の生成
VECTOR EnemyManager::RandomSpawnPos(void) const
{
	// ランダムな数値を取得
	float x = Utility::RandRangeF(-RANDOM_RANGE, RANDOM_RANGE);
	
	// ランダムな数値を取得
	float z = Utility::RandRangeF(-RANDOM_RANGE, RANDOM_RANGE);
	
	// Y座標は0に固定
	float y = 0.0f;
	
	return VGet(x, y, z);
}

// 更新処理
void EnemyManager::Update(void)
{
	//各敵の更新処理
	for (auto& enemy : enemies_)
	{
		if (auto slime = dynamic_cast<EnemySlime*>(enemy.get()))
		{
			slime->SetTargetPos(targetPos_);
		}
		
		//共通の更新
		enemy->Update();
	}

	// 死亡した場合敵を削除
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
		[this](const std::shared_ptr<EnemyBase>& enemy)
		{
			// 敵の体力が０だった場合
			if (enemy->GetHp() <= 0)
			{
				// 生成数を減らす
				curSpawns_[enemy->GetType()]--;
				
				//削除対象
				return true;
			}
			//生存
			return false;
		}),
		
		enemies_.end()

	);
}

// 描画処理
void EnemyManager::Draw(void)
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}

#ifdef _DEBUG

#endif
}

// 解放処理
void EnemyManager::Release(void)
{	
	enemies_.clear();
}

//追従対象の設定
void EnemyManager::SetTargetPos(const VECTOR& pos)
{
	targetPos_ = pos;
}

// 敵の座標を取得
VECTOR EnemyManager::GetEnemyPos(void) const
{
	// 敵がいない場合
	if (enemies_.empty())
	{
		return Utility::VECTOR_ZERO;
	}

	// 中心座標を計算
	VECTOR center = Utility::VECTOR_ZERO;
	for (const auto& enemy : enemies_)
	{
		center = VAdd(center, enemy->GetPos());
	}

	// 平均を取る
	center = VScale(center, 1.0f / static_cast<float>(enemies_.size()));

	return center;
}

//敵の生成
std::shared_ptr<EnemyBase> EnemyManager::CreateEnemy(const EnemyInfo& info)
{
	auto& res = ResourceManager::GetInstance();
	if (info.type == "SLIME")
	{
		auto slime = std::make_shared<EnemySlime>();
		
		//スライムモデルロード
		int modelId = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_SLIME);
		
		//モデルIDを渡す
		slime->Load(modelId);
		
		//初期データを適用
		slime->ApplyData(info);
		return slime;
	}
	
	// 対応するタイプがなければ nullptr を返す
	return std::shared_ptr<EnemyBase>(nullptr);
}

// 全ての敵の座標を取得
std::vector<VECTOR> EnemyManager::GetAllEnemyPositions(void) const
{
	std::vector<VECTOR> positions;
	positions.reserve(enemies_.size());

	for (const auto& enemy : enemies_)
	{
		positions.push_back(enemy->GetPos());
	}

	return positions;
}