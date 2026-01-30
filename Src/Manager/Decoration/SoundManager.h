#pragma once
#include<unordered_map>

/// <summary>
/// サウンド管理クラス
/// DxLibの3D空間上の音声再生機能を利用して、
/// 音声に距離感を持たせることが可能なサウンド管理システム
/// </summary>
class SoundManager
{
public:
	/// <summary>
	/// 再生の種類を分類する列挙型
	/// </summary>
	enum class TYPE
	{
		NONE,
		BGM,   // バックグラウンドミュージック
		SE     // サウンドエフェクト
	};

	/// <summary>
	/// 使用する音声データの種類を定義する列挙型
	/// </summary>
	enum class SOUND
	{
		//ここに使用する音楽や効果音などを羅列
		NONE,
		BGM_TITLE,      // タイトル画面BGM
		BGM_GAME,       // ゲーム画面BGM
		BGM_FAITE,      // 戦闘BGM
		BGM_SCORE,      // スコア画面BGM
		SE_CANCEL,      // キャンセル音
		SE_SELECT,      // 選択音
		SE_PUSH,        // 決定音 
		SE_WATER,       // 水攻撃音
		SE_FIRE,        // 火の攻撃音
		SE_FREEZE,      // 凍結音
		SE_BLAST,       // 爆発音
		SE_LEVER_UP,    // レベルアップ音
		SE_SLASH,       // 斬撃音
		SE_GLIDER       // グライダー音

	};

	/// <summary>
	/// 音声データを格納する構造体
	/// </summary>
	struct SOUND_DATA
	{
		int data;       // 音声データ格納
		TYPE type;      // 音声データの種類 ※この要素は削除するかも
		int playMode;   // 音声データの再生タイプ
	};

	/// <summary>
	/// インスタンスを生成する
	/// シングルトンパターンの実装
	/// </summary>
	static void CreateInstance(void);

	/// <summary>
	/// インスタンスを取得する
	/// </summary>
	/// <returns>SoundManagerの唯一のインスタンス</returns>
	static SoundManager& GetInstance(void);

	void Init(void);

	/// <summary>
	/// サウンドの追加
	/// </summary>
	/// <param name="_type">音の種類分け(SEかBGMか)</param>
	/// <param name="_sound">具体的な用途</param>
	/// <param name="_data">音のデータ</param>
	void Add(const TYPE type, const SOUND sound, const int _data);

	/// <summary>
	/// 音声データの再生
	/// </summary>
	/// <param name="_sound">再生する音声データ</param>
	void Play(const SOUND _sound);

	/// <summary>
	/// 音声データの停止処理
	/// </summary>
	/// <param name="_sound">停止する音声データ</param>
	void Stop(const SOUND _sound);

	/// <summary>
	/// すべてのBGMを停止する
	/// BGMとして登録されている音声データを停止します。
	/// </summary>
	void StopAllBGM(void);

	/// <summary>
	/// 音声データの解放処理
	/// </summary>
	void Release(void);

	/// <summary>
	/// 音量調節
	/// </summary>
	/// <param name="_sound">調整対象の音声</param>
	/// <param name="_persent">調整割合(0%～100%)</param>
	void AdjustVolume(const SOUND _sound, const int _persent);

	//なり終わってるかどうか
	bool IsPlaying(SOUND sound);

	/// <summary>
	/// インスタンスの破棄
	/// </summary>
	void Destroy(void);

	// マスター音量の設定(BGM)
	void SetMasterVolumeBGM(int volume);

	// マスター音量の設定(SE)
	void SetMasterVolumeSE(int volume);

	// 現在のマスター音量を取得(BGM)
	int GetMasterVolumeBGM(void) const { return masterVolumeBGM_; }

	// 現在のマスター音量を取得(SE)
	int GetMasterVolumeSE(void) const { return masterVolumeSE_; }

	// すべての音量設定を再適用
	void ApplyMasterVolumes(void);

private:
	//インスタンス用
	static SoundManager* instance_;

	/// <summary>
	/// 音声データ格納用マップ
	/// SOUND列挙型をキーとして、対応する音声データを管理
	/// </summary>
	std::unordered_map<SOUND, SOUND_DATA>sounds_;

	// スレッド間の交通整理用
	static std::mutex g_soundMutex;

	// マンスタ音量(BGM)
	int masterVolumeBGM_;

	// マスター音量(SE)
	int masterVolumeSE_;

	/// <summary>
	/// コンストラクタ（シングルトンパターンのため外部からのインスタンス化を防止）
	/// </summary>
	SoundManager() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundManager() = default;
};
