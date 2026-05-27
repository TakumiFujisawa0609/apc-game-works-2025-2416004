#pragma once

/// @brief 全てのシーン（タイトル、ゲーム本編、リザルト等）の基底クラス
class SceneBase
{
public:
    /// @brief コンストラクタ
    SceneBase(void) = default;

    /// @brief デストラクタ
    virtual ~SceneBase(void) = 0;

    /// @brief 初期化処理
    virtual void Initialize(void) = 0;

    /// @brief 更新処理
    virtual void Update(void) = 0;

    /// @brief 描画処理
    virtual void Draw(void) = 0;

    /// @brief 解放処理
    virtual void Release(void) = 0;

    /// @brief 中断したシーンの再開処理（ポーズ解除時など）
    virtual void OnResume(void) {}

    /// @brief リソースの読み込み開始
    virtual void Load(void) = 0;

    /// @brief リソースの読み込み完了処理
    virtual void EndLoad(void) = 0;

    /// @brief 現在ロード中かどうかを取得
    /// @return ロード中ならtrue
    bool IsLoading(void) const;

private:
    // ロード中フラグ
    bool isLoading_;    
};