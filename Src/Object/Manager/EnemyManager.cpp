#include "EnemyManager.h"

#include "../../Utility/Utility.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../Enemy/EnemySlime.h"

//コンストラクタ
EnemyManager::EnemyManager(void)
{
    // 各敵の生成上限を設定
    maxSpawns_[ENEMY_TYPE::SLIME] = 10;

    // 現在数を初期化
    curSpawns_[ENEMY_TYPE::SLIME] = 0;
	
}

// モデル読み込み
void EnemyManager::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    // スライムのモデルをロード
    modelIds_[ENEMY_TYPE::SLIME] = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);

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
void EnemyManager::RandomSpawn(ENEMY_TYPE type)
{
    // 上限をチェック
    if (curSpawns_[type] >= maxSpawns_[type]) { return; }

    //座標をランダム生成
    VECTOR spawnPos = RandomSpawnPos();

    //敵の生成
    std::unique_ptr<EnemyBase> enemy = nullptr;

    switch (type)
    {
    case ENEMY_TYPE::SLIME:

        //スライムの生成
        auto slime = std::make_unique<EnemySlime>();

        slime->Load(modelIds_[ENEMY_TYPE::SLIME]);
        
        slime->Init(spawnPos);

        enemy = std::move(slime);

        break;

    }

    if (enemy)
    {
        enemies_.push_back(std::move(enemy));
        curSpawns_[type]++;
    }

}

//ランダム座標の生成
VECTOR EnemyManager::RandomSpawnPos(void) const
{
    float x = Utility::RandRangeF(-RANDOM_RANGE, RANDOM_RANGE);
    return VGet(x, x, x);
}