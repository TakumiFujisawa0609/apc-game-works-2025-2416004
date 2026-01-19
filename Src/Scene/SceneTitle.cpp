#include "SceneTitle.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/Generic/Camera.h"
#include "../Scene/SceneGame.h"
#include "../Scene/SceneTutorial.h"
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
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::BGM_TITLE, 30);

    // キャンセル音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_CANCEL, ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_CANCEL).handleId_);

    // キャンセル音の音量調整
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::SE_CANCEL, 50);

    // 選択音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT, ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_SELECT).handleId_);

    // 選択音の音量調整
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::SE_SELECT, 50);

    // 決定音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PUSH, ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_PUSH).handleId_);

    // 決定音の音量調整
    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::SE_PUSH, 50 );

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
    SetMouseDispFlag(TRUE);

    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE);

    // 動画再生の開始
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
    howToPlayPage_ = 0;
    showBlackBackground_ = false;
    isDecided_ = false;
    exitRequested_ = false;
    isPlay_ = true;
    pauseUiCount_ = PAUSE_UI_COUNT;

    // UIはLoadで生成済みなので、ここで初期位置設定
    if (uiMain_) uiMain_->SetCurrentIndex(0);
}

void SceneTitle::Update(void)
{
    // 動画のループ処理
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
    auto ui = uiMain_.get();
    int currentIndex = ui->GetCurrentIndex();
    int maxIndex = ui->GetMaxIndex() - 1;

    // --- マウスによる選択更新 ---
    Vector2 mousePos = input.GetMousePos();

    // 描画基準位置
    int menuStartY = Application::SCREEN_SIZE_Y / 2 + 80;
    int itemHeight = 80; // 1項目の高さ
    int menuWidth = 200; // 判定する横幅

    for (int i = 0; i <= maxIndex; i++)
    {
        int rectLeft = Application::SCREEN_SIZE_X / 2 - (menuWidth / 2);
        int rectRight = Application::SCREEN_SIZE_X / 2 + (menuWidth / 2);

        // 修正：判定範囲の基準を上にずらす
        // i=0のとき、menuStartY を中心とした範囲にするために -20 調整
        int rectTop = menuStartY + (i * itemHeight) - (itemHeight / 2);
        int rectBottom = menuStartY + (i * itemHeight) + (itemHeight / 2);

        // マウスが項目内にあるか判定
        if (mousePos.x >= rectLeft && mousePos.x <= rectRight &&
            mousePos.y >= rectTop && mousePos.y <= rectBottom)
        {
            if (currentIndex != i)
            {
                SoundManager::GetInstance().Play(SoundManager::SOUND::SE_SELECT);
                ui->SetCurrentIndex(i);
            }
        }
    }

    // 上下入力
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

    // 決定操作
    if (input.IsTrgDown(KEY_INPUT_SPACE) || input.IsTrgMouseLeft())
    {
        Application::GetInstance().SetActiveUI(true);
        int selected = ui->GetCurrentIndex();

        switch (selected)
        {
        case 0: // 開始
            isDecided_ = true;
            sound.Play(SoundManager::SOUND::SE_PUSH);
            SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>());
            break;

        case 1: // 遊び方
            sound.Play(SoundManager::SOUND::SE_PUSH);
            SceneManager::GetInstance().PushScene(std::make_shared<SceneTutorial>());
            break;

        case 2: // 操作説明
            sound.Play(SoundManager::SOUND::SE_PUSH);
            showBlackBackground_ = true;
            break;

        case 3: // クレジット
            sound.Play(SoundManager::SOUND::SE_PUSH);
            showBlackBackground_ = true;
            break;

        case 4: // ゲーム終了
            sound.Play(SoundManager::SOUND::SE_PUSH);
            exitRequested_ = true;
            SceneManager::GetInstance().GameEnd();
            break;
        }
    }
}

void SceneTitle::Draw(void)
{
    // 背景動画の描画（画面全体に拡大表示）
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

    // 説明ページ表示
    if (howToPlayPage_ > 0)
    {
        DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), TRUE);

        int handle = -1;
        if (howToPlayPage_ == 1) handle = playHandle_;
        else if (howToPlayPage_ == 2) handle = playHandle2_;
        else if (howToPlayPage_ == 3) handle = atelierHandle_; // 適宜画像ハンドルを割り当て

        if (handle != -1) {
            DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 1.0, 0.0, handle, true);
        }
        DrawString(50, Application::SCREEN_SIZE_Y - 30, "ESCキーで戻る", GetColor(200, 200, 200));
        return;
    }

    // メインメニュー描画
    if (logo_ != -1)
    {
        DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 450, 1.0f, 0.0, logo_, TRUE);
    }

    uiMain_->Draw(Application::SCREEN_SIZE_Y / 2 + 80);

    // 操作説明・クレジットの黒背景描画
    if (showBlackBackground_)
    {
        DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), TRUE);
        int selected = uiMain_->GetCurrentIndex();

        if (selected == 4) // 操作説明
        {
            DrawRotaGraph3(50, 50, 0, 0, 1.0f, 1.0f, 0, operationHandle_, true);
        }
        else if (selected == 5) // クレジット
        {
            // クレジット描画処理
        }
    }
}

void SceneTitle::Release(void)
{
    SetMouseDispFlag(FALSE);

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