#include "SceneTitle.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/Generic/Camera.h"
#include "../Scene/SceneGame.h"
#include "../Scene/SceneTutorial.h"
#include "../Scene/SceneOption.h"
#include "../Scene/SceneCredit.h"
#include "../Object/Grid.h"
#include "../Application.h"
#include "../Manager/System/TimeManager.h"
#include "../Manager/System/Loading.h"

SceneTitle::SceneTitle(void)
{
    logo_ = -1;
    grid_ = nullptr;
    isDecided_ = false;
    movieHandle_ = -1;
    showBlackBackground_ = false;
    isPlay_ = false;
    exitRequested_ = false;
    howToPlayPage_ = 0;
    pauseUiCount_ = 0;
}

void SceneTitle::Load(void)
{
    SceneBase::Load();

    // リソースの読み込み
    ResourceManager::GetInstance().InitTitle();

    // 動画ファイルの読み込み
    movieHandle_ = ResourceManager::GetInstance().
        Load(ResourceManager::SRC::TITLE_MOVIE).handleId_;


    if (movieHandle_ == -1)
    {
        // 読み込み失敗時のエラーハンドリング
        printfDx("Failed to load title movie!\n");
    }

    // タイトルロゴ画像の読み込み
    logo_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::TYTLE_LOGO).handleId_;

    if (logo_ == -1)
    {
        printfDx("Failed to load title logo!\n");
    }

    // BGM・SEロード
    Loading::GetInstance()->SetProgress(25.0f);

    // 音量調整
    Loading::GetInstance()->SetProgress(45.0f);
     
    // ロゴ・操作説明・再生用画像ロード
    Loading::GetInstance()->SetProgress(60.0f);

    // その他画像
    Loading::GetInstance()->SetProgress(80.0f);

    // UI 初期化
    uiMain_ = std::make_unique<SceneUi>();
    uiMain_->AddCharacter("開始");
    uiMain_->AddCharacter("遊び方");
    uiMain_->AddCharacter("設定");
    uiMain_->AddCharacter("クレジット");
    uiMain_->AddCharacter("ゲーム終了");
    uiMain_->SetCurrentIndex(0);

    //時間カウントリセット
    TimeManager::GetInstance().Reset();

    Loading::GetInstance()->SetProgress(100.0f);
}

void SceneTitle::EndLoad(void)
{
    SceneBase::EndLoad();
}

void SceneTitle::Initialize(void)
{
    // タイトルBGM
    SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_TITLE, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_TITLE).handleId_);

    // キャンセル音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_CANCEL,
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_CANCEL).handleId_);

    // 選択音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_SELECT, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_SELECT).handleId_);

    // 決定音
    SoundManager::GetInstance().Add(SoundManager::TYPE::SE, SoundManager::SOUND::SE_PUSH, 
        ResourceManager::GetInstance().Load(ResourceManager::SRC::SE_PUSH).handleId_);


    SetMouseDispFlag(TRUE);

    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_TITLE);

    // 動画再生の開始
    if (movieHandle_ != -1)
    {
        // 動画を最初から再生
        SeekMovieToGraph(movieHandle_, 0);
        PlayMovieToGraph(movieHandle_);
    }

    // カメラ設定 
    auto camera = SceneManager::GetInstance().GetCamera();
    camera->ChangeMode(Camera::MODE::FIXED_POINT);

    // グリッド生成
    grid_ = new Grid();
    grid_->Initialize();

    // UI初期化
    howToPlayPage_ = 0;
    showBlackBackground_ = false;
    isDecided_ = false;
    exitRequested_ = false;
    isPlay_ = true;
    pauseUiCount_ = PAUSE_UI_COUNT;

    if (uiMain_) { uiMain_->SetCurrentIndex(0); }

    Application::GetInstance().SetActiveUI(true);
}

