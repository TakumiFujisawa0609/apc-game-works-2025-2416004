#include "SoundManager.h"
#include<DxLib.h>
#include<cassert>
#include <mutex>

// シングルトンインスタンスの初期化
SoundManager* SoundManager::instance_ = nullptr;

// これを追加：静的メンバ変数の実体を定義する
std::mutex SoundManager::g_soundMutex;

// インスタンス生成メソッド
// 未生成の場合のみ新しいインスタンスを作成する
void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr) {
		instance_ = new SoundManager();
	}
}

// インスタンス取得メソッド
SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

void SoundManager::Init(void)
{
	masterVolumeBGM_ = 70;
	masterVolumeSE_ = 80;
}

// サウンド追加
void SoundManager::Add(const TYPE type, const SOUND sound, const int _data) {
	if (sounds_.find(sound) != sounds_.end()) return;

	int mode = (type == TYPE::BGM) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	sounds_.emplace(sound, SOUND_DATA{ _data, type, mode });

	// 追加時に現在のマスター音量を即座に反映
	int targetVol = (type == TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
	ChangeVolumeSoundMem(targetVol * 255 / 100, _data);
}
void SoundManager::Play(const SOUND _sound)
{
	// instance_ ではなく GetInstance() を呼んで実体を確認
	if (instance_ == nullptr) return;

	// ロックをかける
	std::lock_guard<std::mutex> lock(g_soundMutex);

	// map 自体が初期化されているか、要素があるかを確認
	if (sounds_.empty()) return;

	auto it = sounds_.find(_sound);
	if (it == sounds_.end()) return;

	// ハンドルが有効（正の数）かチェックしてから再生
	if (it->second.data > 0)
	{
		PlaySoundMem(it->second.data, it->second.playMode);
	}
}

// 停止処理
void SoundManager::Stop(const SOUND _sound)
{
	std::lock_guard<std::mutex> lock(g_soundMutex); // 追加
	auto it = sounds_.find(_sound);
	if (it == sounds_.end()) return;
	StopSoundMem(it->second.data);
}

// 全てのBGMを停止する
void SoundManager::StopAllBGM(void)
{
	// 格納されている全ての音声データをチェック
	for (auto& pair : sounds_)
	{
		const SOUND_DATA& data = pair.second;

		// BGMとして登録されている音声のみを停止
		if (data.type == TYPE::BGM)
		{
			StopSoundMem(data.data);
		}
	}
}

// 全音声データの解放処理
void SoundManager::Release(void)
{
	// 連想配列の全要素を削除
	sounds_.clear();
}

// 音量調節
void SoundManager::AdjustVolume(const SOUND sound, const int persent)
{
	std::lock_guard<std::mutex> lock(g_soundMutex); // 追加
	auto it = sounds_.find(sound);
	if (it == sounds_.end()) return;
	ChangeVolumeSoundMem(255 * persent / 100, it->second.data);
}

//なり終わってるかどうか
bool SoundManager::IsPlaying(SOUND sound)
{
	auto it = sounds_.find(sound);
	if (it == sounds_.end()) return false; // 存在しなければ再生していない

	int handle = it->second.data;
	return CheckSoundMem(handle) == 1;  // 1なら再生中
}

// インスタンスの破棄処理
void SoundManager::Destroy(void)
{
	// 全音声データを解放してからインスタンスを削除
	Release();
	delete instance_;
}

// マスター音量の設定(BGM)
void SoundManager::SetMasterVolumeBGM(int volume)
{
	masterVolumeBGM_ = std::clamp(volume, 0, 100);
	ApplyMasterVolumes();
}

// マスター音量の設定(SE)
void SoundManager::SetMasterVolumeSE(int volume)
{
	masterVolumeSE_ = std::clamp(volume, 0, 100);
	ApplyMasterVolumes();
}

// すべての音量設定の再適用
void SoundManager::ApplyMasterVolumes(void)
{
	for (auto& pair : sounds_)
	{
		int targetVol = (pair.second.type == TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;

		// 音量設定は 0 ～　255 なので変換して適用
		ChangeVolumeSoundMem(targetVol * 255 / 100, pair.second.data);
	}
}
