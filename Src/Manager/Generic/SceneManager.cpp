#include "SceneManager.h"
#include "../../Scene/SceneBase.h"
#include "../../Scene/SceneTitle.h"
#include "../System/CollisionController.h" 
#include "../Decoration/SoundManager.h"
#include "../System/TimeManager.h"
#include "Camera.h"
#include "../System/Loading.h"

// インスタンスを初期化する
SceneManager* SceneManager::instance_ = nullptr;

// インスタンスを生成する
void SceneManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new SceneManager();
    }
    instance_->Init();
}

// インスタンスを取得する
SceneManager& SceneManager::GetInstance(void)
{
    return *instance_;
}

// インスタンスを破棄する
void SceneManager::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

// コンストラクタ
SceneManager::SceneManager(void)
    : playerDistance_(0.0f)        
    , enemyDeathCount_(0)
{
    isGameEnd_ = false;
    isSceneChanging_ = false;
    deltaTime_ = 1.0f / 60.0f;
    preTime_ = std::chrono::system_clock::now();

    camera_ = std::make_shared<Camera>();
}

// デストラクタ
SceneManager::~SceneManager(void)
{
    Release();
}

// 初期化する
void SceneManager::Init(void)
{
    SoundManager::CreateInstance();
    TimeManager::CreateInstance();
    Loading::CreateInstance();
    CollisionController::CreateInstance();

    // カメラを初期化する
    camera_->Init();

    // ★ゲーム統計を初期化
    playerDistance_ = 0.0f;
    enemyDeathCount_ = 0;

    // 3D描画設定を初期化する
    Init3D();

    // 最初のシーンを設定する
    ChangeScene(std::make_shared<SceneTitle>());
}

// 3D描画設定を初期化する
void SceneManager::Init3D(void)
{
    // 背景色を設定する
    SetBackgroundColor(0, 0, 0);

    // Zバッファを有効にする
    SetUseZBuffer3D(true);

    // Zバッファへの書き込みを有効にする
    SetWriteZBuffer3D(true);

    // バックカリングを有効にする
    SetUseBackCulling(true);

    // ライティングを有効にする
    SetUseLighting(true);
    SetLightEnable(true);

    SetGlobalAmbientLight(GetColorF(0.8f, 0.8f, 0.8f, 1.0f));

    ChangeLightTypeDir(VGet(0.0f, -1.0f, 1.0f));  // ライトの方向
    SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));  // 拡散光
    SetLightSpcColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));  // 鏡面光
    SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));  // 環境光

    // フォグを設定する
    SetFogEnable(true);
    SetFogColor(5, 5, 5);
    SetFogStartEnd(10000.0f, 20000.0f);
}

// シーンを変更する（全削除→新規追加）
void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
    // 古いシーンを解放
    for (auto& s : scenes_)
        s->Release();
    scenes_.clear();

    // CollisionControllerをクリア
    CollisionController::GetInstance().Clear();

    // BGMを停止する
    SoundManager::GetInstance().StopAllBGM();

    // 新しいシーンを設定
    scenes_.push_back(scene);
    isSceneChanging_ = true;

    // 非同期ロード開始（ロード画面付き）
    Loading::GetInstance()->StartAsyncLoad([scene]() {
        scene->Load();
        });
}

// シーンを積む（上に追加する）
void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
    scenes_.push_back(scene);

    // 即時ロード・初期化
    scene->Load();
    scene->EndLoad();
    scene->Init();
}

// シーンを外す（上を削除する）
void SceneManager::PopScene()
{
    if (scenes_.size() > 1)
    {
        scenes_.back()->Release();
        scenes_.pop_back();
    }
}

// シーンをジャンプする（全削除→新規ロード）
void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
    scenes_.clear();

    // CollisionControllerをクリア
    CollisionController::GetInstance().Clear();

    // BGMを停止する
    SoundManager::GetInstance().StopAllBGM();

    isSceneChanging_ = true;
    scenes_.push_back(scene);

    // 非同期ロードを開始する
    Loading::GetInstance()->StartAsyncLoad([scene]() {
        scene->Load();
        });
}