void SceneTitle::Update(void)
{
    auto camera = SceneManager::GetInstance().GetCamera();
    camera->ChangeMode(Camera::MODE::FIXED_POINT);

    if (Loading::GetInstance()->IsLoading()) return;
    if (!uiMain_) return;

    // 動画のループ処理
    if (movieHandle_ != -1)
    {
        if (GetMovieStateToGraph(movieHandle_) == 0) 
        {
            SeekMovieToGraph(movieHandle_, 0);
            PlayMovieToGraph(movieHandle_);
        }
    }

    auto& sound = SoundManager::GetInstance();
    auto& input = InputManager::GetInstance();
    if (showBlackBackground_)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE) ||
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
        {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            showBlackBackground_ = false;
            uiMain_->SetCurrentIndex(0);
        }
        return;
    }

    if (howToPlayPage_ > 0)
    {
        if (input.IsTrgDown(KEY_INPUT_ESCAPE) || 
            input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
        {
            sound.Play(SoundManager::SOUND::SE_CANCEL);
            howToPlayPage_ = 0;
            uiMain_->SetCurrentIndex(0);
        }
        return;
    }

    // メインメニュー操作
    // ポインタを直接触らずにガードする
    auto ui = uiMain_.get();
    int currentIndex = ui->GetCurrentIndex();
    int maxIndex = ui->GetMaxIndex() - 1;

    InputManager::JOYPAD_IN_STATE padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
    VECTOR padDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    static bool stickFree = true;

    if (stickFree) 
    {
        if (padDir.z > 0.5f) // 上入力
        {
            currentIndex = (currentIndex - 1 + maxIndex + 1) % (maxIndex + 1);
            sound.Play(SoundManager::SOUND::SE_SELECT);
            stickFree = false;
        }
        else if (padDir.z < -0.5f) // 下入力
        {
            currentIndex = (currentIndex + 1) % (maxIndex + 1);
            sound.Play(SoundManager::SOUND::SE_SELECT);
            stickFree = false;
        }
    }
    else 
    {
        if (fabs(padDir.z) < 0.2f) stickFree = true;
    }
    ui->SetCurrentIndex(currentIndex);

    // マウスによる選択更新
    Vector2 mousePos = input.GetMousePos();
    int menuStartY = Application::SCREEN_SIZE_Y / 2 + 80;
    int itemHeight = 80;
    int menuWidth = 200;

    for (int i = 0; i <= maxIndex; i++)
    {
        int rectLeft = Application::SCREEN_SIZE_X / 2 - (menuWidth / 2);
        int rectRight = Application::SCREEN_SIZE_X / 2 + (menuWidth / 2);
        int rectTop = menuStartY + (i * itemHeight) - (itemHeight / 2);
        int rectBottom = menuStartY + (i * itemHeight) + (itemHeight / 2);

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

    bool isPadDecide = input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
    bool isKeyDecide = input.IsTrgDown(KEY_INPUT_SPACE) || input.IsTrgDown(KEY_INPUT_RETURN);
    bool isMouseDecide = input.IsTrgMouseLeft();

    // 決定操作
    if (isPadDecide || isKeyDecide || isMouseDecide)
    {
        Application::GetInstance().SetActiveUI(true);
        int selected = ui->GetCurrentIndex();

        switch (selected)
        {
        case 0: // 開始
            isDecided_ = true;
            sound.Play(SoundManager::SOUND::SE_PUSH);
            Application::GetInstance().SetActiveUI(false);
            SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>());
            break;

        case 1: // 遊び方
            sound.Play(SoundManager::SOUND::SE_PUSH);
            SceneManager::GetInstance().PushScene(std::make_shared<SceneTutorial>());
            break;

        case 2: // 操作説明
            sound.Play(SoundManager::SOUND::SE_PUSH);
            SceneManager::GetInstance().PushScene(std::make_shared<SceneOption>());
            break;

        case 3: // クレジット
            sound.Play(SoundManager::SOUND::SE_PUSH);
            SceneManager::GetInstance().PushScene(std::make_shared<SceneCredit>());
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
    // 背景動画の描画（動画はロードの最初の方で読み込まれるため、これだけは出してもOK）
    if (movieHandle_ != -1)
    {
        int movieWidth, movieHeight;
        GetGraphSize(movieHandle_, &movieWidth, &movieHeight);

        float scaleX = static_cast<float>(Application::SCREEN_SIZE_X) / movieWidth;
        float scaleY = static_cast<float>(Application::SCREEN_SIZE_Y) / movieHeight;
        float scale = (scaleX > scaleY) ? scaleX : scaleY;

        DrawRotaGraph3(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2,
            movieWidth / 2, movieHeight / 2, scale, scale, 0.0, movieHandle_, TRUE);
    }

    if (!uiMain_) return;


    // メインメニュー描画
    if (logo_ != -1)
    {
        DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 450, 1.0f, 0.0, logo_, TRUE);
    }

    // UI描画（uiMain_が存在することが確定している）
    uiMain_->Draw(Application::SCREEN_SIZE_Y / 2 + 80);
}

void SceneTitle::Release(void)
{
    SetMouseDispFlag(FALSE);

    if (movieHandle_ != -1)
    {
        PauseMovieToGraph(movieHandle_);
        movieHandle_ = -1;
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