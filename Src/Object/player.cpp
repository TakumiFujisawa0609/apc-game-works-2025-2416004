#include "../Application.h"
#include "../Utility/Utility.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/Manager/CollisionManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include"player.h"

Player::Player(void)
{
	//モデル
	modelId_ = 0;

	//所持金
	money_ = 0;

	blockedDirX_ = 0;

	blockedDirZ_ = 0;

	movementEnabled_ = true;

	currentAnimType_ = -1;

	//角度
	angles_ = VECTOR();

	//大きさ
	scales_ = VECTOR();
}

Player::~Player(void)
{
}

void Player::Init(void)
{
	movementEnabled_ = true;

	//モデルのロード
	modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);

	// 座標設定
	//初期化
	trans_.pos = DEFAULT_POS;
	prePos_ = trans_.pos;
	MV1SetPosition(modelId_, trans_.pos);

	// 大きさ設定
	scales_ = SCALES;
	MV1SetScale(modelId_, scales_);

	//当たり判定の半径
	radius_ = RADIUS;

	// 色の調整(自己発光)
	MV1SetMaterialEmiColor(modelId_, 0, COLOR_EMI_DEFAULT);

	// モデルの角度
	angles_ = { 0.0f, Utility::Deg2RadF(180.0f), 0.0f };
	MV1SetRotationXYZ(modelId_, angles_);

	// アニメーションコントローラ初期化
	animationController_ = new AnimationController(modelId_);

	// アニメーション登録（外部ファイル）
	animationController_->AddExternal(static_cast<int>(ANIM_TYPE::IDLE), Application::PATH_MODEL + "player/Idle.mv1", 35.0f);
	animationController_->AddExternal(static_cast<int>(ANIM_TYPE::WALK), Application::PATH_MODEL + "player/Walk.mv1", 25.0f);

	// 再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
	currentAnimType_ = static_cast<int>(ANIM_TYPE::IDLE);

	//カメラ方向初期化
	axis_ = { 0.0f,0.0f,0.0f };

	//所持金の初期化
	money_ = 5000;
}

void Player::Update(void)
{
	//移動前座標保存
	prePos_ = trans_.pos;

	animationController_->Update();

	ProcessMove();

	

	// プレイヤーの座標と半径を使って当たり判定
	CollisionManager::GetInstance().CheckHitWithPlayer(this, trans_.pos, radius_, GetHitMin(), GetHitMax());

}

void Player::Draw(void)
{
	// プレイヤーの描画
	MV1DrawModel(modelId_);
#ifdef _DEBUG
	// プレイヤー座標
	DrawFormatString(0, 40, 0x0, "プレイヤー座標:(%.2f, %.2f, %.2f)", trans_.pos.x, trans_.pos.y, trans_.pos.z);
	DrawSphere3D(trans_.pos, radius_, 16, 0xffffff, 0xffffff, false);
	DrawFormatString(0, 120, 0xffffff,"所持金 :%d", money_);
	
#endif //_DEBUG
}

void Player::Release(void)
{
	// プレイヤーモデルの解放
	MV1SetScale(trans_.modelId, trans_.scl);
	MV1SetPosition(trans_.modelId, trans_.pos);
	MV1SetRotationXYZ(trans_.modelId, trans_.rot);
	MV1DrawModel(trans_.modelId);

	// アニメーションコントローラの解放
	//animationController_->Release();
	delete animationController_;
}

VECTOR Player::GetPos(void) const
{
	return trans_.pos;
}

void Player::SetPos(VECTOR pos)
{
	trans_.pos = pos;
	MV1SetPosition(modelId_, trans_.pos);
}

VECTOR Player::GetHitMin(void) const
{
	return {
		trans_.pos.x - radius_,
		trans_.pos.y - radius_,
		trans_.pos.z - radius_
	};
}

VECTOR Player::GetHitMax(void) const
{
	return {
		trans_.pos.x + radius_,
		trans_.pos.y + radius_,
		trans_.pos.z + radius_
	};
}

float Player::GetRadius(void) const
{
	return radius_;
}

int Player::GetMoney(void) const
{
	return money_;
}

void Player::AddMoney(int money)
{
	money_ += money;
}

void Player::SetBlockedDirX(int dir)
{
	blockedDirX_ = dir;
}

void Player::SetBlockedDirZ(int dir)
{
	blockedDirZ_ = dir;
}

void Player::ResetBlockDirs(void)
{
	blockedDirX_ = 0;
	blockedDirZ_ = 0;
}

void Player::SetMovementEnabled(bool enabled)
{
	movementEnabled_ = enabled;
	if (!movementEnabled_)
	{
		PlayAnim(ANIM_TYPE::IDLE, true);
	}
}

bool Player::IsMovementEnabled(void) const
{
	return movementEnabled_;
}


void Player::PlayAnim(ANIM_TYPE type, bool loop)
{
	int animIndex = static_cast<int>(type);
	if (currentAnimType_ != animIndex)
	{
		animationController_->Play(animIndex, loop);
		currentAnimType_ = animIndex;
	}
}


void Player::ProcessMove(void)
{

	if (!movementEnabled_) return;

	InputManager& ins = InputManager::GetInstance();
	ResetBlockDirs();

	VECTOR moveDir = Utility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_W) && blockedDirZ_ != 1)
		moveDir = VAdd(moveDir, Utility::DIR_F);
	if (ins.IsNew(KEY_INPUT_S) && blockedDirZ_ != -1)
		moveDir = VAdd(moveDir, Utility::DIR_B);
	if (ins.IsNew(KEY_INPUT_A) && blockedDirX_ != 1)
		moveDir = VAdd(moveDir, Utility::DIR_L);
	if (ins.IsNew(KEY_INPUT_D) && blockedDirX_ != -1)
		moveDir = VAdd(moveDir, Utility::DIR_R);

	if (!Utility::EqualsVZero(moveDir))
	{
		moveDir = VNorm(moveDir);
		VECTOR movePow = VScale(moveDir, SPEED_MOVE);
		trans_.pos = VAdd(trans_.pos, movePow);

		angles_.y = atan2(moveDir.x, moveDir.z) + Utility::Deg2RadF(180.0f);
		MV1SetRotationXYZ(modelId_, angles_);
		MV1SetPosition(modelId_, trans_.pos);

		// 歩行アニメ再生
		PlayAnim(ANIM_TYPE::WALK);
	}
	else
	{
		// 待機アニメ再生
		PlayAnim(ANIM_TYPE::IDLE);
	}
}
