#pragma once

#include <DxLib.h>

#include "../UnitBase.h"

/// @brief ステージの地面を構成する部品クラス
class Ground : public UnitBase
{
public:
    /// @brief コンストラクタ
    Ground(void);

    /// @brief デストラクタ
    ~Ground(void) override;

    /// @brief 初期化処理
    /// @param pos 設置座標
    /// @param modelId 使用するモデルのハンドル
    void Initialize(const VECTOR& pos, int modelId);

    /// @brief 初期化処理（CollisionControllerへの登録を行わない）
    /// @param pos 設置座標
    /// @param modelId 使用するモデルのハンドル
    void InitWithoutRegister(const VECTOR& pos, int modelId);

    /// @brief リソース読み込み（本クラスでは使用しない）
    void Load(void) override {}

    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) const override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief タイルサイズの取得
    /// @return スケール適用済みのタイルサイズ
    float GetTileSize(void) const;

protected:
    /// @brief 衝突判定の初期化
    void InitCollider(void) override;

private:

    // 定数関連
    static constexpr float BASE_TILE_SIZE = 100.0f; // タイルの基本サイズ（スケール適用前）
    static constexpr float TILE_SCALE = 10.0f;      // タイルの表示スケール

    // CollisionControllerに登録済みか
    bool isRegistered_;           

};