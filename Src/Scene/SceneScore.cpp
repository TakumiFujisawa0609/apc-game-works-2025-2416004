#include "SceneScore.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/Loading.h"
#include "../Scene/SceneTitle.h"
#include "../Manager/System/TimeManager.h"
#include "../Application.h"
#include "../DrawUI/Font.h"

SceneScore::SceneScore(void)
    : playerDistance_(0.0f)
    , enemyDeathCount_(0)
    , distanceScore_(0)
    , killScore_(0)
    , totalScore_(0)
    , scoreDisplayTimer_(0.0f)
    , displayedScore_(0)
    , countUpSpeed_(50.0f)
    , bgHandle_(-1)
    , bgMovieId_(-1)
{
}

void SceneScore::Load(void)
{
    SceneBase::Load();
    // 時間カウントリセット
    TimeManager::GetInstance().Reset();

    ResourceManager::GetInstance().InitGameClear();

    Loading::GetInstance()->SetProgress(25.0f);

    bgHandle_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::GAMECLERA_LOGO).handleId_;

    bgMovieId_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::BG_MOVIE).handleId_;



    Loading::GetInstance()->SetProgress(45.0f);

    Loading::GetInstance()->SetProgress(60.0f);

    Loading::GetInstance()->SetProgress(80.0f);

    Loading::GetInstance()->SetProgress(100.0f);

    // サウンドのリソース読み込み
    
}

void SceneScore::EndLoad(void)
{
    SceneBase::EndLoad();
}

void SceneScore::Initialize(void)
{

    // サウンド
    auto& res = ResourceManager::GetInstance();

    SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_SCORE, ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_SCORE).handleId_);

    // 初期BGM
    SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_SCORE);

    // SceneManagerからゲーム統計を取得
    auto& sceneMgr = SceneManager::GetInstance();
    playerDistance_ = sceneMgr.GetPlayerDistance();
    enemyDeathCount_ = sceneMgr.GetEnemyDeathCount();

    // スコアを計算
    CalculateScore();

    // スコアアニメーション初期化
    scoreDisplayTimer_ = 0.0f;
    displayedScore_ = 0;

}

void SceneScore::Update(void)
{
    if (Loading::GetInstance()->IsLoading()) return;

    auto& sound = SoundManager::GetInstance();
    auto& input = InputManager::GetInstance();
    auto& sceneMgr = SceneManager::GetInstance();

    // 動画のループ処理
    if (bgMovieId_ != -1)
    {
        // 動画の再生状態をチェック
        if (GetMovieStateToGraph(bgMovieId_) == 0) // 0 = 再生停止中
        {
            // 動画が終了したら最初から再生
            SeekMovieToGraph(bgMovieId_, 0);
            PlayMovieToGraph(bgMovieId_);
        }
    }

    // スコアアニメーション更新
    UpdateScoreAnimation();

    if (input.IsTrgDown(KEY_INPUT_SPACE) || input.IsClickMouseLeft())
    {
        // 決定音
        sound.Play(SoundManager::SOUND::SE_PUSH);
        // BGM停止
        sound.Stop(SoundManager::SOUND::BGM_TITLE);

        // タイトルに戻る前にゲーム統計をリセット
        sceneMgr.ResetGameStats();

        auto newScene = std::make_shared<SceneTitle>();
        sceneMgr.ChangeScene(newScene);

        return;
    }
}

