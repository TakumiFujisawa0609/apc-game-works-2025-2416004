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

    bgHandle_ = ResourceManager::GetInstance().
        Load(ResourceManager::SRC::GAMECLERA_LOGO).handleId_;

    bgMovieId_ = ResourceManager::GetInstance().
        Load(ResourceManager::SRC::BG_MOVIE).handleId_;



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

    SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_SCORE,
        ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_SCORE).handleId_);

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

    if (input.IsTrgDown(KEY_INPUT_SPACE) || input.IsClickMouseLeft() ||
        input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::DOWN))
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
    // 描画レイアウト関連定数
    const int SCREEN_W = Application::SCREEN_SIZE_X;
    const int SCREEN_H = Application::SCREEN_SIZE_Y;
    const int CENTER_X = SCREEN_W / 2;
    const int CENTER_Y = SCREEN_H / 2;
    const float LEFT_QUARTER_RATE = 0.25f;
    const int CONTENT_WIDTH = 400;
    const int LEFT_QUARTER_X = static_cast<int>(SCREEN_W * LEFT_QUARTER_RATE);
    const int DRAW_X = LEFT_QUARTER_X - (CONTENT_WIDTH / 2);

    // フォントサイズ関連定数
    const int FONT_SIZE_TITLE = 48;
    const int FONT_SIZE_LABEL = 24;
    const int FONT_SIZE_VALUE = 26;
    const int FONT_SIZE_SCORE = 22;
    const int FONT_SIZE_TOTAL = 32;
    const int FONT_SIZE_TOTAL_VALUE = 44;
    const int FONT_SIZE_RANK_LABEL = 30;
    const int FONT_SIZE_RANK_VALUE = 80;
    const int FONT_SIZE_GUIDE = 22;

    // 表示位置オフセット関連定数
    const int START_Y = 160;
    const int STEP_Y_TITLE = 80;
    const int STEP_Y_MARGIN = 50;
    const int STEP_Y_LABEL = 35;
    const int STEP_Y_SECTION = 70;
    const int STEP_Y_LARGE_SECTION = 90;
    const int VALUE_X_OFFSET = 200;
    const int TOTAL_X_OFFSET = 400;
    const int RANK_LABEL_X_OFFSET = 20;
    const int RANK_LABEL_Y_OFFSET = 15;
    const int RANK_VALUE_X_OFFSET = 150;
    const int RANK_VALUE_Y_OFFSET = -10;
    const int GUIDE_Y_BOTTOM_MARGIN = 120;

    // 背景動画描画関連
    if (bgMovieId_ != -1)
    {
        int movieWidth, movieHeight;
        GetGraphSize(bgMovieId_, &movieWidth, &movieHeight);

        // アスペクト比を維持して画面を埋める計算
        float scaleX = static_cast<float>(SCREEN_W) / movieWidth;
        float scaleY = static_cast<float>(SCREEN_H) / movieHeight;
        float scale = (scaleX > scaleY) ? scaleX : scaleY;

        DrawRotaGraph3(CENTER_X, CENTER_Y,
            movieWidth / 2, movieHeight / 2,
            scale, scale, 0.0,
            bgMovieId_, true);
    }

    // 配色設定関連
    const unsigned int titleColor = GetColor(20, 20, 20);     // ほぼ黒
    const unsigned int labelColor = GetColor(50, 50, 50);     // 濃い灰色
    const unsigned int valueColor = GetColor(0, 50, 150);     // 鮮やかな紺
    const unsigned int scoreAddColor = GetColor(0, 120, 0);      // 濃い緑
    const unsigned int guideColor = GetColor(100, 100, 100);  // 中間の灰色
    const unsigned int rankLabelColor = GetColor(80, 80, 80);    // 暗い灰色
    const unsigned int blackColor = GetColor(0, 0, 0);

    auto& font = Font::GetInstance();
    int drawY = START_Y;

    // 背景画像描画関連
    DrawRotaGraph3(0, 0, 0, 0, 1.0f, 1.0f, 0.0f, bgHandle_, true);

    // タイトル表示関連
    font.DrawDefaultText(DRAW_X, drawY, "ゲームリザルト", titleColor, FONT_SIZE_TITLE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += STEP_Y_TITLE + STEP_Y_MARGIN;

    // 移動距離表示関連
    font.DrawDefaultText(DRAW_X, drawY, "移動距離", labelColor, FONT_SIZE_LABEL, Font::FONT_TYPE_ANTIALIASING_EDGE);
    char distStr[64], distScoreStr[64];
    sprintf_s(distStr, "%.2f m", playerDistance_);
    sprintf_s(distScoreStr, "+ %d pts", distanceScore_);

    font.DrawDefaultText(DRAW_X + VALUE_X_OFFSET, drawY, distStr, valueColor, FONT_SIZE_VALUE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += STEP_Y_LABEL;
    font.DrawDefaultText(DRAW_X + VALUE_X_OFFSET, drawY, distScoreStr, scoreAddColor, FONT_SIZE_SCORE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += STEP_Y_SECTION;

    // 撃破数表示関連
    font.DrawDefaultText(DRAW_X, drawY, "エネミー討伐数", labelColor, FONT_SIZE_LABEL, Font::FONT_TYPE_ANTIALIASING_EDGE);
    char killStr[64], killScoreStr[64];
    sprintf_s(killStr, "%d", enemyDeathCount_);
    sprintf_s(killScoreStr, "+ %d pts", killScore_);

    font.DrawDefaultText(DRAW_X + VALUE_X_OFFSET, drawY, killStr, valueColor, FONT_SIZE_VALUE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += STEP_Y_LABEL;
    font.DrawDefaultText(DRAW_X + VALUE_X_OFFSET, drawY, killScoreStr, scoreAddColor, FONT_SIZE_SCORE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += STEP_Y_LARGE_SECTION;

    // 合計スコア表示関連
    font.DrawDefaultText(DRAW_X, drawY, "TOTAL SCORE", blackColor, FONT_SIZE_TOTAL, Font::FONT_TYPE_ANTIALIASING_EDGE);

    char totalStr[64];
    sprintf_s(totalStr, "%d", displayedScore_);

    // カウントアップ完了時に色を変える
    unsigned int totalDisplayColor = (displayedScore_ >= totalScore_) ? GetColor(180, 130, 0) : blackColor;
    font.DrawDefaultText(DRAW_X + TOTAL_X_OFFSET, drawY, totalStr, totalDisplayColor, FONT_SIZE_TOTAL_VALUE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    drawY += STEP_Y_LARGE_SECTION;

    // ランク表示アニメーション関連
    if (displayedScore_ >= totalScore_)
    {
        const char* rank = GetRank();
        unsigned int rankColor = GetColor(40, 40, 40);
        if (rank[0] == 'S') { rankColor = GetColor(180, 0, 180); }
        else if (rank[0] == 'A') { rankColor = GetColor(200, 80, 0); }

        font.DrawDefaultText(DRAW_X + RANK_LABEL_X_OFFSET, drawY + RANK_LABEL_Y_OFFSET, "RANK", rankLabelColor, FONT_SIZE_RANK_LABEL, Font::FONT_TYPE_ANTIALIASING_EDGE);
        font.DrawDefaultText(DRAW_X + RANK_VALUE_X_OFFSET, drawY + RANK_VALUE_Y_OFFSET, rank, rankColor, FONT_SIZE_RANK_VALUE, Font::FONT_TYPE_ANTIALIASING_EDGE);
    }

    // 操作ガイド表示関連
    font.DrawDefaultText(DRAW_X + RANK_LABEL_X_OFFSET, SCREEN_H - GUIDE_Y_BOTTOM_MARGIN, "CLICK or SPACE TO TITLE", guideColor, FONT_SIZE_GUIDE, Font::FONT_TYPE_ANTIALIASING_EDGE);

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

const char* SceneScore::GetRank(void) const
{
    // ランク判定用しきい値関連
    const int rankS = 10000;
    const int rankA = 7000;
    const int rankB = 5000;
    const int rankC = 3000;
    const int rankD = 1000;

    // ランク判定ロジック関連
    if (totalScore_ >= rankS)
    {
        return "S";
    }
    else if (totalScore_ >= rankA)
    {
        return "A";
    }
    else if (totalScore_ >= rankB)
    {
        return "B";
    }
    else if (totalScore_ >= rankC)
    {
        return "C";
    }
    else if (totalScore_ >= rankD)
    {
        return "D";
    }
    else
    {
        return "E";
    }
}