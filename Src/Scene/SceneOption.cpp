#include "../Pch.h"
#include "SceneOption.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Application.h"
#include "../Manager/System/TimeManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../DrawUI/Font.h"
#include "../Manager/Generic/Camera.h"

SceneOption::SceneOption(void)
{
    int startX = Application::SCREEN_SIZE_X / 2 - 150;
    bgmSlider_ = { startX, 300, 300, 20, 0.7f, false, "BGM音量" };
    seSlider_ = { startX, 400, 300, 20, 0.8f, false, "SE音量" };

    int btnY = 50;
    settingBtn_ = { 800, btnY, 120, 40, "設定" };
    controlBtn_ = { 950, btnY, 120, 40, "操作説明" };

    auto camera = SceneManager::GetInstance().GetCamera();

    int cbX = Application::SCREEN_SIZE_X / 2 - 150;
    invertPitchCb_ = { cbX, 600, 24, nullptr, "カメラ上下反転" };
    invertYawCb_ = { cbX, 650, 24, nullptr, "カメラ左右反転" };

    handle_ = -1;
    handle2_ = -1;
    currentMode_ = DISPLAY_MODE ::Settings;

    selectIndex_ = 0;
    stickFree_ = true;
}

// 読み込み
void SceneOption::Load(void)
{
    auto& res = ResourceManager::GetInstance();
    
    handle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_OPTION).handleId_;

    handle2_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::IMG_OPTION_1).handleId_;
}

// 読み込み完了
void SceneOption::EndLoad(void)
{
}


void SceneOption::Initialize(void) 
{
    SetMouseDispFlag(TRUE);

    // SoundManagerから現在の音量を取得して反映
    auto& sm = SoundManager::GetInstance();
    bgmSlider_.value = sm.GetMasterVolumeBGM() / 100.0f;
    seSlider_.value = sm.GetMasterVolumeSE() / 100.0f;

    TimeManager::GetInstance().SetPaused(true);
    Application::GetInstance().SetActiveUI(true);

    auto camera = SceneManager::GetInstance().GetCamera();
    auto& font = Font::GetInstance();

    // 設定ボタンの文字位置を計算
    settingBtn_.textX = settingBtn_.x + ((settingBtn_.w - 
        font.GetDefaultTextWidth(settingBtn_.label)) / 2) - 10;

    settingBtn_.textY = settingBtn_.y + (settingBtn_.h - 24) / 2;

    // 操作説明ボタンの文字位置を計算
    controlBtn_.textX = controlBtn_.x + ((controlBtn_.w - 
        font.GetDefaultTextWidth(controlBtn_.label)) / 2) / 2;

    controlBtn_.textY = controlBtn_.y + (controlBtn_.h - 24) / 2;

    if (camera) 
    {
        camera->ChangeMode(Camera::MODE::FIXED_POINT);
    }
}

