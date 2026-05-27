#include "SceneGame.h"
#include "../Application.h"
#include "../Manager/Generic/Camera.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Player/Player.h"
#include "../Object/Manager/GroundManager.h"
#include "../Object/Manager/EnemyManager.h"
#include "../Object/Manager/StageManager.h"
#include "../Object/Enemy/EnemyData.h"
#include "../Object/Stage/SkyDome.h"
#include "../Scene/SceneScore.h"
#include "../Scene/SceneTutorial.h"
#include "../Manager/System/Loading.h"
#include "../DrawUI/Font.h"

SceneGame::SceneGame(void)
{
	isStartFont_ = true;

	//プレイヤー
	player_ = std::make_shared<Player>();

	//ステージ
	groundManager_ = std::make_shared<GroundManager>();

	//エネミーデータ
	enemyData_ = std::make_shared<EnemyData>();

	// スカイドーム
	skyDome_ = std::make_shared<SkyDome>();

	//エネミーマネージャー
	enemyManager_ = std::make_unique<EnemyManager>();

	// ステージマネージャー
	stageManager_ = std::make_unique<StageManager>();

}

void SceneGame::Load(void)
{
	Loading::GetInstance()->SetProgress(10.0f);

	SceneBase::Load();

	// InitGameの前後で進捗を更新してみる
	ResourceManager::GetInstance().InitGame();
	Loading::GetInstance()->SetProgress(15.0f);

	// プレイヤーのパラメータ読み込み
	player_->LoadParamCSV(Application::PATH_CSV + "Player_param.csv");

	//プレイヤーの読み込み
	player_->Load();

	Loading::GetInstance()->SetProgress(25.0f);

	//ステージの読み込み
	groundManager_->Load();

	Loading::GetInstance()->SetProgress(45.0f);

	//エネミーの読み込み
	enemyManager_->Load();

	//エネミーデータ読み込み
	enemyData_->LoadCSV(Application::PATH_CSV + "EnemyData.csv");

	// ステージマネージャーの読み込み
	stageManager_->Load();

	// スカイドームの読み込み
	skyDome_->Load();


	Loading::GetInstance()->SetProgress(60.0f);

	


	Loading::GetInstance()->SetProgress(80.0f);

	//時間カウントリセット
	TimeManager::GetInstance().Reset();


	Loading::GetInstance()->SetProgress(100.0f);
}

void SceneGame::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGame::Initialize(void)
{

	if (Loading::GetInstance()->IsLoading()) return;

	// ゲームBGM登録
	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_GAME,
		ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_GAME).handleId_);

	// 戦闘BGMの登録
	SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_FAITE, 
		ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_FAITE).handleId_);

	// カメラ設定
	auto camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::TPS_MOUSE);

	isStartFont_ = true;


	// サウンドの読み込み
	groundManager_->Initialize();


	//エネミーマネージャー初期化
	enemyManager_->Initialize();

	//プレイヤーの初期化
	player_->Initialize();
	camera->SetFollowTarget(&player_->GetTransform());

	// ステージマネージャーの初期化
	stageManager_->Initialize(enemyManager_.get());

	// スカイドームの初期化
	skyDome_->Initialize();
	skyDome_->SetFollowTarget(&player_->GetPos());

	// エネミースポナーの配置設定
	SetupEnemySpawners();

	// サウンド再生
	SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_GAME);

	// チュートリアルシーンを挟む
	SceneManager::GetInstance().PushScene(std::make_shared<SceneTutorial>());
}

void SceneGame::SetupEnemySpawners(void)
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

void SceneGame::Update(void)
{

	if (Loading::GetInstance()->IsLoading()) return;

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

	// プレイヤーの位置を設定
	groundManager_->SetPlayerPos(player_->GetPos());

	// 全ての敵の位置を設定
	groundManager_->SetEnemyPos(enemyManager_->GetAllEnemyPositions());

	// ステージの更新
	groundManager_->Update();

	// StageManagerの更新
	stageManager_->Update(player_->GetPos(), SceneManager::GetInstance().GetDeltaTime());

	// エネミーマネージャーの更新（プレイヤー座標を渡す）
	enemyManager_->Update(SceneManager::GetInstance().GetDeltaTime(), player_->GetPos());

	// スカイドームの更新
	skyDome_->Update();

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

	if (times >= LIMIT_TIME || player_->GetHP() <= 0 )
	{

		auto newScene = std::make_shared<SceneScore>();

		SceneManager::GetInstance().ChangeScene(newScene);

		return;
	}
}

void SceneGame::Draw(void)
{
	auto camera = SceneManager::GetInstance().GetCamera();

	// 3D描画 
	groundManager_->Draw(player_->GetPos(), camera->GetPos(), camera->GetFrontVector());
	skyDome_->Draw();
	stageManager_->Draw(player_->GetPos(), camera->GetFrontVector());
	enemyManager_->Draw(camera->GetPos(), camera->GetFrontVector());
	player_->Draw();

	// UI描画 (2D) 
	auto& font = Font::GetInstance();
	auto& time = TimeManager::GetInstance();

	// 残り時間の計算
	float remainingTime = LIMIT_TIME - time.GetGameTime();
	if (remainingTime < 0.0f) remainingTime = 0.0f;

	// 表示用文字列の作成
	char timeStr[64];
	sprintf_s(timeStr, "残り時間: %.1f", remainingTime); 

	// 座標の計算 (右上)
	int textWidth = font.GetDefaultTextWidth(timeStr);

	// 右端から 40ピクセル 離れた位置に配置
	int drawX = Application::SCREEN_SIZE_X - textWidth - 240;
	// 上端から 30ピクセル 離れた位置
	int drawY = 30;

	const int fontSize = 40;

	// 4. 描画
	unsigned int timeColor = (remainingTime <= 5.0f) ? 
		GetColor(255, 50, 50) : GetColor(255, 255, 255);

	font.DrawDefaultText(drawX, drawY, timeStr, timeColor, fontSize, 
		Font::FONT_TYPE_ANTIALIASING_EDGE);

#ifdef _DEBUG
	// デバッグ情報など
#endif
}

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

void SceneGame::OnResume(void)
{
	auto camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::TPS_MOUSE);
	camera->SetFollowTarget(&player_->GetTransform());

	SetMouseDispFlag(FALSE);
	SetMousePoint(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2
	);
}


// 描画(デバック)
void SceneGame::DrawDebug(void)
{
}