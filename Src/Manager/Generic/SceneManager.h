#pragma once

#include<chrono>
#include<vector>
#include<memory>
#include <list>

#include"../../Application.h"

class SceneBase;
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

	//インスタンスの破棄
	static void DestroyInstance(void);

	//初期化処理
	void Init(void);

	//3Dの初期化処理
	void Init3D(void);

	//更新処理
	void Update(void);

	//描画処理
	void Draw(void);

	//リソースの解放
	void Release(void);

	//シーン操作
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	//シーンの追加
	void PushScene(std::shared_ptr<SceneBase> scene);

	//シーンの削除
	void PopScene(void);

	//シーンの強制変更
	void JumpScene(std::shared_ptr<SceneBase> scene);

	// シーンIDの取得
	SCENE_ID GetSceneID(void) const;

	//ゲームの終了
	void GameEnd(void);

	//ゲーム終了の取得
	bool GetGameEnd(void) const;

	//デルタタイムの取得
	float GetDeltaTime(void) const;

	//カメラの取得
	std::shared_ptr<Camera> GetCamera(void) const;

private:

	//静的インスタンス
	static SceneManager* instance_;

	//シーンリスト
	std::list<std::shared_ptr<SceneBase>> scenes_;

	//シーンID
	SCENE_ID sceneId_;

	//ゲーム終了
	bool isGameEnd_;

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
	~SceneManager(void);

	//コピーコンストラクタ
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	//デルタタイムをリセットする
	void ResetDeltaTime(void);


};

