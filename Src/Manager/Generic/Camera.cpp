#include "Camera.h"

#include <EffekseerForDXLib.h>

#include "../../Application.h"
#include "../../Utility/Utility.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "../../Object/Common/Transform.h"

Camera::Camera(void)  
{  
   // モードの初期化  
   mode_ = MODE::NONE;  

   //currentMode_ の初期化
   currentMode_ = MODE::NONE;   

   // 座標の初期化  
   pos_ = Utility::VECTOR_ZERO;

   //defaultPos_ の初期化
   defaultPos_ = Utility::VECTOR_ZERO;   

   // 追従対象の座標  
   targetPos_ = Utility::VECTOR_ZERO;  

   //ロックオン対象
   lockonTarget_ = nullptr;

   // 回転  
   rot_ = Quaternion::Identity();  

   followTransform_ = nullptr;  

   // 左右回転  
   yaw_ = CAMERA_YAW;  

   // 上下回転  
   pitch_ = CAMERA_PITCH;  

   // 対象との距離  
   distance_ = CAMERA_DISTANCE;  

   // カメラの上方向  
   cameraUp_ = { 0.0f, 1.0f, 0.0f };

   // 移動関連の初期化  
   moveDIr_ = Utility::VECTOR_ZERO;

   //moveSpeed_ の初期化 
   moveSpeed_ = 0.0f;  

   // カメラ揺らし関連の初期化  
   shakeDir_ = Utility::VECTOR_ZERO;
   
   // stepShake_ の初期化 
   stepShake_ = 0.0f;  

   // ライト関連の初期化  
   spotLight_ = -1;

   // 速度の初期化  
   velocity_ = Utility::VECTOR_ZERO;

   //マウス感度
   sensitivity_ = DEFAULT_SENSITIVITY;

   //x中央
   centerX_ = Application::DEFA_SCREEN_SIZE_X / 2;

   //y中央
   centerY_ = Application::DEFA_SCREEN_SIZE_Y / 2;

   //xの移動量
   deltaX_ = 0;

   //yの移動量
   deltaY_ = 0;

   offset_ = Utility::VECTOR_ZERO;
}

Camera::~Camera(void)
{

}

//初期化処理
void Camera::Init(void)
{
	//関数ポインタの設定

	//何もない
	setBeforeDrawMode_.emplace(MODE::NONE, std::bind(&Camera::SetBeforeDrawFollow, this));

	//定点カメラ
	setBeforeDrawMode_.emplace(MODE::FIXED_POINT, std::bind(&Camera::SetBeforeDrawFixedPoint, this));

	//フリーカメラ
	setBeforeDrawMode_.emplace(MODE::FREE, std::bind(&Camera::SetBeforeDrawFree, this));

	//追従カメラ
	setBeforeDrawMode_.emplace(MODE::FOLLOW, std::bind(&Camera::SetBeforeDrawFollow, this));

	//ばね付きカメラ
	setBeforeDrawMode_.emplace(MODE::FOLLOW_SPRING, std::bind(&Camera::SetBeforeDrawFollowSpring, this));

	//追従対象カメラ
	setBeforeDrawMode_.emplace(MODE::FOLLOW_PERSPECTIVE, std::bind(&Camera::SetBeforeDrawFollowPerspective, this));

	//カメラ揺らし
	setBeforeDrawMode_.emplace(MODE::SHAKE, std::bind(&Camera::SetBeforeDrawShake, this));

	//マウス操作自由カメラ
	setBeforeDrawMode_.emplace(MODE::FREE_MOUSE, std::bind(&Camera::SetBeforeDrawFreeMouse, this));

	//TPS用マウスカメラ
	setBeforeDrawMode_.emplace(MODE::TPS_MOUSE, std::bind(&Camera::SetBeforeDrawTPSMouse, this));

	//ロックオンカメラ
	setBeforeDrawMode_.emplace(MODE::LOCKON, std::bind(&Camera::SetBeforeDrawLockon, this));

	//カメラの初期設定
	SetDefault();

	//カメラのライト設定
	SetLighting();
}

