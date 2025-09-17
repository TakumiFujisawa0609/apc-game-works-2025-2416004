#include "SoundManager.h"
#include<DxLib.h>
#include<cassert>

// シングルトンインスタンスの初期化
SoundManager* SoundManager::instance_ = nullptr;

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

// サウンドの追加
void SoundManager::Add(const TYPE type, const SOUND sound, const int _data)
{
	// 連想配列内にすでに要素が入っているかを検索
	// 入っていたら処理終了 (重複登録を防止)
	if (sounds_.find(sound) != sounds_.end()) return;

	// 再生するときデータの種類によって
	// ループ再生か単発かを判断する
	int mode = -1;
	if (type == TYPE::BGM) mode = DX_PLAYTYPE_LOOP;  // BGMはループ再生
	else mode = DX_PLAYTYPE_BACK;                    // SEは単発再生

	// 新規データのため情報を追加
	// 注意: 変数名が不一致 (*data → _data)
	sounds_.emplace(sound, SOUND_DATA{ _data, type, mode });
}

// 音声データの再生
void SoundManager::Play(const SOUND _sound)
{
	// 元データがないときは警告 (未登録の音声を再生しようとした場合)
	if (sounds_.find(_sound) == sounds_.end()) assert("設定していない音声を再生しようとしています。");

	// DxLibの関数を使用して音声を再生
	// 音声データと再生モード(ループ/単発)を指定
	PlaySoundMem(sounds_[_sound].data, sounds_[_sound].playMode);
}

// 停止処理
void SoundManager::Stop(const SOUND _sound)
{
	// 元データがないときは警告 (未登録の音声を停止しようとした場合)
	if (sounds_.find(_sound) == sounds_.end()) assert("設定していない音声を停止しようとしています。");

	// DxLibの関数を使用して音声を停止
	StopSoundMem(sounds_[_sound].data);
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
	// 元データがないときは警告 (未登録の音声を設定しようとした場合)
	// 注意: 変数名が不一致 (_sound → *sound)
	if (sounds_.find(sound) == sounds_.end()) assert("設定していない音声を設定しようとしています。");

	// DxLibの関数を使用して音量を変更
	// 0～255の範囲を0～100%に変換
	ChangeVolumeSoundMem(255  * persent / 100, sounds_[sound].data);
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