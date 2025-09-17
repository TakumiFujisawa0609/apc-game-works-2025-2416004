#pragma once

#include <unordered_map>
#include <string>
#include <chrono>

class TimeManager
{
public:
	//明示的にインスタンスを生成する
	static void CreateInstance(void);

	//静的インスタンスの取得
	static TimeManager& GetInstance(void);

	//インスタンスの破棄
	static void Destroy(void);

	//リセット処理
	void Reset(void);

	//初期化
	void Init(void);

	//更新処理
	void Update(void);

	//ゲームない時間
	float GetGameTime(void) const;
	int GetGameHour(void) const;
	int GetGameMinute(void) const;
	int GetGameSecond(void) const;

	void SetGameTime(float time);

	//タイマー管理
	void StartTimer(const std::string& id, float duration);
	bool IsTimerFinished(const std::string& id) const;
	void ResetTimer(const std::string& id);

private:

	//静的インスタンス
	static TimeManager* instance_;

	//コンストラクタ
	TimeManager(void) = default;

	//デストラクタ
	~TimeManager(void) = default;

	//インすタスのコピー禁止
	TimeManager(const TimeManager&) = delete;

	//代入演算子の禁止
	TimeManager& operator=(const TimeManager&) = delete;

	//ゲーム内時間
	float gameTime_;

	//ゲーム内時間の経過時間
	float gameSpeed_;

	//前フレームぞ時間
	std::chrono::steady_clock::time_point prevTime_;

	//タイマー管理用
	struct Timer
	{
		//タイマーの開始時間
		float timeLeft;

		//タイマーの持続時間
		float duration;
	};

	std::unordered_map < std::string, Timer> timers_;

};

