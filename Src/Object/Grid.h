#pragma once
class Grid
{

public:

	//線の長さ
	static constexpr float LEN = 1200.0f;

	//線の長さの半分
	static constexpr float HLEN = LEN / 2.0f;

	//線の間隔
	static constexpr float TERM = 50.0f;

	//線の数
	static const int NUM = static_cast<int>(LEN / TERM);

	//線の数の半分
	static const int HNUM = NUM / 2;

	//コンストラクタ
	Grid(void);

	//デストラクタ
	~Grid(void);

	//初期化
	void Init(void);

	//更新処理
	void Update(void);

	//描画処理
	void Draw(void);

	//解放処理
	void Release(void);
};
