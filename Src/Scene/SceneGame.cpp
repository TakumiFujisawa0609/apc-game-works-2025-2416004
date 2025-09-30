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
#include "../DrawUI/Font.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Common/AnimationController.h"




SceneGame::SceneGame(void)
{
	grid_ = nullptr;
	isStartFont_ = true;
}

void SceneGame::Load()
{
	SceneBase::Load(); // isLoading_ = true

	// ここで必要なリソースを読み込む

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
	camera->ChangeMode(Camera::MODE::FIXED_POINT);

	// グリッド初期化
	grid_ = new Grid();
	grid_->Init();

	// サウンド音量調整

	isStartFont_ = true;
}

void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
}

void SceneGame::Draw(void)
{
	

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

	
}

void SceneGame::DrawDebug(void)
{
	grid_->Draw();
}

