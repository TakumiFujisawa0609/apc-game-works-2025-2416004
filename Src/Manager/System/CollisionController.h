#pragma once
#include "../../Collider/ColliderBase.h"

class UnitBase;

// 衝突結果の情報
struct CollisionInfo
{
    const ColliderBase* myCollider;      // 自分のコライダ
    const ColliderBase* hitCollider;     // 衝突相手のコライダ
    VECTOR hitPosition;                  // 衝突位置
    VECTOR hitNormal;                    // 衝突面の法線
    float penetration;                   // めり込み量
    bool isValid;                        // 衝突が有効か
};

// 衝突時のコールバック関数型
using CollisionCallback = std::function<void(const CollisionInfo&)>;

// 全体の衝突を管理するコントローラー
class CollisionController
{
public:
    // シングルトンインスタンスの生成
    static void CreateInstance(void);

    // シングルトンインスタンスの取得
    static CollisionController& GetInstance(void);

    // シングルトンインスタンスの削除
    static void Destroy(void);

    // 初期化
    void Init(void);

    // 更新（全ての衝突判定を実行）
    void Update(void);

    // ユニットの登録
    void RegisterUnit(UnitBase* actor);

    // ユニットの登録解除
    void UnregisterUnit(UnitBase* actor);

    // 全ユニットのクリア
    void Clear(void);

    // 距離カリングの有効/無効
    void SetDistanceCulling(bool enable) { enableDistanceCulling_ = enable; }

    // カリング距離の設定
    void SetCullingDistance(float distance) { cullingDistance_ = distance; }

    // 2つのコライダ間の衝突判定
    bool CheckCollision(const ColliderBase* col1, const ColliderBase* col2, CollisionInfo& outInfo);

private:
    // コンストラクタ
    CollisionController(void);

    // デストラクタ
    ~CollisionController(void);

    // コピー禁止
    CollisionController(const CollisionController&) = delete;
    CollisionController& operator=(const CollisionController&) = delete;

    // ムーブ禁止
    CollisionController(CollisionController&&) = delete;
    CollisionController& operator=(CollisionController&&) = delete;

    // シングルトンインスタンス
    static CollisionController* instance_;

    // 登録されたユニット
    std::vector<UnitBase*> actors_;

    // 距離カリングの有効/無効
    bool enableDistanceCulling_;

    // カリング距離
    float cullingDistance_;

    // 更新頻度制御用のタイマー
    float updateTimer_;

    // 更新間隔（秒）
    static constexpr float UPDATE_INTERVAL = 0.016f;

    // デフォルトのカリング距離
    static constexpr float DEFAULT_CULLING_DISTANCE = 1500.0f;

    // コライダペアの更新処理（★追加）
    void UpdateCollisionPairs(void);

    // 線分とモデルの衝突判定
    bool CheckLineVsModel(const ColliderBase* lineCol, const ColliderBase* modelCol, CollisionInfo& outInfo);

    // 球体同士の衝突判定
    bool CheckSphereVsSphere(const ColliderBase* sphere1, const ColliderBase* sphere2, CollisionInfo& outInfo);

    // 球体とカプセルの衝突判定
    bool CheckSphereVsCapsule(const ColliderBase* sphere, const ColliderBase* capsule, CollisionInfo& outInfo);

    // 衝突可能かどうかの判定
    bool CanCollide(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const;

    // 距離カリングのチェック
    bool IsInCullingRange(const VECTOR& pos1, const VECTOR& pos2) const;
};