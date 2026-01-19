#include "PauseMenu.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../DrawUI/Font.h"
#include "../../Application.h"

PauseMenu::PauseMenu(void)
    : currentIndex_(0)
    , visible_(false)
    , decisionMade_(false)
    , howToPlayPage_(0)
    , mode_(MODE_POUSE::SELECT)
    , controlHandle_(-1)
    , reninHandle_(-1)
    , mokihiHandle_(-1)
    , atelierHandle_(-1)
    , guildHandle_(-1)
    , gardenHandle_(-1)
{
    // メインメニュー
    menuItems_ = {
        "続ける",
        "遊び方",
        "操作説明",
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
    // 各種画像のロード
    reninHandle_   = LoadGraph((Application::PATH_IMAGE + "UI/renkint.png").c_str());
    mokihiHandle_  = LoadGraph((Application::PATH_IMAGE + "UI/mokuhyou.png").c_str());
    controlHandle_ = LoadGraph((Application::PATH_IMAGE + "UI/sousa.png").c_str());
    atelierHandle_ = LoadGraph((Application::PATH_IMAGE + "UI/atrieT.png").c_str());
    guildHandle_   = LoadGraph((Application::PATH_IMAGE + "UI/girudo.png").c_str());
    gardenHandle_  = LoadGraph((Application::PATH_IMAGE + "UI/gadenT.png").c_str());
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
        case 0: visible_ = false; break;          // 続ける
        case 1: mode_ = MODE_POUSE::HOW_TO_PLAY_PAGE; howToPlayPage_ = 1; break;
        case 2: mode_ = MODE_POUSE::CONTROL;break;
        case 3: SceneManager::GetInstance().GameEnd(); visible_ = false; break;
        }
    }
}

void PauseMenu::Draw(void)
{
    if (!visible_) return;

    const int screenW = Application::SCREEN_SIZE_X;
    const int screenH = Application::SCREEN_SIZE_Y;

    // --- 各種説明ページ（背景黒） ---
    if (mode_ == MODE_POUSE::HOW_TO_PLAY_PAGE || mode_ == MODE_POUSE::CONTROL)
    {
        DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);
        int handle = -1;
        if (mode_ == MODE_POUSE::CONTROL) handle = controlHandle_;
        else {
            switch (howToPlayPage_) {
            case 1: handle = mokihiHandle_; break;
            case 2: handle = reninHandle_; break;
            case 3: handle = atelierHandle_; break;
            case 4: handle = guildHandle_; break;
            case 5: handle = gardenHandle_; break;
            }
        }
        if (handle != -1) DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, handle, TRUE);
        DrawString(50, screenH - 30, "ESCまたはクリックで戻る", GetColor(200, 200, 200));
        return;
    }

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
    DeleteGraph(controlHandle_);
    DeleteGraph(reninHandle_);
    DeleteGraph(mokihiHandle_);
    DeleteGraph(atelierHandle_);
    DeleteGraph(guildHandle_);
    DeleteGraph(gardenHandle_);
}
