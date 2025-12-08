#include "SceneGame.h"
#include "../Manager/Generic/Camera.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Player/Player.h"
#include "../Object/Manager/GroundManager.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/StageManager.h"
#include "../Object/Enemy/EnemyData.h"
#include "../Scene/SceneScore.h"
#include "../Manager/System/Loading.h"
#include "../Collider/ColliderBase.h"

// コンストラクタ
SceneGame::SceneGame(void)
{
	isStartFont_ = true;

	//プレイヤー
	player_ = std::make_shared<Player>();

	//ステージ
	groundManager_ = std::make_shared<GroundManager>();

	//エネミーデータ
	enemyData_ = std::make_shared<EnemyData>();

	//エネミーマネージャー
	enemyManager_ = std::make_unique<EnemyManager>();

	// ステージマネージャー
	stageManager_ = std::make_unique<StageManager>();
}

// 読み込み
void SceneGame::Load()
{
	SceneBase::Load();

	// ここで必要なリソースを読み込む
	ResourceManager::GetInstance().InitGame();

	// プレイヤーのパラメータ読み込み
	player_->LoadParamCSV(Application::PATH_CSV + "Player_param.csv");

	//プレイヤーの読み込み
	player_->Load();
	Loading::GetInstance()->SetProgress(25.0f);

	//ステージの読み込み
	groundManager_->Load();
	Loading::GetInstance()->SetProgress(40.0f);

	//エネミーの読み込み
	enemyManager_->Load();

	//エネミーデータ読み込み
	enemyData_->LoadCSV(Application::PATH_CSV + "EnemyData.csv");

	// ステージマネージャーの読み込み
	stageManager_->Load();

	Loading::GetInstance()->SetProgress(60.0f);

	// サウンドの読み込み

	Loading::GetInstance()->SetProgress(80.0f);

	//時間カウントリセット
	TimeManager::GetInstance().Reset();

	Loading::GetInstance()->SetProgress(100.0f);
}

// 読み込み終了
void SceneGame::EndLoad()
{
	SceneBase::EndLoad();
}

// 初期化
void SceneGame::Init()
{
	// カメラ設定
	auto camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::TPS_MOUSE);

	// サウンド音量調整

	isStartFont_ = true;

	//ステージの初期化
	groundManager_->Init();

	//エネミーマネージャー初期化
	enemyManager_->Init();

	//プレイヤーの初期化
	player_->Init();
	camera->SetFollow(&player_->GetTransform());

	// ステージマネージャーの初期化
	stageManager_->Init(enemyManager_.get());

	// エネミースポナーの配置設定
	SetupEnemySpawners();
}

// エネミースポナーの配置設定
void SceneGame::SetupEnemySpawners()
{
	// スポナーの基本設定
	stageManager_->SetSpawnerSettings(
		500.0f,         // エネミーのスポーン範囲
		"SLIME",        // エネミータイプ
		800.0f,         // プレイヤー検知範囲（この範囲に入るとスポーン開始）
		0.1f,           // スポーン間隔（秒）※0.1秒で高速スポーン
		5               // 1つのスポナーから生成される最大エネミー数（5体同時）
	);

	// 原点座標を設定（プレイヤーの初期位置を基準にする）
	stageManager_->SetOriginPos(player_->GetOriginPos());

	// 動的レベルを有効化
	stageManager_->EnableDynamicLevel(true, 10000.0f);

#ifdef _DEBUG
	printfDx("=== Enemy Spawner Settings ===\n");
	printfDx("Spawn Range: 500.0f\n");
	printfDx("Activation Range: 800.0f\n");
	printfDx("Spawn Interval: 0.1s\n");
	printfDx("Max Enemies per Spawner: 5\n");
	printfDx("Dynamic Level: Enabled (1000 units per level)\n");
	printfDx("Max Enemy Level: 10\n");
	printfDx("Origin Position: (%.1f, %.1f, %.1f)\n",
		player_->GetOriginPos().x,
		player_->GetOriginPos().y,
		player_->GetOriginPos().z);
#endif
}

