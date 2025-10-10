#pragma once

#include <vector>

#include "../Stage/Ground.h"

//ステージの地面生成マネージャー
class GroundManager
{
public:
	//地面の最大数
	static constexpr int TILE_COUNT = 100;

	//地面の大きさ
	static constexpr float TILE_SIZE = 100.0f;

	//コンストラクタ
	GroundManager(void);

	//デストラクタ
	~GroundManager(void);

	//読み込み処理
	void Load(void);

	//初期化
	void Init(void);

	//更新処理
	void Update(void);

	//描画処理
	void Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir);

	//解放処理
	void Release(void);

private:

	//地面部品
	std::vector<Ground> grounds_;

	//元モデル
	int baseModelId_;

	//ロード済みか判定
	bool isLoaded_;
};