//更新処理
void Camera::Update(void)
{
	//ライトの移動
	SetLightPositionHandle(spotLight_, pos_);

	//ライトの向き更新
	SetLightDirectionHandle(spotLight_, rot_.ToEuler());
}

void Camera::SetBeforeDraw(void)
{
	//クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	//モードによる設定切り替え
	setBeforeDrawMode_[mode_]();

	//カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec
	(
		pos_,
		targetPos_,
		cameraUp_
	);

	//DXライブラリのカメラとEffekseerのカメラを同期する
	Effekseer_Sync3DSetting();

}

//定点カメラ
void Camera::SetBeforeDrawFixedPoint(void)
{

}

//フリーカメラ
void Camera::SetBeforeDrawFree(void)
{
	auto& ins = InputManager::GetInstance();

	//移動操作
	ProcessMove();

	//減速
	Decelerate(MOVE_DEC);

	//移動
	Move();
}

//追従カメラ
void Camera::SetBeforeDrawFollow(void)
{
	if (!followTransform_) return;

	//追従対象の位置
	VECTOR followPos = followTransform_->pos;

	//追従対象の向き
	Quaternion followRot = followTransform_->quaRot;

	//追従対象からカメラまでの相対座標
	VECTOR relativeCPos = followRot.PosAxis(RELATIVE_F2C_POS_FOLLOW);

	//カメラの位置の更新
	pos_ = VAdd(followPos, relativeCPos);

	//カメラ位置から注視点までの相対座標
	VECTOR relativeTPos = followRot.PosAxis(RELATIVE_C2T_POS);

	//注視点の更新
	targetPos_ = VAdd(pos_, relativeTPos);

	//カメラの上方向
	cameraUp_ = followRot.PosAxis(rot_.GetUp());
}

//ばね付き追従カメラ
void Camera::SetBeforeDrawFollowSpring(void)
{
	if (!followTransform_) return;
	auto& ins = InputManager::GetInstance();

	//Cキー押下でカメラを揺らす
	if (ins.IsTrgDown(KEY_INPUT_C))
	{
		currentMode_ = mode_;
		ChangeMode(MODE::SHAKE);
	}

	//ばね定数(ばねの強さ)
	float POW_SPRING = 50.0f;

	//ばね定数(ばねの抵抗)
	float dampening = 2.0f * sqrt(POW_SPRING);

	//デルタタイム
	float delta = SceneManager::GetInstance().GetDeltaTime();

	//3D酔いする人用
	//delta = 1.0f / 60.0f;

	//追従対象の位置
	VECTOR followPos = followTransform_->pos;

	//追従対象の向き
	Quaternion followRot = followTransform_->quaRot;
	VECTOR zero = { 0.0f, 0.0f, 0.0f };

	//カメラの方向を固定する用
	Quaternion forward = Quaternion::Euler(zero);

	//追従対象からカメラまでの相対座標
	VECTOR relativeCPos = forward.PosAxis(RELATIVE_F2C_POS_FOLLOW);

	//理想位置
	VECTOR idealPos = VAdd(followPos, relativeCPos);

	//実際と理想の差
	VECTOR diff = VSub(pos_, idealPos);

	//力 =- ばねの強さ × ばねの伸び - 抵抗 × カメラ速度
	VECTOR force = VScale(diff, -POW_SPRING);
	force = VSub(force, VScale(velocity_, dampening));

	//速度の更新
	velocity_ = VAdd(pos_, VScale(velocity_, delta));

	//カメラ位置の更新
	pos_ = VAdd(pos_, VScale(velocity_, delta));

	//カメラ位置から注視点までの相対座標
	VECTOR relativeTPos = forward.PosAxis(RELATIVE_C2T_POS);

	//注視点の更新
	targetPos_ = VAdd(pos_, relativeTPos);

	//カメラの上方向
	cameraUp_ = forward.PosAxis(rot_.GetUp());
}

