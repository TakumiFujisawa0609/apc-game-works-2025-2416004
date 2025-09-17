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
		BGM_GAME,       // ゲームプレイ中BGM
		BGM_GAMEOVER,   // ゲームオーバー時BGM
		BGM_GAMECLEAR,  // ゲームクリア時BGM
		BGM_GARDEN_DAY, //ガーデンの朝昼BGM
		BGM_GARDEN_NIGHT, //ガーデンの夕方夜BGM
		BGM_ATELIER,    //アトリエBGM
		BGM_GUILD,      //ギルドのBGM

		SE_PUSH,        // ボタン押下時効果音
		SE_CANCEL,              //キャンセル音
		SE_SELECT,      // カーソル移動
		SE_DAMAGE,      // ダメージ受けた時効果音
		SE_GET,         // アイテム取得時効果音
		SE_ALCHEMY,     // 錬金時効果音	
		SE_ALCHEMY_FAIL, //錬金失敗効果音
		SE_ALCHEMY_SUCCESS, //錬金成功効果音
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

private:
	//インスタンス用
	static SoundManager* instance_;

	/// <summary>
	/// 音声データ格納用マップ
	/// SOUND列挙型をキーとして、対応する音声データを管理
	/// </summary>
	std::unordered_map<SOUND, SOUND_DATA>sounds_;

	/// <summary>
	/// コンストラクタ（シングルトンパターンのため外部からのインスタンス化を防止）
	/// </summary>
	SoundManager() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundManager() = default;
};
