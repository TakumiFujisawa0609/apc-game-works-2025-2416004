#include "GroundManager.h"

#include <DxLib.h>
#include <cmath>

#include "../../Manager/Generic/ResourceManager.h"
#include "../../Object/player.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Manager/Generic/Camera.h"
#include "../../Utility/Utility.h"

// コンストラクタ
GroundManager::GroundManager(void)
{
	baseModelId_ = -1;
	isLoaded_ = false;
}

// デストラクタ
GroundManager::~GroundManager(void)
{
}

// 読み込み
void GroundManager::Load(void)
{
	if (isLoaded_) return;

	auto& res = ResourceManager::GetInstance();

	baseModelId_ = res.LoadModelDuplicate(ResourceManager::SRC::MODEL_GROUND);

	if (baseModelId_ == -1)
	{
		printfDx("モデル読み込み失敗\n");
		return;
	}

	isLoaded_ = true;
}

// 初期化
void GroundManager::Init(void)
{
	if (!isLoaded_)
	{
		printfDx("ステージロード失敗\n");
		return;
	}

	grounds_.reserve(TILE_COUNT * TILE_COUNT);

	float halfSize = (TILE_COUNT * TILE_SIZE) * 0.5f;

	for (int z = 0; z < TILE_COUNT; z++)
	{
		for (int x = 0; x < TILE_COUNT; x++)
		{
			float worldX = (x * TILE_SIZE) - halfSize;
			float worldZ = (z * TILE_SIZE) - halfSize;
			VECTOR pos = VGet(worldX, 0.0f, worldZ);

			Ground g;
			g.Init(pos, MV1DuplicateModel(baseModelId_));
			grounds_.push_back(std::move(g));
		}
	}
}

// 更新処理
void GroundManager::Update(void)
{
	
}

// 描画処理
void GroundManager::Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir)
{
	const float cullDistance = 1500.0f;   // プレイヤーからの描画距離制限
	const float viewAngleCos = cosf(Utility::Deg2RadF(80.0f)); // 視野80度

	for (auto& g : grounds_)
	{
		VECTOR toGround = VSub(g.GetPos(), centerPos);
		float distSq = VSquareSize(toGround);

		// 一定距離外なら描画しない
		if (distSq > cullDistance * cullDistance) continue;

		// 視野外（カメラ後方）なら描画しない
		VECTOR toGroundCam = VNorm(VSub(g.GetPos(), cameraPos));
		float dot = VDot(cameraDir, toGroundCam);
		if (dot < viewAngleCos) continue;

		// 表示
		g.Draw();
	}
}

// 解放処理
void GroundManager::Release(void)
{
	for (auto& g : grounds_)
	{
		MV1DeleteModel(g.GetModelId());
	}
	grounds_.clear();

	if (baseModelId_ != -1)
	{
		MV1DeleteModel(baseModelId_);
		baseModelId_ = -1;
	}

	isLoaded_ = false;
}
