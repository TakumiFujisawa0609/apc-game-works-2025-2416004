#pragma once
#include<string>
#include<vector>

/// @brief 画像、モデル、サウンドなどのリソース管理を行うクラス
class Resource
{
public:

	/// @brief リソースの種類を定義する列挙型
	enum class TYPE
	{
		NONE,       // 未設定
		IMG,        // 単一画像
		IMGS,       // 分割画像
		MASK,       // マスク画像
		MODEL,      // 3Dモデル
		ANIM,       // アニメーション
		EFFEKSEER,  // エフェクト
		SOUND,      // 音源
	};

	/// @brief デフォルトコンストラクタ
	Resource(void);

	/// @brief 通常リソース用コンストラクタ
	/// @param type リソースの種類
	/// @param path ファイルパス
	Resource(TYPE type, const std::string& path);

	/// @brief 分割画像リソース用コンストラクタ
	/// @param type リソースの種類（IMGS想定）
	/// @param path ファイルパス
	/// @param numX 横方向の分割数
	/// @param numY 縦方向の分割数
	/// @param sizeX 1枚あたりの横幅
	/// @param sizeY 1枚あたりの縦幅
	Resource(TYPE type, const std::string& path, int numX, int numY, int sizeX, int sizeY);

	/// @brief デストラクタ
	~Resource(void);

	/// @brief リソースの読み込み処理を実行
	void Load(void);

	/// @brief リソースの解放処理を実行
	void Release(void);

	/// @brief 複数画像ハンドルを外部配列にコピーする
	/// @param imgs コピー先のポインタ
	void CoopyHandle(int* imgs);

	/// @brief リソースのハンドルIDを取得する
	/// @return ハンドルID（失敗時は-1等）
	int GetHandle(void) const;

	// リソース情報関連
	TYPE resType_;                   // リソースタイプ
	std::string path_;               // リソースのファイルパス

	// 画像関連
	int handleId_;                   // 画像やモデルのハンドルID
	int* handleIds_;                 // 分割画像用ハンドル配列（LoadDivGraphで使用）
	int numX_;                       // 分割画像の横方向の分割数
	int numY_;                       // 分割画像の縦方向の分割数
	int sizeX_;                      // 各分割画像の横幅（ピクセル単位）
	int sizeY_;                      // 各分割画像の縦幅（ピクセル単位）

	// 複製されたモデルのハンドルリスト
	std::vector<int> duplicateModelIds_;
};