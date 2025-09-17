#pragma once
#include<string>
#include<vector>

class Resource
{

public:

	//リソースタイプ
	enum class TYPE
	{
		NONE,
		IMG,
		IMGS,
		MASK,
		MODEL,
		ANIM,
		EFFEKSEER,
		SOUND,
	};

	//コンストラクタ
	Resource(void);

	//コンストラクタ
	Resource(TYPE type, const std::string& path);

	//コンストラクタ(IMGS用)
	Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY);

	//デストラクタ
	~Resource(void);

	//読み込み
	void Load(void);

	//解放
	void Release(void);

	//複数画像ハンドルを別配にコピー
	void CoopyHandle(int* imgs);

	//リソースタイプ
	TYPE resType_;

	//リソースの読み込み先
	std::string path_;

	//画像とモデルのハンドルID
	int handleId_;

	// 配列変数: IMGS::LoadDivGraphで使用するハンドルIDを格納する
	int* handleIds_;

	// 分割画像の横方向の分割数
	int numX_;

	// 分割画像の縦方向の分割数
	int numY_;

	// 各分割画像の横幅（ピクセル単位）
	int sizeX_;

	// 各分割画像の縦幅（ピクセル単位）
	int sizeY_;

	//モデル複製用
	std::vector<int> duplicateModelIds_;
};