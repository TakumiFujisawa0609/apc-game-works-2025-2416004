#pragma once
#include <DxLib.h>
#include <memory>
#include <map>
#include <vector>
#include "Common/Transform.h"

class AnimationController;
class ColliderBase;
struct CollisionInfo;

/// @brief 全てのオブジェクト（プレイヤー・敵・オブジェクト等）の基底クラス
class UnitBase
{
public:
    // アニメーション定数関連
    enum class ANIM
    {
        NONE,
        IDEL,   // 待機
        WALK,   // 歩き
        ATTACK, // 攻撃
        JUMP,   // ジャンプ
        GLIDE   // 滑空
    };

    // 衝突判定種別関連
    enum class COLLIDER_TYPE
    {
        NONE = -1,
        LINE = 0,      // 線分（地面判定用）
        SPHERE,        // 球体（本体の当たり判定）
        CAPSULE,       // カプセル（攻撃・体全体）
        MODEL,         // モデル（地形）
        MAX,
    };

    /// @brief コンストラクタ
    UnitBase(void);

    /// @brief デストラクタ
    virtual ~UnitBase(void);

    /// @brief リソースの読み込み
    virtual void Load(void);

    /// @brief 初期化処理
    virtual void Initialize(void);

    /// @brief 更新処理
    virtual void Update(void);

    /// @brief 描画処理
    virtual void Draw(void) const;

    /// @brief 解放処理
    virtual void Release(void);

    /// @brief トランスフォーム情報の取得（非const版）
    /// @return Transformの参照
    Transform& GetTransform(void);

    /// @brief トランスフォーム情報の取得（const版）
    /// @return Transformの定数参照
    const Transform& GetTransform(void) const;

    /// @brief 座標の取得
    /// @return 現在の座標（VECTOR）
    const VECTOR& GetPos(void) const;

    /// @brief 座標の設定
    /// @param pos 設定する座標
    void SetPos(const VECTOR& pos);

    /// @brief 回転の取得
    /// @return 現在の回転（VECTOR）
    const VECTOR& GetRot(void) const;

    /// @brief 回転の設定
    /// @param rot 設定する回転角
    void SetRot(const VECTOR& rot);

    /// @brief スケールの取得
    /// @return 現在のスケール（VECTOR）
    const VECTOR& GetScl(void) const;

    /// @brief スケールの設定
    /// @param scl 設定するスケール値
    void SetScl(const VECTOR& scl);

    /// @brief 1フレーム前の座標を取得
    /// @return 移動前の座標
    const VECTOR& GetPrePos(void) const;

    /// @brief 半径の取得
    /// @return 当たり判定用の半径
    float GetRadius(void) const;

    /// @brief 半径の設定
    /// @param r 設定する半径
    void SetRadius(float r);

    /// @brief 移動力の直接設定
    /// @param pow 設定する移動ベクトル
    void SetMovePow(const VECTOR& pow);

    /// @brief 移動力の取得
    /// @return 現在の移動ベクトル
    const VECTOR& GetMovePow(void) const;

    /// @brief クォータニオンによる回転制御
    /// @param deg 回転角度（度数法）
    /// @param axis 回転軸
    void Turn(float deg, const VECTOR& axis);

    /// @brief アニメーションの再生
    /// @param aanimType 再生するアニメーション種類
    /// @param loop ループするかどうか
    /// @param blendTime アニメーション補間時間
    void PlayAnim(ANIM aanimType, bool loop, float blendTime);

    /// @brief アニメーション制御クラスの初期化
    void InitAnimaiton(void);

    /// @brief 自身の保持する全てのコライダを取得
    /// @return コライダのマップ参照
    const std::map<int, ColliderBase*>& GetOwnColliders(void) const
    {
        return ownColliders_;
    }

    /// @brief 特定のキーに対応する自身のコライダを取得
    /// @param key コライダの種類（int）
    /// @return コライダへのポインタ（なければnullptr）
    const ColliderBase* GetOwnCollider(int key) const;

    /// @brief 衝突対象（相手側）のコライダを登録
    /// @param hitCollider 相手のコライダ
    void AddHitCollider(const ColliderBase* hitCollider);

    /// @brief 登録されている衝突対象をすべてクリア
    void ClearHitCollider(void);

    /// @brief 一定範囲内のコライダのみを衝突対象として登録（最適化用）
    /// @param colliders 対象となる全コライダのリスト
    /// @param maxDistance 登録する最大距離
    void AddHitCollidersInRange(const std::vector<const ColliderBase*>& colliders, float maxDistance);

    // 衝突イベントコールバック
    virtual void OnCollisionEnter(const CollisionInfo& info) {}
    virtual void OnCollisionStay(const CollisionInfo& info) {}
    virtual void OnCollisionExit(const CollisionInfo& info) {}

protected:
    // 定数関連
    static constexpr float GRAVITY_POW = 1.0f;           // 重力加速度
    static constexpr float SPEED_MAX_JUMP_DOWN = 30.0f;  // 落下速度の最大制限

    // トランスフォーム関連
    Transform trans_;           // モデルの行列・座標・回転・スケール
    VECTOR prePos_;             // 1フレーム前の座標
    VECTOR movePow_;            // 移動量ベクトル
    VECTOR jumpPow_;            // ジャンプ上昇力

    // パラメータ関連
    float radius_;              // オブジェクトの半径
    float speed_;               // 移動スピード

    // アニメーション関連
    std::unique_ptr<AnimationController> anim_; // アニメーション制御
    ANIM currentAnim_;                          // 現在再生中のアニメーション

    // 衝突判定関連
    std::map<int, ColliderBase*> ownColliders_;         // 自身のコライダ（ID, インスタンス）
    std::vector<const ColliderBase*> hitColliders_;     // 判定対象となる相手のコライダ群

    /// @brief 汎用的な衝突判定処理
    virtual void Collision(void);

    /// @brief 重力の影響を計算
    virtual void CalcGravityPow(void);

    /// @brief 地面との衝突および接地処理
    void CollisionGravity(void);

    /// @brief 球体同士の衝突による押し出し
    void CollisionSphereVsSphere(void);

    /// @brief エネミーとの接触（ダメージ判定）処理
    void CollisionWithEnemy(void);

    /// @brief コライダの初期化（純粋仮想関数）
    virtual void InitCollider(void) = 0;

    /// @brief カプセル判定を用いた衝突処理（主に剣などの攻撃判定用）
    void CollisionWithCapsule(void);
};