#include"Application.h"

#include "Manager/Decoration/EffectManager.h"
#include "Manager/Generic/ResourceManager.h"
#include "Manager/Generic/InputManager.h"
#include "Manager/Generic/SceneManager.h"
#include "DrawUI/Font.h"
#include "Fps/FpsController.h"
#include "DrawUI/SceneUI/PauseMenu.h"
#include "Manager/System/Loading.h"
#include "Manager/System/TimeManager.h"


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
		instance_->Initialize();
	}
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Initialize(void)
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

	SetUseASyncLoadFlag(TRUE);

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
	pauseMenu_->Initialize();
	
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

		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		inputManager.Update();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(1);

		// --- ESCキーでポーズ表示 ---
		if (isActiveUI_ == false)
		{
			if (!pauseMenu_->IsVisible() && inputManager.IsTrgDown(KEY_INPUT_ESCAPE) == 1 || 
				inputManager.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
					InputManager::JOYPAD_BTN::START_BUTTON))
			{
				pauseMenu_->Show();  // メニュー表示
			}
		}

		if (pauseMenu_->IsVisible())
		{
			if (!isActiveUI_)
			{
				pauseMenu_->Update();
			}

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
					break;
				}
			}
		}
		else
		{
			// 通常ゲーム処理
			sceneManager.Update();
		}

		sceneManager.Draw();

		// エフェクト更新
		UpdateEffekseer3D();


		// エフェクト描画
		DrawEffekseer3D();

		// 平均FPS描画
		fps_->Draw();


		// ポーズメニュー（最前面）
		if (pauseMenu_->IsVisible() && !Loading::GetInstance()->IsLoading() && !isActiveUI_)
		{

			TimeManager::GetInstance().SetPaused(true);
			pauseMenu_->Draw();
		}
		else
		{
			TimeManager::GetInstance().SetPaused(false);
		}

		fps_->Wait();

 		ScreenFlip();

		
		
	}
}

void Application:: DestroyInstance(void)
{
	InputManager::GetInstance(). DestroyInstance();
	ResourceManager::GetInstance(). DestroyInstance();
	SceneManager::GetInstance(). DestroyInstanceInstance();
	Font::GetInstance(). DestroyInstance();

	EffectManager::GetInstance(). DestroyInstance();

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