//追従対象視点カメラ
void Camera::SetBeforeDrawFollowPerspective(void)
{
	//追従対象の位置
	VECTOR followPos = followTransform_->pos;

	//追従対象の向き
	Quaternion followRot = followTransform_->quaRot;

	//カメラ位置から注視点までの相対座標
	VECTOR relativeTPos = followRot.PosAxis(RELATIVE_C2T_POS_FOLLOW_PERSPECTIVE);

	//注視点の更新
	targetPos_ = VAdd(pos_, relativeTPos);

	//カメラの上方向
	cameraUp_ = followRot.PosAxis(rot_.GetUp());
}

//カメラ揺らし
void Camera::SetBeforeDrawShake(void)
{
	//一定時間カメラを揺らす
	stepShake_ -= SceneManager::GetInstance().GetDeltaTime();

	if (stepShake_ < 0.0f)
	{
		pos_ = defaultPos_;

		ChangeMode(MODE::FOLLOW_SPRING);
		return;
	}

	//-1.0f～1.0f
	float f = sinf(stepShake_ * SPEED_SHAKE);

	//-1000.0f～1000.0f
	f *= 1000.0f;

	//-1000 or 1000
	int d = static_cast<int>(f);

	//0 or 1
	int shake = d % 2;

	//0 or 2
	shake *= 2;

	//-1 or 1
	shake -= 1;

	//移動量
	VECTOR velocity = VScale(shakeDir_, (float)(shake)*WIDTH_SHAKE);

	//移動先座標
	pos_ = VAdd(defaultPos_, velocity);
}

//マウス自由操作カメラ
void Camera::SetBeforeDrawFreeMouse(void)
{
	auto& ins = InputManager::GetInstance();
	//マウス座標を取得
	Vector2 mousePos = ins.GetMousePos();

	//移動量
	deltaX_ = static_cast<int>(mousePos.x) - centerX_;
	deltaY_ = static_cast<int>(mousePos.y) - centerY_;

	//水平回転を適用
	Quaternion yaw = Quaternion::AngleAxis(Utility::Deg2RadF(deltaX_ * sensitivity_), Utility::AXIS_Y);

	//垂直回転を適用
	Quaternion pitch = Quaternion::AngleAxis(Utility::Deg2RadF(deltaY_ * sensitivity_), Utility::AXIS_X);

	//回転を適用
	rot_ = yaw.Mult(rot_);

	rot_ = pitch.Mult(rot_);

	//注視点更新
	VECTOR rotLocalPos = rot_.PosAxis(RELATIVE_C2T_POS);
	targetPos_ = VAdd(pos_, rotLocalPos);

	//上ベクトル更新
	cameraUp_ = rot_.GetUp();

	//マウスを中央に戻す
	SetMousePoint(centerX_, centerY_);
}

void Camera::SetBeforeDrawTPSMouse(void)
{
	if (!followTransform_) return;

	auto& ins = InputManager::GetInstance();

	//マウス座標を取得
	Vector2 mousePos = ins.GetMousePos();

	//移動量
	deltaX_ = static_cast<int>(mousePos.x) - centerX_;

	deltaY_ = static_cast<int>(mousePos.y) - centerY_;

	//回転角度更新
	yaw_ += deltaX_ * sensitivity_;
	pitch_ -= deltaY_ * sensitivity_;

	//ピッチ制限
	if (pitch_ > PITCH_UP) { pitch_ = PITCH_UP; }
	if (pitch_ < PITCH_DWON) { pitch_ = PITCH_DWON; }

	//対象位置
	VECTOR followPos = followTransform_->pos;
	
	//球面座標でカメラ位置を計算
	offset_;
	offset_.x = distance_ * cosf(Utility::Deg2RadF(pitch_)) * sinf(Utility::Deg2RadF(yaw_));
	offset_.y = distance_ * sinf(Utility::Deg2RadF(pitch_));
	offset_.z = distance_ * cosf(Utility::Deg2RadF(pitch_)) * cosf(Utility::Deg2RadF(yaw_));

	//カメラ位置
	pos_ = VAdd(followPos, offset_);

	//注視点
	targetPos_ = followPos;

	//上方向は固定
	cameraUp_ = VGet(0, 1, 0);

	//マウスを中央に戻す
	SetMousePoint(centerX_, centerY_);
}

