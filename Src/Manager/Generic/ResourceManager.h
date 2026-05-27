#pragma once
#include <map>
#include <mutex>
#include "Resource.h"

/// @brief 画像、モデル、音源などの全リソースを一括管理するクラス（シングルトン）
class ResourceManager
{
public:

	/// @brief 管理するリソースの識別子定義
	enum class SRC
	{
		//ここに保存しておきたい画像やモデル、音源などの名前を記す

		// ロゴ
		TYTLE_LOGO,				//タイトルロゴ
		GAMEOVER_LOGO,			//ゲームオーバー
		GAMECLERA_LOGO,			//ゲームクリア

		// 画像
		IMG_FIRE_SUKILL,        // 火のスキルアイコン
		IMG_FIRE_SUKILL_CD,     // 火のスキルアイコン(クールダウン中)
		IMG_WATER_SUKILL,       // 水のスキルアイコン
		IMG_WATER_SUKILL_CD,    // 水のスキルアイコン(クールダウン中)
		IMG_TUTORIAL_1,         // チュートリアル画像1
		IMG_TUTORIAL_2,         // チュートリアル画像2
		IMG_TUTORIAL_3,         // チュートリアル画像3
		IMG_OPTION,             // 設定背景画像
		IMG_OPTION_1,           // 設定画像

		// モデル
		MODEL_PLAYER,			// プレイヤーモデル
		MODEL_GROUND,			// ステージ地面のモデル
		MODEL_SLIME,            // スライムモデル
		MODEL_SWORD,            // 剣モデル
		MODEL_SKY_DOME,         // スカイドーム
		MODEL_ENEMYSPAWNER,     // エネミースポナーモデル
		MODEL_TOWER,            // タワーモデル
		MODEL_GLIDER,           // グライダーモデル

		// サウンド
		BGM_TITLE,              // タイトル画面BGM
		BGM_SCORE,              // スコア画面BGM
		BGM_GAME,               // ゲーム画面BGM
		BGM_FAITE,              // 戦闘BGM

		// 効果音
		SE_CANCEL,              // キャンセル音
		SE_SELECT,              // 選択音
		SE_PUSH,                // 決定音
		SE_SLASH,               // 斬撃音
		SE_WATER,               // 水攻撃音
		SE_FIRE,                // 火の攻撃音
		SE_FREEZE,              // 凍結音
		SE_BLAST,               // 爆発音
		SE_LEVER_UP,            // レベルアップ音
		SE_GLIDER,              // グライダー音
		
		// エフェクト
		EFFECT_FIRE,            // 火のエフェクト
		EFFECT_WATER,           // 水のエフェクト
		EFFECT_BLAST,           // 爆発エフェクト
		EFFECT_FREEZE,          // 凍結エフェクト
		EFFECT_LEVER_UP,        // レベルアップエフェクト
		EFFECT_FIRE_BURN,       // やけどエフェクト
		EFFECT_WET,             // 湿潤エフェクト
		EFFECT_AMMONIUM,        // 硝酸アンモニウムエフェクト


		// アニメーション
		ANIM_PLAYER_IDEL,       //プレイヤーの待機アニメーション
		ANIM_PLAYER_WALK,       //プレイヤーの歩きアニメーション
		ANIM_PLAYER_ATTACK,     //プレイヤーの攻撃アニメーション
		ANIM_PLAYER_JAMP,       //プレイヤーのジャンプアニメーション
		ANIM_PLAYER_GLIDE,      //プレイヤーのグライドアニメーション

		// 映像
		TITLE_MOVIE,            // タイトルムービー
		BG_MOVIE,               // 背景映像
	};

	/// @brief 明示的にインスタンスを生成する
	static void CreateInstance(void);

	/// @brief 静的インスタンスの取得
	/// @return ResourceManagerの参照
	static ResourceManager& GetInstance(void); 

	/// @brief システム全体の初期化
	void Initialize(void);

	/// @brief タイトルシーン用リソースの読み込み登録
	void InitTitle(void);

	/// @brief ゲームシーン用リソースの読み込み登録
	void InitGame(void);

	/// @brief ゲームオーバー用リソースの読み込み登録
	void InitGameOver(void);

	/// @brief ゲームクリア用リソースの読み込み登録
	void InitGameClear(void);

	/// @brief チュートリアル用リソースの読み込み登録
	void InitTutorial(void);

	/// @brief 設定画面用リソースの読み込み登録
	void InitOption(void);

	/// @brief プレイヤー用リソースの読み込み登録
	void ResourcePlayer(void);

	/// @brief 敵キャラクター用リソースの読み込み登録
	void ResourceEnemy(void);

	/// @brief シーン切り替え時などのリソース解放
	void Release(void);

	/// @brief プログラム終了時のリソース完全破棄
	void  DestroyInstance(void);

	/// @brief リソースの読み込み（未ロードなら実行）
	/// @param src リソース識別子
	/// @return Resourceオブジェクト
	Resource Load(SRC src);

	/// @brief モデルリソースを複製して読み込む（個別の動きをさせる場合に使用）
	/// @param src モデルのリソース識別子
	/// @return 複製されたモデルのハンドルID
	int LoadModelDuplicate(SRC src);

	/// @brief ロード済みリソースのハンドルを取得する
	/// @param src リソース識別子
	/// @return ハンドルID
	int GetHandle(SRC src);

private:

	//静的インスタンス
	static ResourceManager* instance_;

	//リソース管理の対象
	std::map<SRC, Resource> resourcesMap_;

	//読み込み済みリソース
	std::map<SRC, Resource*> loadedMap_;

	// スレッドセーフ用ミューテックス
	static std::mutex g_resMutex;

	/// @brief コンストラクタ
	ResourceManager(void);

	/// @brief デストラクタ
	~ResourceManager(void) = default;

	/// @brief コピー禁止コンストラクタ
	ResourceManager(const ResourceManager&) = delete;

	/// @brief コピー代入演算子禁止
	ResourceManager& operator=(const ResourceManager&) = delete;

	/// @brief ムーブコンストラクタ禁止
	ResourceManager(ResourceManager&&) = delete;

	/// @brief ムーブ代入演算子禁止
	ResourceManager& operator=(ResourceManager&&) = delete;

	/// @brief アドレス取得演算子(参照演算子)禁止
	ResourceManager* operator&() = delete;

	/// @brief 内部読み込み処理の実体
	/// @param src リソース識別子
	/// @return 内部管理用ポインタ
	Resource* _Load(SRC src);

};
