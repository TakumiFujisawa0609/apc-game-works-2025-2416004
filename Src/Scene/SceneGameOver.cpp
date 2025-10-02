#include "SceneGameOver.h"

#include<DxLib.h>

#include"../Manager/Generic/Resource.h"
#include"../Manager/Generic/ResourceManager.h"
#include"../Manager/Generic/SceneManager.h"
#include"../Manager/Generic/InputManager.h"
#include"../Manager/Decoration/SoundManager.h"

SceneGameOver::SceneGameOver(void)
{
}

void SceneGameOver::Init(void)
{
	//サウンド
	auto& sound = SoundManager::GetInstance();
	auto& res = ResourceManager::GetInstance();

}

void SceneGameOver::Update(void)
{
	auto& sound = SoundManager::GetInstance();

	//シーン遷移(デバッグ)
	if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_SPACE))
	{
		//決定音
		sound.Play(SoundManager::SOUND::SE_PUSH);

		//BGM停止
		sound.Stop(SoundManager::SOUND::BGM_TITLE);

		//シーン遷移
		//SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAMECLEAR);

		//処理終了
		return;
	}
}

void SceneGameOver::Draw(void)
{
	DrawFormatString(0, 0, 0xffffff, "ゲームオーバー");
}

void SceneGameOver::Release(void)
{
}

void SceneGameOver::DrawDebug(void)
{
}
