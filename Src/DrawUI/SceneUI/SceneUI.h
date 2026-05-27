#pragma once
#include <vector>
#include <string>

/// @brief シーン上のUI表示（選択リストや点滅演出）を管理するクラス
class SceneUi
{
public:
	/// @brief フォント描画用のデータ構造体
	struct FontData
	{
		std::string message; // 表示する文字列
	};

	/// @brief コンストラクタ
	/// @param void 
	SceneUi(void);

	/// @brief デストラクタ
	/// @param void 
	~SceneUi(void);

	/// @brief 更新処理
	/// @param void 
	void Update(void);

	/// @brief 描画処理
	/// @param baseYOverride 描画開始Y座標のオフセット（-1でデフォルト）
	void Draw(int baseYOverride = -1);

	/// @brief 表示する文字列項目を追加
	/// @param message 追加する文字列
	void AddCharacter(const char* message);

	/// @brief 現在の選択インデックスを設定
	/// @param index 設定するインデックス
	void SetCurrentIndex(int index);

	/// @brief 現在の選択インデックスを取得
	/// @return 選択中のインデックス
	int GetCurrentIndex(void) const;

	/// @brief 登録されている項目数を取得
	/// @return 項目総数
	int GetMaxIndex(void) const;

private:
	/// @brief 点滅状態の更新
	/// @param void 
	void UpdateBlinking(void);

	/// @brief フォントリストの描画
	/// @param baseY 描画開始Y座標
	void DrawFontList(int baseY);

private:
	// 定数
	static constexpr int kBlinkInterval = 30; // 点滅の間隔（フレーム数）
	static constexpr int kLineSpacing = 70;   // 行間
	static constexpr int kFontSize = 45;      // 文字サイズ

	// 制御用変数
	std::vector<FontData> fontList_; // 描画する項目のリスト
	int frameCount_;                 // アニメーション用フレームカウンタ
	bool isVisible_;                 // 点滅制御用の表示フラグ
	int currentIndex_;               // 現在選択されている項目の番号
};