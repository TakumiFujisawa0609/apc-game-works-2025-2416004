#include "AnimationController.h"

#include <DxLib.h>

#include "../../Manager/Generic/SceneManager.h"

//コンストラクタ
AnimationController::AnimationController(int modelId)
{
	//モデルIDを保存
	modeId_ = modelId;

	//再生中のアニメーションタイプ初期化
	playType_ = -1;

	// 前回のアニメーションタイプ初期化
	prevType_ = -1;

	//ブレンド時間初期化
	blendTIme_ = 0.0f;

	//ブレンドタイマー初期化
	blendTimer_ = 0.0f;

	//ループ設定初期化
	isLoop_ = false;
}

//デストラクタ
AnimationController::~AnimationController(void)
{
	Release();
}

//内部アニメーションの追加
void AnimationController::AddInternal(int type, int animIndex, float speed)
{
	//アニメーションデータ
	AnimData anim;

	//アニメーションモード初期化
	anim.model = -1;

	//アニメーションインデックス保存
	anim.animIndex = animIndex;

	//再生速度保存
	anim.speed = speed;

	//アニメーションモード保存
	anim.mode = ANIM_MODE::INTERNAL;

	//アニメーションの追加
	Add(type, anim);
}

//外部アニメーションの追加
void AnimationController::AddExternal(int type, const std::string& path, float speed)
{
	//アニメーションデータ
	AnimData anim;

	//アニメーションモデルの読み込み
	anim.model = MV1LoadModel(path.c_str());

	//アニメーションインデックス初期化
	anim.animIndex = 0;

	//再生速度保存
	anim.speed = speed;

	//アニメーションモード保存
	anim.mode = ANIM_MODE::EXTERNAL;

	//アニメーションの追加
	Add(type, anim);
}

//アニメーションの追加
void AnimationController::Add(int type, AnimData anim)
{
	//アニメーションデータを保存
	animations_[type] = anim;
}

//アニメーションの再生
void AnimationController::Play(int type, bool isLoop, float blendTime)
{

	//同じアニメーションなら何もしない
	if (type == playType_) return;

	//以前のブレンドを中断する
	if (prevType_ != -1)
	{
		auto& prevAnim = animations_[prevType_];
		MV1DetachAnim(modeId_, prevAnim.attachNo);
		prevType_ = -1;
	}

	//現在のアニメを前回にセット
	if (playType_ != -1)
	{
		prevType_ = playType_;
	}

	//再生中のアニメーションを保存
	playType_ = type;

	//ループ設定
	isLoop_ = isLoop;

	//ブレンド時間設定
	blendTIme_ = blendTime;

	//ブレンドタイマーリセット
	blendTimer_ = 0.0f;

	//再生するアニメーションデータ
	auto& anim = animations_[type];

	//アニメーションの進行時間リセット
	anim.step = 0.0f;

	//内部アニメーションの場合
	if (anim.mode == ANIM_MODE::INTERNAL)
	{
		anim.attachNo = MV1AttachAnim(modeId_, anim.animIndex);
	}
	else
	{
		anim.attachNo = MV1AttachAnim(modeId_, anim.animIndex, anim.model);
	}

	//アニメーションの総時間を取得
	anim.totalTime = MV1GetAttachAnimTotalTime(modeId_, anim.attachNo);

	// 前回のアニメーションがない場合は即座に100%、ある場合は0%から開始
	if (prevType_ == -1)
	{
		MV1SetAttachAnimBlendRate(modeId_, anim.attachNo, 1.0f);
	}
	else
	{
		MV1SetAttachAnimBlendRate(modeId_, anim.attachNo, 0.0f);
	}
}

//アニメーションの更新
void AnimationController::Update(void)
{
	//デルタタイム
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	//再生中のアニメーションの進行
	if (playType_ != -1)
	{
		//再生中のアニメーションデータ
		auto& anim = animations_[playType_];

		//アニメーションの進行
		anim.step += deltaTime * anim.speed;

		//ループ設定があり、アニメーションの終了時間を超えた場合
		if (isLoop_ && anim.step >= anim.totalTime)
		{
			//アニメーションの進行をリセットする
			anim.step = 0.0f;
		}

		//アニメーションの時間を設定
		MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);
	}

	//フェード的な旧アニメーションの処理
	if (prevType_ != -1)
	{
		//ブレンドタイマーを進める
		blendTimer_ += deltaTime;

		//ブレンド率
		float t = (blendTIme_ > 0.0f) ? (blendTimer_ / blendTIme_) : 1.0f;

		//旧アニメーション
		auto& prevAnim = animations_[prevType_];

		//新アニメーション
		auto& newAnim = animations_[playType_];

		//旧アニメーションと新アニメーションのブレンド率を設定
		MV1SetAttachAnimBlendRate(modeId_, prevAnim.attachNo, 1.0f - t);
		MV1SetAttachAnimBlendRate(modeId_, newAnim.attachNo, t);

		//ブレンド終了した場合
		if (t >= 1.0f)
		{
			//旧アニメーションをデタッチ
			MV1DetachAnim(modeId_, prevAnim.attachNo);
			prevType_ = -1;

			//新アニメーションのブレンド率を100%にする
			MV1SetAttachAnimBlendRate(modeId_, newAnim.attachNo, 1.0f);
		}
	}
}

//アニメーションの終了判定
bool AnimationController::IsEnd(void) const
{
	//アニメーションが再生されていないかループ設定がある場合は終了しない
	if (playType_ == -1 || isLoop_) return false;

	//アニメーションの進行時間が総時間を超えたら終了
	return animations_.at(playType_).step >= animations_.at(playType_).totalTime;
}

//再生中のアニメーションタイプを取得
int AnimationController::GetPlayType(void) const
{
	return playType_;
}

//解放処理
void AnimationController::Release(void)
{
	//全てのアニメーションを解放
	for (auto& [type, anim] : animations_)
	{
		//外部アニメーションの場合
		if (anim.mode == ANIM_MODE::EXTERNAL && anim.model != -1)
		{
			//モデルの解放
			MV1DeleteModel(anim.model);
		}

		//タッチされている場合
		if (anim.attachNo != -1)
		{
			//アニメーションのデタッチ
			MV1DetachAnim(modeId_, anim.attachNo);
		}
	}

	animations_.clear();
	playType_ = -1;
	prevType_ = -1;
	blendTimer_ = 0.0f;
	blendTIme_ = 0.0f;
}