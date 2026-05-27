#include "../../Pch.h"
#include"ResourceManager.h"

#include"../../Application.h"
#include"Resource.h"

// シングルトンのインスタンス初期化
ResourceManager* ResourceManager::instance_ = nullptr;

// スレッドセーフ用ミューテックスの定義
std::mutex ResourceManager::g_resMutex;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Initialize();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Initialize(void)
{
	Resource res;

	// スカイドームモデルの登録
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "SkyDome/skyDome.mv1");
	resourcesMap_.emplace(SRC::MODEL_SKY_DOME, res);

	// SE登録

	// キャンセル音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_60461.mp3");
	resourcesMap_.emplace(SRC::SE_CANCEL, res);

	// 選択音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "nc444082.wav");
	resourcesMap_.emplace(SRC::SE_SELECT, res);

	// 決定音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_130310");
	resourcesMap_.emplace(SRC::SE_PUSH, res);

	// チュートリアルシーン用リソースの初期化
	InitTutorial();

	// 設定のリソース初期化
	InitOption();
}

void ResourceManager::InitTitle(void)
{
	Resource res;
	
	// BGM

	// タイトル用BGM
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "AS_1456298.mp3");
	resourcesMap_.emplace(SRC::BGM_TITLE, res);

	// 動画
	
	// 背景動画（ムービー）
	res = Resource(Resource::TYPE::IMG, Application::PATH_MOVIE + "TitleHaikei.mp4");
	resourcesMap_.emplace(SRC::TITLE_MOVIE, res);

	// 画像

	// タイトルロゴ
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Tittle.png");
	resourcesMap_.emplace(SRC::TYTLE_LOGO, res);

}

void ResourceManager::InitGame(void)
{
	Resource res;

	// モデル

	// 地面モデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/stage.mv1");
	resourcesMap_.emplace(SRC::MODEL_GROUND, res);

	// エネミースポナー（敵の拠点）モデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/EnemyHouse.mv1");
	resourcesMap_.emplace(SRC::MODEL_ENEMYSPAWNER, res);

	// タワーモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/tower.mv1");
	resourcesMap_.emplace(SRC::MODEL_TOWER, res);

	// BGM

	// ゲームBGM登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "AS_814620.mp3");
	resourcesMap_.emplace(SRC::BGM_GAME, res);

	// 戦闘BGM登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "AS_1312744.mp3");
	resourcesMap_.emplace(SRC::BGM_FAITE, res);

	// プレイヤー関連リソースの初期化
	ResourcePlayer();

	// 敵関連リソースの初期化
	ResourceEnemy();
}

void ResourceManager::InitGameOver(void)
{
	Resource res;

	// 画像

	// ゲームオーバーロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "");
	resourcesMap_.emplace(SRC::GAMEOVER_LOGO, res);

}

void ResourceManager::InitGameClear(void)
{
	Resource res;

	// 画像

	// ゲームクリアロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "score.png");
	resourcesMap_.emplace(SRC::GAMECLERA_LOGO, res);

	// BGM

	// BGM登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "AS_1528820.mp3");
	resourcesMap_.emplace(SRC::BGM_SCORE, res);

	// 動画

	// 背景映像
	res = Resource(Resource::TYPE::IMG, Application::PATH_MOVIE + "BgMovie.mp4");
	resourcesMap_.emplace(SRC::BG_MOVIE, res);
}

void ResourceManager::InitTutorial(void)
{
	Resource res;

	// 画像

	// チュートリアル画像1
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Tutorial1.png");
	resourcesMap_.emplace(SRC::IMG_TUTORIAL_1, res);

	// チュートリアル画像2
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Tutorial2.png");
	resourcesMap_.emplace(SRC::IMG_TUTORIAL_2, res);

	// チュートリアル画像3
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Tutorial3.png");
	resourcesMap_.emplace(SRC::IMG_TUTORIAL_3, res);
}


void ResourceManager::InitOption(void)
{
	Resource res;
	
	// 画像

	// 設定背景画像
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Option.png");
	resourcesMap_.emplace(SRC::IMG_OPTION, res);

	// 操作画像
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Option1.png");
	resourcesMap_.emplace(SRC::IMG_OPTION_1, res);
}

