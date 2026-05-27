#pragma once
#include <map>

/// @brief 3Dモデルのアニメーション切り替えやブレンド、再生状態を制御するクラス
class AnimationController
{
public:
	
	/// @brief アニメーションの読み込みモード
	enum class ANIM_MODE
	{
		INTERNAL,    // モデル内部に含まれるアニメーション
		EXTERNAL,    // 外部ファイルから読み込むアニメーション
	};

	/// @brief 1つのアニメーション再生に必要な設定データ
	struct AnimData
	{
		int model = -1;                         // モデルID（DxLibのハンドル）
		int attachNo = -1;                      // アタッチ番号
		int animIndex = 0;                      // アニメーションインデックス
		float speed = 1.0f;                     // 再生速度
		float totalTime = 0.0f;                 // アニメーションの総時間
		float step = 0.0f;                      // アニメーションの現在の進行時間
		ANIM_MODE mode = ANIM_MODE::INTERNAL;   // アニメーションモード（内部/外部）
	};

	/// @brief コンストラクタ
	/// @param modelId 制御対象となるモデルのハンドル
	AnimationController(int modelId);

	/// @brief デストラクタ
	~AnimationController(void);

	/// @brief モデル内部のアニメーションを登録する
	/// @param type ユーザー定義のアニメーションタイプID（例：ID_IDLE）
	/// @param animIndex モデル内のインデックス
	/// @param speed 再生速度
	void AddInternal(int type, int animIndex, float speed);

	/// @brief 外部モデルファイルからアニメーションを登録する
	/// @param type ユーザー定義のアニメーションタイプID
	/// @param modelHandle 外部モデルのハンドル
	/// @param speed 再生速度
	void AddExternal(int type, const int modelHandle, float speed);

	/// @brief 指定したタイプのアニメーションを再生する
	/// @param type 再生するアニメーションタイプID
	/// @param isLoop ループ再生するかどうか
	/// @param blendTime 前のアニメーションからの補完時間
	void Play(int type, bool isLoop, float blendTime);

	/// @brief アニメーションの状態を更新する（毎フレーム呼び出し）
	void Update(void);

	/// @brief 現在再生中のアニメーションが終了したか判定する
	/// @return 終了していればtrue（ループ設定時は常にfalse）
	bool IsEnd(void) const;

	/// @brief 現在再生中のアニメーションタイプを取得する
	/// @return アニメーションタイプID
	int GetPlayType(void) const;

	/// @brief アニメーションのリソースを解放し、アタッチを解除する
	void Release(void);

	/// @brief 特定のアニメーションが再生中かチェックする
	/// @param type チェックしたいアニメーションタイプID
	/// @return 再生中ならtrue
	bool IsPlaying(int type) const;

	/// @brief 再生を一時停止する
	void Pause(void);

	/// @brief 一時停止を解除して再生を再開する
	void Resume(void);

	/// @brief 現在一時停止中かどうかを取得する
	/// @return 一時停止中ならtrue
	bool IsPaused(void) const;

	/// @brief 現在のアニメーションを特定の時間で停止させる
	/// @param time 停止させる時間
	void PauseAtTime(float time);

	/// @brief 指定したアニメーションタイプを特定の時間で停止させる
	/// @param animType アニメーションタイプID
	/// @param time 停止させる時間
	void PauseAtTime(int animType, float time);

	/// @brief 現在のアニメーションを特定のフレーム数で停止させる
	/// @param frameNumber 停止させるフレーム番号
	void PauseAtFrame(int frameNumber);

	/// @brief 指定したアニメーションタイプを特定のフレーム数で停止させる
	/// @param animType アニメーションタイプID
	/// @param frameNumber 停止させるフレーム番号
	void PauseAtFrame(int animType, int frameNumber);

	/// @brief 現在再生中のアニメーションの現在のフレームを取得する
	/// @return 現在のフレーム番号
	int GetCurrentFrame(void) const;

	/// @brief 指定したアニメーションタイプの現在のフレームを取得する
	/// @param animType アニメーションタイプID
	/// @return 現在のフレーム番号
	int GetCurrentFrame(int animType) const;

	/// @brief 現在再生中のアニメーションの総フレーム数を取得する
	/// @return 総フレーム数
	int GetTotalFrames(void) const;

	/// @brief 指定したアニメーションタイプの総フレーム数を取得する
	/// @param animType アニメーションタイプID
	/// @return 総フレーム数
	int GetTotalFrames(int animType) const;

	/// @brief 現在再生中のアニメーションの経過時間を取得する
	/// @return 現在の経過時間（秒）
	float GetCurrentTime(void) const;

	/// @brief 指定したアニメーションタイプの現在時間を取得する
	/// @param animType アニメーションタイプID
	/// @return 指定したアニメーションの経過時間（秒）
	float GetCurrentTimes(int animType) const;

	/// @brief 現在のアニメーション時間を直接設定する
	/// @param time 設定する時間
	void SetCurrentTime(float time);

	/// @brief 現在のアニメーションの総再生時間を取得する
	/// @return 総時間（秒）
	float GetTotalTime(void) const;

private:

	// ID・リソース管理関連
	int modeId_;                             // 操作対象のモデルハンドル
	std::map<int, AnimData> animations_;    // 登録済みアニメーションのマップ

	// 再生状態関連
	int playType_;   // 現在再生中のアニメーションタイプ
	int prevType_;   // 1つ前に再生していたアニメーションタイプ（ブレンド用）
	bool isLoop_;    // 現在のアニメーションをループ再生するか
	bool isPaused_;  // 一時停止フラグ

	// ブレンド（補完）関連
	float blendTIme_;   // アニメーション切り替え時の総補完時間
	float blendTimer_;  // 現在の補完進行度

	/// @brief アニメーションデータを内部マップに追加する
	/// @param type アニメーションタイプID
	/// @param anim 構築済みAnimData構造体
	void Add(int type, AnimData anim);
};