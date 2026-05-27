#include "../../Pch.h"
#include "EffectManager.h"

// シングルトンインスタンスの初期化
EffectManager* EffectManager::instance_ = nullptr;

void EffectManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new EffectManager();
	}
}

EffectManager& EffectManager::GetInstance(void)
{
	return *instance_;
}

void EffectManager:: DestroyInstance(void)
{
	if (instance_ != nullptr)
	{
		// 管理しているリソースの解放
		instance_->Release();
		delete instance_;
		instance_ = nullptr;
	}
}

EffectManager::EffectManager(void)
{
}

void EffectManager::Add(const EFFECT& efc, int resourceHandle)
{
	// 既に同じエフェクトタイプが登録されている場合はスキップ
	if (resourceMap_.find(efc) != resourceMap_.end())
	{
		return;
	}
	resourceMap_.emplace(efc, resourceHandle);
}

void EffectManager::Play(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, float size, SoundManager::SOUND sound)
{
	auto it = resourceMap_.find(efc);
	if (it == resourceMap_.end())
	{
		assert(!"登録されていないエフェクトを再生しようとしました。");
		return;
	}

	// 簡易再生モード：同じ種類が既に再生中なら、多重再生を避けるために古い方を停止
	Stop(efc);

	// Effekseerエフェクトの再生（インスタンスハンドルを取得）
	int handle = PlayEffekseer3DEffect(it->second);
	playHandleMap_[efc] = handle;

	// 再生直後に位置・回転・スケールを同期
	SyncEffect(handle, pos, qua, size);

	// SEの指定がある場合は同時に再生
	if (sound != SoundManager::SOUND::NONE)
	{
		SoundManager::GetInstance().Play(sound);
	}
}

int EffectManager::PlayAndGetHandle(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, float size, SoundManager::SOUND sound)
{
	auto it = resourceMap_.find(efc);
	if (it == resourceMap_.end())
	{
		return -1;
	}

	// 詳細再生モード：種類ごとの上書きを行わず、新しいハンドルを発行して返す
	int handle = PlayEffekseer3DEffect(it->second);
	SyncEffect(handle, pos, qua, size);

	if (sound != SoundManager::SOUND::NONE)
	{
		SoundManager::GetInstance().Play(sound);
	}

	return handle;
}

void EffectManager::Stop(const EFFECT& efc)
{
	auto it = playHandleMap_.find(efc);
	if (it != playHandleMap_.end())
	{
		// 簡易再生用の管理マップからハンドルを検索して停止
		StopEffekseer3DEffect(it->second);
		playHandleMap_.erase(it);
	}
}

void EffectManager::StopHandle(int handle)
{
	// 指定された再生ハンドルを直接停止（再生中かどうかを確認してから実行）
	if (handle != -1 && IsEffekseer3DEffectPlaying(handle) == 0)
	{
		StopEffekseer3DEffect(handle);
	}
}

void EffectManager::SyncEffect(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, float size)
{
	auto it = playHandleMap_.find(efc);
	if (it != playHandleMap_.end())
	{
		// 簡易再生モード用の同期処理
		SyncEffect(it->second, pos, qua, size);
	}
}

void EffectManager::SyncEffect(int handle, const VECTOR& pos, const Quaternion& qua, float size)
{
	// ハンドルが無効、または既に再生終了している場合は何もしない
	if (handle == -1 || IsEffekseer3DEffectPlaying(handle) != 0)
	{
		return;
	}

	// 位置の更新
	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y, pos.z);

	// 回転の更新（クォータニオンをオイラー角に変換して適用）
	VECTOR euler = qua.ToEuler();
	SetRotationPlayingEffekseer3DEffect(handle, euler.x, euler.y, euler.z);

	// スケールの更新（全軸均等）
	SetScalePlayingEffekseer3DEffect(handle, size, size, size);
}

bool EffectManager::IsPlayEffect(const EFFECT& efc)
{
	auto it = playHandleMap_.find(efc);
	if (it == playHandleMap_.end())
	{
		return false;
	}
	return IsPlayEffect(it->second);
}

bool EffectManager::IsPlayEffect(int handle)
{
	if (handle == -1)
	{
		return false;
	}
	// EffekseerのAPI: 0が再生中、それ以外(負の値など)は停止状態を示す
	return IsEffekseer3DEffectPlaying(handle) == 0;
}

void EffectManager::Release(void)
{
	// 現在管理している全てのインスタンスハンドルを停止させる
	for (auto& pair : playHandleMap_)
	{
		StopEffekseer3DEffect(pair.second);
	}

	// マップ情報のクリア（リソースハンドル自体は別途読み込み側で管理される想定）
	playHandleMap_.clear();
	resourceMap_.clear();
}