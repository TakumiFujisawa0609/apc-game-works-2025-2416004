#include "../Pch.h"
#include "SceneCredit.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Application.h"
#include "../DrawUI/Font.h"
#include "../Manager/Generic/ResourceManager.h"

SceneCredit::SceneCredit(void)
{
    handle_ = -1;
}

void SceneCredit::Initialize(void)
{
    // 画面中央(0)を基準に、上下にどれくらいずらすかを設定
    credits_ =
    {
        {"--- クレジット ---",                  -150, GetColor(0, 0, 0), 32},
        {"Unity Asset Store",               -100, GetColor(0, 0, 0), 32},
        {"3D Low Poly Fantasy Swords Pack", -50,  GetColor(0, 0, 0), 32},
        {"SE_BGM Source",                    50,  GetColor(0, 0, 0), 32},
        {"ニコニコモンズ",                   100, GetColor(0, 0, 0), 32},
        {"AudioStock",                       150, GetColor(0, 0, 0), 32}
    };

    handle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_OPTION).handleId_;
}

void SceneCredit::Update(void)
{
    auto& input = InputManager::GetInstance();
    if (input.IsTrgDown(KEY_INPUT_ESCAPE) ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::RIGHT))
    {
        SceneManager::GetInstance().PopScene();
    }
    
}

void SceneCredit::Draw(void)
{
    // 背景を暗くする
    DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, handle_, TRUE);

    auto& font = Font::GetInstance();
    int centerY = Application::SCREEN_SIZE_Y / 2;

    int centerX = (Application::SCREEN_SIZE_X / 2) - 50;

    const int offset = 100;

    const int unifiedSize = 32; 

    for (const auto& line : credits_)
    {
        // テキストの幅を取得
        int textWidth = font.GetDefaultTextWidth(line.text.c_str());

        int drawX = centerX - (textWidth / 2);
        int drawY = centerY + line.offsetCenterY;

        font.DrawDefaultText(drawX, drawY, line.text.c_str(), line.color, unifiedSize);
    }

    // 画面下部の案内も同様に少し左に寄せる
    int helpWidth = font.GetDefaultTextWidth("ESC/Bボタンで戻る");
    font.DrawDefaultText(centerX - (helpWidth / 2), Application::SCREEN_SIZE_Y - offset, "ESC/Bボタンで戻る", GetColor(0, 0, 0), 24);
}

void SceneCredit::Release(void)
{
}