#pragma once

#include <DxLib.h>
#include <map>
#include <functional>

#include "../../Common/Quaternion.h"
#include "../../Object/UnitBase.h"

class Transform;
class PauseMenu;

/// @brief カメラの制御および描画設定を行うクラス
class Camera : public UnitBase
{
public:
    /// @brief カメラの動作モード定義
    enum class MODE
    {
        NONE,               // 無効 / 未設定
        FIXED_POINT,        // 定点
        FOLLOW,             // シンプル追従
        FOLLOW_SPRING,      // スプリング付き追従
        FOLLOW_PERSPECTIVE, // 対象視点
        FREE,               // 自由移動
        SHAKE,              // 揺れ演出
        FREE_MOUSE,         // マウス自由回転
        TPS_MOUSE,          // TPS（三人称）マウス操作
        VERSATILITY_LOCKON  // 汎用ロックオン
    };

public:

    /// @brief コンストラクタ
    Camera(void);

    /// @brief デストラクタ
    virtual ~Camera(void) override = default;

    /// @brief リソースの読み込み（現在は不使用）
    void Load(void) override {}

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief 衝突判定前の更新処理
    void UpdateBeforeCollision(void);

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 更新処理
    void SetBeforeDraw(void);

    /// @brief 描画処理（デバッグ表示用など）
    void Draw(void) const override;
   
    /// @brief カメラの座標を取得
    /// @return VECTOR形式の座標
    VECTOR GetPosition(void) const;
    
    /// @brief カメラのモードを切り替える
    /// @param mode 設定する新しいモード
    void ChangeMode(MODE mode);

    /// @brief 現在のカメラモードを取得
    /// @return 現在のMODE
    MODE GetMode(void) const;

    /// @brief 追従対象の設定
    /// @param target 追従する対象のTransformポインタ
    void SetFollowTarget(const Transform* target);

    /// @brief カメラ座標と注視点の直接設定
    /// @param pos カメラの座標
    /// @param target 注視する座標
    void SetPosition(const VECTOR& pos, const VECTOR& target);

    /// @brief カメラの前方向ベクトルを取得
    /// @return 前方向の単位ベクトル
    VECTOR GetFrontVector(void) const;

    /// @brief カメラの右方向ベクトルを取得
    /// @return 右方向の単位ベクトル
    VECTOR GetRightVector(void) const;

    /// @brief ロックオン対象の設定
    /// @param target ロックオンする対象のTransformポインタ
    void SetLockonTarget(const Transform* target);

    /// @brief ロックオンの有効/無効を設定
    /// @param isEnabled 有効にするならtrue
    void SetLockonEnabled(bool isEnabled);

    /// @brief 現在ロックオン中かを取得
    /// @return ロックオン有効ならtrue
    bool IsLockonEnabled(void) const;

    /// @brief 追従処理の凍結（一時停止）設定
    /// @param isFreeze 凍結させるならtrue
    void SetFollowFreeze(bool isFreeze);

    /// @brief 軌道上のカメラ位置を取得（TPS操作等用）
    /// @return 計算された座標
    VECTOR GetOrbitPosition(void) const;

    /// @brief キー入力による回転速度を設定
    /// @param speed 回転速度
    void SetKeyRotationSpeed(float speed);

    /// @brief ピッチ操作（上下）の反転設定
    /// @param inverted 反転させるならtrue
    void SetPitchInvert(bool inverted);

    /// @brief ヨー操作（左右）の反転設定
    /// @param inverted 反転させるならtrue
    void SetYawInvert(bool inverted);

    /// @brief ピッチ反転設定の取得
    /// @return 反転設定ならtrue
    bool GetPitchInvert(void) const { return isPitchInverted_; }

    /// @brief ヨー反転設定の取得
    /// @return 反転設定ならtrue
    bool GetYawInvert(void) const { return isYawInverted_; }

    /// @brief マウス・スティックの感度を設定
    /// @param sensitivity 感度値
    void SetSensitivity(float sensitivity);

    /// @brief 現在の感度を取得
    /// @return 感度値 
    float GetSensitivity() const { return sensitivity_; }

protected:

