#pragma once

class SceneBase
{
public:

	//コンストラクタ
	SceneBase(void) = default;

	//デストラクタ
	virtual ~SceneBase(void) = 0;

	//初期化
	virtual void Init(void) = 0;

	//更新処理
	virtual void Update(void) = 0;

	//描画処理
	virtual void Draw(void) = 0;

	//解放処理
	virtual void Release(void) = 0;
};
