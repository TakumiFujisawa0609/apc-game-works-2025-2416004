#include "SceneManager.h"

#include <DxLib.h>
#include<cassert>

#include "../../Scene/SceneBase.h" 
#include "../../Scene/SceneTitle.h"
#include "ResourceManager.h"
#include "../System/Collision.h"
#include "../Decoration/SoundManager.h"
#include "../System/CollisionManager.h"
#include "../System/TimeManager.h"
#include "Camera.h"
#include "../System/Loading.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}

SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

void SceneManager::DestroyInstance(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

SceneManager::SceneManager(void)
{
	sceneId_ = SCENE_ID::NONE;

	isGameEnd_ = false;

	isSceneChanging_ = false;

	deltaTime_ = 1.0f / 60.0f;

	preTime_ = std::chrono::system_clock::now();

	camera_ = std::make_shared<Camera>();
}

SceneManager::~SceneManager(void)
{
	Release();
}

void SceneManager::Init(void)
{
	//各マネジャーの生成
	//判定の生成
	Collision::CreateInstance();
	SoundManager::CreateInstance();
	TimeManager::CreateInstance();
	CollisionManager::CreateInstance();
	Loading::CreateInstance();


	//カメラ
	camera_->Init();

	//3D用の初期化処理
	Init3D();

	//初期シーンの設定
	ChangeScene(std::make_shared<SceneTitle>());
}

void SceneManager::Init3D(void)
{
	//背景色設定
	SetBackgroundColor(0, 0, 0);

	//Zバッファを有効にする
	SetUseZBuffer3D(true);

	//Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	//バックカリングを有効にする
	SetUseBackCulling(true);

	// ライトの設定
	SetUseLighting(true);

	// ライトの設定
	SetLightEnable(true);

	// 正面から斜め下に向かったライト
	//ChangeLightTypeDir({ 0.00f, -1.00f, 1.00f });

	// ライトの設定
	//ChangeLightTypeDir({ 0.3f, -0.7f, 0.8f });

	// フォグ設定
	SetFogEnable(true);
	SetFogColor(5, 5, 5);
	SetFogStartEnd(10000.0f, 20000.0f);
}
// ChangeScene は古いシーンを破棄して新しいシーンに切り替える
void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	// 古いシーンを解放
	for (auto& s : scenes_)
		s->Release();
	scenes_.clear();

	scenes_.push_back(scene);
	isSceneChanging_ = true;

	Loading::GetInstance()->StartAsyncLoad([scene]() {
		scene->Load();
		});
}

// PushScene は現在のシーンを保持したまま、新しいシーンを上に積む
void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	scenes_.push_back(scene);
	isSceneChanging_ = true;

	Loading::GetInstance()->StartAsyncLoad([scene]() {
		scene->Load();
		});
}

// PopScene は上に積んだシーンを取り除く
void SceneManager::PopScene()
{
	if (scenes_.size() > 1)
	{
		scenes_.back()->Release();  // 解放する場合は必要
		scenes_.pop_back();
	}
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	scenes_.clear();

	isSceneChanging_ = true;

	scenes_.push_back(scene);

	Loading::GetInstance()->StartAsyncLoad([scene]()
		{
			scene->Load();
		});
}


void SceneManager::Update(void)
{
	if (scenes_.empty()) return;

	TimeManager::GetInstance().Update();

	// デルタタイム
	auto nowTime = std::chrono::system_clock::now();
	deltaTime_ = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count() / 1000000000.0);
	preTime_ = nowTime;

	// scenes_.back() をコピーして安全に扱う
	std::shared_ptr<SceneBase> current = scenes_.back();

	// 非同期ロード
	if (isSceneChanging_)
	{
		Loading::GetInstance()->Update();

		if (!Loading::GetInstance()->IsLoading())
		{
			current->EndLoad();
			isSceneChanging_ = false;
		}
	}
	else
	{
		if (current) current->Update();
	}

	if (camera_) camera_->Update();

	// フラグチェック後に終了処理
	if (isGameEnd_)
	{
		scenes_.clear();
		Release();
	}
}

void SceneManager::Draw(void)
{
	if (scenes_.empty()) return;

	// 描画先グラフィック領域の指定
	SetDrawScreen(DX_SCREEN_BACK);

	// バックバッファのクリア
	ClearDrawScreen();

	// カメラの設定
	if (camera_) camera_->SetBeforeDraw();

	// シーン描画（コピーを使って安全）
	std::vector<std::shared_ptr<SceneBase>> scenesCopy(scenes_.begin(), scenes_.end());
	for (auto& scene : scenesCopy)
	{
		if (scene) scene->Draw();
	}

	if (camera_) camera_->Draw();
}

void SceneManager::Release(void)
{
	// 非同期ロード終了待ち
	if (Loading::GetInstance()->IsLoading()) 
	{
		while (Loading::GetInstance()->IsLoading())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	for (auto& scene : scenes_)
	{
		scene->Release();
	}

	scenes_.clear();

	camera_.reset();

	SoundManager::GetInstance().Destroy();
	CollisionManager::GetInstance().Destroy();
	TimeManager::GetInstance().Destroy();
	Loading::GetInstance()->DestroyInstance();
}

SceneManager::SCENE_ID SceneManager::GetSceneID(void) const
{
	return sceneId_;
}

void SceneManager::GameEnd(void)
{
	isGameEnd_ = true;
}

bool SceneManager::GetGameEnd(void) const
{
	return isGameEnd_;
}

float SceneManager::GetDeltaTime(void)const
{
	return deltaTime_;
}

std::shared_ptr<Camera> SceneManager::GetCamera(void) const
{
	return camera_;
}

void SceneManager::ResetDeltaTime(void)
{
	deltaTime_ = 1.0f / 60.0f;
	preTime_ = std::chrono::system_clock::now();
}
