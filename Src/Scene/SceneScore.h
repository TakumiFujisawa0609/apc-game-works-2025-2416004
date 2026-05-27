#pragma once
#include <memory>
#include "SceneBase.h"

/// @brief ゲーム終了後のスコア算出・リザルト表示を行うシーンクラス
class SceneScore : public SceneBase
{
public:
    /// @brief コンストラクタ
    SceneScore(void);

    /// @brief デストラクタ
    ~SceneScore(void) = default;

    /// @brief リソースの読み込み開始
    void Load(void) override;

    /// @brief リソースの読み込み完了処理
    void EndLoad(void) override;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

private:
    /// @brief デバッグ用情報の描画
    void DrawDebug(void);

    // スコア数値関連
    float playerDistance_;      // プレイヤーの移動距離
    int enemyDeathCount_;       // エネミーの死亡数
    int distanceScore_;         // 距離から算出されたスコア
    int killScore_;             // 撃破数から算出されたスコア
    int totalScore_;            // 全項目の合計スコア

    // リソース関連
    int bgHandle_;              // スコア背景の画像ハンドル
    int bgMovieId_;             // 背景で再生する映像のハンドル

    // スコア計算定数関連
    static constexpr float DISTANCE_SCORE_RATE = 0.1f;  // 距離に対するスコア倍率
    static constexpr int KILL_SCORE_RATE = 100;         // 撃破1体あたりのスコア

    // 演出・アニメーション関連
    float scoreDisplayTimer_;   // 演出開始からの経過時間
    int displayedScore_;        // 画面上でカウントアップ中の表示用スコア
    float countUpSpeed_;        // スコアが加算される速度

    /// @brief 各種実績から最終的なスコアを計算する
    void CalculateScore(void);

    /// @brief スコアが徐々に増えていくアニメーションの更新
    void UpdateScoreAnimation(void);

    /// @brief 合計スコアに基づいたランク（S, A, B...）を取得する
    /// @return ランク文字列のポインタ
    const char* GetRank(void) const;
};