    /// @brief 衝突判定の初期化
    /// @param void
    void InitCollider(void) override;

    /// @brief 衝突が継続している際のコールバック処理
    /// @param info 衝突相手や接触点などの情報
    void OnCollisionStay(const CollisionInfo& info) override;

    /// @brief 重力による加速度や移動量の計算
    void CalcGravityPow(void) override;

    /// @brief 衝突判定の実行
    void Collision(void) override;
private:

    /// @brief カメラのコライダ形状定義
    enum class COLLIDER_TYPE
    {
        SPHERE, // 球体
        MAX,    // 最大数
    };

    // カメラ設定関連
    static constexpr VECTOR DEFAULT_POS = { 0.0f, 100.0f, -500.0f }; // 初期座標
    static constexpr float  CLIP_NEAR = 1.0f;                        // 描画開始距離
    static constexpr float  CLIP_FAR = 30000.0f;                     // 描画終了距離
    static constexpr float  DEFAULT_YAW = 0.0f;                      // 初期水平角度（ヨー）
    static constexpr float  DEFAULT_VERTICAL = 30.0f;                // 初期垂直角度（ピッチ）
    static constexpr float  DEFAULT_DISTANCE = 400.0f;               // 対象との初期距離

    static constexpr VECTOR RELATIVE_C2T_POS = { 0.0f, 0.0f, 50.0f };             // カメラから見た注視点の相対位置
    static constexpr VECTOR RELATIVE_F2C_POS_FOLLOW = { 0.0f, 300.0f, -600.0f };  // 追従対象からのカメラオフセット
    static constexpr VECTOR RELATIVE_C2T_POS_PERSPECTIVE = { 0.0f, 0.0f, 20.0f }; // FPS視点時の注視点オフセット

    // 揺れ（シェイク）設定
    static constexpr float SHAKE_TIME = 0.7f;    // 継続時間
    static constexpr float SHAKE_WIDTH = 20.0f;  // 振幅（揺れの大きさ）
    static constexpr float SHAKE_SPEED = 150.0f; // 振動速度

    // 移動制御
    static constexpr float MOVE_ACC_POW = 3.0f;   // 加速力
    static constexpr float MOVE_DEC_POW = 3.0f;   // 減速力
    static constexpr float MAX_MOVE_SPEED = 7.0f; // 最大速度

    // 角度制限
    static constexpr float PITCH_LIMIT_UP = 89.0f;    // 見上げ限界角度
    static constexpr float PITCH_LIMIT_DOWN = -89.0f; // 見下ろし限界角度

    // 物理・衝突関連
    static constexpr float COLLISION_RADIUS = 35.0f;   // 当たり判定の半径
    static constexpr float MIN_GROUND_HEIGHT = 20.0f;  // 地面からの最低維持高度
    static constexpr float MIN_CAMERA_WORLD_Y = 20.0f; // ワールド座標における最低高度制限

    // デバッグ表示用定数
    static constexpr float  DEBUG_SPHERE_RADIUS = 10.0f;     // カメラ可視化球の半径
    static constexpr int    DEBUG_SPHERE_DIV = 8;            // 球のポリゴン分割数
    static const unsigned int DEBUG_COLOR_YELLOW = 0xFFFF00; // 黄色 (16進数表記)

    // スポットライト設定用定数
    static constexpr float LIGHT_OUTER_ANGLE = DX_PI_F / 2.0f;   // 外角（最大放射角度）
    static constexpr float LIGHT_INNER_ANGLE = DX_PI_F / 4.0f;   // 内角（減衰開始角度）
    static constexpr float LIGHT_RANGE = 2000.0f;          // 有効距離
    static constexpr float LIGHT_ATTEN_QUAD = 0.002f;           // 距離減衰率（2次）

    // 全カメラインスタンスで共有される操作感度
    static float sharedSensitivity_;

    // 外部参照・状態管理
    PauseMenu* pauseMenu_;               // ポーズメニュー参照
    MODE mode_;                          // 現在のカメラモード
    MODE prevMode_;                      // 前フレームのモード（遷移検知用）

