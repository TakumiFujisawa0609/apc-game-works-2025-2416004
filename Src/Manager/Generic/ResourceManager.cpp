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
	Resource res;
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
}
// タイトルシーン用リソースの初期化
void ResourceManager::InitTitle(void)
{
	Resource res;

	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "AS_1456298.mp3");
	resourcesMap_.emplace(SRC::BGM_TITLE, res);

	res = Resource(Resource::TYPE::IMG, Application::PATH_MOVIE + "TitleHaikei.mp4");
	resourcesMap_.emplace(SRC::TITLE_MOVIE, res);

	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "Tittle.png");
	resourcesMap_.emplace(SRC::TYTLE_LOGO, res);

}

// ゲームシーン用リソースの初期化
void ResourceManager::InitGame(void)
{
	Resource res;
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/stage.mv1");
	resourcesMap_.emplace(SRC::MODEL_GROUND, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/EnemyHouse.mv1");
	resourcesMap_.emplace(SRC::MODEL_ENEMYSPAWNER, res);

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Stage/tower.mv1");
	resourcesMap_.emplace(SRC::MODEL_TOWER, res);

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

// ゲームオーバーシーン用リソースの初期化
void ResourceManager::InitGameOver(void)
{
	Resource res;

	// ゲームオーバーロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "");
	resourcesMap_.emplace(SRC::GAMEOVER_LOGO, res);

}

// ゲームクリアシーン用リソースの初期化
void ResourceManager::InitGameClear(void)
{
	Resource res;

	// ゲームクリアロゴ画像を登録
	res = Resource(Resource::TYPE::IMG, Application::PATH_IMAGE + "score.png");
	resourcesMap_.emplace(SRC::GAMECLERA_LOGO, res);

	// BGM登録
	res = Resource(Resource::TYPE::SOUND, Application::PATH_BGM + "AS_1528820.mp3");
	resourcesMap_.emplace(SRC::BGM_SCORE, res);

	// 背景映像
	res = Resource(Resource::TYPE::IMG, Application::PATH_MOVIE + "BgMovie.mp4");
	resourcesMap_.emplace(SRC::BG_MOVIE, res);
}

// チュートリアルシーン用リソースの初期化
void ResourceManager::InitTutorial(void)
{
	Resource res;

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


// プレイヤー用リソース初期化
void ResourceManager::ResourcePlayer(void)
{
	Resource res;

	// プレイヤーのモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/bouningen.mv1");
	resourcesMap_.emplace(SRC::MODEL_PLAYER, res);

	// 剣のモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/Sword.mv1");
	resourcesMap_.emplace(SRC::MODEL_SWORD, res);

	// グライダーのモデル
	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Player/glide.mv1");
	resourcesMap_.emplace(SRC::MODEL_GLIDER, res);

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

	// 火攻撃のエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "FireAttack/patch_stElmo_area.efkproj");
	resourcesMap_.emplace(SRC::EFFECT_FIRE, res);

	// 水攻撃のエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "WaterAttack/MagicWater.efkproj");
	resourcesMap_.emplace(SRC::EFFECT_WATER, res);

	// レベルアップのエフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "LevelUp/levelUp.efkefc");
	resourcesMap_.emplace(SRC::EFFECT_LEVER_UP, res);

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
}

// 敵用リソース初期化
void ResourceManager::ResourceEnemy(void)
{
	Resource res;

	res = Resource(Resource::TYPE::MODEL, Application::PATH_MODEL + "Enemy/sulim.mv1");
	resourcesMap_.emplace(SRC::MODEL_SLIME, res);

	// 凍結エフェクト
	res = Resource(Resource::TYPE::EFFEKSEER, Application::PATH_EFFECT + "")
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
	if (!res || res->handleId_ == -1)
		return -1;

	int duId = MV1DuplicateModel(res->handleId_);
	res->duplicateModelIds_.push_back(duId);

	return duId;
}

int ResourceManager::GetHandle(SRC src)
{
	auto it = resourcesMap_.find(src);

	if (it == resourcesMap_.end()) { return -1; }

	// モデルならロードしてなければロード
	return it->second.GetHandle();
}

// コンストラクタ
ResourceManager::ResourceManager(void)
{
}

// 内部リソース読み込み処理
Resource* ResourceManager::_Load(SRC src)
{
	// すでに読み込み済みか確認
	auto itLoaded = loadedMap_.find(src);
	if (itLoaded != loadedMap_.end())
		return itLoaded->second;

	// 登録済みリソースか確認
	auto itRes = resourcesMap_.find(src);
	if (itRes == resourcesMap_.end())
		return nullptr;

	// リソース読み込み（handleId_ がセットされる）
	itRes->second.Load();

	// loadedMap_ にコピーして保持
	Resource* newRes = new Resource(itRes->second);

	// ここで loadedMap に追加
	loadedMap_.emplace(src, newRes);

	return newRes;
}
