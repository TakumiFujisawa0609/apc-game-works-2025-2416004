#include "SceneManager.h"

#include <DxLib.h>
#include <cassert>

#include "../../Scene/SceneBase.h"
#include "../../Scene/SceneTitle.h"
#include "ResourceManager.h"
#include "../System/Collision.h"
#include "../System/CollisionManager.h"
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
    // 各マネージャーを生成する
    Collision::CreateInstance();
    SoundManager::CreateInstance();
    TimeManager::CreateInstance();
    Loading::CreateInstance();
    CollisionManager::CreateInstance();

    // カメラを初期化する
    camera_->Init();

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
void SceneManager::PopScene(void)
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

    // 時間を更新する
    TimeManager::GetInstance().Update();

    // デルタタイムを計算する
    auto nowTime = std::chrono::system_clock::now();
    deltaTime_ = static_cast<float>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - preTime_).count()
        / 1000000000.0
        );
    preTime_ = nowTime;

    // 現在のシーンを取得する
    std::shared_ptr<SceneBase> current = scenes_.back();

    // シーン切り替え中の場合、ロードを更新する
    if (isSceneChanging_)
    {
        Loading::GetInstance()->Update();

        // ロード完了を確認する
        if (!Loading::GetInstance()->IsLoading())
        {
            current->EndLoad();
            current->Init();
            isSceneChanging_ = false;
        }
    }
    else
    {
        if (current) current->Update();
    }

    // カメラを更新する
    if (camera_) camera_->Update();

    // 終了フラグを確認する
    if (isGameEnd_)
    {
        scenes_.clear();
        Release();
    }

    // 衝突を更新する
    CollisionManager::GetInstance().Update();
}

// 描画する
void SceneManager::Draw(void)
{
    if (scenes_.empty()) return;

    // 描画先をバックバッファに設定する
    SetDrawScreen(DX_SCREEN_BACK);

    // バックバッファをクリアする
    ClearDrawScreen();

    // 非同期ロード中は進捗バーを描画する
    if (Loading::GetInstance()->IsLoading())
    {
        Loading::GetInstance()->Draw();
        ScreenFlip();
        return;
    }

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
    CollisionManager::GetInstance().Destroy();
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
