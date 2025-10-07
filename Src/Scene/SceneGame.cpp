#include "SceneGame.h"

#include <DxLib.h>

#include "../Common/Easing.h"
#include "../Manager/Generic/Camera.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/Resource.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/Collision.h"

#include "../Object/Grid.h"
#include "../Object/Player.h"
#include "../Object/EnemyDummy.h"
#include "../DrawUI/Font.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Common/AnimationController.h"




SceneGame::SceneGame(void)
{
	grid_ = nullptr;
	isStartFont_ = true;

	//プレイヤー
	player_ = std::make_shared<Player>();

	dummy_ = std::make_shared<EnemyDummy>();

	lockonTimer_ = 0.0f;

	deltaTime_ = 0.0f;

	
}

void SceneGame::Load()
{
	SceneBase::Load(); // isLoading_ = true

	// ここで必要なリソースを読み込む
	ResourceManager::GetInstance().InitGame();

	//プレイヤーのリソース読み込み
	player_->Load();
	// サウンドの読み込み

	//仮のエネミーの読み込みs
	dummy_->Load();

	//ロード完了
	EndLoad();
}

void SceneGame::EndLoad()
{
	SceneBase::EndLoad();
}


void SceneGame::Init()
{
	// カメラ設定
	auto camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::TPS_MOUSE);

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	// サウンド音量調整

	isStartFont_ = true;

	deltaTime_ = SceneManager::GetInstance().GetDeltaTime();

	//プレイヤーの初期化
	player_->Init();
	camera->SetFollow(&player_->GetTransform());

	// 仮ターゲット初期化
	dummy_->Init();
	camera->SetLockonTarget(&dummy_->GetTransform());
}

void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto camera = SceneManager::GetInstance().GetCamera();

	//プレイヤーの更新
	player_->Update();

	dummy_->Update(); // 仮ターゲット更新

	//// スペース押下でロックオン開始
	//if (input.IsTrgDown(KEY_INPUT_RETURN))
	//{
 //  		camera->SetLockon(true);
	//	lockonTimer_ = 0.0f; // タイマーリセット
	//}

	//// ロックオン中のタイマー更新
	//if (camera->IsLockon() == true)
	//{
	//	lockonTimer_ += deltaTime_;

	//	// スペースを押していない状態で2秒経過したらTPS_MOUSEに戻す
 //  		if (!input.IsNew(KEY_INPUT_RETURN) && lockonTimer_ >= 2.0f)
	//	{
	//		camera->SetLockon(false);
	//	}
	//}

}

void SceneGame::Draw(void)
{
	//プレイヤーの描画
	player_->Draw();

	dummy_->Draw();   // 仮ターゲット描画

#ifdef _DEBUG
	//デバック表示
	DrawDebug();
#endif // _DEBUG
}

void SceneGame::Release(void)
{

	grid_->Release();
	delete grid_;
	grid_ = nullptr;

	//プレイヤーの解放
	player_->Release();
	player_.reset();

	if (dummy_)
	{
		dummy_->Release();
		dummy_.reset();
	}

	
}

void SceneGame::DrawDebug(void)
{
	grid_->Draw();
}

