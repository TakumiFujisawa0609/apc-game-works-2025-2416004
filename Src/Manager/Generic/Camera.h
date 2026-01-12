#pragma once
#include "../../Common/Quaternion.h"
#include "../../Object/UnitBase.h"

class Transform;

class Camera : public UnitBase
{
public:
    // カメラモード
    enum class MODE
    {
        NONE,
        FIXED_POINT,
        FOLLOW,
        FOLLOW_SPRING,
        FOLLOW_PERSPECTIVE,
        FREE,
        SHAKE,
        FREE_MOUSE,
        TPS_MOUSE,
        VERSATILITY_LOCKON,
    };

    // コンストラクタ
    Camera(void);

    // デストラクタ
    ~Camera(void) override;

    // 初期化処理
    void Init(void) override;

    // リソース読み込み（使用しない）
    void Load(void) override {}

    // 更新処理（衝突判定前）
    void UpdateBeforeCollision(void);

    // 更新処理
    void Update(void) override;

    // 描画前処理
    void SetBeforeDraw(void);

    // 描画処理
    void Draw(void) const override;

    // 解放処理
    void Release(void) override;

    // 座標の取得
    VECTOR GetPos(void) const;

    // モードの変更
    void ChangeMode(MODE mode);

    // 追従対象の設定
    const void SetFollow(const Transform* follow);

    // 座標の設定
    void SetPos(const VECTOR& pos, const VECTOR& target);

    // 前方向ベクトルを取得
    VECTOR GetFrontVec(void) const;

    // 右方向ベクトルを取得
    VECTOR GetRightVec(void) const;

    // モードの取得
    MODE GetMode(void) const;

    // ロックオンフラグ設定
    void SetLockon(bool loc);

    // ロックオン中か
    bool IsLockon(void) const;

    // 追従凍結の設定
    void SetFreezeFollow(bool freeze);

    // 軌道位置を取得
    VECTOR GetOrbitPosition(void) const;

    // キー入力による回転を設定
    void SetKeyRotation(float rotSpeed);

    // ロックオン対象の設定
    void SetLockonTarget(const Transform* target);

protected:
    // 衝突判定の初期化
    void InitCollider(void) override;

    // 衝突判定のコールバック
    void OnCollisionStay(const CollisionInfo& info) override;

    // 重力計算
    void CalcGravityPow(void) override;

    // 衝突判定
    void Collision(void) override;

private:

    // 衝突判定
    enum class COLLIDER_TYPE
    {
        SPHERE,
        MAX,
    };

    // カメラの初期設定
    static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f, 100.0f, -500.0f };

    // カメラのクリップ距離
    static constexpr float CAMERA_NEAR = 1.0f;
    static constexpr float CAMERA_FAR = 30000.0f;

    // カメラの初期角度
    static constexpr float CAMERA_YAW = 0.0f;
    static constexpr float CAMERA_PITCH = 30.0f;
    static constexpr float CAMERA_DISTANCE = 400.0f;

    // カメラから注視点までの相対座標
    static constexpr VECTOR RELATIVE_C2T_POS = { 0.0f, 0.0f, 50.0f };

    // 追従対象からカメラまでの相対座標
    static constexpr VECTOR RELATIVE_F2C_POS_FOLLOW = { 0.0f, 300.0f, -600.0f };

    // 追従対象視点カメラ
    static constexpr VECTOR RELATIVE_C2T_POS_FOLLOW_PERSPECTIVE = { 0.0f, 0.0f, 20.0f };

    // カメラ揺れ
    static constexpr float TIME_SHAKE = 0.7f;
    static constexpr float WIDTH_SHAKE = 20.0f;
    static constexpr float SPEED_SHAKE = 150.0f;

    // 移動スピード
    static constexpr float MOVE_ACC = 3.0f;
    static constexpr float MOVE_DEC = 3.0f;
    static constexpr float MAX_MOVE_SPEED = 7.0f;

    // マウス感度
    static constexpr float DEFAULT_SENSITIVITY = 0.2f;

    // ピッチ制限
    static constexpr float PITCH_UP = 89.0f;
    static constexpr float PITCH_DWON = -89.0f;

    // カメラ用当たり半径
    static constexpr float COLLISION_RADIUS = 35.0f;
   

    // 地面からの最低高度
    static constexpr float MIN_HEIGHT_FROM_GROUND = 20.0f;

    // 最低カメラ高度
    static constexpr float MIN_CAMERA_Y = 20.0f;

    // モード
    MODE mode_;

    // 現在のモード
    MODE currentMode_;

    // カメラの座標(自機から追従)
    // VECTOR pos_; // UnitBase::trans_.posを使用

    // カメラのデフォルト座標
    VECTOR defaultPos_;

    // 注視点
    VECTOR targetPos_;

    // ロックオン対象
    const Transform* lockonTarget_;

    // カメラの回転
    Quaternion rot_;

    // 追従対象
    const Transform* followTransform_;

    // 左右回転
    float yaw_;

    // 上下回転
    float pitch_;

    // 対象との距離
    float distance_;

    // カメラの上方向
    VECTOR cameraUp_;

    // 移動方向
    VECTOR moveDIr_;

    // 移動スピード
    float moveSpeed_;

    // カメラ揺らし方向
    VECTOR shakeDir_;

    // カメラ揺らしステップ
    float stepShake_;

    // ライトハンドル
    int spotLight_;

    // 速度
    VECTOR velocity_;

    // マウス感度
    float sensitivity_;

    // 画面中央のX座標
    int centerX_;

    // 画面中央のY座標
    int centerY_;

    // マウスのX移動量
    int deltaX_;

    // マウスのY移動量
    int deltaY_;

    // カメラオフセット
    VECTOR offset_;

    // ロックオンフラグ
    bool lockonFlag_;

    // 追従停止フラグ
    bool freezeFollow_;

    // 凍結時の注視点
    VECTOR frozenTargetPos_;

    // 凍結時のカメラ位置
    VECTOR frozenCameraPos_;

    // 凍結開始時のyaw角度
    float initialYaw_;

    // 凍結開始時の距離
    float initialDistance_;

    // キー入力による回転速度
    float keyRotateSpeed_;

    // 凍結開始時のY軸オフセット
    float initialHeightOffset_;

    // 押し出し前の座標
    VECTOR prePos_;

    // カメラモード別更新処理
    std::map<MODE, std::function<void(void)>> setBeforeDrawMode_;

    // 定点カメラ
    void SetBeforeDrawFixedPoint(void);

    // フリーカメラ
    void SetBeforeDrawFree(void);

    // 追従カメラ
    void SetBeforeDrawFollow(void);

    // ねじり追従カメラ
    void SetBeforeDrawFollowSpring(void);

    // 追従対象視点カメラ
    void SetBeforeDrawFollowPerspective(void);

    // カメラ揺らし
    void SetBeforeDrawShake(void);

    // マウス自由操作カメラ
    void SetBeforeDrawFreeMouse(void);

    // TPS用カメラ
    void SetBeforeDrawTPSMouse(void);

    // ロックオンカメラ
    void SetBeforeDrawLockon(void);

    // カメラの初期設定
    void SetDefault(void);

    // ライトの設定
    void SetLighting(void);

    // 移動操作
    void ProcessMove(void);

    // 移動
    void Move(void);

    // 加速
    void Acceleration(float speed);

    // 減速
    void Decelerate(float speed);

    // 押し出し処理用
    void HandleCollisionPushback(void);

    // 地面の高さを取得
    float GetGroundHeight(const VECTOR& pos);

};