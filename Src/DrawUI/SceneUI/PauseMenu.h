#pragma once

class PauseMenu
{
public:
    enum class MODE_POUSE
    {
        SELECT,             // 通常のメニュー選択
        HOW_TO_PLAY_MENU,   // 「遊び方」サブメニュー
        HOW_TO_PLAY_PAGE,   // サブメニューの個別ページ
        CONTROL             // 操作説明
    };

    PauseMenu(void);
    ~PauseMenu(void) = default;

    // 表示/非表示
    void Show(void);
    void Hide(void);
    bool IsVisible(void) const;

    // 更新・描画
    void Init(void);
    void Update(void);
    void Draw(void);
    void Release(void);

    // 結果取得
    bool IsDecisionMade(void) const;
    int GetSelectedIndex(void) const;

private:
    // メインメニュー項目
    std::vector<std::string> menuItems_;
    int currentIndex_;
    bool visible_;
    bool decisionMade_;

    int howToPlayPage_;   // 1=目標 2=錬金 3=アトリエ 4=ギルド 5=ガーデン

    // 表示モード
    MODE_POUSE mode_;
};
