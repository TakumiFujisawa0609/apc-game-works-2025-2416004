#pragma once
#include "../Stage/Ground.h"
#include <vector>
#include <memory>
#include <DxLib.h>

// 前方宣言を追加
class Camera;

// ステージの地面生成マネージャー
class GroundManager
{
public:

    // コンストラクタ
    GroundManager(void);

    // デストラクタ
    ~GroundManager(void);

    // 読み込み処理
    void Load(void);

    // 初期化
    void Init(void);

    // カメラ位置に近いタイルのモデルIDと位置を取得
    std::vector<std::pair<int, VECTOR>> GetNearbyTiles(const VECTOR& cameraPos, float range) const;

    // 更新処理
    void Update(void);

    // 描画処理
    void Draw(const VECTOR& centerPos, const VECTOR& cameraPos, const VECTOR& cameraDir);

    // 解放処理
    void Release(void);

    // プレイヤーの座標を設定
    void SetPlayerPos(const VECTOR& pos);

    // 敵の座標を設定
    void SetEnemyPos(const std::vector<VECTOR>& positions);

private:
    // 地面の最大数
    static constexpr int TILE_COUNT = 100;

    // 地面の大きさ
    static constexpr float TILE_SIZE = 100.0f;

    // 登録範囲
    static constexpr float REGISTER_RANGE = 100.0f;

    // 登録範囲の二乗
    static constexpr float REGISTER_RANGE_SQ = REGISTER_RANGE * REGISTER_RANGE;

    // 地面部品
    std::vector<std::shared_ptr<Ground>> grounds_;

    // 登録済みの地面
    std::vector<std::shared_ptr<Ground>> registeredGrounds_;

    // 元モデル
    int baseModelId_;

    // ロード済みか判定
    bool isLoaded_;

    // プレイヤーの座標を取得
    VECTOR playerPos_;

    // 敵のの座標を取得
    std::vector<VECTOR> enemyPoss_;

    // 周辺の地面を登録（追加）
    void RegisterNearbyGrounds(void);
};