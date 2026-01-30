#pragma once

class SceneUi
{
public:
    struct FontData
    {
        std::string message;  // 表示する文字列
    };

    SceneUi(void);
    ~SceneUi(void);

    void Draw(int baseYOverride = -1);
    void FontBlinking(void);
    void DrawFont(int baseYOverride);

    void AddCharctor(const char* _char);
    void SetCurrentIndex(int index);
    int GetCurrentIndex(void) const;
    int GetMaxIndex(void) const;

private:
    int frameCount_;
    const int blinkInterval_ = 30;
    bool isBlinking_;
    std::vector<FontData> fontList_;
    int currentIndex_;  // 現在選択中のインデックス
};

