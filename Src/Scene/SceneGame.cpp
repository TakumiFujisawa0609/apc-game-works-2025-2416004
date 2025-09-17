#include "SceneGame.h"

#include <DxLib.h>

#include "../Common/Easing.h"
#include "../Manager/Generic/Camera.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Object/Manager/ItemManager.h"
#include "../Manager/Generic/Resource.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/Collision.h"
#include "../Object/Manager/StageManager.h"
#include "../Object/Manager/AlchemyManager.h"
#include "../Object/Grid.h"
#include "../Object/player.h"
#include "../DrawUI/SceneUI/InventoryUI.h"
#include "../DrawUI/Font.h"
#include "../Manager/System/DateTimeManager.h"
#include "../Manager/System/TimeManager.h"
#include "../DrawUI/SceneUI/QuestUI.h"
#include "../Object/PlayerStop.h"
#include "../DrawUI/SceneUI/ItemPickupUI.h"
#include "../Object/Common/AnimationController.h"




SceneGame::SceneGame(void)
{
	grid_ = nullptr;
	isStartFont_ = true;
}

void SceneGame::Init(void)
{
	//アイテムマネージャー初期化
	ItemManager::CreateInstance();

	//カメラ
	auto camera = SceneManager::GetInstance().GetCamera();

	//カメラを固定に設定
	camera->ChangeMode(Camera::MODE::FOLLOW);

	//アイテムマネージャー
	auto itemManager_ = &ItemManager::GetInstance();

	// ゲーム開始時などに初期化
	PlayerStop::CreateInstance();

	//日数
	dateTimeManager_ = new DateTimeManager();
	dateTimeManager_->Init();

	//クエストがデータを受け取る
	QuestUI::GetInstance().SetDateTimeManager(dateTimeManager_);

	//グリッド線
	grid_ = new Grid();
	grid_->Init();

	//プレイヤー
	player_ = std::make_shared<Player>();
	player_->Init();

	//アイテム入手UI
	itemPickup_ = std::make_shared<ItemPickupUI>();
	itemPickup_->Init();

	// プレイヤー登録
	PlayerStop::GetInstance().SetPlayer(player_.get());

	//カメラをプレイヤーに追従
	camera->SetFollow(&player_->GetTransform());

	// ステージ管理初期
	stageManager_ = new StageManager();
	stageManager_->Init(player_,dateTimeManager_);

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

	

	// UIの生成
	inventoryUI_ = new InventoryUI();

	

	AlchemyManager::GetInstance().Init();
	


}

void SceneGame::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& alchemy = AlchemyManager::GetInstance();

	////シーン遷移(デバッグ)
	//if (input.IsTrgDown(KEY_INPUT_SPACE))
	//{
	//	//決定音
	//	sound.Play(SoundManager::SOUND::SE_PUSH);

	//	//BGM停止
	//	sound.Stop(SoundManager::SOUND::BGM_TITLE);

	//	//シーン遷移
	//	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMEOVER);

	//	//処理終了
	//	return;
	//}

	//日数
	dateTimeManager_->Update();

	// インベントリUIの更新
	inventoryUI_->Update();

	

	//// Rキーで錬金メニューの開閉
	//if (input.IsTrgDown(KEY_INPUT_R))
	//{
	//	if (alchemy.IsOpen())
	//		alchemy.Close();
	//	else
	//		alchemy.Open();
	//}

	//// 錬金メニューが開いているときのみ更新
	//if (alchemy.IsOpen())
	//{
	//	alchemy.Update();
	//}


	//プレイヤー
	player_->Update();

	stageManager_->Update();

	itemPickup_->Update();

	

	
}

void SceneGame::Draw(void)
{
	auto& alchemy = AlchemyManager::GetInstance();


	//プレイヤーの描画
	player_->Draw();

	//ステージの描画
	stageManager_->Draw();

	int x;
	x = Application::DEFA_SCREEN_SIZE_X;

	//QuestUI::GetInstance().Draw();

	// インベントリUIの描画
	inventoryUI_->Draw();

	itemPickup_->Draw();

	
	

	

	/*if(alchemy.IsOpen())
	{
		alchemy.Draw();
	}*/
	

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

	//インベントリUIの解放
	delete inventoryUI_;
	inventoryUI_ = nullptr;

	//ステージマネージャーの削除
	stageManager_->Destroy();
	delete stageManager_;

	PlayerStop::GetInstance().Destroy();

	ItemManager::GetInstance().Destroy();

	
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

