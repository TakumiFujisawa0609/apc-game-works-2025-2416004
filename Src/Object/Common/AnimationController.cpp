#include "AnimationController.h"
#include <DxLib.h>
#include "../../Manager/Generic/SceneManager.h"

AnimationController::AnimationController(int modelId)
    : modelId_(modelId), playType_(-1), isLoop_(true)
{
}

AnimationController::~AnimationController()
{
    Release();
}

void AnimationController::AddInternal(int type, int animIndex, float speed)
{
    Animation anim;
    anim.model = -1;
    anim.animIndex = animIndex;
    anim.speed = speed;
    anim.mode = Mode::Internal;
    Add(type, anim);
}

void AnimationController::AddExternal(int type, const std::string& path, float speed)
{
    Animation anim;
    anim.model = MV1LoadModel(path.c_str());
    anim.animIndex = 0;
    anim.speed = speed;
    anim.mode = Mode::External;
    Add(type, anim);
}

void AnimationController::Add(int type, Animation anim)
{
    animations_[type] = anim;
}

void AnimationController::Play(int type, bool isLoop)
{
    if (type == playType_)
        return;

    if (playType_ != -1) {
        MV1DetachAnim(modelId_, playAnim_.attachNo);
    }

    playAnim_ = animations_[type];
    playType_ = type;
    isLoop_ = isLoop;
    playAnim_.step = 0.0f;

    if (playAnim_.mode == Mode::Internal) {
        playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex);
    }
    else {
        playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex, playAnim_.model);
    }

    playAnim_.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);
}

void AnimationController::Update()
{
    float deltaTime = SceneManager::GetInstance().GetDeltaTime();

    playAnim_.step += deltaTime * playAnim_.speed;

    if (isLoop_ && playAnim_.step >= playAnim_.totalTime) {
        playAnim_.step = 0.0f;
    }

    MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);
}

bool AnimationController::IsEnd() const
{
    if (isLoop_) return false;
    return playAnim_.step >= playAnim_.totalTime;
}

int AnimationController::GetPlayType() const
{
    return playType_;
}

void AnimationController::Release()
{
    for (auto& [type, anim] : animations_) {
        if (anim.mode == Mode::External && anim.model != -1) {
            MV1DeleteModel(anim.model);
        }
    }

    if (playType_ != -1) {
        MV1DetachAnim(modelId_, playAnim_.attachNo);
    }

    animations_.clear();
    playType_ = -1;
}
