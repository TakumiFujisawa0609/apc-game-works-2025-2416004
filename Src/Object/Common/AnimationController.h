#pragma once
#include <map>

class AnimationController
{
public:
	//アニメモード
	enum class ANIM_MODE
	{
		INTERNAL,    //内部
		EXTERNAL,    //外部
	};

	//アニメーションデータ
	struct AnimData
	{
		int model = -1;									 //モデルID
		int attachNo = -1;								 //アタッチ番号
		int animIndex = 0;								 //アニメーションインデックス
		float speed = 1.0f;								 //再生速度
		float totalTime = 0.0f;							 //アニメーションの総時間
		float step = 0.0f;								 //アニメーションの進行時間
		ANIM_MODE mode = ANIM_MODE::INTERNAL;			 //アニメーションモード
	};

	//コンストラクタ
	AnimationController(int modelId);

	//デストラクタ
	~AnimationController(void);

	//内部アニメーションの追加
	void AddInternal(int type, int animIndex, float speed);

	//外部アニメーションの追加
	void AddExternal(int type, const int modelHandle, float speed);

	//アニメーションの再生
	void Play(int type, bool isLoop, float blendTime);

	//アニメーションの更新
	void Update(void);

	//アニメーションの終了判定
	bool IsEnd(void) const;

	//アニメーションの再生タイプを取得
	int GetPlayType(void) const;

	//アニメーションの解放
	void Release(void);

	// アニメーションが再生中かチェック
	bool IsPlaying(int type) const;

	// アニメーションの一時停止
	void Pause(void);

	// アニメーションの再開
	void Resume(void);

	// アニメーションが一時停止中か
	bool IsPaused(void) const;

	// 特定のフレーム時間で停止
	void PauseAtTime(float time);

	// 特定のアニメーションタイプを指定してフレーム時間で停止
	void PauseAtTime(int animType, float time);

	void PauseAtFrame(int frameNumber);

	void PauseAtFrame(int animType, int frameNumber);

	int GetCurrentFrame(void) const;

	int GetCurrentFrame(int animType) const;

	int GetTotalFrames(void) const;

	int GetTotalFrames(int animType) const;

	// 現在のアニメーション時間を取得
	float GetCurrentTime(void) const;

	// 指定したアニメーションタイプの現在時間を取得
	float GetCurrentTimes(int animType) const;

	// 現在のアニメーション時間を設定
	void SetCurrentTime(float time);

	// アニメーションの総時間を取得
	float GetTotalTime(void) const;

private:
	//アニメーションの追加
	void Add(int type, AnimData anim);

	//アニメーションID
	int modeId_;

	//アニメーションデータ
	std::map<int, AnimData> animations_;

	//再生中のアニメーションタイプ
	int playType_;

	//前回のアニメーションタイプ
	int prevType_;

	//ブレンド時間
	float blendTIme_;

	//ブレンドタイマー
	float blendTimer_;

	//ループするかどうか
	bool isLoop_;

	//一時停止フラグ
	bool isPaused_;
};