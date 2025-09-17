#include "SceneGame.h"

#include <DxLib.h>

#include "../Common/Easing.h"
#include "../Manager/Generic/Camera.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/Resource.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/Collision.h"
#include "../Object/Grid.h"
#include "../Object/player.h"
#include "../DrawUI/Font.h"
#include "../Manager/System/DateTimeManager.h"
#include "../Manager/System/TimeManager.h"
#include "../Object/Common/AnimationController.h"




SceneGame::SceneGame(void)
{
	grid_ = nullptr;
	isStartFont_ = true;
}

void SceneGame::Init(void)
{

	//カメラ
	auto camera = SceneManager::GetInstance().GetCamera();

	//カメラを固定に設定
	camera->ChangeMode(Camera::MODE::FOLLOW);

	//日数
	dateTimeManager_ = new DateTimeManager();
	dateTimeManager_->Init();

	//グリッド線
	grid_ = new Grid();
	grid_->Init();

	//プレイヤー
	player_ = std::make_shared<Player>();
	player_->Init();

	//カメラをプレイヤーに追従
	camera->SetFollow(&player_->GetTransform());

	//サウンド
	auto& sound = SoundManager::GetInstance();
	auto& res = ResourceManager::GetInstance();

	

	//SE音の追加
	sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PUSH, res.Load(ResourceManager::SRC::SE_PUSH).handleId_);
	sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_CANCEL, res.Load(ResourceManager::SRC::SE_CANCEL).handleId_);
	sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_ALCHEMY, res.Load(ResourceManager::SRC::SE_ALCHEMY).handleId_);
	sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_ALCHEMY_FAIL, res.Load(ResourceManager::SRC::SE_ALCHEMY_FAIL).handleId_);
	sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_ALCHEMY_SUCCESS, res.Load(ResourceManager::SRC::SE_ALCHEMY_SUCCESS).handleId_);

	
	//SEの音量調整
	sound.AdjustVolume(SoundManager::SOUND::SE_CANCEL, 30);
	sound.AdjustVolume(SoundManager::SOUND::SE_PUSH, 30);
	sound.AdjustVolume(SoundManager::SOUND::SE_ALCHEMY, 30);
	sound.AdjustVolume(SoundManager::SOUND::SE_ALCHEMY_FAIL, 100);
	sound.AdjustVolume(SoundManager::SOUND::SE_ALCHEMY_SUCCESS, 30);

}

void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();

	//日数
	dateTimeManager_->Update();
	
	//プレイヤー
	player_->Update();

}

void SceneGame::Draw(void)
{
	//プレイヤーの描画
	player_->Draw();
	

#ifdef _DEBUG
	//デバック表示
	DrawDebug();
#endif // _DEBUG
}

void SceneGame::Release(void)
{

	grid_->Release();
	delete grid_;
	grid_ = nullptr;

	//プレイヤーの解放
	player_->Release();
	
}

void SceneGame::DrawDebug(void)
{
	grid_->Draw();

	int day = dateTimeManager_->GetDay();

	// 時間帯を文字列に変換
	const char* timeZoneStr = nullptr;
	switch (dateTimeManager_->GetTimeZone())
	{
	case DateTimeManager::TIME_ZONE::MORNING: timeZoneStr = "朝"; break;
	case DateTimeManager::TIME_ZONE::DAY:     timeZoneStr = "昼"; break;
	case DateTimeManager::TIME_ZONE::EVENING: timeZoneStr = "夕方"; break;
	case DateTimeManager::TIME_ZONE::NIGHT:   timeZoneStr = "夜"; break;
	}

	// 表示用（画面左上）
	DrawFormatString(20, 100, GetColor(255, 255, 255), "日付: %d日目", dateTimeManager_->GetDay());
	DrawFormatString(20, 80, GetColor(0, 255, 0), "時刻: %02d:%02d:%02d",
		TimeManager::GetInstance().GetGameHour(),
		TimeManager::GetInstance().GetGameMinute(),
		TimeManager::GetInstance().GetGameSecond());
}