// 更新処理
void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = TimeManager::GetInstance();
	auto camera = SceneManager::GetInstance().GetCamera();
	auto loader = Loading::GetInstance();

	// プレイヤーの更新
	player_->Update();

	// プレイヤーの移動距離をSceneManagerに保存
	float distance = player_->GetDistanceFromOriginXZ();
	SceneManager::GetInstance().SetPlayerDistance(distance);

	// ステージの更新
	groundManager_->Update();

	// StageManagerの更新
	stageManager_->Update(player_->GetPos(), SceneManager::GetInstance().GetDeltaTime());

	// エネミーマネージャーの更新（プレイヤー座標を渡す）
	enemyManager_->Update(SceneManager::GetInstance().GetDeltaTime(), player_->GetPos());

	// 撃破した敵からの経験値を取得してプレイヤーに付与
	auto expRewards = enemyManager_->GetAndClearExpRewards();
	for (int exp : expRewards)
	{
		player_->AddExperinece(exp);
	}

	// エネミーの死亡数をSceneManagerに保存
	int deathCount = enemyManager_->GetDeathCount();
	SceneManager::GetInstance().SetEnemyDeathCount(deathCount);

	// プレイヤーを追従対象
	enemyManager_->SetTargetPos(player_->GetPos());

	// 時間を取得
	float times = time.GetGameTime();

	if (times >= LIMIT_TIME)
	{
		sound.Play(SoundManager::SOUND::SE_PUSH);

		auto newScene = std::make_shared<SceneScore>();

		SceneManager::GetInstance().ChangeScene(newScene);

		return;
	}

	// プレイヤーの位置を設定
	groundManager_->SetPlayerPos(player_->GetPos());

	// 全ての敵の位置を設定
	groundManager_->SetEnemyPos(enemyManager_->GetAllEnemyPositions());
}

// 描画処理
void SceneGame::Draw(void)
{
	auto camera = SceneManager::GetInstance().GetCamera();

	//ステージの描画
	groundManager_->Draw(player_->GetPos(), camera->GetPos(), camera->GetFrontVec());

	//プレイヤーの描画
	player_->Draw();

	// ステージマネージャーの描画（カリング対応・デバッグ表示）
	stageManager_->Draw(camera->GetPos(), camera->GetFrontVec());

	// 敵の描画（カリング対応）
	enemyManager_->Draw(camera->GetPos(), camera->GetFrontVec());

#ifdef _DEBUG
	//デバック表示
	DrawDebug();
#endif // _DEBUG
}

// 解放処理
void SceneGame::Release(void)
{
	// ステージマネージャーの解放
	stageManager_->Release();
	stageManager_.reset();

	//プレイヤーの解放
	player_->Release();
	player_.reset();

	//ステージの解放
	groundManager_->Release();
	groundManager_.reset();

	//敵の解放
	enemyManager_->Release();
	enemyManager_.reset();

	//エネミーデータの解放
	enemyData_.reset();
}

// 描画(デバック)
void SceneGame::DrawDebug(void)
{
#ifdef _DEBUG
	// ★デバッグ情報表示
	int y = 300;

	// プレイヤー情報
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"=== Player ===");
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"Level: %d", player_->GetLevel());
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"EXP: %d / %d", player_->GetExperinece(), player_->GetRequireExp());
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"ATK: %d  DEF: %d  HP: %d/%d",
		player_->GetParam().attack,
		player_->GetParam().defensse,
		player_->GetParam().hp,
		player_->GetParam().maxHp);
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"Distance from Origin: %.1f", player_->GetDistanceFromOriginXZ());

	y += 40;

	// エネミー情報
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"=== Enemies ===");
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"Active Enemies: %d", enemyManager_->GetEnemyCount());
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"Total Defeated: %d", enemyManager_->GetDeathCount());
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"Active Spawners: %d", stageManager_->GetActiveSpawnerCount());
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 255),
		"Total Spawners: %d", stageManager_->GetSpawnerCount());

	// レベルシステム情報
	y += 40;
	DrawFormatString(10, y, GetColor(255, 255, 0),
		"=== Level System ===");
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 0),
		"Enemy Level = Distance / 1000");
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 0),
		"Current Zone Level: %d",
		static_cast<int>(player_->GetDistanceFromOriginXZ() / 1000.0f) + 1);
	y += 20;
	DrawFormatString(10, y, GetColor(255, 255, 0),
		"Max Enemy Level: 10");
#endif
}