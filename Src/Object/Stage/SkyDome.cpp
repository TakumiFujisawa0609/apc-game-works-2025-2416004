#include "../../Pch.h"
#include "SkyDome.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Utility/Utility.h"

void SkyDome::InitCollider(void)
{
}

SkyDome::SkyDome(void)
	: UnitBase()
{

}

SkyDome::~SkyDome(void)
{
}

void SkyDome::Load(void)
{
	trans_.modelId = ResourceManager::GetInstance().Load(ResourceManager::SRC::MODEL_SKY_DOME).handleId_;
}

void SkyDome::Initialize(void)
{
	// スケール
	trans_.scl = SKY_DOME_SCL;

	// 座標
	trans_.pos = SKY_DOME_POS;

	// ワールド回転の初期化
	trans_.rot = Utility::VECTOR_ZERO;

	// クォータニオンのワールド回転の初期化
	trans_.quaRot = Quaternion::Identity();

	// クォータニオンのローカル回転の初期化
	trans_.quaRotLocal = Quaternion::Euler(SKY_DOME_LOC_ROT);

	trans_.Update();
}

void SkyDome::Update(void)
{
	trans_.quaRot = Quaternion::Mult(trans_.quaRot, Quaternion::AngleAxis(Utility::Deg2RadF(-0.01f), Utility::AXIS_Y));

	if (followTarget_)
	{
		trans_.pos = *followTarget_; 
		trans_.Update();
	}
}

void SkyDome::Draw(void) const
{
	SetUseBackCulling(FALSE); 
	SetFogEnable(FALSE);        

	MV1DrawModel(trans_.modelId);

	SetFogEnable(TRUE);
	SetUseBackCulling(TRUE);
}

void SkyDome::SetFollowTarget(const VECTOR* target)
{
	followTarget_ = target;
}
