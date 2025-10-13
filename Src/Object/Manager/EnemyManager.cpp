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

    //ターゲット座標を初期化
    targetPos_ = Utility::VECTOR_ZERO;
	
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
        //スライムの追従対象を更新
        if (enemy->GetType() == ENEMY_TYPE::SLIME)
        {
            auto slime = static_cast<EnemySlime*>(enemy.get());

            slime->SetTargetPos(targetPos_);
        }

        enemy->Update();
    }

    // 死亡した場合敵を削除
    enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
        [this](const std::unique_ptr<EnemyBase>& enemy)
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
}

// 解放処理
void EnemyManager::Release(void)
{
    for (auto& [type, handle] : modelIds_)
    {
        if (handle >= 0)
        {
            MV1DeleteModel(handle);
        }
    }

    modelIds_.clear();

    enemies_.clear();

    curSpawns_.clear();
}

void EnemyManager::SetTargetPos(const VECTOR& pos)
{
    targetPos_ = pos;
}
