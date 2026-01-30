#pragma once
#include<map>
#include<string>
#include"Resource.h"

class ResourceManager
{
public:

	//リソース名
	enum class SRC
	{
		//ここに保存しておきたい画像やモデル、音源などの名前を記す

		/*例*/
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

	//明示的にインスタンスを生成する
	static void CreateInstance(void);

	//静的インスタンスの取得
	static ResourceManager& GetInstance(void); 

	//初期化
	void Init(void);

	//タイトルで使うリソース初期化
	void InitTitle(void);

	//ゲームで使うリソース初期化
	void InitGame(void);

	//ゲームオーバーで使うリソース初期化
	void InitGameOver(void);

	//ゲームクリアで使うリソース初期化
	void InitGameClear(void);

	// チュートリアルで使うリソース初期化
	void InitTutorial(void);

	// 設定で使うリソース初期化
	void InitOption(void);

	//プレイヤーが使うリソース初期化
	void ResourcePlayer(void);

	//敵が使うリソース
	void ResourceEnemy(void);

	//解放(シーン切り替え時に一旦解放)
	void Release(void);

	//リソース完全破棄
	void Destroy(void);

	//リソースのロード
	Resource Load(SRC src);

	//リソースの複製ロード(モデル用)
	int LoadModelDuplicate(SRC src);

	// ハンドルを取得
	int GetHandle(SRC src);

private:

	//静的インスタンス
	static ResourceManager* instance_;

	//リソース管理の対象
	std::map<SRC, Resource> resourcesMap_;

	//読み込み済みリソース
	std::map<SRC, Resource*> loadedMap_;

	static std::mutex g_resMutex;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);

	//デストラクタも同様
	~ResourceManager(void) = default;

	// コピー禁止コンストラクタ
	ResourceManager(const ResourceManager&) = delete;

	// コピー代入演算子禁止
	ResourceManager& operator=(const ResourceManager&) = delete;

	// ムーブコンストラクタ禁止
	ResourceManager(ResourceManager&&) = delete;

	// ムーブ代入演算子禁止
	ResourceManager& operator=(ResourceManager&&) = delete;

	// アドレス取得演算子(参照演算子)禁止
	ResourceManager* operator&() = delete;

	//内部ロード
	Resource* _Load(SRC src);

};
