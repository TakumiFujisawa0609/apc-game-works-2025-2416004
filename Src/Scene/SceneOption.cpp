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
    sensitivitySlider_ = { startX, 500, 300, 20, 0.5f, false, "マウス感度" };

    // 右上の切り替えボタン設定
    int btnY = 50;
    settingBtn_ = { 800, btnY, 120, 40, "設定" };
    controlBtn_ = { 950, btnY, 120, 40, "操作説明" };

    auto camera = SceneManager::GetInstance().GetCamera();

    // チェックボックスの配置設定
    int cbX = Application::SCREEN_SIZE_X / 2 - 150;
    invertPitchCb_ = { cbX, 600, 24, nullptr, "カメラ上下反転" };
    invertYawCb_ = { cbX, 650, 24, nullptr, "カメラ左右反転" };

    handle_ = -1;
    handle2_ = -1;
    currentMode_ = DisplayMode::Settings; // 初期状態
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
    settingBtn_.textX = settingBtn_.x + ((settingBtn_.w - font.GetDefaultTextWidth(settingBtn_.label)) / 2) - 10;
    settingBtn_.textY = settingBtn_.y + (settingBtn_.h - 24) / 2;

    // 操作説明ボタンの文字位置を計算
    controlBtn_.textX = controlBtn_.x + ((controlBtn_.w - font.GetDefaultTextWidth(controlBtn_.label)) / 2) / 2;
    controlBtn_.textY = controlBtn_.y + (controlBtn_.h - 24) / 2;

    if (camera) {
        sensitivitySlider_.value = camera->GetSensitivity();
    }

    camera->ChangeMode(Camera::MODE::FIXED_POINT);
}

