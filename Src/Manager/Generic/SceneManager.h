#pragma once
#include "../../Application.h"

class SceneBase;
class Camera;

class SceneManager 
{
public:

    // インスタンスを生成する
    static void CreateInstance(void);

    // インスタンスを取得する
    static SceneManager& GetInstance(void);

    // インスタンスを破棄する
    static void DestroyInstance(void);

    // 初期化する
    void Initialize(void);

    // 更新する
    void Update(void);

    // 描画する
    void Draw(void);

    // 解放する
    void Release(void);

    // シーンを変更する（全削除→新規追加）
    void ChangeScene(std::shared_ptr<SceneBase> scene);

    // シーンを積む（上に追加する）
    void PushScene(std::shared_ptr<SceneBase> scene);

    // シーンを外す（上を削除する）
    void PopScene(void);

    // シーンをジャンプする（全削除→新規ロード）
    void JumpScene(std::shared_ptr<SceneBase> scene);

    // ゲーム終了フラグを取得する
    bool GetGameEnd(void) const;

    // デルタタイムを取得する
    float GetDeltaTime(void) const;

    // カメラを取得する
    std::shared_ptr<Camera> GetCamera(void) const;

    // ゲームを終了させる
    void GameEnd(void);

    // プレイヤーの移動距離を設定
    void SetPlayerDistance(float distance);
    
    // プレイヤーの移動距離を取得
    float GetPlayerDistance(void) const;
    
    // プレイヤーの移動距離をリセット
    void ResetPlayerDistance(void);

    // エネミーの死亡数を設定
    void SetEnemyDeathCount(int count);
   
    // エネミーの死亡数を取得
    int GetEnemyDeathCount(void) const;
    
    // エネミーの死亡数をリセット
    void ResetEnemyDeathCount(void);
    
    // エネミーの死亡数を加算
    void AddEnemyDeathCount(int add = 1);

    // ゲーム統計をリセット（距離と死亡数を一括リセット）
    void ResetGameStats(void);

private:
    // 唯一のインスタンス
    static SceneManager* instance_;

    // シーンを保持する（スタック構造）
    std::list<std::shared_ptr<SceneBase>> scenes_;

    // シーンアクセスを保護するミューテックス
    std::mutex sceneMutex_;

    // ゲーム終了フラグ
    bool isGameEnd_;

    // シーン切り替え中フラグ
    bool isSceneChanging_;

    // カメラ
    std::shared_ptr<Camera> camera_;

    // 前フレームの時刻
    std::chrono::system_clock::time_point preTime_;

    // デルタタイム
    float deltaTime_;

    // プレイヤーの移動距離
    float playerDistance_;
    
    // エネミーの死亡数
    int enemyDeathCount_;

    // 3D描画設定を初期化する
    void Init3D(void);

    // デルタタイムをリセットする
    void ResetDeltaTime(void);

    // コンストラクタ
    SceneManager(void);

    // デストラクタ
    ~SceneManager(void);

    // コピーコンストラクタを禁止する
    SceneManager(const SceneManager&) = delete;

    // 代入演算子を禁止する
    SceneManager& operator=(const SceneManager&) = delete;

    // ムーブコンストラクタを禁止する
    SceneManager(SceneManager&&) = delete;

    // ムーブ代入演算子を禁止する
    SceneManager& operator=(SceneManager&&) = delete;
};
