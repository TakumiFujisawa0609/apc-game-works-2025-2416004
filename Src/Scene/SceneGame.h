#pragma once
#include "SceneBase.h"
#include "../Object/Manager/GroundManager.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/StageManager.h"

class Player;
class EnemyData;
class SkyDome;

/// @brief ゲーム本編のメインロジックを管理するシーンクラス
class SceneGame : public SceneBase
{
public:
    // 定数関連
    /// @brief ゲームの制限時間（秒）
    static constexpr float LIMIT_TIME = 120.0f;

    /// @brief コンストラクタ
    SceneGame(void);

    /// @brief デストラクタ
    ~SceneGame(void) = default;

    /// @brief リソースの読み込み開始
    void Load(void) override;

    /// @brief リソースの読み込み完了処理
    void EndLoad(void) override;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief ポーズ解除などの再開処理
    void OnResume(void) override;

private:
    // 操作キャラクター
    std::shared_ptr<Player> player_;            

    // ステージ・環境関連
    std::shared_ptr<GroundManager> groundManager_; // 地面・地形の管理
    std::shared_ptr<SkyDome> skyDome_;             // 空の描画・制御
    std::unique_ptr<StageManager> stageManager_;   // ステージ全体の進行管理

    // エネミー関連
    std::shared_ptr<EnemyData> enemyData_;      // 敵の基礎パラメータデータ
    std::unique_ptr<EnemyManager> enemyManager_; // 敵の生成・更新・削除管理

    // スタート時の文字表示フラグ
    bool isStartFont_;                         

    /// @brief エネミースポナーの配置と初期設定を行う
    void SetupEnemySpawners();

    /// @brief デバッグ情報の描画
    void DrawDebug(void);
};