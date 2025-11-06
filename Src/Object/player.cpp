#include "Player.h"

#include <fstream>
#include <sstream>

#include "../Utility/Utility.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/Camera.h"
#include "../Application.h"
#include "Common/AnimationController.h"
#include "Common/Transform.h"
#include "../DrawUI/Font.h"


// コンストラクタ
Player::Player(void)
{
    // モデルの初期化
    modelId_ = -1;

    // 移動制限xの初期化
    blockedDirX_ = 0;

    // 移動制限zの初期化
    blockedDirZ_ = 0;

    // ジャンプ力の初期化
    jumpPower_ = param_.jumpPower;

    // 重力加速度
    gravity_ = GRAVITY;

    // 現在のY方向速度
    velocityY_ = 0.0f;

    // 地面にいるかどうか
    isGround_ = true;

    // 動きているかどうか
    isMoving_ = false;

    // 移動可能かの初期化
    movementEnabled_ = true;

    // 表示上のHP
    hpDisplay_ = param_.hp;

    // 遅延スピード
    hpDelaySpeed_ = DELAY_SPEED;
}

// デストラクタ
Player::~Player(void)
{

}

// CSV読み込み
void Player::LoadParamCSV(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { return; }

    std::string line;

    // ヘッダを飛ばす
    std::getline(file, line);

    if (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string value;

        // 名前
        std::getline(ss, value, ',');

        // 攻撃力
        std::getline(ss, value, ','); param_.attack = std::stoi(value);

        // 防御力
        std::getline(ss, value, ','); param_.defensse = std::stoi(value);

        // 現在体力
        std::getline(ss, value, ','); param_.hp = std::stoi(value);

        // 最大体力
        std::getline(ss, value, ','); param_.maxHp = std::stoi(value);

        // スタミナ
        std::getline(ss, value, ','); param_.stamina = std::stoi(value);

        // 当たり判定(球体)
        std::getline(ss, value, ','); param_.collisionRadius = std::stoi(value);;

        // レベル
        std::getline(ss, value, ','); param_.level = std::stoi(value);

        // 最大レベル
        std::getline(ss, value, ','); param_.maxLevel = std::stoi(value);

        // ジャンプ力
        std::getline(ss, value, ','); param_.jumpPower = std::stoi(value);
    }

    file.close();
}

// リソースの読み込み
void Player::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    // モデル読み込み
    modelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
    trans_.SetModel(modelId_);
}

// 初期化
void Player::Init(void)
{
    auto& res = ResourceManager::GetInstance();

    // 初期化座標
    trans_.pos = Utility::VECTOR_ZERO;

    // 回転の初期化
    trans_.rot = VGet(0, 0, 0);

    // ローカル回転の初期化
    trans_.quaRotLocal = Quaternion::Identity();

   

    // スケールの初期化
    trans_.scl = PLAYER_SCL;

    // 当たり判定
    radius_ = param_.collisionRadius;

    // アニメーションの初期化
    anim_ = std::make_unique<AnimationController>(modelId_);

    anim_->AddExternal(static_cast<int>(ANIM::IDEL), res.Load(ResourceManager::SRC::ANIM_PLAYER_IDEL).handleId_, 35.0f);
    anim_->AddExternal(static_cast<int>(ANIM::WALK), res.Load(ResourceManager::SRC::ANIM_PLAYER_IDEL).handleId_, 25.0f);

}

// 更新処理
void Player::Update(void)
{
    // 前座標の保存
    prePos_ = trans_.pos;

    // 入力による移動制御
    ProcessMove();

    // Transform　更新
    trans_.Update();

    // アニメーションの更新
    if (anim_) anim_->Update();

    // HPバーの遅延表示
    if (hpDisplay_ > param_.hp)
    {
        // ダメージを受けた
        hpDisplay_ -= hpDelaySpeed_;

        if (hpDisplay_ < param_.hp)
        {
            hpDisplay_ = param_.hp;
        }
    }
    else
    {
        hpDisplay_ = param_.hp;
    }
}

