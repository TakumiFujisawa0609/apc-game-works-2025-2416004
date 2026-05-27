#pragma once
#include <vector>
#include <string>

/// @brief ポーズメニューの管理クラス
class PauseMenu
{
public:
	/// @brief ポーズメニューの表示状態モード
	enum class MODE_POUSE
	{
		SELECT,             // 通常のメニュー選択
		HOW_TO_PLAY_MENU,   // 「遊び方」サブメニュー
		HOW_TO_PLAY_PAGE,   // サブメニューの個別ページ
		CONTROL             // 操作説明
	};

	/// @brief コンストラクタ
	/// @param void 
	PauseMenu(void);

	/// @brief デストラクタ
	/// @param void 
	~PauseMenu(void) = default;

	/// @brief メニューを表示する
	/// @param void 
	void Show(void);

	/// @brief メニューを非表示にする
	/// @param void 
	void Hide(void);

	/// @brief メニューが表示中か取得
	/// @return 表示中ならtrue
	bool IsVisible(void) const;

	/// @brief 初期化処理
	/// @param void 
	void Initialize(void);

	/// @brief 更新処理
	/// @param void 
	void Update(void);

	/// @brief 描画処理
	/// @param void 
	void Draw(void);

	/// @brief 解放処理
	/// @param void 
	void Release(void);

	/// @brief 決定がなされたか取得
	/// @return 決定済みならtrue
	bool IsDecisionMade(void) const;

	/// @brief 現在選択されている項目のインデックスを取得
	/// @return 選択インデックス
	int GetSelectedIndex(void) const;

private:

	// 描画関連
	const int KMENU_BOX_WIDTH = 400;    // メニュー枠の横幅
	const int KMENU_ITEM_HEIGHT = 50;   // 1項目あたりの高さ
	const int KMENU_PADDING_Y= 20;     // 上下の余白

	// メニュー関連
	std::vector<std::string> menuItems_; // メインメニュー項目名リスト
	int currentIndex_;                   // 現在の選択インデックス
	bool visible_;                       // 表示フラグ
	bool decisionMade_;                  // 決定フラグ

	// ページ管理
	int howToPlayPage_;                  

	// ステック判定
	bool stickFree_;

	// 表示モード
	MODE_POUSE mode_;                    
};