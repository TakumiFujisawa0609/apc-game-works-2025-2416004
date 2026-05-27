#pragma once

/// @brief 開発の目安となる地面の格子状（グリッド）ラインを描画するクラス
class Grid
{
public:
    // 線の全長
    static constexpr float LEN = 1200.0f;

    // 線の全長の半分
    static constexpr float HLEN = LEN / 2.0f;

    // 線と線の間隔
    static constexpr float TERM = 50.0f;

    // 線の総本数
    static const int NUM = static_cast<int>(LEN / TERM);

    // 線の本数の半分
    static const int HNUM = NUM / 2;

    /// @brief コンストラクタ
    Grid(void);

    /// @brief デストラクタ
    ~Grid(void);

    /// @brief 初期化処理
    void Initialize(void);

    /// @brief 更新処理
    void Update(void);

    /// @brief 描画処理
    void Draw(void);

    /// @brief 解放処理
    void Release(void);
};