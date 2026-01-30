#include "PauseMenu.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Scene/SceneOption.h"
#include "../../Scene/SceneTutorial.h"
#include "../../DrawUI/Font.h"
#include "../../Application.h"
#include "../../Manager/System/TimeManager.h"

PauseMenu::PauseMenu(void)
    : currentIndex_(0)
    , visible_(false)
    , decisionMade_(false)
    , howToPlayPage_(0)
    , mode_(MODE_POUSE::SELECT)
{
    // メインメニュー
    menuItems_ = {
        "続ける",
        "遊び方",
        "設定",
        "ゲーム終了"
    };
}

void PauseMenu::Show(void)
{
    visible_ = true;
    currentIndex_ = 0;
    decisionMade_ = false;
    mode_ = MODE_POUSE::SELECT;
    SetMouseDispFlag(TRUE);
}

void PauseMenu::Hide(void)
{
    visible_ = false;
    decisionMade_ = false;
    mode_ = MODE_POUSE::SELECT;
    SetMouseDispFlag(FALSE);
    SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

}

bool PauseMenu::IsVisible(void) const
{
    return visible_;
}

bool PauseMenu::IsDecisionMade(void) const
{
    return decisionMade_;
}

int PauseMenu::GetSelectedIndex(void) const
{
    return currentIndex_;
}

void PauseMenu::Init(void)
{
}

void PauseMenu::Update(void)
{
    if (!visible_) return;

    SoundManager& sound = SoundManager::GetInstance();
    auto& input = InputManager::GetInstance();

    // 説明ページ表示中の処理
    if (mode_ == MODE_POUSE::HOW_TO_PLAY_PAGE || mode_ == MODE_POUSE::CONTROL)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE) || input.IsTrgMouseLeft())
        {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            mode_ = MODE_POUSE::SELECT;
        }
        return;
    }

    // ----- メインメニュー操作 -----
    // マウス判定
    Vector2 mousePos = input.GetMousePos();
    const int screenW = Application::SCREEN_SIZE_X;
    const int screenH = Application::SCREEN_SIZE_Y;
    const int boxW = 400;
    const int itemHeight = 50;
    const int boxH = static_cast<int>(menuItems_.size()) * itemHeight + 40;
    const int startX = (screenW - boxW) / 2;
    const int startY = (screenH - boxH) / 2 + 20;

    for (int i = 0; i < menuItems_.size(); ++i) {
        int rectTop = startY + i * itemHeight;
        if (mousePos.x >= startX && mousePos.x <= startX + boxW &&
            mousePos.y >= rectTop && mousePos.y <= rectTop + itemHeight) {
            if (currentIndex_ != i) {
                sound.Play(SoundManager::SOUND::SE_SELECT);
                currentIndex_ = i;
            }
        }
    }

    // キー入力
    if (input.IsTrgDown(KEY_INPUT_UP)) {
        sound.Play(SoundManager::SOUND::SE_SELECT);
        currentIndex_ = (currentIndex_ + menuItems_.size() - 1) % menuItems_.size();
    }
    if (input.IsTrgDown(KEY_INPUT_DOWN)) {
        sound.Play(SoundManager::SOUND::SE_SELECT);
        currentIndex_ = (currentIndex_ + 1) % menuItems_.size();
    }

    // 決定
    if (input.IsTrgDown(KEY_INPUT_SPACE) || input.IsTrgMouseLeft())
    {
        sound.Play(SoundManager::SOUND::SE_PUSH);
        switch (currentIndex_) {
        case 0: 
            visible_ = false; 
            break;          // 続ける
        case 1: 
            mode_ = MODE_POUSE::HOW_TO_PLAY_PAGE; howToPlayPage_ = 1;
            SceneManager::GetInstance().PushScene(std::make_shared<SceneTutorial>());
            visible_ = false;
            break;
        case 2: 
            mode_ = MODE_POUSE::CONTROL;
            SceneManager::GetInstance().PushScene(std::make_shared<SceneOption>());
            visible_ = false;
            break;
        case 3: 
            SceneManager::GetInstance().GameEnd(); visible_ = false; 
            break;
        }
    }
}

void PauseMenu::Draw(void)
{
    if (!visible_) return;

    const int screenW = Application::SCREEN_SIZE_X;
    const int screenH = Application::SCREEN_SIZE_Y;

   

    // --- 通常のポーズメニュー ---
    const int boxW = 400;
    const int itemHeight = 50;
    const int boxH = static_cast<int>(menuItems_.size()) * itemHeight + 40;
    const int x = (screenW - boxW) / 2;
    const int y = (screenH - boxH) / 2;

    DrawBox(x, y, x + boxW, y + boxH, GetColor(0, 0, 0), TRUE);
    DrawBox(x, y, x + boxW, y + boxH, GetColor(255, 255, 255), FALSE);

    for (int i = 0; i < menuItems_.size(); ++i)
    {
        int itemY = y + 20 + i * itemHeight;
        if (i == currentIndex_) {
            DrawBox(x + 10, itemY - 5, x + boxW - 10, itemY + 35, 0xFFFF00, FALSE);
        }
        Font::GetInstance().DrawDefaultText(x + (boxW / 2) - (Font::GetInstance().GetDefaultTextWidth(menuItems_[i]) / 2),
            itemY, menuItems_[i].c_str(), 0xffffff, 24, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }
}


void PauseMenu::Release(void)
{
}
