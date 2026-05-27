#pragma once

#include <string>

#include "../../Object/UnitBase.h"

/// @brief エネミーの生成（スポーン）を管理するクラス
class EnemySpawner : public UnitBase
{
public:
    /// @brief コンストラクタ
    /// @param position スポナーの設置座標
    /// @param spawnRange エネミーが出現する半径
    /// @param enemyType 生成するエネミーの種類
    EnemySpawner(const VECTOR& position, float spawnRange, const std::string& enemyType);

    /// @brief デストラクタ
    ~EnemySpawner(void) = default;

    /// @brief 初期化処理
    void Initialize(void) override;
    /// @brief 更新処理
    void Update(void) override;

    /// @brief 描画処理
    void Draw(void) const override;

    /// @brief 解放処理
    void Release(void) override;

    /// @brief 更新処理（デルタタイム版）
    /// @param deltaTime 前フレームからの経過時間
    void Update(float deltaTime);

    /// @brief スポーン座標をランダムに生成
    /// @return スポーン範囲内のランダムな座標
    VECTOR GetRandomSpawnPos(void) const;

    /// @brief 設置座標の取得
    /// @return スポナーの座標
    const VECTOR& GetPosition(void) const;

    /// @brief スポーン範囲の取得
    /// @return スポーン半径
    float GetSpawnRange(void) const;

    /// @brief エネミータイプの取得
    /// @return エネミーの種類名
    const std::string& GetEnemyType(void) const;

    /// @brief スポーン間隔の取得
    /// @return 秒単位の間隔
    float GetSpawnInterval(void) const;

    /// @brief 最大エネミー数の取得
    /// @return 最大数
    int GetMaxEnemies(void) const;

    /// @brief アクティブ状態の取得
    /// @return 有効ならtrue
    bool IsActive(void) const;

    /// @brief 起動範囲の取得
    /// @return プレイヤーが近づく必要がある距離
    float GetActivationRange(void) const;

    /// @brief エネミーレベルの取得
    /// @return 設定レベル
    int GetEnemyLevel(void) const;

    /// @brief プレイヤーの接近が必要かどうかの取得
    /// @return 必要ならtrue
    bool IsRequirePlayerInRange(void) const;

    /// @brief スポーン間隔の設定
    /// @param interval 設定する間隔（秒）
    void SetSpawnInterval(float interval);

    /// @brief 最大エネミー数の設定
    /// @param max 同時に存在できる最大数
    void SetMaxEnemies(int max);

    /// @brief アクティブ状態の設定
    /// @param active 有効にするならtrue
    void SetActive(bool active);

    /// @brief 現在のエネミー数の設定
    /// @param count 生存数
    void SetCurrentEnemyCount(int count);

    /// @brief 起動範囲の設定
    /// @param range プレイヤーを検知する半径
    void SetActivationRange(float range);

    /// @brief プレイヤー接近の必要性の設定
    /// @param require 必要ならtrue
    void SetRequirePlayerInRange(bool require);

    /// @brief エネミーレベルの設定
    /// @param level 出現させるレベル
    void SetEnemyLevel(int level);

    /// @brief モデルIDの設定
    /// @param modelId 使用するモデルのハンドル
    void SetModelId(int modelId);

    /// @brief スポーン可能かチェック
    /// @param playerPos プレイヤーの現在座標
    /// @return 生成可能ならtrue
    bool CanSpawn(const VECTOR& playerPos) const;

    /// @brief スポーンタイマーをリセット
    void ResetSpawnTimer(void);

    /// @brief プレイヤーが範囲内にいるかチェック
    /// @param playerPos プレイヤーの現在座標
    /// @return 範囲内ならtrue
    bool IsPlayerInRange(const VECTOR& playerPos) const;

    /// @brief 重力の計算
    void CalcGravityPow(void) override;

protected:
    /// @brief 衝突開始イベント
    /// @param info 衝突情報
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// @brief 衝突継続イベント
    /// @param info 衝突情報
    void OnCollisionStay(const CollisionInfo& info) override;

    /// @brief コライダー初期化
    void InitCollider(void) override;

private:
    // コライダー関連定数
    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f }; // 線分開始
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };    // 線分終了

    // 座標・範囲関連
    VECTOR position_;               // スポナーの座標
    float spawnRange_;              // スポーン可能半径

    // エネミー設定関連
    std::string enemyType_;         // スポーンする種類
    int enemyLevel_;                // スポーンするレベル
    int maxEnemies_;                // 最大生存数
    int currentEnemyCount_;         // 現在の生存数

    // タイマー・間隔関連
    float spawnInterval_;           // スポーン間隔
    float spawnTimer_;              // 経過時間カウント

    // 状態・条件関連
    bool isActive_;                 // 有効フラグ
    bool requirePlayerInRange_;     // プレイヤー接近必須フラグ
    float activationRange_;         // プレイヤー検知範囲

    // 見た目用モデルID
    int modelId_;                  
};