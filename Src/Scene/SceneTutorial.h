#pragma once
#include <memory>
#include "SceneBase.h"

class SceneTutorial : public SceneBase
{
public:
    // コンストラクタ
    SceneTutorial(void);
    // デストラクタ
    ~SceneTutorial(void) = default;
    // 読み込み
    void Load(void) override;
    // 読み込み完了
    void EndLoad(void) override;
    // 初期化処理
    void Initialize(void) override;
    // 更新処理
    void Update(void) override;
    // 描画処理
    void Draw(void) override;
    // 解放処理
    void Release(void) override;

private:

    static constexpr int MAX_PAGES = 2;

    int pageIndex_;                // 現在表示している画像の番号 (0, 1, 2)
    std::vector<int> tutorialImgs_; // 画像ハンドルを格納する配列

    // 描画処理(デバッグ)
    void DrawDebug(void);

};