// 描画処理
void Player::Draw(void) const
{

    // モデルの描画
    if (trans_.modelId != -1)
    {

        // 座標の設定
        MV1SetPosition(trans_.modelId, trans_.pos);

        // 大きさの設定
        MV1SetScale(trans_.modelId, trans_.scl);

        // 回転の設定
        MV1SetRotationXYZ(trans_.modelId, trans_.rot);

        MV1DrawModel(trans_.modelId);
    }

    DrawHpBar();

    // デバック表示
#ifdef _DEBUG
    DrawFormatString(0, 40, 0xffffff, "Player Pos:(%.2f, %.2f, %.2f)", trans_.pos.x, trans_.pos.y, trans_.pos.z);
#endif 

}

//解放処理
void Player::Release(void)
{
    if (anim_)
    {
        anim_->Release();
        anim_.reset();
    }
}

//移動可能かを設定
void Player::SetMovementEndbled(bool enabled)
{
    movementEnabled_ = enabled;
}

//移動かのかを取得
bool Player::IsMovementEndbled(void) const
{
    return movementEnabled_;
}

// パラメータの取得
const Player::Param& Player::GetParam(void) const
{
    return param_;
}

// プレイヤーのHPバー
void Player::DrawHpBar(void) const
{
    auto& font = Font::GetInstance();

    // hpバーの長さ
    int barWidth = HP_BAR_WIDTH;

    // hpバーの高さ
    int barHeight = HP_BAR_HEIGHT;

    // 画面中央
    int barX = (Application::SCREEN_SIZE_X - barWidth) / 2;

    // 画面中央下
    int barY = Application::SCREEN_SIZE_Y - BAR_Y;

    // 結合
    float hpRate = (float)param_.hp / param_.maxHp;

    float dispRate = hpDisplay_ / param_.maxHp;

    hpRate = std::clamp(hpRate, 0.0f, 1.0f);

    dispRate = std::clamp(dispRate, 0.0f, 1.0f);

    int curHPWidth = (int)(barWidth * hpRate);
    int dispHPWidth = (int)(barWidth * dispRate);

    // 枠
    DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(255, 255, 255), false);

    // 遅延バー（赤）
    DrawBox(barX, barY, barX + dispHPWidth, barY + barHeight, GetColor(255, 60, 60), true);

    // 現在HPバー（緑）
    DrawBox(barX, barY, barX + curHPWidth, barY + barHeight, GetColor(0, 255, 0), true);

    // 数値
    char hpStr[32];
    sprintf_s(hpStr, "%d / %d", param_.hp, param_.maxHp);

    int textWidth = font.GetDefaultTextWidth(hpStr);
    int textX = barX + (barWidth / 2) - (textWidth / 2);
    int textY = barY + (barHeight / 2) - 8;

    font.DrawDefaultText(textX, textY, hpStr, GetColor(255, 255, 255), 18);


}

