#include "SceneScore.h"
#include "../Manager/Generic/ResourceManager.h"
#include "../Manager/Generic/SceneManager.h"
#include "../Manager/Generic/InputManager.h"
#include "../Manager/Decoration/SoundManager.h"
#include "../Manager/System/Loading.h"
#include "../Scene/SceneTitle.h"
#include "../Manager/System/TimeManager.h"
#include "../Application.h"

SceneScore::SceneScore(void)
    : playerDistance_(0.0f)
    , enemyDeathCount_(0)
    , distanceScore_(0)
    , killScore_(0)
    , totalScore_(0)
    , scoreDisplayTimer_(0.0f)
    , displayedScore_(0)
    , countUpSpeed_(50.0f)  // 1秒あたり50点ずつカウントアップ
{
}

void SceneScore::Load(void)
{
    SceneBase::Load();
    // 時間カウントリセット
    TimeManager::GetInstance().Reset();

    ResourceManager::GetInstance().InitGameClear();

    Loading::GetInstance()->SetProgress(25.0f);

    SoundManager::GetInstance().Add(SoundManager::TYPE::BGM, SoundManager::SOUND::BGM_SCORE, ResourceManager::GetInstance().Load(ResourceManager::SRC::BGM_SCORE).handleId_);

    SoundManager::GetInstance().AdjustVolume(SoundManager::SOUND::BGM_SCORE, 30);

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

void SceneScore::Init(void)
{
    // サウンド
    auto& res = ResourceManager::GetInstance();

    // 初期BGM
    //SoundManager::GetInstance().Play(SoundManager::SOUND::BGM_SCORE);

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
    auto& sound = SoundManager::GetInstance();
    auto& input = InputManager::GetInstance();
    auto& sceneMgr = SceneManager::GetInstance();

    // スコアアニメーション更新
    UpdateScoreAnimation();

    if (input.IsTrgDown(KEY_INPUT_SPACE))
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
    // 画面サイズ
    const int SCREEN_W = Application::SCREEN_SIZE_X;
    const int SCREEN_H = Application::SCREEN_SIZE_Y;
    const int CENTER_X = SCREEN_W / 2;

    // 背景を暗くする
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // タイトル
    const char* title = "GAME RESULT";
    DrawFormatString(CENTER_X - 150, 100, GetColor(255, 255, 0), title);

    // 区切り線
    DrawLine(CENTER_X - 300, 150, CENTER_X + 300, 150, GetColor(255, 255, 255));

    // 移動距離の表示
    DrawFormatString(CENTER_X - 250, 200, GetColor(200, 200, 200), "Distance:");
    DrawFormatString(CENTER_X + 50, 200, GetColor(255, 255, 255), "%.2f m", playerDistance_);
    DrawFormatString(CENTER_X - 250, 230, GetColor(200, 200, 200), "Distance Score:");
    DrawFormatString(CENTER_X + 50, 230, GetColor(255, 255, 255), "%d pts", distanceScore_);

    // 撃破数の表示
    DrawFormatString(CENTER_X - 250, 280, GetColor(200, 200, 200), "Enemies Defeated:");
    DrawFormatString(CENTER_X + 50, 280, GetColor(255, 255, 255), "%d", enemyDeathCount_);
    DrawFormatString(CENTER_X - 250, 310, GetColor(200, 200, 200), "Kill Score:");
    DrawFormatString(CENTER_X + 50, 310, GetColor(255, 255, 255), "%d pts", killScore_);

    // 区切り線
    DrawLine(CENTER_X - 300, 360, CENTER_X + 300, 360, GetColor(255, 255, 255));

    // 合計スコアの表示（アニメーション付き）
    DrawFormatString(CENTER_X - 250, 400, GetColor(255, 255, 0), "TOTAL SCORE:");

    // カウントアップアニメーション
    int displayScore = displayedScore_;
    if (displayScore >= totalScore_)
    {
        displayScore = totalScore_;
        // スコア確定時に文字を大きく表示
        DrawFormatString(CENTER_X + 50, 395, GetColor(255, 255, 0), "%d", displayScore);
    }
    else
    {
        DrawFormatString(CENTER_X + 50, 400, GetColor(255, 255, 255), "%d", displayScore);
    }

    // ★ランク表示
    if (displayedScore_ >= totalScore_)
    {
        const char* rank = GetRank();
        DrawFormatString(CENTER_X - 100, 470, GetColor(255, 200, 0), "RANK: %s", rank);
    }

    // 区切り線
    DrawLine(CENTER_X - 300, 520, CENTER_X + 300, 520, GetColor(255, 255, 255));

    // 操作説明
    DrawFormatString(CENTER_X - 150, 580, GetColor(150, 150, 150), "Press SPACE to Title");

#ifdef _DEBUG
    DrawDebug();
#endif
}

void SceneScore::Release(void)
{
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