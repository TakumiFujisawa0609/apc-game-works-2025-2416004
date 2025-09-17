#pragma once

#include <DxLib.h>
#include <vector>
#include <memory>

#include "../Interact/HitObject.h"
#include "../NullHitObject.h"

class Player;

class CollisionManager
{
public:
	// 表示開始距離
	static constexpr float SHOW_RADIUS = 50.0f; 
	
	// 非表示開始距離（少し大きく）
	static constexpr float HIDE_RADIUS = 60.0f; 

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static CollisionManager& GetInstance(void);

	// 初期化処理
	void Init(void);


	// 登録(ポインタ渡し)
	void Register(const std::shared_ptr<HitObject>& obj);

	// 全てクリア
	void Clear(void);

	// プレイヤーとの当たり判定チェック(UI表示)
	void CheckHitWithPlayer(Player* player, VECTOR& playerPos, float playerRadius, const VECTOR& playerMin, const VECTOR& playerMax);



	// リソースの解放
	void Destroy(void);

private:

	// 静的インスタンス
	static CollisionManager* instance_;

	// コンストラクタ
	CollisionManager(void) = default;

	// デストラクタ
	~CollisionManager(void) = default;

	std::vector<std::shared_ptr<HitObject>> hitObjects_;

	NullHitObject nullObject_;

	VECTOR objPos_;

	float objRadius_;

	VECTOR objMin_;

	VECTOR objMax_;

	float objSizeX_;
	
	float objSizeZ_;

	VECTOR diff_;

	HitObject* SafeGet(HitObject* obj);

	//判定方式別の処理
	void CheckHitSphere(std::shared_ptr<HitObject> obj, VECTOR& playerPos, float playerRadius);
	void CheckHitAABB(std::shared_ptr<HitObject> obj, Player* player, VECTOR& playerPos, const VECTOR& playerMin, const VECTOR& playerMax);
};
