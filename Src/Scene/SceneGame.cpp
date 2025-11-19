#include "SceneGame.h"
#include "../Manager/Generic/Camera.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Player/Player.h"
#include "../Object/Manager/GroundManager.h"
#include "../Object/Enemy/EnemyData.h"
#include "../Scene/SceneScore.h"
#include "../Manager/System/Loading.h"



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

	
	// プレイヤーに自分自身の shared_ptr を設定
	player_->SetSelfPtr(player_);

	//プレイヤーの初期化
	player_->Init();
	camera->SetFollow(&player_->GetTransform());

	//敵のランダム生成(初期スポーン)
	for (int i = 0; i < 250; ++i)
	{
		enemyManager_->RandomSpawn("SLIME", *enemyData_);
	}
}

// 更新処理
void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = TimeManager::GetInstance();
	auto camera = SceneManager::GetInstance().GetCamera();
	auto loader = Loading::GetInstance();

	//プレイヤーの更新
	player_->Update();

	//ステージの更新
	groundManager_->Update();

	//エネミーマネージャーの更新
	enemyManager_->Update();

	//プレイヤーを追従対象
	enemyManager_->SetTargetPos(player_->GetPos());

	//時間を取得
	float times = time.GetGameTime();

	if (times >= LIMIT_TIME)
	{
		sound.Play(SoundManager::SOUND::SE_PUSH);

		auto newScene = std::make_shared<SceneScore>();

		SceneManager::GetInstance().ChangeScene(newScene);

		return;
	}

	// プレイヤーの位置を取得
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

	//敵の描画
	enemyManager_->Draw();

#ifdef _DEBUG
	//デバック表示
	DrawDebug();
#endif // _DEBUG
}

// 解放処理
void SceneGame::Release(void)
{

	//プレイヤーの解放
	player_->Release();
	player_.reset();

	//ステージの解放
	groundManager_->Release();
	groundManager_.reset();

	//敵の解放
	enemyManager_->Release();
	enemyManager_.reset();

	
}

// 描画(デバック)
void SceneGame::DrawDebug(void)
{
}

