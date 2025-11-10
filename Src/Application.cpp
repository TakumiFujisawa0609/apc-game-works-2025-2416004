#include"Application.h"

#include "Manager/Decoration/EffectManager.h"
#include "Manager/Generic/ResourceManager.h"
#include "Manager/Generic/InputManager.h"
#include "Manager/Generic/SceneManager.h"
#include "DrawUI/Font.h"
#include "Fps/FpsController.h"
#include "DrawUI/SceneUI/PauseMenu.h"
#include "Scene/SceneTitle.h"


Application* Application::instance_ = nullptr;

//ファイル指定パス
const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_ANIM = "Data/Anim/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_TEXT = "Data/Text/";
const std::string Application::PATH_FONT = "Data/Font/";
const std::string Application::PATH_BGM = "Data/Sound/BGM/";
const std::string Application::PATH_SE = "Data/Sound/SE/";
const std::string Application::PATH_MOVIE = "Data/Movie/";
const std::string Application::PATH_MAP_DATA = "Data/MapData/MapData.csv";
const std::string Application::PATH_CSV = "Data/CSV/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
		instance_->Init();
	}
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{
	//アプリケーションの初期設定
	SetWindowText("Fly");
	
	if (debugSc_)
	{
		//ウィンドウのサイズ
		SetGraphMode(DEFA_SCREEN_SIZE_X, DEFA_SCREEN_SIZE_Y, 32);

		ChangeWindowMode(true);
	}
	else
	{
		//ウィンドウのサイズ
		SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);

		ChangeWindowMode(false);
	}

	

	

	//非アクティブ状態でも動作する
	SetAlwaysRunFlag(TRUE);

	//FPS制御クラス
	fps_ = std::make_unique<FpsController>(DEFAULT_FPS);

	//DXLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		//エラー処理
		isInitFail_ = true;
		return;
	}
	SetMouseDispFlag(FALSE);

	//エフェクシアの初期化
	InitEffekseer();

	// キー制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	//リソース管理初期化
	ResourceManager::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();

	//フォントの初期化
	Font::CreateInstance();

	EffectManager::CreateInstance();

	pauseMenu_ = new PauseMenu();
	pauseMenu_->Init();
	
	activeUI_ = ActiveUI::NONE;

}

void Application::Run(void)
{
	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	

	MSG msg;

	while (ProcessMessage() == 0)
	{
		if (sceneManager.GetGameEnd()) break;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(1);

		isActiveUI_ = false;

		// --- ESCキーでポーズ表示 ---
		if (isActiveUI_ == false)
		{
			if (!pauseMenu_->IsVisible() && inputManager.IsTrgDown(KEY_INPUT_ESCAPE) == 1)
			{
				pauseMenu_->Show();  // メニュー表示
			}
		}
		

		inputManager.Update();

		if (pauseMenu_->IsVisible())
		{
			pauseMenu_->Update();
			if (pauseMenu_->IsDecisionMade())
			{
				int index = pauseMenu_->GetSelectedIndex();
				switch (index)
				{
				case 0: // 続ける
					pauseMenu_->Hide();
					break;

				case 1: // 遊び方
					// TODO: Help UI表示
					break;

				case 2: // 操作説明
					// TODO: 操作説明 UI表示
					break;

				case 3: // ゲーム終了
					sceneManager.GameEnd();
					return;
				}
			}
		}
		else
		{
			// 通常ゲーム処理
			sceneManager.Update();
		}
		// エフェクト更新
		UpdateEffekseer3D();

		

		// 描画
		sceneManager.Draw();
		// 平均FPS描画
		fps_->Draw();

		// エフェクト描画
		DrawEffekseer3D();

		if (pauseMenu_->IsVisible())
		{
			pauseMenu_->Draw();  // ポーズメニュー前面に
		}

		fps_->Wait();

		ScreenFlip();

		
		
	}
}

void Application::Destroy(void)
{
	InputManager::GetInstance().Destroy();
	ResourceManager::GetInstance().Destroy();
	SceneManager::GetInstance().DestroyInstance();
	Font::GetInstance().Destroy();

	EffectManager::GetInstance().Destroy();

	//エフェクシアの終了
	Effkseer_End();

	//DXLibの終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}
	
	if (fps_)
	{
		fps_.reset();
	}
	delete instance_;
}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

bool Application::IsActiveUI(void) const
{
	return isActiveUI_;
}

void Application::SetActiveUI(bool isActive)
{
	isActiveUI_ = isActive;
}

void Application::SetsActiveUI(ActiveUI ui)
{
	activeUI_ = ui;
}

bool Application::IsTeleportUIActive(void) const
{
	return activeUI_ == ActiveUI::TELEPORT;;
}

ActiveUI Application::GetActiveUI(void) const
{
	return activeUI_;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;

	// デバックスクリーンかどうか
	debugSc_ = false;
}