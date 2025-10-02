#include "Player.h"

#include <DxLib.h>

#include "../Utility/Utility.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Application.h"

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
}

//デストラクタ
Player::~Player(void)
{

}

//リソースの読み込み
void Player::Load(void)
{
	auto& res = ResourceManager::GetInstance();

	////モデル読み込み
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
	trans_.scl = { 0.5f, 0.5f, 0.5f };

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

	VECTOR moveDir = Utility::VECTOR_ZERO;

	auto& input = InputManager::GetInstance();

	//入力処理
	if (input.IsNew(KEY_INPUT_W)) { moveDir = VAdd(moveDir, Utility::DIR_F); }

	if (input.IsNew(KEY_INPUT_S)) { moveDir = VAdd(moveDir, Utility::DIR_B); }

	if (input.IsNew(KEY_INPUT_A)) { moveDir = VAdd(moveDir, Utility::DIR_L); }

	if (input.IsNew(KEY_INPUT_D)) { moveDir = VAdd(moveDir, Utility::DIR_R); }

	//ジャンプ入力
	if (isGround_ && input.IsTrgDown(KEY_INPUT_SPACE))
	{
		velocityY_ = jumpPower_;
		isGround_ = false;
	}

	//重力処理
	if (!isGround_)
	{
		velocityY_ += gravity_;

		trans_.pos.y += velocityY_;

		//地面判定
		if (trans_.pos.y <= 0.0f)
		{
			trans_.pos.y = 0.0f;

			velocityY_ = 0.0f;
			
			isGround_ = true;
		}
	}

	//移動制限
	if (blockedDirX_ == 1 && moveDir.x < 0) { moveDir.x = 0; }
	
	if (blockedDirX_ == -1 && moveDir.x < 0) { moveDir.x = 0; }
	
	if (blockedDirX_ == 1 && moveDir.z < 0) { moveDir.z = 0; }
	
	if (blockedDirX_ == -1 && moveDir.z < 0) { moveDir.z = 0; }

	//アニメーションの制御
	if (!Utility::EqualsVZero(moveDir))
	{
		moveDir = VNorm(moveDir);

		VECTOR movePow = VScale(moveDir, 10.0f);

		trans_.pos = VAdd(trans_.pos, movePow);

		//向きの設定
		VECTOR rot = trans_.rot;

		rot.y = atan2f(moveDir.x, moveDir.z) + Utility::Deg2RadF(180.0f);

		trans_.rot = rot;

		//歩きアニメーション
		PlayAnim(ANIM::WALK, true, 0.2f);
	}
	else
	{
		//待機アニメーション
		PlayAnim(ANIM::IDEL, true, 0.2f);
	}

}
