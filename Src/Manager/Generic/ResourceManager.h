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

		//完成品アイテム
		RECOVERY_POTION,		//回復ポーション
		ANTIDOTE_POTION,		//解毒ポーション
		ANTIPARALYSIS_POTION,   //解麻痺ポーション
		MAGIC_POTION,			//魔力ポーション
		SPEED_POTION,           //俊敏ポーション
		POWER_POTION,           //力ポーション
		DEFENSE_POTION,         //防御ポーション
		GARBAGE,				//失敗の作品
		FIRE_SORD,              //火の剣
		WATER_SWORD,            //水の剣
		WIND_SWORD,             //風の剣
		EARTH_SWORD,            //土の剣
		ICE_SWORD,              //氷の剣
		LIGHT_SWORD,            //光の剣
		DARK_SWORD,             //闇の剣
		FIRE_WAND,              //火の杖
		WATER_WAND,             //水の杖
		WIND_WAND,              //風の杖
		EARTH_WAND,             //土の杖
		ICE_WAND,               //氷の杖
		LIGHT_WAND,             //光の杖
		DARK_WAND,              //闇の杖
		PHILOSOPHERS_STONE,     //賢者の石

		//材料
		HERB,					//薬草
		ANTIDOTE_HERB,			//解毒草
		MAGIC_FLOWER,			//魔力草
		PARALYSIS_HERB,         //麻痺草
		GALE_HERB,              //風走草
		DEMON_POWER_HERB,       //鬼力草
		HARD_BODY_HERB,         //硬体草
		WATER,					//水
		IRON_ORE,               //鉄鉱石
		FIRE_MAGIC_STONE,       //火の魔石
		WATER_MAGIC_STONE,      //水の魔石
		WIND_MAGIC_STONE,       //風の魔石
		EARTH_MAGIC_STONE,      //土の魔石
		ICE_MAGIC_STONE,        //氷の魔石
		LIGHT_MAGIC_STONE,      //光の魔石
		DARK_MAGIC_STONE,       //闇の魔石
		SWORD,                  //剣
		WAND,                   //杖
		BLOOD_BAG,              //血袋
		CINNABAR,               //辰砂

		//種子
		SEED,					//種

		//プレイー
		MODEL_PLAYER,			//プレイやー

		//ギルド関係
		BULLETIN_BOARD,			//掲示板
		IMAGE_BOARD,			//掲示板の画像
		IMAGE_REQUEST,			//依頼書
		TABLE_SET,				//テーブルセット
		COUNTER,				//カウンター
		RECEPTIONIST,			//受付嬢

		//アトリエ関係
		BOOKS_HELF,				//本棚
		ALCHEMYPOT,				//錬金釜
		STAGE_ATELIER,			//アトリエ全体
		BOX,					//アイテムインベントリ

		//庭関係
		BLOCK_DIRT,				//土ブロック
		BLOCK_GFRASS,			//草ブロック
		BLOCK_LOOD,             //道ブロック
		SEED_MODEL,				//発芽モデル
		GROWING_MODEL,			//成長中モデル
		MATURE_MODEL,			//成熟モデル
		ORE_MODEL,				//鉱石モデル
		FENCE_MODEL,			//フェンスモデル
		WELL_MODEL,				//井戸のモデル
		DOWA_MODEL,				//ドア

		//UI画像関連
		UI_FRAME,               //フレーム
		MORNING,				//朝のアイコン
		DAY,					//昼のアイコン
		EVENING,                //夕方のアイコン
		NIGHT,                  //夜のアイコン
		MANEY,					//お金のアイコン
		OPERATION,              //操作説明の画像
		PLAY_GUIDE,             //遊び方の画像1
		PLAY_GUIDE2,            //遊び方の画像2
		ATREA,
		GUILD,
		GARDEN,

		//ミニマップ関係
		MAP_FRAME,				//ミニマップ枠
		MAP_GROUND,				//ミニマップ枠内
		MAP_MASK,				//ミニマップマスク用
		MAP_PLAYER,				//ミニマップ用プレイヤー
		MAP_ENEMY,				//ミニマップ用敵

		//サウンド
		BGM_TITLE,				//タイトルBGM
		BGM_GAME,				//ゲームBGM
		BGM_GAMEOVER,			//ゲームオーバーBGM
		BGM_GAMECLEAR,			//ゲームクリアBGM
		BGM_GARDEN_DAY,         //庭の朝昼のBGM
		BGM_GARDEN_NIGHT,       //庭の夕方夜のBGM
		BGM_ATELIER,            //アトリエのBGM
		BGM_GUILD,              //ギルドのBGM

		SE_PUSH,				//ボタン決定音
		SE_SELECT,              //カーソル移動音
		SE_CANCEL,              //キャンセル音
		SE_DAMAGE,				//ダメージ音
		SE_GET,					//アイテム取得音
		SE_ALCHEMY,             //錬金音
		SE_ALCHEMY_FAIL,        //錬金失敗音	
		SE_ALCHEMY_SUCCESS,     //錬金成功音

		//エフェクト
		EFFECT_ALCHEMY,         //錬金エフェクト
		EFFECT_BLAST,          //爆発エフェクト
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

	//アトリエで使うリソース
	void ResourceAtelier(void);

	//ギルドで使うリソース
	void ResourceGuild(void);

	//庭で使うリソース
	void ResourceGarden(void);

	//プレイヤーが使うリソース
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

private:

	//静的インスタンス
	static ResourceManager* instance_;

	//リソース管理の対象
	std::map<SRC, Resource> resourcesMap_;

	//読み込み済みリソース
	std::map<SRC, Resource*> loadedMap_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);

	//デストラクタも同様
	~ResourceManager(void) = default;

	//内部ロード
	Resource* _Load(SRC src);

};
