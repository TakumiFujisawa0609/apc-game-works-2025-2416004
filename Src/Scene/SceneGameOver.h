#pragma once

#include<memory>

#include"SceneBase.h"


class SceneGameOver : public SceneBase
{
public:
	//コンストラクタ
	SceneGameOver(void);

	//デストラクタ
	~SceneGameOver(void) = default;

	//初期化処理
	void Init(void)override;

	//更新処理
	void Update(void)override;

	//描画処理
	void Draw(void)override;

	//解放処理
	void Release(void)override;

private:

	//描画処理(デバッグ)
	void DrawDebug(void);
};

