#include"ResourceManager.h"

#include<DxLib.h>

#include"../../Application.h"
#include"Resource.h"

// シングルトンのインスタンス初期化
ResourceManager* ResourceManager::instance_ = nullptr;

// インスタンス生成（初回のみ）＋初期化呼び出し
void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

// インスタンス参照を返す
ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

// 共通初期化処理（今は空）
void ResourceManager::Init(void)
{
	
}
// タイトルシーン用リソースの初期化
void ResourceManager::InitTitle(void)
{
	Resource res;
	// 操作説明画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/sousa.png");
	resourcesMap_.emplace(SRC::OPERATION, res);

	//遊び方画像1
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/mokuhyou.png");
	resourcesMap_.emplace(SRC::PLAY_GUIDE, res);

	//遊び方画像2
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/renkint.png");
	resourcesMap_.emplace(SRC::PLAY_GUIDE2, res);

	// タイトルロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/titleRog.png");
	resourcesMap_.emplace(SRC::TYTLE_LOGO, res);

	//遊び方画像(アトリエ)
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/atrieT.png");
	resourcesMap_.emplace(SRC::ATREA, res);

	//遊び方画像(ギルド)
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/girudo.png");
	resourcesMap_.emplace(SRC::GUILD, res);

	//遊び方画像(ガーデン)
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/gadenT.png");
	resourcesMap_.emplace(SRC::GARDEN, res);

	// タイトルBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "nc49298.mp3");
	resourcesMap_.emplace(SRC::BGM_TITLE, res);

	// 決定音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "se_pikon19.mp3");
	resourcesMap_.emplace(SRC::SE_PUSH, res);

	// キャンセル音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "nc298207.mp3");
	resourcesMap_.emplace(SRC::SE_CANCEL, res);

	// カーソル移動音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "Cursormovementsound.mp3");
	resourcesMap_.emplace(SRC::SE_SELECT, res);
}

// ゲームシーン用リソースの初期化
void ResourceManager::InitGame(void)
{
	Resource res;
	
	// ゲームBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "");
	resourcesMap_.emplace(SRC::BGM_GAME, res);

	// カーソル移動音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "Cursormovementsound.mp3");
	resourcesMap_.emplace(SRC::SE_SELECT, res);

	// 決定音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "se_pikon1.mp3");
	resourcesMap_.emplace(SRC::SE_PUSH, res);

	// キャンセル音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "nc298207.mp3");
	resourcesMap_.emplace(SRC::SE_CANCEL, res);

	// ダメージ音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "");
	resourcesMap_.emplace(SRC::SE_DAMAGE, res);

	// アイテム取得音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "");
	resourcesMap_.emplace(SRC::SE_GET, res);

	//フレーム
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/DHUI.png");
	resourcesMap_.emplace(SRC::UI_FRAME, res);

	//朝アイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/moningUI.png");
	resourcesMap_.emplace(SRC::MORNING, res);

	//昼アイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/HirumaUI.png");
	resourcesMap_.emplace(SRC::DAY, res);

	//夕方アイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/HiruUI.png");
	resourcesMap_.emplace(SRC::EVENING, res);

	//夜アイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/nightUI.png");
	resourcesMap_.emplace(SRC::NIGHT, res);

	//夜アイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/Maney.png");
	resourcesMap_.emplace(SRC::MANEY, res);

	//血袋
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/Bloodinaleatherbag.png");
	resourcesMap_.emplace(SRC::BLOOD_BAG, res);

	//辰砂
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/cinnabar.png");
	resourcesMap_.emplace(SRC::CINNABAR, res);

	//賢者の石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "UI/PhilosophersStone.png");
	resourcesMap_.emplace(SRC::CINNABAR, res);

	// プレイヤー関連リソースの初期化
	ResourcePlayer();

	// 敵関連リソースの初期化
	ResourceEnemy();

	//アトリエ関連リソースの初期化
	ResourceAtelier();

	//ギルドリソースの初期化
	ResourceGuild();

	//庭のリソースを初期化
	ResourceGarden();
}