void SceneScore::Draw(void)
{
    // 背景動画の描画（画面全体に拡大表示）
    if (bgMovieId_ != -1)
    {
        // 動画のサイズを取得
        int movieWidth, movieHeight;
        GetGraphSize(bgMovieId_, &movieWidth, &movieHeight);

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
            bgMovieId_, true);
    }

    // インスタンス取得
    auto& font = Font::GetInstance();
    const int SCREEN_W = Application::SCREEN_SIZE_X;
    const int SCREEN_H = Application::SCREEN_SIZE_Y;

    // 【変更】画面の左側 25% の位置を基準にする (1280pxなら 320px地点)
    const int LEFT_QUARTER_X = static_cast<int>(SCREEN_W * 0.25f);
    const int CONTENT_WIDTH = 400; // コンテンツの横幅
    const int DRAW_X = LEFT_QUARTER_X - (CONTENT_WIDTH / 2); // 25%地点を中央にする

    // --- レイアウト設定 ---
    int drawY = 160;

    // 背景画像の描画
    DrawRotaGraph3(0, 0, 0, 0, 1.0f, 1.0f, 0.0f, bgHandle_, true);

    // --- 配色設定（白背景用） ---
    unsigned int titleColor = GetColor(20, 20, 20);     // ほぼ黒
    unsigned int labelColor = GetColor(50, 50, 50);     // 濃い灰色
    unsigned int valueColor = GetColor(0, 50, 150);     // 鮮やかな紺
    unsigned int scoreAddColor = GetColor(0, 120, 0);   // 濃い緑
    unsigned int lineColor = GetColor(150, 150, 150);   // 中間の灰色

    // 1. タイトル
    font.DrawDefaultText(DRAW_X, drawY, "ゲームリザルト", titleColor, 48, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += 80;

    drawY += 50;

    // 2. 移動距離
    font.DrawDefaultText(DRAW_X, drawY, "移動距離", labelColor, 24, Font::FONT_TYPE_ANTIALIASING_EDGE);
    char distStr[64], distScoreStr[64];
    sprintf_s(distStr, "%.2f m", playerDistance_);
    sprintf_s(distScoreStr, "+ %d pts", distanceScore_);

    font.DrawDefaultText(DRAW_X + 200, drawY, distStr, valueColor, 26, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += 35;
    font.DrawDefaultText(DRAW_X + 200, drawY, distScoreStr, scoreAddColor, 22, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += 70;

    // 3. 撃破数
    font.DrawDefaultText(DRAW_X, drawY, "エネミー討伐数", labelColor, 24, Font::FONT_TYPE_ANTIALIASING_EDGE);
    char killStr[64], killScoreStr[64];
    sprintf_s(killStr, "%d", enemyDeathCount_);
    sprintf_s(killScoreStr, "+ %d pts", killScore_);

    font.DrawDefaultText(DRAW_X + 200, drawY, killStr, valueColor, 26, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += 35;
    font.DrawDefaultText(DRAW_X + 200, drawY, killScoreStr, scoreAddColor, 22, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += 90;

    // 4. 合計スコア
    font.DrawDefaultText(DRAW_X, drawY, "TOTAL SCORE", GetColor(0, 0, 0), 32, Font::FONT_TYPE_ANTIALIASING_EDGE);

    char totalStr[64];
    sprintf_s(totalStr, "%d", displayedScore_);
    unsigned int totalDisplayColor = (displayedScore_ >= totalScore_) ? GetColor(180, 130, 0) : GetColor(0, 0, 0);
    font.DrawDefaultText(DRAW_X + 400, drawY, totalStr, totalDisplayColor, 44, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += 90;

    // 5. ランク表示
    if (displayedScore_ >= totalScore_)
    {
        const char* rank = GetRank();
        unsigned int rankColor = GetColor(40, 40, 40);
        if (rank[0] == 'S') rankColor = GetColor(180, 0, 180);
        else if (rank[0] == 'A') rankColor = GetColor(200, 80, 0);

        font.DrawDefaultText(DRAW_X + 20, drawY + 15, "RANK", GetColor(80, 80, 80), 30, Font::FONT_TYPE_ANTIALIASING_EDGE);
        font.DrawDefaultText(DRAW_X + 150, drawY - 10, rank, rankColor, 80, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }

    // 6. 操作説明
    font.DrawDefaultText(DRAW_X + 30, SCREEN_H - 120, "CLICK or SPACE TO TITLE", GetColor(100, 100, 100), 22, Font::FONT_TYPE_ANTIALIASING_EDGE);

#ifdef _DEBUG
    DrawDebug();
#endif
}

void SceneScore::Release(void)
{
    if (bgMovieId_ != -1)
    {
        PauseMovieToGraph(bgMovieId_);
        bgMovieId_ = -1;
    }
}

void SceneScore::DrawDebug(void)
{
#ifdef _DEBUG
    DrawFormatString(10, 10, GetColor(0, 255, 0), "[DEBUG] Score Scene");
    DrawFormatString(10, 30, GetColor(255, 255, 255), "Raw Distance: %.2f", playerDistance_);
    DrawFormatString(10, 50, GetColor(255, 255, 255), "Raw Kills: %d", enemyDeathCount_);
    DrawFormatString(10, 70, GetColor(255, 255, 255), "Total: %d", totalScore_);
    DrawFormatString(10, 90, GetColor(255, 255, 255), "Displayed: %d", displayedScore_);
#endif
}

// スコアを計算
void SceneScore::CalculateScore(void)
{
    // 距離スコア: 移動距離 × 係数
    distanceScore_ = static_cast<int>(playerDistance_ * DISTANCE_SCORE_RATE);

    // 撃破スコア: 撃破数 × 係数
    killScore_ = enemyDeathCount_ * KILL_SCORE_RATE;

    // 合計スコア
    totalScore_ = distanceScore_ + killScore_;

    // 負の値にならないようにする
    if (totalScore_ < 0) totalScore_ = 0;
}

// スコア表示のアニメーション更新
void SceneScore::UpdateScoreAnimation(void)
{
    auto& sceneMgr = SceneManager::GetInstance();
    float deltaTime = sceneMgr.GetDeltaTime();

    // まだカウントアップ中の場合
    if (displayedScore_ < totalScore_)
    {
        // カウントアップ速度に応じてスコアを増やす
        displayedScore_ += static_cast<int>(countUpSpeed_ * deltaTime * 60.0f);

        // 最大値を超えないようにする
        if (displayedScore_ > totalScore_)
        {
            displayedScore_ = totalScore_;
        }
    }
}

// ランク判定
const char* SceneScore::GetRank(void) const
{
    if (totalScore_ >= 10000)
    {
        return "S";
    }
    else if (totalScore_ >= 7000)
    {
        return "A";
    }
    else if (totalScore_ >= 5000)
    {
        return "B";
    }
    else if (totalScore_ >= 3000)
    {
        return "C";
    }
    else if (totalScore_ >= 1000)
    {
        return "D";
    }
    else
    {
        return "E";
    }
}