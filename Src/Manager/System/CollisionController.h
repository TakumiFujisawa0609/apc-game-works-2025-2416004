#pragma once

#include <vector>
#include <functional>

#include "../../Collider/ColliderBase.h"

class UnitBase;

/// @brief 衝突結果の詳細情報を保持する構造体
struct CollisionInfo
{
    const ColliderBase* myCollider;      // 自分のコライダ
    const ColliderBase* hitCollider;     // 衝突相手のコライダ
    VECTOR hitPosition;                  // 衝突位置
    VECTOR hitNormal;                    // 衝突面の法線
    float penetration;                   // めり込み量
    bool isValid;                        // 衝突が有効か
};

/// @brief 衝突時に呼び出されるコールバック関数の型定義
using CollisionCallback = std::function<void(const CollisionInfo&)>;

/// @brief ゲーム内の全ユニット間の衝突判定を統括管理するクラス（シングルトン）
class CollisionController
{
public:

    /// @brief シングルトンインスタンスを生成する
    static void CreateInstance(void);

    /// @brief シングルトンインスタンスを取得する
    /// @return CollisionControllerの参照
    static CollisionController& GetInstance(void);

    /// @brief シングルトンインスタンスを削除する
    static void   DestroyInstance(void);

    /// @brief 初期化処理
    void Initialize(void);

    /// @brief 全ての登録済みユニット間で衝突判定を更新
    void Update(void);

    /// @brief 判定対象としてユニットを登録
    /// @param actor 登録するユニットのポインタ
    void RegisterUnit(UnitBase* actor);

    /// @brief 判定対象からユニットを登録解除
    /// @param actor 解除するユニットのポインタ
    void UnregisterUnit(UnitBase* actor);

    /// @brief 登録されている全ユニットをクリア
    void Clear(void);

    /// @brief 距離による判定のスキップ（カリング）の有効設定
    /// @param enable 有効にするならtrue
    void SetDistanceCulling(bool enable) { enableDistanceCulling_ = enable; }

    /// @brief カリングを適用する距離を設定
    /// @param distance 距離
    void SetCullingDistance(float distance) { cullingDistance_ = distance; }

    /// @brief 指定した2つのコライダ間で衝突があるか判定する
    /// @param col1 コライダ1
    /// @param col2 コライダ2
    /// @param outInfo 衝突情報の格納先
    /// @return 衝突していればtrue
    bool CheckCollision(const ColliderBase* col1, const ColliderBase* col2, CollisionInfo& outInfo);

private:

    // 判定用定数関連
    static constexpr int COLL_TARGET_ALL = -1;       // モデル全域判定用
    static constexpr float MATH_EPSILON = 0.0001f;   // ゼロ除算防止用
    static constexpr float MIN_PUSH_OUT = 0.5f;      // 最小押し出し量

    // 更新設定関連
    static constexpr float UPDATE_INTERVAL = 0.016f;           // 更新間隔（秒）
    static constexpr float DEFAULT_CULLING_DISTANCE = 1500.0f; // デフォルトのカリング距離

    // インスタンス・管理リスト関連
    static CollisionController* instance_; // シングルトンインスタンス
    std::vector<UnitBase*> actors_;        // 登録されたユニットのリスト

    // カリング関連
    bool enableDistanceCulling_; // 距離カリングの有効/無効
    float cullingDistance_;      // カリング距離

    // 更新頻度制御用のタイマー
    float updateTimer_;          

    /// @brief コンストラクタ（外部生成禁止）
    CollisionController(void);

    /// @brief デストラクタ
    ~CollisionController(void);

    /// @brief コピー禁止
    CollisionController(const CollisionController&) = delete;
    CollisionController& operator=(const CollisionController&) = delete;

    /// @brief ムーブ禁止
    CollisionController(CollisionController&&) = delete;
    CollisionController& operator=(CollisionController&&) = delete;

    /// @brief 登録された全コライダのペアを精査して更新
    void UpdateCollisionPairs(void);

    /// @brief 線分とモデルの衝突判定
    /// @param lineCol 線分コライダ
    /// @param modelCol モデルコライダ
    /// @param outInfo 衝突情報の格納先
    /// @return 衝突していればtrue
    bool CheckLineVsModel(const ColliderBase* lineCol, 
        const ColliderBase* modelCol, CollisionInfo& outInfo);

    /// @brief 球体同士の衝突判定
    /// @param sphereA 球体コライダA
    /// @param sphereB 球体コライダB
    /// @param outInfo 衝突情報の格納先
    /// @return 衝突していればtrue
    bool CheckSphereVsSphere(const ColliderBase* sphereA, 
        const ColliderBase* sphereB, CollisionInfo& outInfo);

    /// @brief 球体とカプセルの衝突判定
    /// @param sphere 球体コライダ
    /// @param capsule カプセルコライダ
    /// @param outInfo 衝突情報の格納先
    /// @return 衝突していればtrue
    bool CheckSphereVsCapsule(const ColliderBase* sphere, 
        const ColliderBase* capsule, CollisionInfo& outInfo);

    /// @brief カプセルとモデルの衝突判定
    /// @param capsuleCol カプセルコライダ
    /// @param modelCol モデルコライダ
    /// @param outInfo 衝突情報の格納先
    /// @return 衝突していればtrue
    bool CheckCapsuleVsModel(const ColliderBase* capsuleCol, 
        const ColliderBase* modelCol, CollisionInfo& outInfo);

    /// @brief タグの組み合わせに基づいて衝突可能かチェック
    /// @param tagA 自分のタグ
    /// @param tagB 相手のタグ
    /// @return 衝突可能ならtrue
    bool CanCollide(ColliderBase::TAG tagA, ColliderBase::TAG tagB) const;

    /// @brief 2点間の距離がカリング範囲内かチェック
    /// @param posA 座標A
    /// @param posB 座標B
    /// @return 範囲内（判定が必要）ならtrue
    bool IsInCullingRange(const VECTOR& posA, const VECTOR& posB) const;
};