    // 座標・ベクトル関連
    VECTOR targetPos_;                   // 注視点 (Target Position)
    VECTOR cameraUp_;                    // カメラの上方向ベクトル
    VECTOR defaultPos_;                  // 初期座標 / 基準座標
    VECTOR prePushPos_;                  // 押し出し（衝突判定）前の座標

    // 回転・距離関連
    Quaternion rotation_;                // カメラの回転クォータニオン
    float yaw_;                          // 水平回転角（ヨー）
    float pitch_;                        // 垂直回転角（ピッチ）
    float distance_;                     // 対象（注視点）との距離

    // ターゲット参照
    const Transform* followTarget_;      // 追従対象のトランスフォーム
    const Transform* lockonTarget_;      // ロックオン対象のトランスフォーム

    // 移動・物理関連
    VECTOR moveDir_;                     // 移動方向ベクトル
    float moveSpeed_;                    // 現在の移動速度
    VECTOR velocity_;                    // 慣性計算用の速度ベクトル

    // 演出関連-
    VECTOR shakeDir_;                    // 揺れる方向
    float shakeStep_;                    // 揺れ演出の残り時間（ステップ）

    // 操作・入力関連
    float sensitivity_;                  // 操作感度
    int screenCenterX_;                  // 画面中央X座標
    int screenCenterY_;                  // 画面中央Y座標
    int mouseDeltaX_;                    // マウスのX軸移動量
    int mouseDeltaY_;                    // マウスのY軸移動量
    VECTOR cameraOffset_;                // プレイヤーからの相対オフセット

    // 制御フラグ
    bool isLockonEnabled_;               // ロックオン有効フラグ
    bool isFollowFrozen_;                // 追従凍結フラグ
    bool isPitchInverted_;               // 上下反転設定
    bool isYawInverted_;                 // 左右反転設定

    // 凍結・一時保存用
    VECTOR frozenTargetPos_;             // 凍結時の注視点座標
    VECTOR frozenCameraPos_;             // 凍結時のカメラ座標
    float initialYaw_;                   // 凍結開始時のYaw角度
    float initialDistance_;              // 凍結開始時の対象との距離
    float initialHeightOffset_;          // 凍結開始時のY軸高さオフセット
    float keyRotateSpeed_;               // キー入力による追加回転速度

    // カメラ用スポットライトハンドル
    int spotLightHandle_;                

    // モード別描画前更新処理テーブル
    std::map<MODE, std::function<void()>> beforeDrawFuncTable_; 

    /// @brief 定点カメラモード
    void SetBeforeDrawFixedPoint(void);

    /// @brief フリーカメラモード
    void SetBeforeDrawFree(void);

    /// @brief 追従カメラモード
    void SetBeforeDrawFollow(void);

    /// @brief ねじり追従（スプリング）カメラモード
    void SetBeforeDrawFollowSpring(void);

    /// @brief 追従対象視点カメラモード
    void SetBeforeDrawFollowPerspective(void);

    /// @brief カメラ揺らし演出
    void SetBeforeDrawShake(void);

    /// @brief マウス自由操作カメラモード
    void SetBeforeDrawFreeMouse(void);

    /// @brief TPS用カメラモード
    void SetBeforeDrawTPSMouse(void);

    /// @brief ロックオンカメラモード
    void SetBeforeDrawLockon(void);

    /// @brief カメラの各種パラメータを初期値にリセット
    void SetUpDefault(void);

    /// @brief カメラ用ライト（スポットライト等）の設定
    void SetUpLighting(void);

    /// @brief 入力に基づいた移動ベクトルの計算
    void ProcessMove(void);

    /// @brief 計算されたベクトルによる座標更新
    void Move(void);

    // @brief 加速処理
    /// @param speed 加算する速度量
    void Acceleration(float speed);

    /// @brief 減速処理
    /// @param speed 減算する速度量
    void Decelerate(float speed);

    /// @brief 衝突判定による押し出し（めり込み防止）処理
    void HandleCollisionPushback(void);

    /// @brief 指定座標の地面の高さ（Y座標）を取得
    /// @param pos 調査する座標
    /// @return 地面の高さ
    float GetGroundHeight(const VECTOR& pos);

};