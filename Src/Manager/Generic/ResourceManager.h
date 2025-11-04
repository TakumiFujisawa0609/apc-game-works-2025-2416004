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

		// モデル
		MODEL_PLAYER,			//プレイヤーモデル
		MODEL_GROUND,			//ステージ地面のモデル

		// サウンド

		// エフェクト

		// アニメーション
		ANIM_PLAYER_IDEL
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

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	ResourceManager(void);

	//デストラクタも同様
	~ResourceManager(void) = default;

	//内部ロード
	Resource* _Load(SRC src);

};