void ResourceManager::ResourcePlayer(void)
{
	Resource res;

	// モデル

	// プレイヤーのモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/bouningen.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, res);

	// 剣のモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Sword.mv1");
	resourcesMap_.emplace(SRC::MODEL_SWORD, res);

	// グライダーのモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/glide.mv1");
	resourcesMap_.emplace(SRC::MODEL_GLIDER, res);

	// アニメーション

	// プレイヤーの待機アニメーション
	res = Resource(Resource::TYPE::ANIM, Application::PATH_ANIM + "Player/Unarmed Idle.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_IDEL, res);

	// プレイヤーの歩くアニメーション
	res = Resource(Resource::TYPE::ANIM, Application::PATH_ANIM + "Player/Walking.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_WALK, res);

	// プレイヤーの攻撃アニメーション
	res = Resource(Resource::TYPE::ANIM, Application::PATH_ANIM + "Player/Attack.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_ATTACK, res);

	// プレイヤーのジャンプアニメーション
	res = Resource(Resource::TYPE::ANIM, Application::PATH_ANIM + "Player/Jumping.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_JAMP, res);

	// プレイヤーのグライドアニメーション
	res = Resource(Resource::TYPE::ANIM, Application::PATH_ANIM + "Player/Victory.mv1");
	resourcesMap_.emplace(SRC::ANIM_PLAYER_GLIDE, res);

	// エフェクト

	// 火攻撃のエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "FireAttack/patch_stElmo_area.efkproj");
	resourcesMap_.emplace(SRC::EFFECT_FIRE, res);

	// 水攻撃のエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "WaterAttack/MagicWater.efkproj");
	resourcesMap_.emplace(SRC::EFFECT_WATER, res);

	// レベルアップのエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "LevelUp/levelUp.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_LEVER_UP, res);

	// 画像

	// 火のスキルアイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "fire.png");
	resourcesMap_.emplace(SRC::IMG_FIRE_SUKILL, res);

	// 火のスキルアイコン(クールダウン中)
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "fireCool.png");
	resourcesMap_.emplace(SRC::IMG_FIRE_SUKILL_CD, res);

	// 水のスキルアイコン
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "water.png");
	resourcesMap_.emplace(SRC::IMG_WATER_SUKILL, res);

	// 水のスキルアイコン(クールダウン中)
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "waterCool.png");
	resourcesMap_.emplace(SRC::IMG_WATER_SUKILL_CD, res);

	// 効果音

	// 火攻撃音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_821624.mp3");
	resourcesMap_.emplace(SRC::SE_FIRE, res);

	// 水攻撃音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_1443393.mp3");
	resourcesMap_.emplace(SRC::SE_WATER, res);

	// レベルアップ音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "Level-up_VR.mp3");
	resourcesMap_.emplace(SRC::SE_LEVER_UP, res);

	//　斬撃音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_153419.mp3");
	resourcesMap_.emplace(SRC::SE_SLASH, res);

	// グライダー音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "kazekirionn.mp3");
	resourcesMap_.emplace(SRC::SE_GLIDER, res);
}

void ResourceManager::ResourceEnemy(void)
{
	Resource res;

	// モデル

	// エネミーモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Enemy/sulim.mv1");
	resourcesMap_.emplace(SRC::MODEL_SLIME, res);

	// エフェクト

	// 凍結エフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "Freeze/aice.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_FREEZE, res);

	// 爆発エフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "BlastHit/BlastHit.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_BLAST, res);

	// やけどエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "fire/fire.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_FIRE_BURN, res);

	// 湿潤エフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "water/water.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_WET, res);

	// 硝酸アンモニウムエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "anmonium/anmoniumu.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_AMMONIUM, res);

	// 効果音

	// 爆発音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_1496191.mp3");
	resourcesMap_.emplace(SRC::SE_BLAST, res);

	// 凍結音
	res = Resource(Resource::TYPE::SOUND, Application::PATH_SE + "AS_1091522.mp3");
	resourcesMap_.emplace(SRC::SE_FREEZE, res);

}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		// リソース解放
		p.second->Release(); 
		delete p.second;     
	}

	// ロード済みリソースマップをクリア
	loadedMap_.clear();     

	// 登録済みリソースマップをクリア
	resourcesMap_.clear();  
}

void ResourceManager:: DestroyInstance(void)
{
	Release();        // リソース解放
	delete instance_; // インスタンス削除
}

Resource ResourceManager::Load(SRC src)
{
	std::lock_guard<std::mutex> lock(g_resMutex);

	Resource* res = _Load(src);
	if (res == nullptr)
	{
		// 空のリソースを返す
		return Resource(); 
	}

	// コピーして返す
	return *res; 
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	std::lock_guard<std::mutex> lock(g_resMutex);

	Resource* res = _Load(src);
	if (!res || res->handleId_ == -1)
	{
		return -1;
	}

	// オリジナルのモデルハンドルから複製
	int duId = MV1DuplicateModel(res->handleId_);
	res->duplicateModelIds_.push_back(duId);

	return duId;
}

int ResourceManager::GetHandle(SRC src)
{
	auto it = resourcesMap_.find(src);

	if (it == resourcesMap_.end()) { return -1; }

	// 内部でロード状況を確認してハンドルを返す
	return it->second.GetHandle();
}

ResourceManager::ResourceManager(void)
{
}

Resource* ResourceManager::_Load(SRC src)
{
	// すでに読み込み済みか確認
	auto itLoaded = loadedMap_.find(src);

	if (itLoaded != loadedMap_.end())
	{
		return itLoaded->second;
	}

	// 登録済みリソースか確認
	auto itRes = resourcesMap_.find(src);
	if (itRes == resourcesMap_.end())
	{
		return nullptr;
	}

	// リソース読み込み（handleId_ がセットされる）
	itRes->second.Load();

	// loadedMap_ にコピーして保持
	Resource* newRes = new Resource(itRes->second);

	// ここで loadedMap に追加
	loadedMap_.emplace(src, newRes);

	return newRes;
}