// 更新する
void SceneManager::Update(void)
{
    if (scenes_.empty()) return;

    // 経過時間
    TimeManager::GetInstance().Update();

    auto nowTime = std::chrono::system_clock::now();
    deltaTime_ = std::chrono::duration<float>(nowTime - preTime_).count();
    preTime_ = nowTime;

    // ゲーム終了フラグが立っていたら何もしない
    if (isGameEnd_)
    {
        return; // ※破棄は Application 側で行う
    }

    std::shared_ptr<SceneBase> current = scenes_.back();

    // ロード中
    if (isSceneChanging_)
    {
        Loading::GetInstance()->Update();

        if (!Loading::GetInstance()->IsLoading())
        {
            current->EndLoad();
            current->Init();
            isSceneChanging_ = false;
        }
        return;
    }

    // 通常更新
    if (current) current->Update();

    if (camera_) camera_->UpdateBeforeCollision();

    // 衝突
    CollisionController::GetInstance().Update();

    if (camera_) camera_->Update();
}

// 描画する
void SceneManager::Draw(void)
{
    if (scenes_.empty()) return;

    // 描画先をバックバッファに設定する
    SetDrawScreen(DX_SCREEN_BACK);

    // 非同期ロード中は進捗バーのみ描画する
    if (Loading::GetInstance()->IsLoading() || isSceneChanging_)
    {
        Loading::GetInstance()->Draw();
        return;
    }

    // バックバッファをクリアする
    ClearDrawScreen();

    // カメラの設定を行う
    if (camera_) camera_->SetBeforeDraw();

    // シーンを描画する（コピーを使用して安全に処理）
    std::vector<std::shared_ptr<SceneBase>> scenesCopy(scenes_.begin(), scenes_.end());
    for (auto& scene : scenesCopy)
    {
        if (scene) scene->Draw();
    }

    // カメラの描画を行う
    if (camera_) camera_->Draw();
}

// 解放する
void SceneManager::Release(void)
{
    // ロード完了を待機する
    if (Loading::GetInstance()->IsLoading())
    {
        while (Loading::GetInstance()->IsLoading())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // 各シーンを解放する
    for (auto& scene : scenes_)
    {
        scene->Release();
    }
    scenes_.clear();

    // カメラを解放する
    camera_.reset();

    // 各マネージャーを破棄する
    SoundManager::GetInstance().Destroy();
    TimeManager::GetInstance().Destroy();
    Loading::GetInstance()->DestroyInstance();
    CollisionController::Destroy();
}

// ゲームを終了させる
void SceneManager::GameEnd(void)
{
    isGameEnd_ = true;
}

// ゲーム終了フラグを取得する
bool SceneManager::GetGameEnd(void) const
{
    return isGameEnd_;
}

// デルタタイムを取得する
float SceneManager::GetDeltaTime(void) const
{
    return deltaTime_;
}

// カメラを取得する
std::shared_ptr<Camera> SceneManager::GetCamera(void) const
{
    return camera_;
}

// デルタタイムをリセットする
void SceneManager::ResetDeltaTime(void)
{
    deltaTime_ = 1.0f / 60.0f;
    preTime_ = std::chrono::system_clock::now();
}


// プレイヤーの移動距離を設定
void SceneManager::SetPlayerDistance(float distance)
{
    playerDistance_ = distance;
}

// プレイヤーの移動距離を取得
float SceneManager::GetPlayerDistance(void) const
{
    return playerDistance_;
}

// プレイヤーの移動距離をリセット
void SceneManager::ResetPlayerDistance(void)
{
    playerDistance_ = 0.0f;
}

// エネミーの死亡数を設定
void SceneManager::SetEnemyDeathCount(int count)
{
    enemyDeathCount_ = count;
}

// エネミーの死亡数を取得
int SceneManager::GetEnemyDeathCount(void) const
{
    return enemyDeathCount_;
}

// エネミーの死亡数をリセット
void SceneManager::ResetEnemyDeathCount(void)
{
    enemyDeathCount_ = 0;
}

// エネミーの死亡数を加算
void SceneManager::AddEnemyDeathCount(int add)
{
    enemyDeathCount_ += add;
}

// ゲーム統計をリセット（距離と死亡数を一括リセット）
void SceneManager::ResetGameStats(void)
{
    playerDistance_ = 0.0f;
    enemyDeathCount_ = 0;
}