void SceneOption::Update(void)
{
    // 各種マネージャー・カメラの取得関連
    auto& input = InputManager::GetInstance();
    auto& sm = SoundManager::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();

    // マウス入力取得関連
    int mx, my;
    GetMousePoint(&mx, &my);
    bool isClick = InputManager::GetInstance().IsTrgMouseLeft();

    // パッド・キーボード入力取得関連
    InputManager::JOYPAD_IN_STATE padState;
    padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    VECTOR padDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    bool isPushA = input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
    bool isPushB = input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);

    // パッドによる項目選択（上下移動）関連
    if (stickFree_)
    {
        if (padDir.z > 0.5f) // スティック上方向
        {
            selectIndex_ = (selectIndex_ - 1 + 6) % 6; // 6項目でループ
            sm.Play(SoundManager::SOUND::SE_SELECT);
            stickFree_ = false;
        }
        else if (padDir.z < -0.5f) // スティック下方向
        {
            selectIndex_ = (selectIndex_ + 1) % 6;
            sm.Play(SoundManager::SOUND::SE_SELECT);
            stickFree_ = false;
        }
    }

    // 決定ボタン（Aボタン等）による処理関連
    if (isPushA)
    {
        if (selectIndex_ == 0) // モード切替：設定
        {
            currentMode_ = DISPLAY_MODE::Settings;
            sm.Play(SoundManager::SOUND::SE_PUSH);
        }
        else if (selectIndex_ == 1) // モード切替：操作
        {
            currentMode_ = DISPLAY_MODE::Controls;
            sm.Play(SoundManager::SOUND::SE_PUSH);
        }
        else if (currentMode_ == DISPLAY_MODE::Settings)
        {
            if (selectIndex_ == 4) // カメラ上下反転
            {
                camera->SetPitchInvert(!camera->GetPitchInvert());
                sm.Play(SoundManager::SOUND::SE_PUSH);
            }
            else if (selectIndex_ == 5) // カメラ左右反転
            {
                camera->SetYawInvert(!camera->GetYawInvert());
                sm.Play(SoundManager::SOUND::SE_PUSH);
            }
        }
    }

    // 設定モード中の値調整（左右移動）関連
    if (currentMode_ == DISPLAY_MODE::Settings)
    {
        float moveSpeed = 0.01f; // スライダーの変動速度

        if (selectIndex_ == 2) // BGM音量
        {
            if (padDir.x > 0.5f) { bgmSlider_.value = std::clamp(bgmSlider_.value + moveSpeed, 0.0f, 1.0f); }
            if (padDir.x < -0.5f) { bgmSlider_.value = std::clamp(bgmSlider_.value - moveSpeed, 0.0f, 1.0f); }
            sm.SetMasterVolumeBGM(static_cast<int>(bgmSlider_.value * 100));
        }
        else if (selectIndex_ == 3) // SE音量
        {
            if (padDir.x > 0.5f) { seSlider_.value = std::clamp(seSlider_.value + moveSpeed, 0.0f, 1.0f); }
            if (padDir.x < -0.5f) { seSlider_.value = std::clamp(seSlider_.value - moveSpeed, 0.0f, 1.0f); }
            sm.SetMasterVolumeSE(static_cast<int>(seSlider_.value * 100));
        }
    }

    // スティックの遊び・連打防止関連
    if (fabs(padDir.z) < 0.2f && fabs(padDir.x) < 0.2f)
    {
        stickFree_ = true;
    }

    // マウス操作関連
    if (isClick)
    {
        // ボタンクリック判定（モード切替）
        if (mx >= settingBtn_.x && mx <= settingBtn_.x + settingBtn_.w &&
            my >= settingBtn_.y && my <= settingBtn_.y + settingBtn_.h) 
        {
            currentMode_ = DISPLAY_MODE::Settings;
            selectIndex_ = 0;
        }
        if (mx >= controlBtn_.x && mx <= controlBtn_.x + controlBtn_.w &&
            my >= controlBtn_.y && my <= controlBtn_.y + controlBtn_.h) 
        {
            currentMode_ = DISPLAY_MODE::Controls;
            selectIndex_ = 1;
        }

        // 設定画面内での個別クリック判定
        if (currentMode_ == DISPLAY_MODE::Settings)
        {
            UpdateSlider(bgmSlider_, mx);
            UpdateSlider(seSlider_, mx);

            if (IsMouseOver(bgmSlider_, mx, my)) { selectIndex_ = 2; }
            if (IsMouseOver(seSlider_, mx, my)) { selectIndex_ = 3; }

            // チェックボックス判定（上下・左右反転）
            if (mx >= invertPitchCb_.x && mx <= invertPitchCb_.x + 200 &&
                my >= invertPitchCb_.y && my <= invertPitchCb_.y + invertPitchCb_.size) 
            {
                camera->SetPitchInvert(!camera->GetPitchInvert());
                selectIndex_ = 4;
            }
            if (mx >= invertYawCb_.x && mx <= invertYawCb_.x + 200 &&
                my >= invertYawCb_.y && my <= invertYawCb_.y + invertYawCb_.size)
            {
                camera->SetYawInvert(!camera->GetYawInvert());
                selectIndex_ = 5;
            }
        }
    }

    // シーン終了（戻る）操作関連
    bool isExitPad = input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, 
        InputManager::JOYPAD_BTN::RIGHT);

    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE) || isExitPad)
    {
        Application::GetInstance().SetActiveUI(false);
        TimeManager::GetInstance().SetPaused(false);
        SceneManager::GetInstance().PopScene(); // オプションシーンを閉じる
    }
}
void SceneOption::UpdateSlider(Slider& slider, int mx)
{
    bool isMousePush = (GetMouseInput() & MOUSE_INPUT_LEFT);
    int my; GetMousePoint(&mx, &my);

    if (isMousePush)
    {
        if (IsMouseOver(slider, mx, my) || slider.isDragging)
        {
            slider.isDragging = true;
            float newValue = static_cast<float>(mx - slider.x) / slider.width;
            slider.value = std::clamp(newValue, 0.0f, 1.0f);

            auto& sm = SoundManager::GetInstance();
            auto& im = InputManager::GetInstance();

            // ラベルの先頭文字で判別
            if (slider.label[0] == 'B')
            {
                sm.SetMasterVolumeBGM(static_cast<int>(slider.value * 100));
            }
            else if (slider.label[0] == 'S' && slider.label[1] == 'E')
            {
                sm.SetMasterVolumeSE(static_cast<int>(slider.value * 100));
            }
            else if (slider.label[0] == 'M')
            {
                auto camera = SceneManager::GetInstance().GetCamera();
                if (camera)
                {
                    camera->SetSensitivity(slider.value);
                }
            }
        }
    }
    else
    {
        slider.isDragging = false;
    }
}

