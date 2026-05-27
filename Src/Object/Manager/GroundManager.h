#pragma once

#include <vector>
#include <map>
#include <memory>

#include "../Stage/Ground.h"

class Camera;

/// @brief ステージの地面生成マネージャー
/// @details プレイヤーの周囲に動的に地面タイル（Ground）を生成・管理する
class GroundManager
{
public:

    /// @brief コンストラクタ
    GroundManager(void);

    /// @brief デストラクタ
    ~GroundManager(void);

    /// @brief 地面モデルなどのリソース読み込み
    void Load(void);

    /// @brief 初期化処理
    void Initialize(void);

    /// @brief 更新処理（生成・削除の判定など）
    void Update(void);

    /// @brief 描画処理（カリング考慮）
    /// @param centerPos 描画の中心座標
    /// @param cameraPos カメラ座標
    /// @param cameraDir カメラ方向ベクトル
    void Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir);

    /// @brief 解放処理
    void Release(void);

    /// @brief プレイヤーの現在座標を設定する
    /// @param pos プレイヤー座標
    void SetPlayerPos(const VECTOR& pos);

    /// @brief 全エネミーの現在座標を設定する
    /// @param positions エネミー座標のリスト
    void SetEnemyPos(const std::vector<VECTOR>& positions);

private:

    /// @brief グリッド座標を識別するための構造体
    struct GridPos
    {
        int x, z;
        // mapのキーとして使用するための比較演算子
        bool operator<(const GridPos& other) const {
            return x < other.x || (x == other.x && z < other.z);
        }
    };

    // 配置・サイズ設定関連の定数
    static constexpr int TILE_COUNT = 160;                                      // 地面の最大保持数
    static constexpr float TILE_SIZE = 1150.0f;                                 // タイル1枚の一辺の長さ
    static constexpr float REGISTER_RANGE = TILE_SIZE * 2.5f;                   // 登録を行う範囲
    static constexpr float REGISTER_RANGE_SQ = REGISTER_RANGE * REGISTER_RANGE; // 登録範囲の二乗（計算用）
    static constexpr float SPAWN_RANGE = 12000.0f;                              // 地面を生成・維持する最大距離

    // タイル管理関連の変数
    std::map<GridPos, std::shared_ptr<Ground>> activeGrounds_; // 現在有効なグリッド座標とインスタンスの紐付け
    std::vector<std::shared_ptr<Ground>> grounds_;             // 全ての地面インスタンス
    std::vector<std::shared_ptr<Ground>> registeredGrounds_;   // 更新・描画対象として登録された地面

    // リソース・状態管理関連の変数
    int baseModelId_;                                        // 地面の元となるモデルID
    bool isLoaded_;                                          // ロードが完了しているかのフラグ

    // 位置情報関連の変数
    VECTOR playerPos_;                                       // プレイヤーの現在座標
    std::vector<VECTOR> enemyPoss_;                          // 全エネミーの現在座標リスト

    /// @brief プレイヤーやカメラの周辺にある地面を検索し、処理対象として登録する
    void RegisterNearbyGrounds(void);

    /// @brief 座標から対応するグリッドのインデックスを算出する
    /// @param pos 算出したい座標
    /// @return グリッドインデックス
    int GetGridIndex(const VECTOR& pos) const;

    /// @brief 指定座標の周辺にあるグリッドインデックスの一覧を取得する
    /// @param pos 中心座標
    /// @param range 取得範囲
    /// @return インデックスのリスト
    std::vector<int> GetNearbyGridIndices(const VECTOR& pos, float range) const;
};