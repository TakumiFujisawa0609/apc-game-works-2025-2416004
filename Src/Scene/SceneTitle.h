#pragma once
#include "SceneBase.h"
#include "../DrawUI/SceneUI/SceneUi.h"

class Grid;

class SceneTitle : public SceneBase
{
public:

    static constexpr int PAUSE_UI_COUNT = 2;

    SceneTitle(void);
    ~SceneTitle(void) = default;

    void Initialize(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;

    bool IsExitRequested(void) const;

    void Load(void) override;
    void EndLoad(void) override;


private:
    Grid* grid_;

    std::unique_ptr<SceneUi> uiMain_;


    int logo_;
    int movieHandle_;
    int pauseUiCount_;

    bool isDecided_;
    bool showBlackBackground_;
    bool exitRequested_;
    bool isPlay_;


    int howToPlayPage_;

    void DrawDebug(void);

};