void Camera::SetBeforeDrawLockon(void)
{
	if (!followTransform_ || !lockonTarget_) return;

	//プレイヤー位置
	VECTOR followPos = followTransform_->pos;

	//球面座標でカメラを配置
	offset_.x = distance_ * cosf(Utility::Deg2RadF(pitch_)) * sinf(Utility::Deg2RadF(yaw_));
	offset_.y = distance_ * sinf(Utility::Deg2RadF(pitch_));
	offset_.z = distance_ * cosf(Utility::Deg2RadF(pitch_)) * cosf(Utility::Deg2RadF(yaw_));

	//カメラ位置は対象基準
	pos_ = VAdd(followPos, offset_);
	
	//注視点はターゲット
	targetPos_ = lockonTarget_->pos;

	//上方向は固定
	cameraUp_ = VGet(0, 1, 0);

	//マウスを中央座標に戻す
	SetMousePoint(centerX_, centerY_);
}

void Camera::Draw(void)
{
	//DrawFormatString(0, 0, 0xffffff,"カメラ座標 = { %.2f, %.2f, %.2f}",pos_.x, pos_.y, pos_.z);
}

void Camera::Release(void)
{
	//ライト無効化
	SetLightEnableHandle(spotLight_, false);

	//ライトハンドル削除
	DeleteLightHandle(spotLight_);
}

VECTOR Camera::GetPos(void) const
{
	return pos_;
}

void Camera::ChangeMode(MODE mode)
{
	//カメラの初期設定
	//SetDefault();

	//カメラモードの変更
	mode_ = mode;

	//変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
		
	case Camera::MODE::FREE:
		break;

	case Camera::MODE::FOLLOW:
		break;

	case Camera::MODE::FOLLOW_SPRING:
		break;

	case Camera::MODE::SHAKE:
		stepShake_ = TIME_SHAKE;
		shakeDir_ = VNorm({ 0.7f, 0.7f, 0.0f });
		defaultPos_ = pos_;
		break;
	}
}

//座標の取得
const void Camera::SetFollow(const Transform* follow)
{
	followTransform_ = follow;
}

//座標の設定
void Camera::SetPos(const VECTOR& pos, const VECTOR& target)
{
	pos_ = pos;
	targetPos_ = target;
}

VECTOR Camera::GetFrontVec(void) const
{
	// pos_ → targetPos_ の方向ベクトルを正規化
	VECTOR front = VSub(targetPos_, pos_);
	float length = sqrtf(front.x * front.x + front.y * front.y + front.z * front.z);
	if (length > 0.0001f)
	{
		front.x /= length;
		front.y /= length;
		front.z /= length;
	}
	return front;
}

void Camera::SetLockonTarget(const Transform* target)
{
	lockonTarget_ = target;
}

//カメラの初期設定
void Camera::SetDefault(void)
{
	//カメラの初期設定
	pos_ = DEFAULT_CAMERA_POS;

	//注視点
	targetPos_ = VAdd(pos_, RELATIVE_C2T_POS);

	//カメラ上方向
	cameraUp_ = { 0.0f, 1.0f, 0.0f };

	//カメラはX軸に傾いているが
	//この傾いた状態を角度ゼロ、傾きなしとする
	rot_ = Quaternion::Identity();

	velocity_ = Utility::VECTOR_ZERO;
}

//ライトの設定
void Camera::SetLighting(void)
{
	//ライトハンドルを作成
	spotLight_ = CreateSpotLightHandle(
		pos_,
		VGet(0.0f, -1.0f, 0.0f),
		DX_PI_F / 2.0f,
		DX_PI_F / 4.0f,
		2000.0f,
		0.0f,
		0.002f,
		0.0f
	);

	//ライト有効化
	SetLightEnableHandle(spotLight_, true);
}

