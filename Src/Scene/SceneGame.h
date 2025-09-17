#pragma once

#include<memory>

#include "SceneBase.h"
#include "../Application.h"

class Grid;
class Player;
class InventoryUI;
class StageManager;
class DateTimeManager;
class ItemPickupUI;


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

private:

	//ステージ

	
	//プレイヤー
	std::shared_ptr<Player> player_;

	//アイテム入手UI
	std::shared_ptr<ItemPickupUI> itemPickup_;



	//グリッド線
	Grid* grid_;

	//インベントリUI
	InventoryUI* inventoryUI_;

	//ステージマネージャー
	StageManager* stageManager_;

	//日数
	DateTimeManager* dateTimeManager_;

	bool isStartFont_;

	//描画(デバッグ)
	void DrawDebug(void);

};

