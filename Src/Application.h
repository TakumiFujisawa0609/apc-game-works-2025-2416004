#pragma once
#include<string>
#include<windows.h>
class Fps;
class PauseMenu;

enum class ActiveUI {
	NONE,
	QUEST,
	DELIVERY,
	SHOP,
	TELEPORT
};

class Application
{
public:

	//ウインドサイズ
	static constexpr int SCREEN_SIZE_X = 1920;
	static constexpr int SCREEN_SIZE_Y = 1080;

	//フルスクリーンサイズ
	static constexpr int DEFA_SCREEN_SIZE_X = 1920;
	static constexpr int DEFA_SCREEN_SZIE_Y = 1080;

	

	//FPS
	static constexpr int DEFAULT_FPS = 60;
	static constexpr int FRAME_RATE = 1000 / 60;

	//データパス関連
	//----------------------------------------
	static const std::string PATH_IMAGE;
	static const std::string PATH_MODEL;
	static const std::string PATH_ANIM;
	static const std::string PATH_EFFECT;
	static const std::string PATH_TEXT;
	static const std::string PATH_FONT;
	static const std::string PATH_JSON;
	static const std::string PATH_BGM;
	static const std::string PATH_SE;
	static const std::string PATH_MOVIE;
	static const std::string PATH_MAP_DATA;
	//----------------------------------------

	//明治的にインスタンスを生成する
	static void CreateInstance(void);

	//性的インスタンスの取得
	static Application& GetInstance(void);

	//初期化
	void Init(void);

	//ゲームループ開始
	void Run(void);

	//リソースの破棄
	void Destroy(void);

	//初期化成功/失敗の判定
	bool IsInitFail(void) const;

	//解放成功/失敗の判定
	bool IsReleaseFail(void)const;

	//UIの表示状態
	bool IsActiveUI(void) const;

	void SetActiveUI(bool isActive);

	void SetsActiveUI(ActiveUI ui);
	bool IsTeleportUIActive(void) const;
	ActiveUI GetActiveUI(void) const;

private:

	//性的インスタンス
	static Application* instance_;

	//初期化失敗
	bool isInitFail_;

	//解放処理
	bool isReleaseFail_;

	bool isActiveUI_;

	//フレームレート制御
	Fps* fps_;

	PauseMenu* pauseMenu_;

	ActiveUI activeUI_;;

	//エフェクシアの初期化
	void InitEffekseer(void);

	//デフォルトコンストラクタ private にして
	//外部から生成できない様にする
	Application(void);

	//コピーコンストラクタも同様
	Application(const Application&);

	//デストラクタ
	~Application(void) = default;
};
