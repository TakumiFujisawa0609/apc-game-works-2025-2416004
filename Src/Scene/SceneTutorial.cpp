#include "SceneTutorial.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Application.h"
#include "../DrawUI/Font.h"
#include "../Manager/System/TimeManager.h"
#include "../Manager/Generic/Camera.h"

// コンストラクタ
SceneTutorial::SceneTutorial(void)
    : pageIndex_(0)
{
}

// 読み込み
void SceneTutorial::Load(void)
{
    auto& res = ResourceManager::GetInstance();

    // 画像を3枚配列に格納
    tutorialImgs_.clear();
    tutorialImgs_.push_back(res.Load(ResourceManager::SRC::IMG_TUTORIAL_1).handleId_);
    tutorialImgs_.push_back(res.Load(ResourceManager::SRC::IMG_TUTORIAL_2).handleId_);
    tutorialImgs_.push_back(res.Load(ResourceManager::SRC::IMG_TUTORIAL_3).handleId_);
}

// 読み込み完了
void SceneTutorial::EndLoad(void)
{
}

// 初期化処理
void SceneTutorial::Initialize(void)
{
    SetMouseDispFlag(TRUE);

    pageIndex_ = 0;

    TimeManager::GetInstance().SetPaused(true);
    auto camera = SceneManager::GetInstance().GetCamera();
    camera->ChangeMode(Camera::MODE::FIXED_POINT);
}

// 更新処理
void SceneTutorial::Update(void)
{
    auto& input = InputManager::GetInstance();
    auto& sound = SoundManager::GetInstance();

    if (input.IsTrgMouseLeft() || input.IsTrgDown(KEY_INPUT_SPACE)|| 
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
    {
        sound.Play(SoundManager::SOUND::SE_SELECT);

        pageIndex_++;

        // 全ページ見終わったらゲーム本編へ
        if (pageIndex_ >= static_cast<int>(tutorialImgs_.size()))
        {
            TimeManager::GetInstance().SetPaused(false);

            Application::GetInstance().SetActiveUI(false);
            SceneManager::GetInstance().PopScene();
        }
    }
}

// 描画処理
void SceneTutorial::Draw(void)
{
    if (pageIndex_ >= 0 && pageIndex_ < static_cast<int>(tutorialImgs_.size()))
    {
        int handle = tutorialImgs_[pageIndex_];
        if (handle != -1)
        {
            // 2D描画を最前面にする設定
            SetUseZBuffer3D(FALSE);
            SetWriteZBuffer3D(FALSE);

            // 画面全体に描画
            DrawExtendGraph(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, handle, TRUE);

            SetUseZBuffer3D(TRUE);
            SetWriteZBuffer3D(TRUE);
        }
    }

    // ガイドテキスト（右下に表示）
    auto& font = Font::GetInstance();
    const char* guideText = "左クリックまたはスペースで次へ";
    int textX = Application::SCREEN_SIZE_X - 500;
    int textY = Application::SCREEN_SIZE_Y - 60;

    // 白背景でも見えるように縁取りありで描画
    font.DrawDefaultText(textX, textY, guideText, GetColor(255, 255, 255), 24, Font::FONT_TYPE_ANTIALIASING_EDGE);
}

// 解放処理
void SceneTutorial::Release(void)
{
    tutorialImgs_.clear();
}

void SceneTutorial::DrawDebug(void)
{
}