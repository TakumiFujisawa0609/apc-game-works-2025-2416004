#include "SceneUi.h"
#include "../../DrawUI/Font.h"
#include "../../Application.h"

SceneUi::SceneUi(void)
    : frameCount_(0), isBlinking_(false), currentIndex_(0)
{
}

SceneUi::~SceneUi(void)
{
}

void SceneUi::Draw(int baseYOverride)
{
    DrawFont(baseYOverride); // 上のオーバーロード呼ぶ
}


void SceneUi::FontBlinking(void)
{
    frameCount_++;
    isBlinking_ = (frameCount_ / blinkInterval_) % 2 ? true : false;
}

void SceneUi::DrawFont(int baseYOverride)
{
    int baseY = baseYOverride;   // 呼び出し側から指定されたY座標
    int spacing = 80;

    for (size_t i = 0; i < fontList_.size(); ++i)
    {
        const auto& font = fontList_[i];
        int textWidth = GetDrawStringWidth(font.message.c_str(), (int)font.message.length());

        int xPos = (Application::SCREEN_SIZE_X / 2) - textWidth;
        int yPos = baseY + (int)i * spacing;

        int color = ((int)i == currentIndex_) ? GetColor(255, 255, 0) : GetColor(170, 170, 170);
        Font::GetInstance().DrawDefaultText(xPos, yPos, font.message.c_str(), color, 42, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }
}

void SceneUi::AddCharctor(const char* _char)
{
    fontList_.push_back(FontData{ _char });
}

void SceneUi::SetCurrentIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(fontList_.size()))
    {
        currentIndex_ = index;
    }
}

int SceneUi::GetCurrentIndex(void) const
{
    return currentIndex_;
}

int SceneUi::GetMaxIndex(void) const
{
    return static_cast<int>(fontList_.size());
}
