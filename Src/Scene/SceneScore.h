#pragma once

#include<memory>

#include"SceneBase.h"


class SceneScore : public SceneBase
{
public:

	//コンストラクタ
	SceneScore(void);

	//デストラクタ
	~SceneScore(void) = default;

	//読み込み
	void Load(void) override;

	//読み込み完了
	void EndLoad(void) override;

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

