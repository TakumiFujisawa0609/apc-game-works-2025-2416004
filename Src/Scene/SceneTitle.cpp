#include "SceneTitle.h"

#include <DxLib.h>
#include "../Manager/Generic/Resource.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/Generic/Camera.h"
#include "../Scene/SceneGame.h"
#include "../Object/Grid.h"
#include "../Application.h"
#include "../DrawUI/Font.h"
#include "../Manager/System/Loading.h"
#include "../Manager/System/TimeManager.h"

SceneTitle::SceneTitle(void)
{
   logo_ = -1;
   grid_ = nullptr;
   isDecided_ = false;
   blackAlpha_ = 0;
   operationHandle_ = -1;
   movieHandle_ = -1;
   showBlackBackground_ = false;
   playHandle_ = -1;
   playHandle2_ = -1;
   isPlay_ = false;
   exitRequested_ = false;
   howToPlayPage_ = 0;
   atelierHandle_ = -1;
   gardenHandle_ = -1;
   guildHandle_ = -1;
   inHowToPlayMenu_ = false;
   pauseUiCount_ = 0;
}

void SceneTitle::Load(void)
{
    // isLoading_ を true に
    SceneBase::Load(); 

    // BGM・SEロード
    

    // 音量調整

    // ロゴ・操作説明・再生用画像ロード

    // その他画像

    // 動画ロード
   
    // UI 初期化
    uiMain_ = std::make_unique<SceneUi>();
    uiMain_->AddCharctor("開始");
    uiMain_->AddCharctor("遊び方");
    uiMain_->AddCharctor("操作説明");
    uiMain_->AddCharctor("クレジット");
    uiMain_->AddCharctor("ゲーム終了");
    uiMain_->SetCurrentIndex(0);

    uiHowToPlay_ = std::make_unique<SceneUi>();
    uiHowToPlay_->AddCharctor("目標について");
    uiHowToPlay_->AddCharctor("戦闘方法");
    uiHowToPlay_->AddCharctor("化学反応");
    uiHowToPlay_->AddCharctor("戻る");
    uiHowToPlay_->SetCurrentIndex(0);

    //時間カウントリセット
    TimeManager::GetInstance().Reset();
}

void SceneTitle::EndLoad(void)
{
    SceneBase::EndLoad();
}

void SceneTitle::Init(void)
{
    // --- カメラ設定 ---
    auto camera = SceneManager::GetInstance().GetCamera();
    camera->ChangeMode(Camera::MODE::FIXED_POINT);

    // --- グリッド生成 ---
    grid_ = new Grid();
    grid_->Init();

    // --- UI初期化 ---
    inHowToPlayMenu_ = false;
    howToPlayPage_ = 0;
    showBlackBackground_ = false;
    isDecided_ = false;
    exitRequested_ = false;
    isPlay_ = true;
    pauseUiCount_ = PAUSE_UI_COUNT;

    // UIはLoadで生成済みなので、ここで初期位置設定
    if (uiMain_) uiMain_->SetCurrentIndex(0);
    if (uiHowToPlay_) uiHowToPlay_->SetCurrentIndex(0);
}

void SceneTitle::Update(void)
{
    auto& sound = SoundManager::GetInstance();
    auto& input = InputManager::GetInstance();

    // --- ESCキーで黒背景を閉じる ---
    if (showBlackBackground_)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE))
        {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            showBlackBackground_ = false;
            uiMain_->SetCurrentIndex(0); // 最初の項目「開始」に戻す
        }
        return; // 背景表示中は他の処理をしない
    }

    // --- 遊び方説明ページ表示中 ---
    if (howToPlayPage_ > 0)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE))
        {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            howToPlayPage_ = 0; // 説明を閉じる
            uiMain_->SetCurrentIndex(0); // 最初の項目に戻す
        }
        return;
    }

    // ----- メインメニュー操作 -----
    if (!inHowToPlayMenu_)
    {
        auto ui = uiMain_.get();
        int currentIndex = ui->GetCurrentIndex();
        int maxIndex = ui->GetMaxIndex() - 1;

        if (input.IsTrgDown(KEY_INPUT_UP)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            currentIndex = (currentIndex - 1 + maxIndex + 1) % (maxIndex + 1);
            ui->SetCurrentIndex(currentIndex);
        }
        else if (input.IsTrgDown(KEY_INPUT_DOWN)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            currentIndex = (currentIndex + 1) % (maxIndex + 1);
            ui->SetCurrentIndex(currentIndex);
        }

        if (input.IsTrgDown(KEY_INPUT_SPACE)) 
        {
            Application::GetInstance().SetActiveUI(true);
            int selected = ui->GetCurrentIndex();

            // ゲーム開始
            if (selected == 0) 
            {
                isDecided_ = true;

                sound.Play(SoundManager::SOUND::SE_PUSH);

                auto newScene = std::make_shared<SceneGame>();

                SceneManager::GetInstance().ChangeScene(newScene);
                return;
            }
            else if (selected == 1) { // 遊び方
                sound.Play(SoundManager::SOUND::SE_PUSH);
                inHowToPlayMenu_ = true;
                uiHowToPlay_->SetCurrentIndex(0);
            }
            else if (selected == maxIndex) { // ゲーム終了
                sound.Play(SoundManager::SOUND::SE_PUSH);
                exitRequested_ = true;
                return;
            }
            else {
                showBlackBackground_ = true; // 操作説明やクレジットはそのまま黒背景
            }
        }
    }
    // ----- サブメニュー操作 -----
    else
    {
        auto ui = uiHowToPlay_.get();
        int currentIndex = ui->GetCurrentIndex();
        int maxIndex = ui->GetMaxIndex() - 1;

        if (input.IsTrgDown(KEY_INPUT_UP)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            currentIndex = (currentIndex - 1 + maxIndex + 1) % (maxIndex + 1);
            ui->SetCurrentIndex(currentIndex);
        }
        else if (input.IsTrgDown(KEY_INPUT_DOWN)) {
            sound.Play(SoundManager::SOUND::SE_SELECT);
            currentIndex = (currentIndex + 1) % (maxIndex + 1);
            ui->SetCurrentIndex(currentIndex);
        }

        if (input.IsTrgDown(KEY_INPUT_SPACE)) {
            int selected = ui->GetCurrentIndex();
            if (selected == 0) { // 目標について
                howToPlayPage_ = 1;
            }
            else if (selected == 1) { // 錬金について
                howToPlayPage_ = 2;
            }
            else if (selected == 2) { // アトリエについて
                howToPlayPage_ = 3;
            }
            else if (selected == 3) { // ギルドについて
                howToPlayPage_ = 4;
            }
            else if (selected == 4) { // ガーデンについて
                howToPlayPage_ = 5;
            }
            else if (selected == 5) { // 戻る
                inHowToPlayMenu_ = false;
            }
        }
    }
}

