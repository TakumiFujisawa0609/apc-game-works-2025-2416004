#pragma once
#include "../Enemy/EnemyData.h"

class EnemySpawner
{
public:
    // コンストラクタ
    EnemySpawner(const VECTOR& position, float spawnRange, const std::string& enemyType);

    // デストラクタ
    ~EnemySpawner() = default;

    // 更新処理
    void Update(float deltaTime);

    // 描画処理（デバッグ用）
    void Draw(void) const;

    // スポーン座標をランダムに生成
    VECTOR GetRandomSpawnPos(void) const;


    const VECTOR& GetPosition(void) const;
    
    float GetSpawnRange(void) const;
    
    const std::string& GetEnemyType(void) const;
    
    float GetSpawnInterval(void) const;
    
    int GetMaxEnemies(void) const;
    
    bool IsActive(void) const;
    
    float GetActivationRange(void) const;

    int GetEnemyLevel(void) const;
    
    bool IsRequirePlayerInRange(void) const;

    void SetSpawnInterval(float interval);
    
    
    void SetMaxEnemies(int max);
    
    void SetActive(bool active);
    
    void SetCurrentEnemyCount(int count);

    void SetActivationRange(float range);

    void SetRequirePlayerInRange(bool require);

    void SetEnemyLevel(int level);

    // スポーン可能かチェック
    bool CanSpawn(const VECTOR& playerPos) const;

    // スポーンタイマーをリセット
    void ResetSpawnTimer(void);

    // プレイヤーが範囲内にいるかチェック
    bool IsPlayerInRange(const VECTOR& playerPos) const;

private:
    // スポナーの座標
    VECTOR position_;      

    // スポーン範囲（半径）
    float spawnRange_;      

    // スポーンするエネミーのタイプ
    std::string enemyType_;  

    // スポーン間隔（秒）
    float spawnInterval_;

    // スポーンタイマー
    float spawnTimer_;          

    // このスポナーから生成できる最大数
    int maxEnemies_;            

    // 現在このスポナーから生成されているエネミー数
    int currentEnemyCount_;    

    // スポナーが有効かどうか
    bool isActive_;             

    // プレイヤーが入る必要がある範囲（0の場合は無制限）
    float activationRange_;     

    // プレイヤーが範囲内にいる必要があるか
    bool requirePlayerInRange_; 

    // スポーンするエネミーのレベル
    int enemyLevel_;
};