#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>

class SceneCredit : public SceneBase
{
public:
    SceneCredit(void);
    ~SceneCredit(void) override = default;

    void Initialize(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;
    void Load(void) override {}
    void EndLoad(void) override {}

private:
    struct CreditLine
    {
        std::string text;
        int offsetCenterY; // 画面中央からの上下オフセット
        unsigned int color;
        int fontSize;
    };

    int handle_;

    std::vector<CreditLine> credits_;
};