//入力による移動制御
void Player::ProcessMove(void)
{
    if (!movementEnabled_) return;

    auto& input = InputManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    // 前回位置保存
    prePos_ = trans_.pos;

    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    //前
    if (input.IsNew(KEY_INPUT_W)) { forwardInput += 1.0f; }

    //後ろ
    if (input.IsNew(KEY_INPUT_S)) { forwardInput -= 1.0f; }

    //左
    if (input.IsNew(KEY_INPUT_A)) { rightInput -= 1.0f; }

    //右
    if (input.IsNew(KEY_INPUT_D)) { rightInput += 1.0f; }

    isMoving_ = false;

    //左右移動
    if (rightInput != 0.0f && forwardInput == 0.0f)
    {
        //カメラを停止
        camera->SetFreezeFollow(true);


        // 回転速度
        float keyRotSpeed = rightInput * 1.5f;

        // カメラにキー入力による回転速度を設定
        camera->SetKeyRotation(keyRotSpeed);

        // カメラの位置を取得
        VECTOR camPos = camera->GetPos();

        // カメラからプレイヤーへのベクトル（XZ平面のみ）
        VECTOR toPlayer = VSub(trans_.pos, camPos);

        // Y座標は保持
        float currentHeight = toPlayer.y;

        toPlayer.y = 0.0f;

        // 現在の距離を保存
        float radius = VSize(toPlayer);

        // 現在の角度を計算
        float currentAngle = atan2f(toPlayer.x, toPlayer.z);

        // 新しい角度（キー入力による回転を加える）
        float newAngle = currentAngle + Utility::Deg2RadF(keyRotSpeed);

        // 新しい位置を計算（極座標→直交座標）
        VECTOR newOffset;
        newOffset.x = radius * sinf(newAngle);
        newOffset.y = currentHeight;
        newOffset.z = radius * cosf(newAngle);

        // カメラ位置を基準に新しい位置を設定
        trans_.pos = VAdd(camPos, newOffset);

        isMoving_ = true;

        VECTOR tangent;

        if (rightInput > 0)
        {
            tangent.x = -toPlayer.z;

            tangent.z = toPlayer.x;
        }
        else
        {
            tangent.x = toPlayer.z;
            tangent.z = -toPlayer.x;
        }

        tangent.y = 0.0f;
        
        tangent = VNorm(tangent);

        Quaternion targetLocalRot = Quaternion::LookRotation(tangent);

        trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
    }
    else
    {
        // 左右移動していない時はカメラの追従を再開
        camera->SetFreezeFollow(false);

        //前後左右入力がある場合
        if (forwardInput != 0.0f || rightInput != 0.0f)
        {
            // カメラの前方向
            VECTOR camForward = camera->GetFrontVec();
            camForward.y = 0.0f;
            camForward = VNorm(camForward);

            // カメラの右方向
            VECTOR camRight = camera->GetRightVec();
            camRight.y = 0.0f;
            camRight = VNorm(camRight);

            // 移動方向を合成
            VECTOR moveDir = Utility::VECTOR_ZERO;
            moveDir = VAdd(moveDir, VScale(camForward, forwardInput));
            moveDir = VAdd(moveDir, VScale(camRight, -rightInput));
            moveDir.y = 0.0f;

            // 移動方向を正規化
            if (VSize(moveDir) > 0.0001f)
            {
                moveDir = VNorm(moveDir);

                // 移動量を適用
                VECTOR movement = VScale(moveDir, 10.5f);
                trans_.pos = VAdd(trans_.pos, movement);

                isMoving_ = true;

                // 移動方向に向く
                Quaternion targetLocalRot = Quaternion::LookRotation(VScale(moveDir, -1.0f));
                trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
            }
        }
    }

    // アニメーション制御
    if (isMoving_)
    {
        PlayAnim(ANIM::WALK, true, 0.2f);
    }
    else
    {
        PlayAnim(ANIM::IDEL, true, 0.2f);
    }

    // ジャンプ処理
    if (isGround_ && input.IsTrgDown(KEY_INPUT_SPACE))
    {
        velocityY_ = jumpPower_;
        isGround_ = false;
    }

    // 重力処理
    if (!isGround_)
    {
        velocityY_ += gravity_;
        trans_.pos.y += velocityY_;
        if (trans_.pos.y <= 0.0f)
        {
            trans_.pos.y = 0.0f;
            velocityY_ = 0.0f;
            isGround_ = true;
        }
    }

    // 移動制限（壁など）
    VECTOR moveDir = VSub(trans_.pos, prePos_);
    if ((blockedDirX_ == 1 && moveDir.x < 0) || (blockedDirX_ == -1 && moveDir.x > 0))
    {
        trans_.pos.x = prePos_.x;
    }

    if ((blockedDirZ_ == 1 && moveDir.z < 0) || (blockedDirZ_ == -1 && moveDir.z > 0))
    {
        trans_.pos.z = prePos_.z;
    }
}
