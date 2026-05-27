#pragma once
#include "SceneBase.h"
#include <vector>
#include <string>

/// @brief 制作スタッフや権利表記を表示するクレジットシーンクラス
class SceneCredit : public SceneBase
{
public:
    /// @brief コンストラクタ
    SceneCredit(void);

    /// @brief デストラクタ
    ~SceneCredit(void) override = default;

    /// @brief 初期化処理
    void Initialize(void) override;

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief リソース読み込み（本シーンでは使用しない、またはInitializeで完結）
    void Load(void) override {}

    /// @brief ロード完了処理
    void EndLoad(void) override {}

private:
    /// @brief クレジットの1行分の情報を保持する構造体
    struct CreditLine
    {
        std::string text;   // 表示する文字列
        int offsetCenterY;  // 画面中央からの上下オフセット（正：下、負：上）
        unsigned int color; // 文字色
        int fontSize;       // フォントサイズ
    };

    // 背景画像やBGM等のハンドル
    int handle_;                        

    // クレジット行のリスト
    std::vector<CreditLine> credits_;  
};