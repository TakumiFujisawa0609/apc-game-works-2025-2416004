#include "PauseMenu.h"
#include <DxLib.h>
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../DrawUI/Font.h"
#include "../../Application.h"

PauseMenu::PauseMenu(void)
    : currentIndex_(0), visible_(false), decisionMade_(false),
      howToPlayIndex_(0), howToPlayPage_(0), mode_(MODE_POUSE::SELECT),
      controlHandle_(-1), reninHandle_(-1), mokihiHandle_(-1),
      atelierHandle_(-1), guildHandle_(-1), gardenHandle_(-1)
{
    // メインメニュー
    menuItems_ = {
        "続ける",
        "遊び方",
        "操作説明",
        "ゲーム終了"
    };

    // サブメニュー
    howToPlayItems_ = {
        "目標について",
        "錬金について",
        "アトリエについて",
        "ギルドについて",
        "ガーデンについて",
        "戻る"
    };
}

void PauseMenu::Show(void)
{
    visible_ = true;
    currentIndex_ = 0;
    decisionMade_ = false;
    mode_ = MODE_POUSE::SELECT;
}

void PauseMenu::Hide(void)
{
    visible_ = false;
    decisionMade_ = false;
    mode_ = MODE_POUSE::SELECT;
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
    SoundManager& sound = SoundManager::GetInstance();
    auto& input = InputManager::GetInstance();

    if (mode_ == MODE_POUSE::SELECT)
    {
        if (input.IsTrgDown(KEY_INPUT_UP)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            currentIndex_ = (currentIndex_ + menuItems_.size() - 1) % menuItems_.size();
        }
        if (input.IsTrgDown(KEY_INPUT_DOWN)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            currentIndex_ = (currentIndex_ + 1) % menuItems_.size();
        }

        if (input.IsTrgDown(KEY_INPUT_RETURN))
        {
            sound.Play(SoundManager::SOUND::SE_PUSH);
            if (currentIndex_ == 0) {           // 続ける
                visible_ = false;
            }
            else if (currentIndex_ == 1) {      // 遊び方
                mode_ = MODE_POUSE::HOW_TO_PLAY_MENU;
                howToPlayIndex_ = 0;
            }
            else if (currentIndex_ == 2) {      // 操作説明
                mode_ = MODE_POUSE::CONTROL;
            }
            else if (currentIndex_ == 3) {      // ゲーム終了
                decisionMade_ = true;
            }
        }
    }
    else if (mode_ == MODE_POUSE::HOW_TO_PLAY_MENU)
    {
        if (input.IsTrgDown(KEY_INPUT_UP)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            howToPlayIndex_ = (howToPlayIndex_ + howToPlayItems_.size() - 1) % howToPlayItems_.size();
        }
        if (input.IsTrgDown(KEY_INPUT_DOWN)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            howToPlayIndex_ = (howToPlayIndex_ + 1) % howToPlayItems_.size();
        }
        if (input.IsTrgDown(KEY_INPUT_RETURN)) {
            if (howToPlayIndex_ == howToPlayItems_.size() - 1) {
                mode_ = MODE_POUSE::SELECT; // 戻る
            } else {
                howToPlayPage_ = howToPlayIndex_ + 1;
                mode_ = MODE_POUSE::HOW_TO_PLAY_PAGE;
            }
        }
        if (input.IsTrgDown(KEY_INPUT_ESCAPE)) {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            mode_ = MODE_POUSE::SELECT;
        }
    }
    else if (mode_ == MODE_POUSE::HOW_TO_PLAY_PAGE)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE)) {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            mode_ = MODE_POUSE::HOW_TO_PLAY_MENU;
        }
    }
    else if (mode_ == MODE_POUSE::CONTROL)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE)) {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            mode_ = MODE_POUSE::SELECT;
        }
    }
}

void PauseMenu::Draw(void)
{
    if (!visible_) return;

    const int screenW = Application::DEFA_SCREEN_SIZE_X;
    const int screenH = Application::DEFA_SCREEN_SZIE_Y;

    // --- 「遊び方」サブメニュー ---
    if (mode_ == MODE_POUSE::HOW_TO_PLAY_MENU)
    {
        DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);

        const int fontSize = 32;
        const int startY = screenH / 2 - (howToPlayItems_.size() * (fontSize + 20)) / 2;

        for (int i = 0; i < howToPlayItems_.size(); ++i) {
            int itemY = startY + i * (fontSize + 25);
            int textWidth = Font::GetInstance().GetDefaultTextWidth(howToPlayItems_[i]);
            int textX = (screenW / 2) - textWidth;
            const int marginX = 75; // ← 横の余白を増やす
            const int marginY = 8;  // ← 縦の余白も少し広げる

            if (i == howToPlayIndex_) {
                int boxW = textWidth + marginX * 2;
                int boxX = (screenW - boxW) / 2;
                DrawBox(boxX, itemY - marginY,
                    boxX + boxW, itemY + fontSize + marginY,
                    0xFFFF00, FALSE);
            }

            // フォントクラスを使って描画
            Font::GetInstance().DrawDefaultText(
                textX,
                itemY,
                howToPlayItems_[i].c_str(),
                GetColor(255, 255, 255),
                fontSize,
                Font::FONT_TYPE_ANTIALIASING_EDGE
            );
        }

        Font::GetInstance().DrawDefaultText(
            50,
            screenH - 50,
            "SPACEで決定 / ESCで戻る",
            GetColor(200, 200, 200),
            24,
            Font::FONT_TYPE_ANTIALIASING_EDGE
        );

        return;
    }


    // --- 「遊び方」個別ページ ---
    if (mode_ == MODE_POUSE::HOW_TO_PLAY_PAGE)
    {
        DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);

        if (howToPlayPage_ == 1) {
            DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, mokihiHandle_, TRUE);
        }
        else if (howToPlayPage_ == 2) {
            DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, reninHandle_, TRUE);
        }
        else if (howToPlayPage_ == 3) {
            DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, atelierHandle_, TRUE);
        }
        else if (howToPlayPage_ == 4) {
            DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, guildHandle_, TRUE);
        }
        else if (howToPlayPage_ == 5) {
            DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, gardenHandle_, TRUE);
        }

        DrawString(50, screenH - 30, "ESCキーで戻る", GetColor(200, 200, 200));
        return;
    }

    // --- 操作説明 ---
    if (mode_ == MODE_POUSE::CONTROL)
    {
        DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);
        DrawRotaGraph(screenW / 2, screenH / 2, 1.0, 0.0, controlHandle_, TRUE);
        DrawString(50, screenH - 30, "ESCキーで戻る", GetColor(200, 200, 200));
        return;
    }

    // --- 通常のポーズメニュー ---
    const int boxW = 400;
    const int boxH = static_cast<int>(menuItems_.size()) * 50 + 40;

    const int x = (screenW - boxW) / 2;
    const int y = (screenH - boxH) / 2;

    DrawBox(x, y, x + boxW, y + boxH, GetColor(0, 0, 0), TRUE);   // 背景
    DrawBox(x, y, x + boxW, y + boxH, GetColor(255, 255, 255), FALSE); // 枠

    for (int i = 0; i < menuItems_.size(); ++i)
    {
        int itemY = y + 20 + i * 50;
        if (i == currentIndex_) {
            DrawBox(x + 10, itemY - 5, x + boxW - 10, itemY + 30, 0xFFFF00, false);
        }
        Font::GetInstance().DrawDefaultText(x + 150, itemY, menuItems_[i].c_str(),
            0xffffff, 24, Font::FONT_TYPE_ANTIALIASING_EDGE);
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
