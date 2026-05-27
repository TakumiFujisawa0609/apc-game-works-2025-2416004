#include "../../Pch.h"
#include "AnimationController.h"
#include "../../Manager/Generic/SceneManager.h"

AnimationController::AnimationController(int modelId)
{
	// 操作対象のモデルIDを保持
	modeId_ = modelId;

	// 再生中・前回のアニメーションを「未設定(-1)」で初期化
	playType_ = -1;
	prevType_ = -1;

	// 時間・フラグ系の初期化
	blendTIme_ = 0.0f;
	blendTimer_ = 0.0f;
	isLoop_ = false;
	isPaused_ = false;
}

AnimationController::~AnimationController(void)
{
	Release();
}

void AnimationController::AddInternal(int type, int animIndex, float speed)
{
	//アニメーションデータ
	AnimData anim;

	// 内部アニメーションなのでモデルハンドルは不要(-1)
	anim.model = -1;
	anim.animIndex = animIndex;
	anim.speed = speed;
	anim.mode = ANIM_MODE::INTERNAL;

	Add(type, anim);
}

void AnimationController::AddExternal(int type, const int modelHandle, float speed)
{
	AnimData anim;

	anim.model = modelHandle;

	// 外部モデルの場合、通常はそのモデルの0番目のアニメを使用する
	anim.animIndex = 0;
	anim.speed = speed;
	anim.mode = ANIM_MODE::EXTERNAL;

	Add(type, anim);
}

void AnimationController::Add(int type, AnimData anim)
{
	//アニメーションデータを保存
	animations_[type] = anim;
}

void AnimationController::Play(int type, bool isLoop, float blendTime)
{

	//同じアニメーションなら何もしない
	if (type == playType_) { return; }

	// 既にブレンド中（前々回のアニメが残っている）なら強制解除
	if (prevType_ != -1)
	{
		auto& prevAnim = animations_[prevType_];
		MV1DetachAnim(modeId_, prevAnim.attachNo);
		prevType_ = -1;
	}

	// 現在のアニメがあるなら、それをフェードアウト用に「前回のアニメ」へ回す
	if (playType_ != -1)
	{
		prevType_ = playType_;
	}

	// 新しい再生情報をセット
	playType_ = type;
	isLoop_ = isLoop;
	blendTIme_ = blendTime;
	blendTimer_ = 0.0f;
	isPaused_ = false;

	auto& anim = animations_[type];
	anim.step = 0.0f; // 再生位置を先頭にリセット

	// アニメーションの種類に応じてモデルにアタッチ
	if (anim.mode == ANIM_MODE::INTERNAL)
	{
		anim.attachNo = MV1AttachAnim(modeId_, anim.animIndex);
	}
	else
	{
		anim.attachNo = MV1AttachAnim(modeId_, anim.animIndex, anim.model);
	}

	// アニメーションの総再生時間を取得
	anim.totalTime = MV1GetAttachAnimTotalTime(modeId_, anim.attachNo);

	// 遷移元がない場合は即座に表示(1.0)、ある場合はフェードイン待ち(0.0)
	if (prevType_ == -1)
	{
		MV1SetAttachAnimBlendRate(modeId_, anim.attachNo, 1.0f);
	}
	else
	{
		MV1SetAttachAnimBlendRate(modeId_, anim.attachNo, 0.0f);
	}
}

void AnimationController::Update(void)
{
	// 一時停止中の処理
	if (isPaused_)
	{
		// 停止中も現在のフレーム位置をモデルに反映させ続ける
		if (playType_ != -1)
		{
			auto& anim = animations_[playType_];
			MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);
		}
		return;
	}

	// フレーム間の経過時間を取得
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	// 再生中アニメーションの進行
	if (playType_ != -1)
	{
		auto& anim = animations_[playType_];

		// 時間を進める
		anim.step += deltaTime * anim.speed;

		// ループ処理
		if (isLoop_ && anim.step >= anim.totalTime)
		{
			anim.step = 0.0f;
		}

		// モデルに反映
		MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);
	}

	// ブレンド（フェード遷移）処理
	if (prevType_ != -1)
	{
		blendTimer_ += deltaTime;

		// ブレンド率を算出（0.0 ～ 1.0）
		float t = (blendTIme_ > 0.0f) ? (blendTimer_ / blendTIme_) : 1.0f;
		if (t > 1.0f) t = 1.0f;

		auto& prevAnim = animations_[prevType_];
		auto& newAnim = animations_[playType_];

		// 古い方を消し、新しい方を出す
		MV1SetAttachAnimBlendRate(modeId_, prevAnim.attachNo, 1.0f - t);
		MV1SetAttachAnimBlendRate(modeId_, newAnim.attachNo, t);

		// ブレンド完了
		if (t >= 1.0f)
		{
			MV1DetachAnim(modeId_, prevAnim.attachNo);
			prevType_ = -1;
			MV1SetAttachAnimBlendRate(modeId_, newAnim.attachNo, 1.0f);
		}
	}
}

bool AnimationController::IsEnd(void) const
{
	// 再生中ではない、またはループ設定なら「終わらない」
	if (playType_ == -1 || isLoop_)
	{
		return false;
	}

	// 現在の再生時間が総時間を超えていれば終了
	return animations_.at(playType_).step >= animations_.at(playType_).totalTime;
}

int AnimationController::GetPlayType(void) const
{
	return playType_;
}

