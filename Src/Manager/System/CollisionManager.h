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

	// 当たり判定情報
	struct CollisionObject
	{
		VECTOR pos;                     //座標

		float radius = 0.0f;            //半径(球体)

		VECTOR min = VGet(0, 0, 0);     //最小座標(直方体)

		VECTOR max = VGet(0, 0, 0);     //最大座標(直方体)

		COLLISION_TYPE type;            //当たり判定タイプ

		bool pushEnabled = false;       //押し出し判定
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// シングルトン取得
	static CollisionManager& GetInstance(void);

	// インスタンスの破棄
	static void Destrpy(void);

	// 初期化
	void Init(void);

	// 登録
	void Register(const std::shared_ptr<CollisionObject>& obj);

	// 全削除
	void Clear(void);

	// 全オブジェクトの当たり判定・押し出し処理
	void Update(void);

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