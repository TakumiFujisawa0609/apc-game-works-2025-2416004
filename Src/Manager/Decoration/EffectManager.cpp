#include "EffectManager.h"
#include<EffekseerForDXLib.h>
#include<cassert>

// シングルトンインスタンスの静的初期化
EffectManager* EffectManager::instance_ = nullptr;

// インスタンス生成メソッド
// 未生成の場合のみ新しいインスタンスを作成する
void EffectManager::CreateInstance(void)
{
    if (instance_ == nullptr) {
        instance_ = new EffectManager();
    }
}

// インスタンス取得メソッド
// シングルトンの唯一のインスタンスへの参照を返す
EffectManager& EffectManager::GetInstance(void)
{
    return *instance_;
}

// コンストラクタ
// 初期化処理を実行
EffectManager::EffectManager(void) {
    int i[NONE_MAX] = {};
    // 配列テスト用（現在コメントアウト）
    //effectTest_.emplace(EFFECT::NONE,i);
}

// エフェクトの追加
void EffectManager::Add(const EFFECT& efc, int data)
{
    // 連想配列内にすでに要素が入っているかを検索
    // 入っていたら処理終了 (重複登録を防止)
    if (effectRes_.find(efc) != effectRes_.end()) return;

    // 新規データのため情報を追加
    // 注意: 変数名が不一致 (_efc → *efc, _data → *data)
    effectRes_.emplace(efc, data);
}

// エフェクトの再生
void EffectManager::Play(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, const float& size, const SoundManager::SOUND _sound)
{
    // 元データがないときは警告 (未登録のエフェクトを再生しようとした場合)
    // 注意: 変数名が不一致 (_efc → *efc)
    if (effectRes_.find(efc) == effectRes_.end()) assert("設定していないエフェクトを再生しようとしています。");

    // 再生配列内に要素が入っていないかを検索
    if (effectPlay_.find(efc) == effectPlay_.end()) {
        // 入っていないとき要素を追加する
        effectPlay_.emplace(efc, PlayEffekseer3DEffect(effectRes_[efc]));
    }
    else {
        // 入っていたら元あるやつに上書きする
        effectPlay_[efc] = PlayEffekseer3DEffect(effectRes_[efc]);
    }

    // 各種設定同期（位置、回転、大きさを設定）
    SyncEffect(efc, pos, qua, size);

    // 効果音の再生
    // NONE以外の効果音が指定されていれば再生
    if (_sound != SoundManager::SOUND::NONE) {
        SoundManager::GetInstance().Play(_sound);
    }
}

// エフェクトの再生停止
// param _efc: エフェクト種類名
void EffectManager::Stop(const EFFECT& _efc)
{
    // 配列内に入っていないものを停止しようとしたら警告
    if (effectPlay_.find(_efc) == effectPlay_.end()) assert("設定していないエフェクトを停止しようとしています。");

    // 再生停止
    StopEffekseer3DEffect(effectPlay_[_efc]);
}

// エフェクトの各パラメータ同期
// 位置、回転、大きさをリアルタイムで更新する
// param _efc: エフェクト名
// param _pos: 位置情報
// param _qua: 回転情報
// param _size: 大きさ
void EffectManager::SyncEffect(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, const float& size)
{
    // 大きさの設定
    // 注意: 変数名が不一致 (_size → *size)
    SetScalePlayingEffekseer3DEffect(effectPlay_[efc], size, size, size);

    // 角度の設定（クォータニオンからオイラー角に変換）
    // 注意: 変数参照の不一致 (*qua.ToEuler() → qua->ToEuler(), _qua → *qua)
    SetRotationPlayingEffekseer3DEffect(effectPlay_[efc], qua.ToEuler().x, qua.ToEuler().y, qua.ToEuler().z);

    // 位置の設定
    // 注意: 変数参照の不一致 (*pos.x → pos->x, _pos → *pos)
    SetPosPlayingEffekseer3DEffect(effectPlay_[efc], pos.x, pos.y, pos.z);
}

// エフェクトの再生確認
// param _efc: エフェクト名
// return: 再生中ならtrue、停止中ならfalse
bool EffectManager::IsPlayEffect(const EFFECT& _efc)
{
    // エフェクトが未設定(-1)か再生中でない(-1)の場合
    if (effectPlay_[_efc] == -1 || IsEffekseer3DEffectPlaying(effectPlay_[_efc]) == -1)
    {
        return true;
    }
    return false;
}

// 解放処理
// エフェクトリソースの解放
void EffectManager::Release(void)
{
    // 配列内の要素を全て消去
    // 元々のデータはリソースマネージャが持っているので問題なし
    effectRes_.clear();
}

// 消去処理
// インスタンスの破棄
void EffectManager::Destroy(void)
{
    // 全てのエフェクトデータを解放してからインスタンスを削除
    Release();
    delete instance_;
}