// ゲームオーバーシーン用リソースの初期化
void ResourceManager::InitGameOver(void)
{
	Resource res;

	// ゲームオーバーロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "");
	resourcesMap_.emplace(SRC::GAMEOVER_LOGO, res);

	// ゲームオーバーBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "");
	resourcesMap_.emplace(SRC::BGM_GAMEOVER, res);

	// 決定音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "");
	resourcesMap_.emplace(SRC::SE_PUSH, res);
}

// ゲームクリアシーン用リソースの初期化
void ResourceManager::InitGameClear(void)
{
	Resource res;

	// ゲームクリアロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "");
	resourcesMap_.emplace(SRC::GAMECLERA_LOGO, res);

	// ゲームクリアBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "");
	resourcesMap_.emplace(SRC::BGM_GAMECLEAR, res);

	// 決定音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "");
	resourcesMap_.emplace(SRC::SE_PUSH, res);
}

void ResourceManager::ResourceAtelier(void)
{
	Resource res;
	//掲示板オブジェクトの登録
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/Atelier/hondana.mv1");
	resourcesMap_.emplace(SRC::BOOKS_HELF, res);

	//錬金釜
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/Atelier/renkingama.mv1");
	resourcesMap_.emplace(SRC::ALCHEMYPOT, res);

	//壁
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/Atelier/AtelierStage.mv1");
	resourcesMap_.emplace(SRC::STAGE_ATELIER, res);

	//アイテムインベントリ
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/Atelier/Box.mv1");
	resourcesMap_.emplace(SRC::BOX, res);

	//薬草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Greenherb.png");
	resourcesMap_.emplace(SRC::HERB, res);
	
	//解毒草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Purpleherb.png");
	resourcesMap_.emplace(SRC::ANTIDOTE_HERB, res);

	//魔力草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/BlueHerb.png");
	resourcesMap_.emplace(SRC::MAGIC_FLOWER, res);

	//麻痺草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Yellowherb.png");
	resourcesMap_.emplace(SRC::PARALYSIS_HERB, res);

	//風走草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Lightblueherb.png");
	resourcesMap_.emplace(SRC::GALE_HERB, res);

	//鬼力草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Redherb.png");
	resourcesMap_.emplace(SRC::DEMON_POWER_HERB, res);

	//硬体草
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Orangeherb.png");
	resourcesMap_.emplace(SRC::HARD_BODY_HERB, res);

	//回復ポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/GreenTriangularPotion.png");
	resourcesMap_.emplace(SRC::RECOVERY_POTION, res);

	//解毒ポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/PurpleTriangularPotion.png");
	resourcesMap_.emplace(SRC::ANTIDOTE_POTION, res);

	//解麻痺ポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/YellowPotion.png");
	resourcesMap_.emplace(SRC::ANTIPARALYSIS_POTION, res);

	//魔力ポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/BlueTriangularPotion.png");
	resourcesMap_.emplace(SRC::MAGIC_POTION, res);

	//俊敏ポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/LightbluePotion.png");
	resourcesMap_.emplace(SRC::SPEED_POTION, res);

	//力のポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/RedTriangularPotion.png");
	resourcesMap_.emplace(SRC::POWER_POTION, res);

	//防御ポーション
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/OrangePotion.png");
	resourcesMap_.emplace(SRC::DEFENSE_POTION, res);
	

	//水
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Water.png");
	resourcesMap_.emplace(SRC::WATER, res);

	//鉄鉱石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Iron.png");
	resourcesMap_.emplace(SRC::IRON_ORE, res);

	//火の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/RedOre.png");
	resourcesMap_.emplace(SRC::FIRE_MAGIC_STONE, res);

	//水の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/BlueOre.png");
	resourcesMap_.emplace(SRC::WATER_MAGIC_STONE, res);

	//風の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/GreenOre.png");
	resourcesMap_.emplace(SRC::WIND_MAGIC_STONE, res);

	//土の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/OrangeOre.png");
	resourcesMap_.emplace(SRC::EARTH_MAGIC_STONE, res);

	//氷の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/WaterOre.png");
	resourcesMap_.emplace(SRC::ICE_MAGIC_STONE, res);

	//光の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/YellowOre.png");
	resourcesMap_.emplace(SRC::LIGHT_MAGIC_STONE, res);

	//闇の魔石
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/PurpleOre.png");
	resourcesMap_.emplace(SRC::DARK_MAGIC_STONE, res);

	//剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Sword.png");
	resourcesMap_.emplace(SRC::SWORD, res);

	//火の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/RedSword.png");
	resourcesMap_.emplace(SRC::FIRE_SORD, res);

	//水の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/BlueSword.png");
	resourcesMap_.emplace(SRC::WATER_SWORD, res);

	//風の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/GreenSword.png");
	resourcesMap_.emplace(SRC::WIND_SWORD, res);

	//土の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/OrangeSword.png");
	resourcesMap_.emplace(SRC::EARTH_SWORD, res);

	//氷の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/WaterSword.png");
	resourcesMap_.emplace(SRC::ICE_SWORD, res);

	//光の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/YellowSword.png");
	resourcesMap_.emplace(SRC::LIGHT_SWORD, res);

	//闇の剣
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/PurpleSword.png");
	resourcesMap_.emplace(SRC::DARK_SWORD, res);

	//杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Cane.png");
	resourcesMap_.emplace(SRC::WAND, res);

	//火の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/RedWand.png");
	resourcesMap_.emplace(SRC::FIRE_WAND, res);

	//水の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/BlueWand.png");
	resourcesMap_.emplace(SRC::WATER_WAND, res);

	//風の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/GreenWand.png");
	resourcesMap_.emplace(SRC::WIND_WAND, res);

	//土の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/OrangeWand.png");
	resourcesMap_.emplace(SRC::EARTH_WAND, res);

	//氷の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/WaterWand.png");
	resourcesMap_.emplace(SRC::ICE_WAND, res);

	//光の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/YellowWand.png");
	resourcesMap_.emplace(SRC::LIGHT_WAND, res);

	//闇の杖
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/PurpleWand.png");
	resourcesMap_.emplace(SRC::DARK_WAND, res);

	//効果音
	// 錬金音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "renkin.mp3");
	resourcesMap_.emplace(SRC::SE_ALCHEMY, res);

	// 錬金失敗音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "nc46976.mp3");
	resourcesMap_.emplace(SRC::SE_ALCHEMY_FAIL, res);

	// 錬金成功音を登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "fanfare3.mp3");
	resourcesMap_.emplace(SRC::SE_ALCHEMY_SUCCESS, res);

	//BGM
	// アトリエのBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "m2.mp3");
	resourcesMap_.emplace(SRC::BGM_ATELIER, res);

	//エフェクト
	//錬金成功エフェクトを登録
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "Simple_Sprite_FixedYAxis.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_ALCHEMY, res);

	//錬金失敗エフェクトを登録
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "BlastHit.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_BLAST, res);
	
}