bool SceneOption::IsMouseOver(const Slider& slider, int mx, int my) 
{
    return (mx >= slider.x && mx <= slider.x + slider.width &&
        my >= slider.y && my <= slider.y + slider.height);
}

void SceneOption::Draw(void)
{
    DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, handle_, true);

    auto& font = Font::GetInstance();
    auto camera = SceneManager::GetInstance().GetCamera();
    int black = GetColor(0, 0, 0);
    int activeColor = GetColor(255, 215, 0);
    int inactiveColor = GetColor(150, 150, 150);
    int focusColor = GetColor(255, 0, 0);

    if (currentMode_ == DISPLAY_MODE::Controls)
    {
        DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, 
            Application::SCREEN_SIZE_Y, handle2_, true);

        font.DrawText("Default", Application::SCREEN_SIZE_X - 500, Application::SCREEN_SIZE_Y - 50,
            "ESCキー(Bボタン)で戻る", black, 36, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }

    // タブの描画 (index 0, 1)
    auto DrawTab = [&](const Button& b, bool isActive, bool isFocused)
        {
            DrawBox(b.x, b.y, b.x + b.w, b.y + b.h, isActive ? 
                GetColor(180, 180, 180) : inactiveColor, true);

            if (isFocused)
            {
                DrawBox(b.x - 3, b.y - 3, b.x + b.w + 3, b.y + b.h + 3,
                    focusColor, false);
            }

            DrawBox(b.x, b.y, b.x + b.w, b.y + b.h, black, false);

            font.DrawText("Default", b.textX, b.textY, b.label, isFocused ? focusColor : black, 
                24, Font::FONT_TYPE_ANTIALIASING_EDGE);
        };

    DrawTab(settingBtn_, currentMode_ == DISPLAY_MODE::Settings, selectIndex_ == 0);
    DrawTab(controlBtn_, currentMode_ == DISPLAY_MODE::Controls, selectIndex_ == 1);

    if (currentMode_ == DISPLAY_MODE::Settings)
    {
        auto DrawSliderUI = [&](const Slider& s, bool isFocused)
            {
                int textColor = isFocused ? focusColor : black;

                font.DrawText("Default", s.x, s.y - 30, s.label, textColor,
                    20, Font::FONT_TYPE_ANTIALIASING_EDGE);

                DrawBox(s.x, s.y, s.x + s.width, s.y + s.height, inactiveColor, true);

                DrawBox(s.x, s.y, s.x + static_cast<int>(s.width * s.value), 
                    s.y + s.height, activeColor, true);

                int circleX = s.x + static_cast<int>(s.width * s.value);
                DrawCircle(circleX, s.y + s.height / 2, s.height / 2 + 5,
                    isFocused ? focusColor : black, true);

                char valText[16];
                sprintf_s(valText, "%.0f", s.value * 10.0f);
                font.DrawText("Default", s.x + s.width + 15, s.y, valText, textColor, 20);
            };

        auto DrawCB = [&](const CheckBox& cb, bool isChecked, bool isFocused)
            {
                int textColor = isFocused ? focusColor : black;
                DrawBox(cb.x, cb.y, cb.x + cb.size, cb.y + cb.size,
                    isFocused ? focusColor : black, false);

                if (isChecked)
                {
                    DrawBox(cb.x + 4, cb.y + 4, cb.x + cb.size - 4, cb.y + cb.size - 4,
                        isFocused ? focusColor : activeColor, true);
                }

                font.DrawText("Default", cb.x + cb.size + 10, cb.y, cb.label, textColor, 20);
            };

        DrawSliderUI(bgmSlider_, selectIndex_ == 2);
        DrawSliderUI(seSlider_, selectIndex_ == 3);
        DrawCB(invertPitchCb_, camera->GetPitchInvert(), selectIndex_ == 4);
        DrawCB(invertYawCb_, camera->GetYawInvert(), selectIndex_ == 5);

        font.DrawText("Default", 950, 750, "ESCキー(Bボタン)で戻る", black, 
            36, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }
}
void SceneOption::Release(void) {}