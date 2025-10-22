#pragma once

#include <DxLib.h>
#include <vector>
#include <memory>
#include <algorithm>

// 消灯管理クラス
class CollisionManager
{
public:

	// 当たり判定タイプ
	enum class COLLISION_TYPE
	{
		SPHERE,      //球体

		BOX,         //直方体

		SPHERE_PUSH, //球体(押し出し)

		BOX_PUSH,    //直方体(押し出し)
	};

	// タグタイプ
	enum class TAG_TYPE
	{
		NONE,        //無し

		PLAYER,      //プレイヤー

		ENEMY,       //エネミー

		WALL,        //壁

		ITEM,        //アイテム
	};

	// 当たり判定情報
	struct CollisionObject
	{
		std::shared_ptr<void> owner;    //所持者

		std::weak_ptr<VECTOR> posPtr;   //座標

		float radius = 0.0f;            //半径(球体)

		VECTOR min = VGet(0, 0, 0);     //最小座標(直方体)

		VECTOR max = VGet(0, 0, 0);     //最大座標(直方体)

		COLLISION_TYPE type;            //当たり判定タイプ

		bool pushEnabled = false;       //押し出し判定

		TAG_TYPE tag = TAG_TYPE::NONE;  // タグ
	};

	

	// インスタンスの生成
	static void CreateInstance(void);

	// シングルトン取得
	static CollisionManager& GetInstance(void);

	// インスタンスの破棄
	static void Destroy(void);

	// 初期化
	void Init(void);

	// 球の登録
	void RegisterSphere(std::shared_ptr<void> owner, std::shared_ptr<VECTOR> pos, float radius, TAG_TYPE tag, bool push = false);

	// BOXの登録
	void RegisterBox(std::shared_ptr<void> owner, std::shared_ptr<VECTOR> pos, VECTOR min, VECTOR max, TAG_TYPE tag, bool push = false);

	// 全削除
	void Clear(void);

	// 全オブジェクトの当たり判定・押し出し処理
	void Update(void);

	// タグ同士で判定するかをチェック
	bool CanCollide(TAG_TYPE tagA, TAG_TYPE tagB) const;

private:

	// コンストラクタ
	CollisionManager(void) = default;

	// デストラクタ
	~CollisionManager(void) = default;

	//シングルトンインスタンスのコピー禁止
	static CollisionManager* instance_;

	//全ての当たり判定オブジェクト
	std::vector<std::shared_ptr<CollisionObject>> objects_;
};