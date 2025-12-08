#pragma once
#include "SceneBase.h"
#include "../Object/Manager/GroundManager.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/StageManager.h"

class Player;
class EnemyData;

class SceneGame : public SceneBase
{
public:
	// 制限時間（秒）
	static constexpr float LIMIT_TIME = 60.0f; // 5分

	// コンストラクタ
	SceneGame(void);

	// デストラクタ
	~SceneGame(void) = default;

	// 読み込み
	void Load(void) override;

	// 読み込み終了
	void EndLoad(void) override;

	// 初期化
	void Init(void) override;

	// 更新処理
	void Update(void) override;

	// 描画処理
	void Draw(void) override;

	// 解放処理
	void Release(void) override;

private:
	// プレイヤー
	std::shared_ptr<Player> player_;

	// ステージ
	std::shared_ptr<GroundManager> groundManager_;

	// エネミーデータ
	std::shared_ptr<EnemyData> enemyData_;

	// エネミーマネージャー
	std::unique_ptr<EnemyManager> enemyManager_;

	// ステージマネージャー
	std::unique_ptr<StageManager> stageManager_;

	// スタート時のフォント表示フラグ
	bool isStartFont_;

	// エネミースポナーの配置設定
	void SetupEnemySpawners();

	// 描画(デバック)
	void DrawDebug(void);
};