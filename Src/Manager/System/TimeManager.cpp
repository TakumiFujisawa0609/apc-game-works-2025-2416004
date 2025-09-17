#include "TimeManager.h"

//シングルトンのインスタンス初期化
TimeManager* TimeManager::instance_ = nullptr;

void TimeManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new TimeManager();
	}

	instance_->Init();
}

TimeManager& TimeManager::GetInstance(void)
{
	return *instance_;
}

void TimeManager::Destroy(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void TimeManager::Reset(void)
{
	gameTime_ = 0.0f;
	timers_.clear();
	prevTime_ = std::chrono::steady_clock::now();
}

void TimeManager::Init(void)
{
	gameTime_ = 0.0f;
	gameSpeed_ = 144.0f;
	timers_.clear();

	prevTime_ = std::chrono::steady_clock::now();
}

void TimeManager::Update(void)
{
	auto now = std::chrono::steady_clock::now();

	std::chrono::duration<float> delta = now - prevTime_;

	prevTime_ = now;

	float deltaTime = delta.count();

	gameTime_ += deltaTime * gameSpeed_;

	for (auto& pair : timers_)
	{
		Timer& timer = pair.second;
		if (timer.timeLeft > 0.0f)
		{
			timer.timeLeft -= deltaTime;
		}
	}
}

float TimeManager::GetGameTime(void) const
{
	return gameTime_;
}

int TimeManager::GetGameHour(void) const
{
	return static_cast<int>(gameTime_) / 3600;
}

int TimeManager::GetGameMinute(void) const
{
	return (static_cast<int>(gameTime_) / 60) % 60;
}

int TimeManager::GetGameSecond(void) const
{
	return static_cast<int>(gameTime_) % 60;
}

void TimeManager::SetGameTime(float time)
{
	gameTime_ = time;
}

void TimeManager::StartTimer(const std::string& id, float duration)
{
	timers_[id] = { duration, duration };
}

bool TimeManager::IsTimerFinished(const std::string& id) const
{
	auto it = timers_.find(id);

	if (it == timers_.end())
	{
		return true;
	}

	return it->second.timeLeft <= 0.0f;
}

void TimeManager::ResetTimer(const std::string& id)
{
	auto it = timers_.find(id);
	if (it != timers_.end())
	{
		it->second.timeLeft = it->second.duration;
	}
}
