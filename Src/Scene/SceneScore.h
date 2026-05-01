#pragma once
#include <memory>
#include "SceneBase.h"

class SceneScore : public SceneBase
{
public:
    // コンストラクタ
    SceneScore(void);
    // デストラクタ
    ~SceneScore(void) = default;
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
    // 描画処理(デバッグ)
    void DrawDebug(void);

    // スコア関連
    float playerDistance_;      // プレイヤーの移動距離
    int enemyDeathCount_;       // エネミーの死亡数
    int distanceScore_;         // 距離スコア
    int killScore_;             // 撃破スコア
    int totalScore_;            // 合計スコア

    // スコア描画用背景
    int bgHandle_;              // スコア背景画像ハンドル
    int bgMovieId_;             // 背景映像

    // スコア計算用の定数
    static constexpr float DISTANCE_SCORE_RATE = 0.1f;  // 1mあたり10点
    static constexpr int KILL_SCORE_RATE = 100;          // 1体あたり100点

    // アニメーション用
    float scoreDisplayTimer_;   // スコア表示タイマー
    int displayedScore_;        // 現在表示中のスコア
    float countUpSpeed_;        // カウントアップ速度

    // スコアを計算
    void CalculateScore(void);

    // スコア表示のアニメーション更新
    void UpdateScoreAnimation(void);

    // ランク判定
    const char* GetRank(void) const;
};