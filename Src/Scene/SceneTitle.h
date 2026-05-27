#pragma once
#include "SceneBase.h"
#include "../DrawUI/SceneUI/SceneUi.h"

class Grid;

/// @brief ゲームの起動直後に表示されるタイトルシーンクラス
class SceneTitle : public SceneBase
{
public:
    // 定数関連
    /// @brief ポーズUIの総数
    static constexpr int PAUSE_UI_COUNT = 2;

    /// @brief コンストラクタ
    SceneTitle(void);

    /// @brief デストラクタ
    ~SceneTitle(void) override = default;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief ゲーム終了が要求されているか取得
    /// @return 終了するならtrue
    bool IsExitRequested(void) const;

    /// @brief リソースの読み込み開始
    void Load(void) override;

    /// @brief リソースの読み込み完了処理
    void EndLoad(void) override;

private:
    // 開発用グリッド表示
    Grid* grid_;                        

    // UI関連
    std::unique_ptr<SceneUi> uiMain_;   // メインUI制御（ボタン等）
    int logo_;                          // タイトルロゴの画像ハンドル
    int pauseUiCount_;                  // ポーズUIの状態カウント
    int howToPlayPage_;                 // 操作説明の現在のページ番号

    // 背景で再生する動画ハンドル
    int movieHandle_;                   

    // 状態管理関連
    bool isDecided_;                    // 項目決定済みフラグ
    bool showBlackBackground_;          // 暗転用背景の表示フラグ
    bool exitRequested_;                // アプリケーション終了フラグ
    bool isPlay_;                       // 動画再生状態フラグ

    /// @brief デバッグ情報の描画
    void DrawDebug(void);
};