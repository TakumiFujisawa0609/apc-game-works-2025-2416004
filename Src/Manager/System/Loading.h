#pragma once

#include <thread>
#include <atomic>
#include <functional>

class Loading
{
public:

	//シングルトンインスタンス生成
	static void CreateInstance(void);

	//シングルトンインスタンス取得
	static Loading* GetInstance(void);

	//シングルトンインスタンス破棄
	void DestroyInstance(void);

	//初期化処理
	void Init(void);

	//更新処理
	void Update(void);

	//描画処理
	void Draw(void);

	//非同期ロード開始
	void StartAsyncLoad(std::function<void()> loadFunc);

	//ロード完了確認
	void EndAsyncLoad(void);

	//ロード中か
	bool IsLoading(void) const;

	//進捗状況を取得
	int GetProgress(void) const;

private:

	//コンストラクタ
	Loading(void) = default;

	//デストラクタ
	~Loading(void);

	//シングルトンインスタンス
	static Loading* instance_;

	// 非同期ロード処理用のスレッド
	std::thread loadingThread_;

	//非同期ロード中か
	std::atomic<bool> isLoading_{ false };

	//進行速度表示用
	std::atomic<float> progress_{ 0 };

	//非同期ロード用のスレッド関数。
	void ThreadFunc(std::function<void()> loadFunc);

};