#include"Grid.h"

#include<DxLib.h>


Grid::Grid(void)
{
}

Grid::~Grid(void)
{
}

void Grid::Initialize(void)
{
}

void Grid::Update(void)
{
}

void Grid::Draw(void)
{
	// X軸方向の線（Z方向に並べる）→ 赤
	for (int z = -NUM; z <= NUM; z++)
	{
		float offsetZ = z * TERM;

		VECTOR sPos = { -LEN, 0.0f, offsetZ };
		VECTOR ePos = { LEN, 0.0f, offsetZ };

		DrawLine3D(sPos, ePos, 0xff0000);
		DrawSphere3D(ePos, 20.0f, 10, 0xff0000, 0xff0000, true);
	}

	// Z軸方向の線（X方向に並べる）→ 青
	for (int x = -NUM; x <= NUM; x++)
	{
		float offsetX = x * TERM;

		VECTOR sPos = { offsetX, 0.0f, -LEN };
		VECTOR ePos = { offsetX, 0.0f, LEN };

		DrawLine3D(sPos, ePos, 0x0000FF);
		DrawSphere3D(ePos, 20.0f, 10, 0x0000ff, 0x0000ff, true);
	}

	// 原点からY軸に向かう線 → 緑（軸表示用）
	VECTOR yStart = { 0.0f, -LEN, 0.0f };
	VECTOR yEnd = { 0.0f, LEN, 0.0f };

	DrawLine3D(yStart, yEnd, 0x00ff00);
}

void Grid::Release(void)
{
	
}