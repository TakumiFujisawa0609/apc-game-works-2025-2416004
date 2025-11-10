#define _CRTDBG_MAP_ALLOC

#include"Application.h"

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;  // NVIDIA用
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; // AMD用
}

//WInMain関数
//------------------------------
int WinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// インスタンスの生成
	Application::CreateInstance();

	// インスタンスの取得
	Application& instance = Application::GetInstance();

	if (instance.IsInitFail())
	{
		// 初期化失敗
		return -1;
	}

	// 実行
	instance.Run();

	// 解放
	instance.Destroy();

	return 0;
}