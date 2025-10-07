#include "EnemyDummy.h"
#include "Common/Transform.h"
#include "../Manager/Generic/ResourceManager.h"

EnemyDummy::EnemyDummy(void)
    : modelId_(-1), trans_(nullptr)
{
}

EnemyDummy::~EnemyDummy(void)
{

}

void EnemyDummy::Load(void)
{
    // Transform ‚ð new ‚ÅŠm•Û
    if (!trans_)
    {
        trans_ = new Transform();
    }

    modelId_ = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER);
    trans_->SetModel(modelId_);
}

void EnemyDummy::Init(void)
{
    trans_->pos = VGet(50.0f, 0.0f, 200.0f);
    trans_->scl = VGet(1.0f, 1.0f, 1.0f);
}

void EnemyDummy::Update(void)
{
    if (trans_)
    {
        trans_->Update();
    }
}

void EnemyDummy::Draw(void)
{
    if (modelId_ != -1)
    {
        MV1DrawModel(modelId_);
    }
}

void EnemyDummy::Release(void)
{
    if (modelId_ != -1)
    {
        MV1DeleteModel(modelId_);
        modelId_ = -1;
    }

    delete trans_;
    trans_ = nullptr;
}

Transform& EnemyDummy::GetTransform(void)
{
    return *trans_;
}
