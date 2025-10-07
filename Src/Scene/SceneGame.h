#pragma once

#include<memory>

#include "SceneBase.h"
#include "../Application.h"

class Grid;
class Player;
class Ground;


class SceneGame : public SceneBase
{
public:

	//コンストラクタ
	SceneGame(void);

	//デストラクタ
	~SceneGame(void) = default;

	//初期化処理
	void Init(void)override;

	//更新処理
	void Update(void)override;

	//描画処理
	void Draw(void)override;

	//解放処理
	void Release(void)override;

	//ロード
	void Load(void) override;

	//ロード完了
	void EndLoad(void) override;

private:

	//ステージ

	
	//プレイヤー
	std::shared_ptr<Player> player_;

	//ステージ
	

	//グリッド線
	Grid* grid_;

	bool isStartFont_;

	//描画(デバッグ)
	void DrawDebug(void);

};

