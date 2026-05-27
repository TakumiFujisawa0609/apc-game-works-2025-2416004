#pragma once
#include "SceneBase.h"

/// @brief ゲームの各種設定や操作説明を行うオプションシーンクラス
class SceneOption : public SceneBase
{
public:
    /// @brief コンストラクタ
    SceneOption(void);

    /// @brief デストラクタ
    ~SceneOption(void) override = default;

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
    // 列挙型関連
    /// @brief オプション画面の表示モード
    enum class DISPLAY_MODE
    {
        Settings,    // 設定（スライダー・チェックボックス）
        Controls     // 操作説明
    };

    // UI構造体関連
    /// @brief ボタンUIの構造体
    struct Button
    {
        int x, y, w, h;     // 座標とサイズ
        const char* label;  // 表示テキスト
        int textX;          // テキスト描画用X座標
        int textY;          // テキスト描画用Y座標
    };

    /// @brief スライダーUIの構造体（音量設定用）
    struct Slider
    {
        int x, y, width, height;
        float value;        // 0.0f ～ 1.0f の値
        bool isDragging;    // ドラッグ中フラグ
        const char* label;
    };

    /// @brief チェックボックスUIの構造体（ON/OFF設定用）
    struct CheckBox
    {
        int x, y;
        int size;
        bool* pTarget;      
        const char* label;
    };

    // 現在表示している画面モード
    DISPLAY_MODE currentMode_;          

    // オーディオ設定関連
    Slider bgmSlider_;                  // BGM音量スライダー
    Slider seSlider_;                   // SE音量スライダー

    // タブ・ボタン関連
    Button settingBtn_;                 // 設定画面切り替えボタン
    Button controlBtn_;                 // 操作説明画面切り替えボタン

    // カメラ設定関連
    CheckBox invertPitchCb_;            // 上下反転設定
    CheckBox invertYawCb_;              // 左右反転設定

    // 入力・操作管理関連
    int selectIndex_;                   // 項目選択用のインデックス
    bool stickFree_;                    // スティック入力の遊び・リセット管理

    // リソース関連
    int handle_;                        // UI用画像ハンドル1
    int handle2_;                       // UI用画像ハンドル2

    /// @brief マウスがスライダーの範囲内にあるかチェック
    /// @param slider 対象のスライダー
    /// @param mx マウスのX座標
    /// @param my マウスのY座標
    /// @return 範囲内ならtrue
    bool IsMouseOver(const Slider& slider, int mx, int my);

    /// @brief スライダーの更新ロジック（ドラッグ処理など）
    /// @param slider 更新するスライダー
    /// @param mx マウスのX座標
    void UpdateSlider(Slider& slider, int mx);
};