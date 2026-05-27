#pragma once
#include <DxLib.h>
#include <unordered_map>
#include "SoundManager.h"
#include "../../Common/Quaternion.h"

/// @brief エフェクト管理クラス（Effekseer対応）
class EffectManager
{
public:
	/// @brief エフェクトの種類定義
	enum class EFFECT
	{
		FIRE,
		WATER,
		BLAST,
		FREEZE,
		FIRE_BURN,
		WET,
		AMMONIUM,
		LEVER_UP
	};

	/// @brief インスタンスを明示的に生成
	/// @param void 
	static void CreateInstance(void);

	/// @brief インスタンス取得
	/// @return EffectManagerインスタンスの参照
	static EffectManager& GetInstance(void);

	/// @brief インスタンス破棄
	/// @param void 
	static void   DestroyInstance(void);

	/// @brief エフェクトリソースの追加
	/// @param efc エフェクトの種類
	/// @param resourceHandle 読み込み済みのリソースハンドル
	void Add(const EFFECT& efc, int resourceHandle);

	/// @brief 全てのリソースを解放
	/// @param void 
	void Release(void);

	/// @brief エフェクトを再生する
	/// @param efc エフェクトの種類
	/// @param pos 再生座標
	/// @param qua 回転（クォータニオン）
	/// @param size スケール
	/// @param sound 同時に再生するSE（デフォルトは無し）
	void Play(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, float size, SoundManager::SOUND sound = SoundManager::SOUND::NONE);

	/// @brief 特定の種類のエフェクトを停止
	/// @param efc エフェクトの種類
	void Stop(const EFFECT& efc);

	/// @brief 再生中のエフェクト座標・回転を同期
	/// @param efc エフェクトの種類
	/// @param pos 同期する座標
	/// @param qua 同期する回転
	/// @param size 同期するスケール
	void SyncEffect(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, float size);

	/// @brief 特定のエフェクトが再生中か確認
	/// @param efc エフェクトの種類
	/// @return 再生中ならtrue
	bool IsPlayEffect(const EFFECT& efc);

	/// @brief エフェクトを再生し、個別の再生ハンドルを取得
	/// @param efc エフェクトの種類
	/// @param pos 再生座標
	/// @param qua 回転
	/// @param size スケール
	/// @param sound SE
	/// @return 再生中のハンドル
	int  PlayAndGetHandle(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, float size, SoundManager::SOUND sound = SoundManager::SOUND::NONE);

	/// @brief 指定したハンドルを停止
	/// @param handle 再生ハンドル
	void StopHandle(int handle);

	/// @brief 指定したハンドルのエフェクトを同期
	/// @param handle 再生ハンドル
	/// @param pos 座標
	/// @param qua 回転
	/// @param size スケール
	void SyncEffect(int handle, const VECTOR& pos, const Quaternion& qua, float size);

	/// @brief 指定したハンドルが再生中か確認
	/// @param handle 再生ハンドル
	/// @return 再生中ならtrue
	bool IsPlayEffect(int handle);

private:
	/// @brief コンストラクタ
	/// @param void 
	EffectManager(void);

	/// @brief デストラクタ
	/// @param void 
	~EffectManager(void) = default;

private:
	static EffectManager* instance_; // シングルトンインスタンス

	// エフェクト関連
	std::unordered_map<EFFECT, int> resourceMap_;   // エフェクトのリソースハンドル（読み込みデータ）
	std::unordered_map<EFFECT, int> playHandleMap_; // 現在再生中のインスタンスハンドル（簡易再生用）
};