void SceneTitle::Draw(void)
{
    // 背景動画
    DrawRotaGraph3(0, 0, 0, 0, 1.0f, 1.0f, 0, movieHandle_, FALSE);

    // ---- 遊び方説明ページ表示中 ----
    if (howToPlayPage_ > 0)
    {
        DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), TRUE);

        if (howToPlayPage_ == 1) {
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, playHandle_, true);
        }
        else if (howToPlayPage_ == 2) {
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, playHandle2_, true);
        }
        else if (howToPlayPage_ == 3) { // アトリエ
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, atelierHandle_, true);
        }
        else if (howToPlayPage_ == 4) { // ギルド
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, guildHandle_, true);
        }
        else if (howToPlayPage_ == 5) { // ガーデン
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, gardenHandle_, true);
        }

        DrawString(50, Application::SCREEN_SIZE_Y - 30, "ESCキーで戻る", GetColor(200, 200, 200));
        return;
    }

    // ---- メインメニュー ----
    if (!inHowToPlayMenu_)
    {
        DrawRotaGraph(Application::SCREEN_SIZE_X / 2 + 55,
            Application::SCREEN_SIZE_Y / 2,
            1.0, 0.0, logo_, true);
        uiMain_->Draw(Application::SCREEN_SIZE_Y / 2);

        // 操作説明やクレジットを選んだとき
        if (showBlackBackground_)
        {
            // 黒背景
            DrawBox(0, 0,
                Application::SCREEN_SIZE_X,
                Application::SCREEN_SIZE_Y,
                GetColor(0, 0, 0), TRUE);

            int selected = uiMain_->GetCurrentIndex();
            if (selected == 2) // 操作説明
            {
                DrawRotaGraph3(50, 50, 0, 0, 1.0f, 1.0f, 0, operationHandle_, true);
            }
            else if (selected == 3) // クレジット
            {
                auto& font = Font::GetInstance();
                std::vector<std::string> lines = {
                   
                };

                int color = GetColor(255, 255, 255);
                int fontSize = 28;
                int fontType = DX_FONTTYPE_ANTIALIASING;

                int centerX = Application::SCREEN_SIZE_X / 2;
                int centerY = Application::SCREEN_SIZE_Y / 2;

                int lineSpacing = fontSize + 24; // ← 行間を広めに (12px 余白)

                int totalHeight = static_cast<int>(lines.size()) * lineSpacing;
                int startY = centerY - totalHeight / 2;

                for (size_t i = 0; i < lines.size(); i++)
                {
                    int textWidth = font.GetDefaultTextWidth(lines[i]);
                    int drawX = centerX - textWidth / 2;
                    int drawY = startY + static_cast<int>(i) * lineSpacing;

                    font.DrawDefaultText(drawX, drawY, lines[i].c_str(), color, fontSize, fontType);
                }
            }

        }

    }
    // ---- 遊び方サブメニュー ----
    else
    {
        DrawBox(0, 0,
            Application::SCREEN_SIZE_X,
            Application::SCREEN_SIZE_Y,
            GetColor(0, 0, 0), TRUE);

        int centerY = Application::SCREEN_SIZE_Y / 2;
        int offsetY = centerY - 100;  // 上にずらす

        uiHowToPlay_->Draw(offsetY);
    }
}

void SceneTitle::Release(void)
{
    DeleteGraph(movieHandle_);
    grid_->Release();
    delete grid_;
    grid_ = nullptr;
}

bool SceneTitle::IsExitRequested(void) const
{
    return exitRequested_;
}

void SceneTitle::DrawDebug(void)
{

}
