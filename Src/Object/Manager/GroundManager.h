#pragma once

#include <vector>

#include "../Stage/Ground.h"

//ステージの地面生成マネージャー
class GroundManager
{
public:
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
	void Draw(void);

	//解放処理
	void Release(void);

private:

	//地面部品
	std::vector<Ground> grounds_;

	//元モデル
	int baseModelId_;
};

