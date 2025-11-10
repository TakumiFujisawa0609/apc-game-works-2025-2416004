#pragma once

// 消灯管理クラス
class CollisionManager
{
public:

	// 当たり判定タイプ
	enum class COLLISION_TYPE
	{
		SPHERE,      //球体

		BOX,         //直方体

		MESH,        //メッシュ

		CAPSULE,     //カプセル

		SPHERE_PUSH, //球体(押し出し)

		BOX_PUSH,    //直方体(押し出し)

		MESH_PUSH,   //メッシュ(押し出し)

		CAPSULE_PUSH //カプセル(押し出し)
		
	};

	// タグタイプ
	enum class TAG_TYPE
	{
		NONE,        //無し

		PLAYER,      //プレイヤー

		ENEMY,       //エネミー

		WALL,        //壁

		ITEM,        //アイテム

		CAMERA,      //カメラ

		GROUND,      //地面
	};

	// 当たり判定情報
	struct CollisionObject
	{
		std::shared_ptr<void> owner;            //所持者

		VECTOR* posPtr;                         //座標

		float radius = 0.0f;                    //半径(球体)

		VECTOR min = { 0.0f, 0.0f, 0.0f };      //最小座標(直方体)

		VECTOR max = { 0.0f, 0.0f, 0.0f };      //最大座標(直方体)

		int modelId = -1;					    //モデルID(メッシュ)

		COLLISION_TYPE type;                    //当たり判定タイプ

		bool pushEnabled = false;               //押し出し判定

		TAG_TYPE tag = TAG_TYPE::NONE;          // タグ

		VECTOR center = { 0.0f, 0.0f, 0.0f };   //中心座標(メッシュ)

		float radiusBound = 0.0f;               //境界半径(メッシュ)

		VECTOR capStart = { 0.0f, 0.0f, 0.0f }; //カプセルの開始座標

		VECTOR capEnd = { 0.0f, 0.0f, 0.0f };   //カプセルの終了座標

		float capRadius = 0.0f;                 //カプセルの半径
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
	void RegisterSphere(std::shared_ptr<void> owner, VECTOR* pos, float radius, TAG_TYPE tag, bool push = false);

	// BOXの登録
	void RegisterBox(std::shared_ptr<void> owner, VECTOR* pos, VECTOR min, VECTOR max, TAG_TYPE tag, bool push = false);

	// カプセルの登録
	void RegistCapsule(std::shared_ptr<void> owner, VECTOR* pos, const VECTOR& start, const VECTOR& end, float radius, TAG_TYPE tag, bool push);

	// メッシュの登録
	void RegisterMesh(std::shared_ptr<void> owner, int modelId, TAG_TYPE tag, bool push = false);

	// 地面用のメッシュ登録
	void RegisterMeshTile(std::shared_ptr<void> owner, int modelId, VECTOR pos, float tileSize);

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