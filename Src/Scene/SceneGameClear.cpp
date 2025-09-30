#include "SceneGameClear.h"

#include<DxLib.h>

#include"../Manager/Generic/Resource.h"
#include"../Manager/Generic/ResourceManager.h"
#include"../Manager/Generic/SceneManager.h"
#include"../Manager/Generic/InputManager.h"
#include"../Manager/Decoration/SoundManager.h"

SceneGameClear::SceneGameClear(void)
{
}

void SceneGameClear::Init(void)
{

	//サウンド
	auto& sound = SoundManager::GetInstance();
	auto& res = ResourceManager::GetInstance();

	sound.Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE, res.Load(ResourceManager::SRC::BGM_TITLE).handleId_);
	sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PUSH, res.Load(ResourceManager::SRC::SE_PUSH).handleId_);
	sound.AdjustVolume(SoundManager::SOUND::BGM_TITLE, 40);
	sound.AdjustVolume(SoundManager::SOUND::SE_PUSH, 30);

	//初期BGM
	sound.Play(SoundManager::SOUND::BGM_TITLE);
}

void SceneGameClear::Update(void)
{
	auto& sound = SoundManager::GetInstance();

	//シーン遷移(デバッグ)
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_RETURN))
	{
		//決定音
		sound.Play(SoundManager::SOUND::SE_PUSH);

		//BGM停止
		sound.Stop(SoundManager::SOUND::BGM_TITLE);

		//シーン遷移
		//SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);

		//処理終了
		return;
	}
}

void SceneGameClear::Draw(void)
{
	DrawFormatString(0, 0, 0xffffff, "ゲームクリア");
}

void SceneGameClear::Release(void)
{
}

void SceneGameClear::DrawDebug(void)
{
}