void AnimationController::Release(void)
{
	for (auto& [type, anim] : animations_)
	{
		// 外部読み込みモデルの場合は削除
		if (anim.mode == ANIM_MODE::EXTERNAL && anim.model != -1)
		{
			MV1DeleteModel(anim.model);
		}

		// アタッチされているものを解除
		if (anim.attachNo != -1)
		{
			MV1DetachAnim(modeId_, anim.attachNo);
		}
	}

	animations_.clear();
	playType_ = -1;
	prevType_ = -1;
	blendTimer_ = 0.0f;
	blendTIme_ = 0.0f;
	isPaused_ = false;
}

bool AnimationController::IsPlaying(int type) const
{
	if (playType_ != type)
	{
		return false;
	}

	if (isLoop_)
	{
		return true;
	}

	auto it = animations_.find(type);
	if (it == animations_.end())
	{
		return false;
	}

	const AnimData& anim = it->second;
	return anim.step < anim.totalTime;
}

void AnimationController::Pause(void)
{
	isPaused_ = true;
}

void AnimationController::Resume(void)
{
	isPaused_ = false;
}

bool AnimationController::IsPaused(void) const
{
	return isPaused_;
}

void AnimationController::PauseAtTime(float time)
{
	if (playType_ != -1)
	{
		auto& anim = animations_[playType_];

		// 範囲内に収める
		if (time < 0.0f) time = 0.0f;
		if (time > anim.totalTime) time = anim.totalTime;

		anim.step = time;
		MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);
		isPaused_ = true;
	}
}

void AnimationController::PauseAtTime(int animType, float time)
{
	// 指定されたタイプのアニメーションが存在するか確認
	auto it = animations_.find(animType);
	if (it == animations_.end())
	{
		return;
	}

	// 指定されたタイプが現在再生中でない場合は再生する
	if (playType_ != animType)
	{
		Play(animType, false, 0.0f);
	}

	// 指定時間に設定して一時停止
	auto& anim = animations_[animType];

	// 時間を範囲内にクランプ
	if (time < 0.0f) time = 0.0f;
	if (time > anim.totalTime) time = anim.totalTime;

	anim.step = time;
	MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);

	// 一時停止
	isPaused_ = true;
}

void AnimationController::PauseAtFrame(int frameNumber)
{
	if (playType_ != -1)
	{
		auto& anim = animations_[playType_];

		// フレーム番号を時間に変換
		// 総フレーム数 = 総時間 × 再生速度
		float totalFrames = anim.totalTime * anim.speed;

		// フレーム番号を0から範囲内にクランプ
		if (frameNumber < 0) frameNumber = 0;
		if (frameNumber > (int)totalFrames) frameNumber = (int)totalFrames;

		// フレーム番号を時間に変換
		float time = (float)frameNumber / anim.speed;

		// 時間を範囲内にクランプ
		if (time > anim.totalTime) time = anim.totalTime;

		// 指定時間に設定
		anim.step = time;
		MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);

		// 一時停止
		isPaused_ = true;
	}
}

void AnimationController::PauseAtFrame(int animType, int frameNumber)
{
	// 指定されたタイプのアニメーションが存在するか確認
	auto it = animations_.find(animType);
	if (it == animations_.end())
	{
		return;
	}

	// 指定されたタイプが現在再生中でない場合は再生する
	if (playType_ != animType)
	{
		Play(animType, false, 0.0f);
	}

	// 指定フレームに設定して一時停止
	auto& anim = animations_[animType];

	// フレーム番号を時間に変換
	float totalFrames = anim.totalTime * anim.speed;

	// フレーム番号を0から範囲内にクランプ
	if (frameNumber < 0) frameNumber = 0;
	if (frameNumber > (int)totalFrames) frameNumber = (int)totalFrames;

	// フレーム番号を時間に変換
	float time = (float)frameNumber / anim.speed;

	// 時間を範囲内にクランプ
	if (time > anim.totalTime) time = anim.totalTime;

	anim.step = time;
	MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);

	// 一時停止
	isPaused_ = true;
}

int AnimationController::GetCurrentFrame(void) const
{
	if (playType_ != -1)
	{
		const auto& anim = animations_.at(playType_);
		// 時間をフレーム番号に変換
		return (int)(anim.step * anim.speed);
	}
	return 0;
}

int AnimationController::GetCurrentFrame(int animType) const
{
	auto it = animations_.find(animType);
	if (it != animations_.end())
	{
		const auto& anim = it->second;
		return (int)(anim.step * anim.speed);
	}
	return 0;
}

int AnimationController::GetTotalFrames(void) const
{
	if (playType_ != -1)
	{
		const auto& anim = animations_.at(playType_);
		return (int)(anim.totalTime * anim.speed);
	}
	return 0;
}

int AnimationController::GetTotalFrames(int animType) const
{
	auto it = animations_.find(animType);
	if (it != animations_.end())
	{
		const auto& anim = it->second;
		return (int)(anim.totalTime * anim.speed);
	}
	return 0;
}

float AnimationController::GetCurrentTime(void) const
{
	if (playType_ != -1)
	{
		return animations_.at(playType_).step;
	}
	return 0.0f;
}

float AnimationController::GetCurrentTimes(int animType) const
{
	auto it = animations_.find(animType);
	if (it != animations_.end())
	{
		return it->second.step;
	}
	return 0.0f;
}

void AnimationController::SetCurrentTime(float time)
{
	if (playType_ != -1)
	{
		auto& anim = animations_[playType_];

		// 時間を範囲内にクランプ
		if (time < 0.0f) time = 0.0f;
		if (time > anim.totalTime) time = anim.totalTime;

		anim.step = time;
		MV1SetAttachAnimTime(modeId_, anim.attachNo, anim.step);
	}
}

float AnimationController::GetTotalTime(void) const
{
	if (playType_ != -1)
	{
		return animations_.at(playType_).totalTime;
	}
	return 0.0f;
}