//ギルドで使うリソース
void ResourceManager::ResourceGuild(void)
{
	Resource res;

	

	//掲示板オブジェクトの登録
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/GuildObject/BulletinBoard.mv1");
	resourcesMap_.emplace(SRC::BULLETIN_BOARD, res);

	//掲示板の画像の登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "/GuildStage/BulletinBoard.png");
	resourcesMap_.emplace(SRC::IMAGE_BOARD, res);

	//カウンタ-オブジェクトの登録
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/GuildObject/counter.mv1");
	resourcesMap_.emplace(SRC::COUNTER, res);

	//カウンタ-オブジェクトの登録
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "/GuildObject/TableSet.mv1");
	resourcesMap_.emplace(SRC::TABLE_SET, res);

	//依頼書の画像の登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "/GuildStage/Request.png");
	resourcesMap_.emplace(SRC::IMAGE_REQUEST, res);
	
	//受付嬢のモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GuildObject/uketuke.mv1");
	resourcesMap_.emplace(SRC::RECEPTIONIST, res);

	//種
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "item/Seed.png");
	resourcesMap_.emplace(SRC::SEED, res);
	
	//BGM
	// ゲームBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "maou_bgm_ethnic10.mp3");
	resourcesMap_.emplace(SRC::BGM_GUILD, res);
}

