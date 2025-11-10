#pragma once

//ステージの地面部品クラス
class Ground
{
public:

	//コンストラクタ
	Ground(void);

	//デストラクタ
	~Ground(void);

	//初期化
	void Init(const VECTOR& pos, int modelId);

	//描画処理
	void Draw(void);

	//モデルIDを取得
	int GetModelId(void) const;

	//座標を取得
	VECTOR GetPos(void) const;

private:
	//モデルID
	int modelId_;

	//座標
	VECTOR pos_;
};

