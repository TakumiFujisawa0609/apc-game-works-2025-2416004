#include "Player.h"

#include <DxLib.h>

#include "../Utility/Utility.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Application.h"
#include "Common/AnimationController.h"
#include "Common/Transform.h"
#include "../Manager/Generic/Camera.h"

//コンストラクタ
Player::Player(void)
{
	//移動制限xの初期化
	blockedDirX_ = 0;

	//移動制限zの初期化
	blockedDirZ_ = 0;

	//移動可能かの初期化
	movementEnabled_ = true;

	//ジャンプ力の初期化
	jumpPower_ = JUMP_POWER;

	//重力加速度
	gravity_ = GRAVITY;

	//地面にいるかどうか
	isGround_ = true;

    //動きているかどうか
    isMoving_ = false;
}

//デストラクタ
Player::~Player(void)
{

}

//リソースの読み込み
void Player::Load(void)
{
	auto& res = ResourceManager::GetInstance();

	//モデル読み込み
	modelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
	trans_.SetModel(modelId_);
}

//初期化
void Player::Init(void)
{

	//初期化座標
	trans_.pos = Utility::VECTOR_ZERO;

	//回転の初期化
	trans_.rot = VGet(0, Utility::Deg2RadF(180.0f), 0);

	//スケールの初期化
	trans_.scl = { 0.2f, 0.2f, 0.2f };

	//当たり判定
	radius_ = 1.0f;

	//アニメーションの初期化
	anim_ = std::make_unique<AnimationController>(modelId_);


	anim_->AddExternal(static_cast<int>(ANIM::IDEL), "player/Idle.mv1", 35.0f);
	anim_->AddExternal(static_cast<int>(ANIM::WALK), "player/Walk.mv1", 25.0f);

}

//更新処理
void Player::Update(void)
{
	//前座標の保存
	prePos_ = trans_.pos;

	//入力による移動制御
	ProcessMove();
	
	//Transform　更新
	trans_.Update();

	//アニメーションの更新
	if (anim_) anim_->Update();
}

//描画処理
void Player::Draw(void) const
{
	//モデルの描画
	if (trans_.modelId != -1)
	{

		//座標の設定
		MV1SetPosition(trans_.modelId, trans_.pos);

		//大きさの設定
		MV1SetScale(trans_.modelId, trans_.scl);

		//回転の設定
		MV1SetRotationXYZ(trans_.modelId, trans_.rot);

		MV1DrawModel(trans_.modelId);
	}

#ifdef _DEBUG
	DrawFormatString(0, 40, 0xffffff, "Player Pos:(%.2f, %.2f, %.2f)", trans_.pos.x, trans_.pos.y, trans_.pos.z);

	// デバッグ用: プレイヤー座標に球を出す
	DrawSphere3D(trans_.pos, 10.0f,16, GetColor(255, 0, 0), GetColor(255, 0, 0), true);

    VECTOR debugEuler = trans_.rot;
    DrawFormatString(0, 60, 0xffffff, "Player Rot Euler: (%.2f, %.2f, %.2f)",
        debugEuler.x, debugEuler.y, debugEuler.z);

    Quaternion currentRot = Quaternion::Euler(trans_.rot);
    DrawFormatString(0, 80, 0xffffff, "Player Quat: w: %.2f x: %.2f y: %.2f z: %.2f",
        currentRot.w, currentRot.x, currentRot.y, currentRot.z);
#endif // _DEBUG

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

        // カメラの前方向を向く
        VECTOR camForward = camera->GetFrontVec();
        camForward.y = 0.0f;

        if (VSize(camForward) > 0.0001f)
        {
            camForward = VNorm(camForward);
            Quaternion targetLocalRot = Quaternion::LookRotation(camForward);
            trans_.quaRotLocal = Quaternion::RotateTowards(trans_.quaRotLocal, targetLocalRot, 30.0f);
        }
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
                Quaternion targetLocalRot = Quaternion::LookRotation(moveDir);
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