//移動操作
void Camera::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();

	//移動
	if (ins.IsNew(KEY_INPUT_W)) { moveDIr_ = Utility::DIR_F; Acceleration(MOVE_ACC); }
	if (ins.IsNew(KEY_INPUT_S)) { moveDIr_ = Utility::DIR_B; Acceleration(MOVE_ACC); }
	if (ins.IsNew(KEY_INPUT_A)) { moveDIr_ = Utility::DIR_L; Acceleration(MOVE_ACC); }
	if (ins.IsNew(KEY_INPUT_D)) { moveDIr_ = Utility::DIR_R; Acceleration(MOVE_ACC); }

	//回転軸と量を決める
	const float ROT_POW = 1.0f;
	VECTOR axisDeg = Utility::VECTOR_ZERO;
	if (ins.IsNew(KEY_INPUT_UP)) { axisDeg.x = -1.0f; }
	if (ins.IsNew(KEY_INPUT_DOWN)) { axisDeg.x = 1.0f; }
	if (ins.IsNew(KEY_INPUT_LEFT)) { axisDeg.y = -1.0f; }
	if (ins.IsNew(KEY_INPUT_RIGHT)) { axisDeg.y = 1.0f; }

	//カメラ座標を中心として、注視点を回転させる
	if (!Utility::EqualsVZero(axisDeg))
	{
		//今回の回転量を合成
		Quaternion rotPow;
		rotPow = rotPow.Mult(
			Quaternion::AngleAxis(Utility::Deg2RadF(axisDeg.z), Utility::AXIS_Z));

		rotPow = rotPow.Mult(
			Quaternion::AngleAxis(Utility::Deg2RadF(axisDeg.x), Utility::AXIS_X));

		rotPow = rotPow.Mult(
			Quaternion::AngleAxis(Utility::Deg2RadF(axisDeg.y), Utility::AXIS_Y));

		//カメラの回転の今回の回転量を加える(合成)
		rot_ = rot_.Mult(rotPow);

		//注視点の相対座標を回転させる
		VECTOR rotLocalPos = rot_.PosAxis(RELATIVE_C2T_POS);

		//注視点更新
		targetPos_ = VAdd(pos_, rotLocalPos);

		//カメラの上方向
		cameraUp_ = rot_.GetUp();
	}
}

//移動
void Camera::Move(void)
{
	//移動処理
	if (Utility::EqualsVZero(moveDIr_))
	{
		//移動 = 座標 * 移動量
		//移動量 =  方向 * スピード

		//入力された方向をカメラの回転譲歩うを使って、
		//カメラの進行方向に変換する
		VECTOR direction = rot_.PosAxis(moveDIr_);

		//移動量
		VECTOR movePow = VScale(direction, moveSpeed_);

		//移動処理
		pos_ = VAdd(pos_, movePow);

		targetPos_ = VAdd(targetPos_, movePow);
	}
}

//加速
void Camera::Acceleration(float speed)
{
	moveSpeed_ += speed;

	//速度制限(右方向)
	if (moveSpeed_ > MAX_MOVE_SPEED)
	{
		moveSpeed_ = MAX_MOVE_SPEED;
	}

	//速度制限(左方向)
	if (moveSpeed_ < -MAX_MOVE_SPEED)
	{
		moveSpeed_ = -MAX_MOVE_SPEED;
	}
}

//減速
void Camera::Decelerate(float speed)
{
	//右方向の移動を減速させる
	if (moveSpeed_ > 0.0f)
	{
		moveSpeed_ -= speed;

		if (moveSpeed_ < 0.0f)
		{
			moveSpeed_ = 0.0f;
		}
	}

	//左方向の移動を減速させる
	if (moveSpeed_ < 0.0f)
	{
		moveSpeed_ += speed;

		if (moveSpeed_ > 0.0f)
		{
			moveSpeed_ = speed;
		}
	}
}