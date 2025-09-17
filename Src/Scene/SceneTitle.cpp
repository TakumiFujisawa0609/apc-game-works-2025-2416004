#include "SceneTitle.h"

#include <DxLib.h>
#include "../Manager/Generic/Resource.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/Generic/Camera.h"
#include "../DrawUI/SceneUI/SceneUI.h"
#include "../Object/Grid.h"
#include "../Application.h"
#include "../DrawUI/Font.h"

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
}

void SceneTitle::Init(void)
{
    
    auto camera = SceneManager::GetInstance().GetCamera();
    camera->ChangeMode(Camera::MODE::FREE);


    grid_ = new Grid();
    grid_->Init();

    uiMain_ = std::make_unique<SceneUi>();
    uiMain_->AddCharctor("開始");
    uiMain_->AddCharctor("遊び方");
    uiMain_->AddCharctor("操作説明");
    uiMain_->AddCharctor("クレジット");
    uiMain_->AddCharctor("ゲーム終了");

    uiHowToPlay_ = std::make_unique<SceneUi>();
    uiHowToPlay_->AddCharctor("目標について");
    uiHowToPlay_->AddCharctor("錬金について");
    uiHowToPlay_->AddCharctor("アトリエについて");  // ★追加
    uiHowToPlay_->AddCharctor("ギルドについて");    // ★追加
    uiHowToPlay_->AddCharctor("ガーデンについて");  // ★追加
    uiHowToPlay_->AddCharctor("戻る");

    inHowToPlayMenu_ = false;
    howToPlayPage_ = 0;

    auto& sound = SoundManager::GetInstance();
    auto& res = ResourceManager::GetInstance();

    sound.Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE, res.Load(ResourceManager::SRC::BGM_TITLE).handleId_);
    sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PUSH, res.Load(ResourceManager::SRC::SE_PUSH).handleId_);
    sound.Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT, res.Load(ResourceManager::SRC::SE_SELECT).handleId_);
    sound.AdjustVolume(SoundManager::SOUND::BGM_TITLE, 40);
    sound.AdjustVolume(SoundManager::SOUND::SE_PUSH, 20);
    sound.AdjustVolume(SoundManager::SOUND::SE_SELECT, 30);

    sound.Play(SoundManager::SOUND::BGM_TITLE);

    movieHandle_ = LoadGraph((Application::PATH_MOVIE + "TitleMovie.mp4").c_str());
    PlayMovieToGraph(movieHandle_, TRUE);
    SetMovieVolumeToGraph(movieHandle_, 255);


    logo_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::TYTLE_LOGO).handleId_;
    operationHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::OPERATION).handleId_;
	playHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAY_GUIDE).handleId_;
	playHandle2_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::PLAY_GUIDE2).handleId_;

    atelierHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::ATREA).handleId_;
    guildHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GUILD).handleId_;
    gardenHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GARDEN).handleId_;

	// 初期カーソル位置
	uiMain_->SetCurrentIndex(0);
	// 黒背景表示フラグ初期化
	showBlackBackground_ = false;
    isPlay_ = true;

    pauseUiCount_ = PAUSE_UI_COUNT;
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

        if (input.IsTrgDown(KEY_INPUT_RETURN)) {
            Application::GetInstance().SetActiveUI(true);
            int selected = ui->GetCurrentIndex();
            if (selected == 0) {
                sound.Play(SoundManager::SOUND::SE_PUSH);
                sound.Stop(SoundManager::SOUND::BGM_TITLE);
                SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
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

        if (input.IsTrgDown(KEY_INPUT_RETURN)) {
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
        DrawBox(0, 0, Application::DEFA_SCREEN_SIZE_X, Application::DEFA_SCREEN_SZIE_Y, GetColor(0, 0, 0), TRUE);

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

        DrawString(50, Application::DEFA_SCREEN_SZIE_Y - 30, "ESCキーで戻る", GetColor(200, 200, 200));
        return;
    }

    // ---- メインメニュー ----
    if (!inHowToPlayMenu_)
    {
        DrawRotaGraph(Application::SCREEN_SIZE_X / 2 + 55,
            Application::SCREEN_SIZE_Y / 2,
            1.0, 0.0, logo_, true);
        uiMain_->Draw(Application::DEFA_SCREEN_SZIE_Y / 2);

        // 操作説明やクレジットを選んだとき
        if (showBlackBackground_)
        {
            // 黒背景
            DrawBox(0, 0,
                Application::DEFA_SCREEN_SIZE_X,
                Application::DEFA_SCREEN_SZIE_Y,
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
                    "クレジット",
                    "効果音ラボ: カーソル移動音2, キラッ2",
                    "ニコニコ・コモンズ: キャンセル音 k45mm, 午後の庭園 kenapo",
                    "BGM: Stream D (ju-nya)",
                    "効果音工房: 決定音01, 決定音19",
                    "ポケットサウンド: ファンファーレ",
                    "魔王魂: 民族10",
                    "H/MIX GALLERY: ホシノキセキ"
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
            Application::DEFA_SCREEN_SIZE_X,
            Application::DEFA_SCREEN_SZIE_Y,
            GetColor(0, 0, 0), TRUE);

        int centerY = Application::DEFA_SCREEN_SZIE_Y / 2;
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
    // 必要に応じて
}