void ResourceManager::ResourceGarden(void)
{
	Resource res;
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/Block_Dirt.mv1");
	resourcesMap_.emplace(SRC::BLOCK_DIRT, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/Block_Grass.mv1");
	resourcesMap_.emplace(SRC::BLOCK_GFRASS, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/Block_Lood.mv1");
	resourcesMap_.emplace(SRC::BLOCK_LOOD, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/Growing.mv1");
	resourcesMap_.emplace(SRC::GROWING_MODEL, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/Seed.mv1");
	resourcesMap_.emplace(SRC::SEED_MODEL , res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/mature.mv1");
	resourcesMap_.emplace(SRC::MATURE_MODEL, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/ore.mv1");
	resourcesMap_.emplace(SRC::ORE_MODEL, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/fence.mv1");
	resourcesMap_.emplace(SRC::FENCE_MODEL, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/well.mv1");
	resourcesMap_.emplace(SRC::WELL_MODEL, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "GardenObject/Dowa.mv1");
	resourcesMap_.emplace(SRC::DOWA_MODEL, res);

	//BGM
	// 庭の朝昼のBGMを登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "nc86886.mp3");
	resourcesMap_.emplace(SRC::BGM_GARDEN_DAY, res);

	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "nc141198_plant_girl.wav");
	resourcesMap_.emplace(SRC::BGM_GARDEN_NIGHT, res);
}

// プレイヤー用リソース初期化（未実装）
void ResourceManager::ResourcePlayer(void)
{
	Resource res;
	res = res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "player/player.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, res);
}

// 敵用リソース初期化
void ResourceManager::ResourceEnemy(void)
{
	Resource res;
}

// 全リソースの解放処理
void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second->Release(); // リソース解放
		delete p.second;     // メモリ解放
	}

	loadedMap_.clear();     // ロード済みリソースマップをクリア
	resourcesMap_.clear();  // 登録済みリソースマップをクリア
}

// インスタンス破棄処理
void ResourceManager::Destroy(void)
{
	Release();        // リソース解放
	delete instance_; // インスタンス削除
}

// リソースの読み込み（読み込み済みなら再利用）
Resource ResourceManager::Load(SRC src)
{
	Resource* res = _Load(src);
	if (res == nullptr)
	{
		return Resource(); // 空のリソースを返す
	}
	return *res; // コピーして返す
}

// モデルの複製を行い、複製IDを返す
int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource* res = _Load(src);
	if (res == nullptr)
	{
		return -1; // 読み込み失敗
	}

	// モデル複製
	int duId = MV1DuplicateModel(res->handleId_);
	res->duplicateModelIds_.push_back(duId);

	return duId;
}

// コンストラクタ
ResourceManager::ResourceManager(void)
{
}

// 内部リソース読み込み処理
Resource* ResourceManager::_Load(SRC src)
{
	// すでに読み込み済みか確認
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return lPair->second;
	}

	// 登録済みリソースか確認
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return nullptr;
	}

	// リソース読み込み実行
	rPair->second.Load();

	// コピーを保持（読み込み済みマップに追加）
	Resource* ret = new Resource(rPair->second);
	loadedMap_.emplace(src, ret);

	return ret;
}
