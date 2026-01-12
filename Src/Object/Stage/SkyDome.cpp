#include "SkyDome.h"
#include "../../Manager/Generic/ResourceManager.h"
#include "../../Utility/Utility.h"

void SkyDome::InitCollider(void)
{
}

// コンストラクタ
SkyDome::SkyDome(void)
	: UnitBase()
{

}

// デストラクタ
SkyDome::~SkyDome(void)
{
}

// リソースの読み込み
void SkyDome::Load(void)
{
	trans_.modelId = ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::MODEL_SKY_DOME);
}

// 初期化
void SkyDome::Init(void)
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

// 更新処理
void SkyDome::Update(void)
{
	trans_.quaRot = Quaternion::Mult(trans_.quaRot, Quaternion::AngleAxis(Utility::Deg2RadF(-0.01f), Utility::AXIS_Y));

	if (followTarget_)
	{
		trans_.pos = *followTarget_; // カメラの位置をコピー
		trans_.Update();
	}
}

// 描画処理
void SkyDome::Draw(void) const
{
	SetUseBackCulling(FALSE);  // 両面描画
	SetFogEnable(FALSE);        // フォグ無効

	MV1DrawModel(trans_.modelId);

	SetFogEnable(TRUE);
	SetUseBackCulling(TRUE);
}

void SkyDome::SetFollowTarget(const VECTOR* target)
{
	followTarget_ = target;
}
