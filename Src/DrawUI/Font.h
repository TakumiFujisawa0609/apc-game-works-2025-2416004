#pragma once
#include <string>
#include <unordered_map>
#include <DxLib.h>

/// @brief フォント管理クラス（シングルトン）
class Font
{
public:

	/// @brief ハッシュ関数の定義（pairをunordered_mapのキーにするため）
	struct PairHash
	{
		std::size_t operator()(const std::pair<int, int>& p) const noexcept;
	};

	// --- フォント定数関連 ---
	static constexpr int FONT_TYPE_NORMAL = DX_FONTTYPE_NORMAL;                             // 通常のフォント
	static constexpr int FONT_TYPE_EDGE = DX_FONTTYPE_EDGE;                                 // 縁取りフォント
	static constexpr int FONT_TYPE_ANTIALIASING = DX_FONTTYPE_ANTIALIASING;                 // アンチエイリアスフォント
	static constexpr int FONT_TYPE_ANTIALIASING_EDGE = DX_FONTTYPE_ANTIALIASING_EDGE;       // アンチエイリアス + 縁取りフォント

	/// @brief デストラクタ
	/// @param void 
	~Font(void);

	/// @brief インスタンスを明示的に生成
	/// @param void 
	static void CreateInstance(void);

	/// @brief インスタンス取得
	/// @return Fontインスタンスの参照
	static Font& GetInstance(void);

	/// @brief フォントシステムの初期化
	/// @param void 
	void Initialize(void);

	/// @brief フォントの追加（ファイルから読み込み）
	/// @param fontId 識別用ID
	/// @param internalFontName フォント名（OSが認識する名称）
	/// @param fontPath フォントファイルのパス
	/// @param fontSize 基本サイズ
	/// @param fontWeight 太さ
	/// @param fontType フォントタイプ
	/// @return 成功ならtrue
	bool AddFont(const std::string& fontId, const std::string& internalFontName, 
		const std::string& fontPath, int fontSize, int fontWeight, int fontType);

	/// @brief 特定のフォントに関連するハンドルをすべて削除
	/// @param fontId 削除対象のID
	void RemoveFont(const std::string& fontId);

	/// @brief デフォルトで使用するフォントを設定
	/// @param fontId 設定するID
	void SetDefaultFont(const std::string& fontId);

	/// @brief 指定したフォントでテキスト描画
	/// @param fontId 使用するフォントID
	/// @param x 描画座標X
	/// @param y 描画座標Y
	/// @param text 描画文字列
	/// @param color 色
	/// @param fontSize サイズ変更（-1でデフォルト）
	/// @param fontType タイプ変更（-1でデフォルト）
	void DrawText(const std::string& fontId, int x, int y, const char* text, 
		int color, int fontSize = -1, int fontType = -1);

	/// @brief デフォルトフォントでテキスト描画
	/// @param x 描画座標X
	/// @param y 描画座標Y
	/// @param text 描画文字列
	/// @param color 色
	/// @param fontSize サイズ変更（-1でデフォルト）
	/// @param fontType タイプ変更（-1でデフォルト）
	void DrawDefaultText(int x, int y, const char* text, int color, 
		int fontSize = -1, int fontType = -1);

	/// @brief デフォルトフォントでのテキスト横幅を取得
	/// @param text 計測する文字列
	/// @return ピクセル幅
	int GetDefaultTextWidth(const std::string& text) const;

	/// @brief フォントリソースの解放とインスタンスの破棄
	/// @param void 
	void  DestroyInstance(void);

private:

	static Font* instance_; // シングルトンインスタンス

	// フォント関連
	std::unordered_map<std::string, std::unordered_map
		<std::pair<int, int>, int, PairHash>> fontHandles_;                        // フォントハンドルの管理マップ（ID -> (サイズ, タイプ) -> ハンドル）
	std::unordered_map<std::string, std::string> fontIdToInternalName_;            // フォントIDから内部フォント名を引くためのマップ
	std::unordered_map<std::pair<int, int>, int, PairHash> dynamicFontHandles_;    // 動的なフォント設定（サイズ/タイプ違い）のキャッシュ
	std::string defaultFont_;                                                      // デフォルトとして扱うフォントID
	std::unordered_map<std::string, std::string> fontNameMap_;                     // フォントIDに対応した内部フォント名のマップ

	/// @brief コンストラクタ
	/// @param void 
	Font(void);

	/// @brief 指定された条件のフォントハンドルを生成またはキャッシュから取得
	/// @param internalFontName フォント名
	/// @param fontSize フォントの大きさ
	/// @param fontWeight フォントの太さ
	/// @param fontType フォントのタイプ
	/// @return 生成されたフォントハンドル
	int GetDynamicFontHandle(const std::string& internalFontName, int fontSize, int fontWeight, int fontType);

};