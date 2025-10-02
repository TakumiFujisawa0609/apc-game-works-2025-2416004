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
#include "../DrawUI/Font.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Common/AnimationController.h"




SceneGame::SceneGame(void)
{
	grid_ = nullptr;
	isStartFont_ = true;

	//プレイヤー
	player_ = std::make_shared<Player>();

}

void SceneGame::Load()
{
	SceneBase::Load(); // isLoading_ = true

	// ここで必要なリソースを読み込む
	ResourceManager::GetInstance().InitGame();

	//プレイヤーのリソース読み込み
	player_->Load();
	// サウンドの読み込み

	EndLoad(); // isLoading_ = false
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

	//プレイヤーの初期化
	player_->Init();
	camera->SetFollow(&player_->GetTransform());
}

void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();

	//プレイヤーの更新
	player_->Update();
}

void SceneGame::Draw(void)
{
	//プレイヤーの描画
	player_->Draw();

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

	
}

void SceneGame::DrawDebug(void)
{
	grid_->Draw();
}

