#include "DateTimeManager.h"

#include "TimeManager.h"
#include "../../Manager/Generic/InputManager.h"

DateTimeManager::DateTimeManager(void)
{
	currentDay_ = 0;

	lastRecordedHour_ = -1;
}

void DateTimeManager::Init(void)
{
	currentDay_ = 0;

	lastRecordedHour_ = TimeManager::GetInstance().GetGameHour();
}

void DateTimeManager::Update(void)
{
	auto& time = TimeManager::GetInstance();

	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_R))
	{
		currentDay_++;
	}
	while (time.GetGameTime() >= HOURS_IN_DAY)
	{
		currentDay_++;
		time.SetGameTime(time.GetGameTime() - HOURS_IN_DAY);
	}
}

void DateTimeManager::Reset(void)
{
	currentDay_ = 0;
	lastRecordedHour_ = TimeManager::GetInstance().GetGameHour();
}

int DateTimeManager::GetDay(void) const
{
	return currentDay_;
}

DateTimeManager::TIME_ZONE DateTimeManager::GetTimeZone(void) const
{
	int hour = TimeManager::GetInstance().GetGameHour();

	if (hour >= 6 && hour < 10)
	{
		return TIME_ZONE::MORNING;
	}
	else if (hour >= 10 && hour < 18)
	{
		return TIME_ZONE::DAY;
	}
	else if (hour >= 18 && hour < 22)
	{
		return TIME_ZONE::EVENING;
	}
	else
	{
		return TIME_ZONE::NIGHT;
	}
}
