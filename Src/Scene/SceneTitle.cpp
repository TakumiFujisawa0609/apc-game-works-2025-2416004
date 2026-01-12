#include "SceneTitle.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/Generic/Camera.h"
#include "../Scene/SceneGame.h"
#include "../Object/Grid.h"
#include "../Application.h"
#include "../DrawUI/Font.h"
#include "../Manager/System/TimeManager.h"
#include "../Manager/System/Loading.h"

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

    // リソースの読み込み
    ResourceManager::GetInstance().InitTitle();

    // ★【追加】動画ファイルの読み込み
    // 動画ファイルのパスを指定（例: "Data/Movie/TitleBG.mp4"）
    movieHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::TITLE_MOVIE).handleId_;


    if (movieHandle_ == -1)
    {
        // 読み込み失敗時のエラーハンドリング
        printfDx("Failed to load title movie!\n");
    }

    // ★【追加】タイトルロゴ画像の読み込み
    logo_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::TYTLE_LOGO).handleId_;

    if (logo_ == -1)
    {
        printfDx("Failed to load title logo!\n");
    }

    // ★【追加】その他の画像読み込み
    operationHandle_ = LoadGraph("Data/Image/Operation.png");
    playHandle_ = LoadGraph("Data/Image/HowToPlay1.png");
    playHandle2_ = LoadGraph("Data/Image/HowToPlay2.png");
    atelierHandle_ = LoadGraph("Data/Image/Atelier.png");
    gardenHandle_ = LoadGraph("Data/Image/Garden.png");
    guildHandle_ = LoadGraph("Data/Image/Guild.png");

    // BGM・SEロード
    Loading::GetInstance()->SetProgress(25.0f);

    // タイトルBGM
    SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE, ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_TITLE).handleId_);

    //　タイトルBGMの音量調整テスト
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::BGM_TITLE, 0);

    // キャンセル音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_CANCEL, ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_CANCEL).handleId_);

    // キャンセル音の音量調整
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::SE_CANCEL, 0);

    // 選択音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT, ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_SELECT).handleId_);

    // 選択音の音量調整
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::SE_SELECT, 0);

    // 決定音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PUSH, ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_PUSH).handleId_);

    // 決定音の音量調整
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::SE_PUSH, 0);

    // 音量調整
    Loading::GetInstance()->SetProgress(45.0f);

    // ロゴ・操作説明・再生用画像ロード
    Loading::GetInstance()->SetProgress(60.0f);

    // その他画像
    Loading::GetInstance()->SetProgress(80.0f);

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

    Loading::GetInstance()->SetProgress(100.0f);
}

void SceneTitle::EndLoad(void)
{
    SceneBase::EndLoad();
}

void SceneTitle::Init(void)
{
    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE);

    // ★【追加】動画再生の開始
    if (movieHandle_ != -1)
    {
        // 動画を最初から再生
        SeekMovieToGraph(movieHandle_, 0);
        PlayMovieToGraph(movieHandle_);
    }

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
    // ★【追加】動画のループ処理
    if (movieHandle_ != -1)
    {
        // 動画の再生状態をチェック
        if (GetMovieStateToGraph(movieHandle_) == 0) // 0 = 再生停止中
        {
            // 動画が終了したら最初から再生
            SeekMovieToGraph(movieHandle_, 0);
            PlayMovieToGraph(movieHandle_);
        }
    }

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
                SceneManager::GetInstance().GameEnd();
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
            else if (selected == 1) {
                howToPlayPage_ = 2;
            }
            else if (selected == 2) {
                howToPlayPage_ = 3;
            }
            else if (selected == 3) {
                howToPlayPage_ = 4;
            }
            else if (selected == 4) {
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
    // ★【修正】背景動画の描画（画面全体に拡大表示）
    if (movieHandle_ != -1)
    {
        // 動画のサイズを取得
        int movieWidth, movieHeight;
        GetGraphSize(movieHandle_, &movieWidth, &movieHeight);

        // 画面サイズに合わせて拡大率を計算
        float scaleX = static_cast<float>(Application::SCREEN_SIZE_X) / movieWidth;
        float scaleY = static_cast<float>(Application::SCREEN_SIZE_Y) / movieHeight;
        float scale = (scaleX > scaleY) ? scaleX : scaleY; // アスペクト比を保ちながら画面を埋める

        // 中央配置で描画
        int centerX = Application::SCREEN_SIZE_X / 2;
        int centerY = Application::SCREEN_SIZE_Y / 2;

        DrawRotaGraph3(centerX, centerY,
            movieWidth / 2, movieHeight / 2,  // 動画の中心
            scale, scale, 0.0,
            movieHandle_, TRUE);
    }

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
        else if (howToPlayPage_ == 3) {
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, atelierHandle_, true);
        }
        else if (howToPlayPage_ == 4) {
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2,
                Application::SCREEN_SIZE_Y / 2,
                1.0, 0.0, guildHandle_, true);
        }
        else if (howToPlayPage_ == 5) {
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
        // ★【修正】タイトルロゴの描画（画面中央上部に配置）
        if (logo_ != -1)
        {
            // int logoWidth, logoHeight;
            // GetGraphSize(logo_, &logoWidth, &logoHeight); // ロゴサイズ取得は不要なためコメントアウト

            // 画面中央上部に配置（Y座標を調整可能）
            int logoX = Application::SCREEN_SIZE_X / 2;
            // Y座標を下にずらす: 200px -> 280px
            int logoY = 450; // 上から280pxの位置 (修正箇所)

            // ロゴを拡大して描画（scale値で大きさ調整可能）
            float logoScale = 1.0f; // 必要に応じて変更（例: 1.5fで1.5倍）

            DrawRotaGraph(logoX, logoY, logoScale, 0.0, logo_, TRUE);
        }

        // UIメニューの描画
        // UIの基準点を画面中央(Y/2)から、さらに80px下にずらす (修正箇所)
        uiMain_->Draw(Application::SCREEN_SIZE_Y / 2 + 80);

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
                    // クレジット情報が空のため、ダミー情報を追加
                    "DEVELOPER",
                    "Programming: YOUR NAME",
                    "Design: YOUR FRIEND'S NAME",
                    "",
                    "Powered by DxLib",
                    // ... 
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
    if (operationHandle_ != -1)
    {
        DeleteGraph(operationHandle_);
        operationHandle_ = -1;
    }
    if (playHandle_ != -1)
    {
        DeleteGraph(playHandle_);
        playHandle_ = -1;
    }
    if (playHandle2_ != -1)
    {
        DeleteGraph(playHandle2_);
        playHandle2_ = -1;
    }
    if (atelierHandle_ != -1)
    {
        DeleteGraph(atelierHandle_);
        atelierHandle_ = -1;
    }
    if (gardenHandle_ != -1)
    {
        DeleteGraph(gardenHandle_);
        gardenHandle_ = -1;
    }
    if (guildHandle_ != -1)
    {
        DeleteGraph(guildHandle_);
        guildHandle_ = -1;
    }

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