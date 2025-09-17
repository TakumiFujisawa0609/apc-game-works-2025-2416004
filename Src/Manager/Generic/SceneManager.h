#pragma once

#include<chrono>
#include<vector>
#include<memory>

#include"../../Application.h"
#include"../../Common/Fader.h"

class SceneBase;
class Fader;
class Camera;

class SceneManager
{
public:

	//シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		GAME,
		GAMECLEAR,
		GAMEOVER,
	};

	//コントローラ系統
	enum class CNTL
	{
		NONE,
		KEYBOARD,
		PAD
	};

	//インスタンスの生成
	static void CreateInstance(void);

	//インスタンスの取得
	static SceneManager& GetInstance(void);

	//初期化処理
	void Init(void);

	//3Dの初期化処理
	void Init3D(void);

	//更新処理
	void Update(void);

	//描画処理
	void Draw(void);

	//リソースの破棄
	void Destroy(void);

	//状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	//デルタタイムの取得
	float GetDeltaTime(void) const;

	SceneBase* GetScene(void) const;

	//カメラの取得
	std::shared_ptr<Camera> GetCamera(void) const;

private:

	//静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;		//現在のシーン
	SCENE_ID waitSceneId_;	//次のシーン

	//フェード
	std::unique_ptr<Fader> fader_;

	//各種シーン
	SceneBase* scene_;

	//カメラ
	std::shared_ptr<Camera> camera_;

	//シーン遷移中判定
	bool isSceneChanging_;

	//デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	//デフォルトコンストラクタをpriateにして、
	//外部から生成できないようにする
	SceneManager(void);

	//コピーコンストラクタ
	SceneManager(const SceneManager&);

	//デストラクタも同様
	~SceneManager(void) = default;



	//デルタタイムをリセットする
	void ResetDeltaTime(void);

	//シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	//フェード
	void Fade(void);
};

