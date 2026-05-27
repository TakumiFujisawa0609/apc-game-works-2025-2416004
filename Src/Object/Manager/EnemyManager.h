#pragma once

#include <vector>
#include <memory>
#include <map>
#include "../Enemy/EnemyBase.h"
#include "../Stage/EnemySpawner.h"
#include "../Enemy/EnemyData.h"

/// @brief エネミー全体を管理するマネージャークラス
/// @details 全エネミーの更新・描画、およびスポナーによる生成管理を行う
class EnemyManager
{
public:
    /// @brief コンストラクタ
    EnemyManager(void);

    /// @brief デストラクタ
    ~EnemyManager(void);

    /// @brief 必要なリソース（モデル等）の読み込み
    void Load(void);

    /// @brief 初期化処理
    void Initialize(void);

    /// @brief 更新処理
    /// @param deltaTime デルタタイム
    /// @param playerPos プレイヤーの現在座標
    void Update(float deltaTime, const VECTOR& playerPos);

    /// @brief 標準の描画処理
    void Draw(void);

    /// @brief カリング（描画対象の選別）を考慮した描画処理
    /// @param cameraPos カメラの座標
    /// @param cameraDir カメラの方向ベクトル
    void Draw(const VECTOR& cameraPos, const VECTOR& cameraDir);

    /// @brief 解放処理
    void Release(void);

    /// @brief スポナーを新規追加する
    /// @param position 配置座標
    /// @param spawnRange 出現範囲
    /// @param enemyType 出現させる敵の種類
    /// @param level 出現させる敵のレベル
    void AddSpawner(const VECTOR& position, float spawnRange, const std::string& enemyType, int level);

    /// @brief 特定のスポナーを削除する
    /// @param index スポナーリストのインデックス
    void RemoveSpawner(int index);

    /// @brief 全てのスポナーをリストから消去する
    void ClearSpawners(void);

    /// @brief 指定したインデックスのスポナーを取得する
    /// @param index インデックス
    /// @return スポナーへのポインタ
    EnemySpawner* GetSpawner(int index);

    /// @brief 全エネミーの追従対象となる座標を設定する
    /// @param pos ターゲットの座標
    void SetTargetPos(const VECTOR& pos);

    /// @brief 代表的なエネミーの中心座標を取得する
    /// @return エネミーの座標（存在しない場合はゼロベクトル）
    VECTOR GetEnemyPos(void) const;

    /// @brief 生存している全エネミーの座標リストを取得する
    /// @return 座標のベクトル
    std::vector<VECTOR> GetAllEnemyPositions(void) const;

    /// @brief 現在生存しているエネミーの総数を取得する
    /// @return エネミー数
    int GetEnemyCount(void) const;

    /// @brief 登録されているスポナーの総数を取得する
    /// @return スポナー数
    int GetSpawnerCount(void) const;

    /// @brief 累計の死亡数を取得する
    /// @return 死亡数
    int GetDeathCount(void) const;

    /// @brief 死亡数カウンターをゼロにリセットする
    void ResetDeathCount(void);

    /// @brief 全エネミーが持つ全コライダーのリストを取得する
    /// @return コライダーへのポインタのベクトル
    std::vector<const ColliderBase*> GetAllEnemyColliders(void) const;

    /// @brief 発生した経験値報酬を取得し、マネージャー内のリストを空にする
    /// @return 獲得した経験値のリスト
    std::vector<int> GetAndClearExpRewards(void);

private:
    // リスト・管理関連
    std::vector<std::shared_ptr<EnemyBase>> enemies_;      // 生存しているエネミーのリスト
    std::vector<std::unique_ptr<EnemySpawner>> spawners_;  // 配置されているスポナーのリスト
    std::map<int, int> enemyToSpawnerMap_;                 // どの敵がどのスポナーから出たかの紐付け（enemyIndex -> spawnerIndex）

    // 報酬・リザルト関連
    std::vector<int> pendingExpRewards_;                    // まだ受け取られていない経験値報酬の蓄積リスト
    int deathCount_;                                       // エネミーの累計死亡数

    // 状態・ターゲット関連
    VECTOR targetPos_;                                     // エネミーが追従すべき目標座標

    /// @brief 敵データ情報を元に、適切な派生クラスのエネミーを生成する
    /// @param info 敵の種類や初期パラメータ
    /// @return 生成されたエネミーの共有ポインタ
    std::shared_ptr<EnemyBase> CreateEnemy(const EnemyInfo& info);

    /// @brief 各スポナーの出現条件をチェックし、必要に応じてスポーンを実行する
    /// @param data 敵のマスターデータ
    /// @param playerPos 距離判定用のプレイヤー座標
    void ProcessSpawners(const EnemyData& data, const VECTOR& playerPos);

    /// @brief 特定のスポナーから現在何体のエネミーが生存しているかカウントする
    /// @param spawnerIndex カウント対象のスポナーインデックス
    /// @return そのスポナー由来の生存数
    int CountEnemiesFromSpawner(int spawnerIndex) const;
};