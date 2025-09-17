#pragma once

#include <memory>
#include "SceneBase.h"

class Grid;
class SceneUi;

class SceneTitle : public SceneBase
{
public:

    static constexpr int PAUSE_UI_COUNT = 2;

    SceneTitle(void);
    ~SceneTitle(void) = default;

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

    bool IsExitRequested(void) const;

private:
    Grid* grid_;

    std::unique_ptr<SceneUi> uiMain_;      // メインメニュー
    std::unique_ptr<SceneUi> uiHowToPlay_; // 遊び方サブメニュー

    int logo_;
    int movieHandle_;
    int operationHandle_;
    int playHandle_;
    int playHandle2_;
    int blackAlpha_;
    int pauseUiCount_;

    int atelierHandle_;
    int guildHandle_;
    int gardenHandle_;

    bool inHowToPlayMenu_;
    bool isDecided_;
    bool showBlackBackground_;
    bool exitRequested_;
    bool isPlay_;

    int howToPlayPage_; // 0=なし, 1=目標, 2=錬金

    void DrawDebug(void);

};
