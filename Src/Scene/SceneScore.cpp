#include "SceneScore.h"

#include<DxLib.h>

#include "../Manager/Generic/Resource.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Scene/SceneTitle.h"
#include "../Manager/System/Loading.h"
#include "../Manager/System/TimeManager.h"

SceneScore::SceneScore(void)
{
}

void SceneScore::Load(void)
{
	SceneBase::Load();

	//時間カウントリセット
	TimeManager::GetInstance().Reset();

	EndLoad();
}

void SceneScore::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneScore::Init(void)
{

	//サウンド
	auto& sound = SoundManager::GetInstance();
	auto& res = ResourceManager::GetInstance();

	//初期BGM
	sound.Play(SoundManager::SOUND::BGM_TITLE);
}

void SceneScore::Update(void)
{
	auto& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();

    if (input.IsTrgDown(KEY_INPUT_SPACE))
    {
        // 決定音
        sound.Play(SoundManager::SOUND::SE_PUSH);

        // BGM停止
        sound.Stop(SoundManager::SOUND::BGM_TITLE);

		auto newScene = std::make_shared<SceneTitle>();

		SceneManager::GetInstance().ChangeScene(newScene);
		
		return;
    }
}

void SceneScore::Draw(void)
{
	//DrawFormatString(0, 0, 0xffffff, "ゲームクリア");
}

void SceneScore::Release(void)
{
}

void SceneScore::DrawDebug(void)
{
}