void SceneOption::Update(void)
{
    int mx, my;
    GetMousePoint(&mx, &my);
    bool isClick = InputManager::GetInstance().IsTrgMouseLeft();

    // モード切り替えボタンの判定
    if (isClick) {
        if (mx >= settingBtn_.x && mx <= settingBtn_.x + settingBtn_.w &&
            my >= settingBtn_.y && my <= settingBtn_.y + settingBtn_.h) {
            currentMode_ = DisplayMode::Settings;
        }
        if (mx >= controlBtn_.x && mx <= controlBtn_.x + controlBtn_.w &&
            my >= controlBtn_.y && my <= controlBtn_.y + controlBtn_.h) {
            currentMode_ = DisplayMode::Controls;
        }
    }

    // 設定モードの時だけスライダーを更新
    if (currentMode_ == DisplayMode::Settings) {
        UpdateSlider(bgmSlider_, mx);
        UpdateSlider(seSlider_, mx);
        UpdateSlider(sensitivitySlider_, mx);
    }

    if (currentMode_ == DisplayMode::Settings) {
        UpdateSlider(bgmSlider_, mx);
        UpdateSlider(seSlider_, mx);
        UpdateSlider(sensitivitySlider_, mx);

        // --- ここからチェックボックスの更新 ---
        auto camera = SceneManager::GetInstance().GetCamera();

        // 上下反転の判定 (判定範囲を横200pxくらいに広げています)
        if (isClick && mx >= invertPitchCb_.x && mx <= invertPitchCb_.x + 200 &&
            my >= invertPitchCb_.y && my <= invertPitchCb_.y + invertPitchCb_.size)
        {
            // 直接 Getter と Setter を呼ぶ
            bool current = camera->GetPitchInvert();
            camera->SetPitchInvert(!current);
        }

        // 左右反転の判定
        if (isClick && mx >= invertYawCb_.x && mx <= invertYawCb_.x + 200 &&
            my >= invertYawCb_.y && my <= invertYawCb_.y + invertYawCb_.size)
        {
            // 直接 Getter と Setter を呼ぶ
            bool current = camera->GetYawInvert();
            camera->SetYawInvert(!current);
        }
    }

    if (InputManager::GetInstance().IsTrgDown(KEY_INPUT_ESCAPE)) {
        Application::GetInstance().SetActiveUI(false);
        TimeManager::GetInstance().SetPaused(false);
        SceneManager::GetInstance().PopScene();
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
            if (slider.label[0] == 'B') {
                // 0.0-1.0 を 0-100 に変換してセット
                sm.SetMasterVolumeBGM(static_cast<int>(slider.value * 100));
            }
            else if (slider.label[0] == 'S' && slider.label[1] == 'E') {
                sm.SetMasterVolumeSE(static_cast<int>(slider.value * 100));
            }
            else if (slider.label[0] == 'M')
            {
                // InputManager ではなくカメラを直接いじる
                auto camera = SceneManager::GetInstance().GetCamera();
                if (camera)
                {
                    // slider.value(0.0~1.0) をそのまま、あるいは倍率をかけて渡す
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
    DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, handle_, TRUE);

   

    auto& font = Font::GetInstance();
    int black = GetColor(0, 0, 0);
    int activeColor = GetColor(255, 215, 0);
    int inactiveColor = GetColor(150, 150, 150);

    if (currentMode_ == DisplayMode::Controls)
    {
        DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, handle2_, TRUE);


        font.DrawText("Default", Application::SCREEN_SIZE_X - 500, Application::SCREEN_SIZE_Y - 50, "ESCキーで戻る", black, 36, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }

    // 描画するだけのシンプルな関数にする
    auto DrawTab = [&](const Button& b, bool isActive) 
        {
        DrawBox(b.x, b.y, b.x + b.w, b.y + b.h, isActive ? activeColor : inactiveColor, TRUE);
        DrawBox(b.x, b.y, b.x + b.w, b.y + b.h, black, FALSE);

        // 事前に計算しておいた座標で描画
        font.DrawText("Default", b.textX, b.textY, b.label, black, 24, Font::FONT_TYPE_ANTIALIASING_EDGE);
        };

    DrawTab(settingBtn_, currentMode_ == DisplayMode::Settings);
    DrawTab(controlBtn_, currentMode_ == DisplayMode::Controls);

    // --- コンテンツの描画 ---
    if (currentMode_ == DisplayMode::Settings) {
        // 既存のスライダー描画 (DrawSliderUIを呼び出す)
        auto DrawSliderUI = [&](const Slider& s) {
            font.DrawText("Default", s.x, s.y - 30, s.label, black, 20, Font::FONT_TYPE_ANTIALIASING_EDGE);
            DrawBox(s.x, s.y, s.x + s.width, s.y + s.height, inactiveColor, TRUE);
            DrawBox(s.x, s.y, s.x + static_cast<int>(s.width * s.value), s.y + s.height, activeColor, TRUE);
            int circleX = s.x + static_cast<int>(s.width * s.value);
            DrawCircle(circleX, s.y + s.height / 2, s.height / 2 + 3, black, TRUE);

            char valText[16];
            sprintf_s(valText, "%.0f", s.value * 10.0f);
            font.DrawText("Default", s.x + s.width + 15, s.y, valText, black, 20);
            };
        DrawSliderUI(bgmSlider_);
        DrawSliderUI(seSlider_);
        DrawSliderUI(sensitivitySlider_);

        font.DrawText("Default", 950, 750, "ESCキーで戻る", black, 36, Font::FONT_TYPE_ANTIALIASING_EDGE);

        auto camera = SceneManager::GetInstance().GetCamera();

        auto DrawCB = [&](const CheckBox& cb, bool isActive) {
            DrawBox(cb.x, cb.y, cb.x + cb.size, cb.y + cb.size, black, FALSE);
            if (isActive) {
                // チェックが入っているときは中に小さな箱を描くか、×印を描く
                DrawBox(cb.x + 4, cb.y + 4, cb.x + cb.size - 4, cb.y + cb.size - 4, activeColor, TRUE);
            }
            font.DrawText("Default", cb.x + cb.size + 10, cb.y, cb.label, black, 20);
            };

        DrawCB(invertPitchCb_, camera->GetPitchInvert());
        DrawCB(invertYawCb_, camera->GetYawInvert());
    }

}
void SceneOption::Release(void) {}