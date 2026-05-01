#pragma once
#include "SceneBase.h"

class SceneOption : public SceneBase
{
public:
    SceneOption(void);
    ~SceneOption(void) override = default;


    // 読み込み
    void Load(void) override;
    // 読み込み完了
    void EndLoad(void) override;

    void Initialize(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

private:

    enum class DisplayMode {
        Settings,    // 設定（スライダー）
        Controls     // 操作説明
    };
    DisplayMode currentMode_ = DisplayMode::Settings;

    struct Button {
        int x, y, w, h;
        const char* label;
        int textX; 
        int textY; 
    };

    // スライダーの構造体
    struct Slider {
        int x, y, width, height;
        float value; // 0.0f ～ 1.0f
        bool isDragging;
        const char* label;
    };

    struct CheckBox {
        int x, y;
        int size;
        bool* pTarget; 
        const char* label;
    };

    Slider bgmSlider_;
    Slider seSlider_;
    Slider sensitivitySlider_;

    Button settingBtn_;
    Button controlBtn_;

    CheckBox invertPitchCb_;
    CheckBox invertYawCb_;

    int handle_;
    int handle2_;

    // マウスが矩形内にあるかチェック
    bool IsMouseOver(const Slider& slider, int mx, int my);
    // スライダーの更新ロジック
    void UpdateSlider(Slider& slider, int mx);
};