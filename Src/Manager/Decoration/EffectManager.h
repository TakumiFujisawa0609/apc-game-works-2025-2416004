#pragma once

#include<DxLib.h>
#include<unordered_map>
#include<string>

#include"SoundManager.h"
#include"../../Common/Quaternion.h"

/// <summary>
/// エフェクト管理クラス
/// ゲーム内で使用される視覚エフェクトを管理する
/// </summary>
class EffectManager
{
public:
    /// <summary>
    /// 各種エフェクトの上限数
    /// </summary>
    static constexpr int NONE_MAX = 5;

    /// <summary>
    /// エフェクトの種類を定義する列挙型
    /// </summary>
    enum class EFFECT
    {
		EFFECT_ALCHENY,   /// <summary>錬金エフェクト</summary>
        EFFECT_BLAST,     /// <summary>爆発エフェクト</summary>
    };

    /// <summary>
    /// インスタンスを生成する
    /// シングルトンパターンの実装
    /// </summary>
    static void CreateInstance(void);

    /// <summary>
    /// インスタンスを取得する
    /// </summary>
    /// <returns>EffectManagerの唯一のインスタンス</returns>
    static EffectManager& GetInstance(void);

    /// <summary>
    /// エフェクトの追加
    /// </summary>
    /// <param name="_efc">エフェクト種類名</param>
    /// <param name="_data">エフェクトのデータ</param>
    void Add(const EFFECT& efc, int data);

    /// <summary>
    /// エフェクトの再生
    /// </summary>
    /// <param name="_efc">エフェクト種類名</param>
    /// <param name="_pos">再生位置</param>
    /// <param name="_qua">角度</param>
    /// <param name="_size">大きさ</param>
    /// <param name="_sound">効果音</param>
    void Play(const EFFECT& _efc,
        const VECTOR& pos, const Quaternion& qua, const float& _size,
        const SoundManager::SOUND _sound);

    /// <summary>
    /// エフェクトの再生停止
    /// </summary>
    /// <param name="_efc">エフェクト種類名</param>
    void Stop(const EFFECT& _efc);

    /// <summary>
    /// エフェクトの各パラメータ同期
    /// 位置、回転、大きさをリアルタイムで更新する
    /// </summary>
    /// <param name="_efc">エフェクト名</param>
    /// <param name="_pos">位置情報</param>
    /// <param name="_qua">回転情報</param>
    /// <param name="_size">大きさ</param>
    void SyncEffect(const EFFECT& efc, const VECTOR& pos, const Quaternion& qua, const float& size);

    /// <summary>
    /// エフェクトの再生確認
    /// </summary>
    /// <param name="_efc">エフェクト名</param>
    /// <returns>再生中ならtrue、停止中ならfalse</returns>
    bool IsPlayEffect(const EFFECT& _efc);

    /// <summary>
    /// 解放処理
    /// エフェクトリソースの解放
    /// </summary>
    void Release(void);

    /// <summary>
    /// 消去処理
    /// インスタンスの破棄
    /// </summary>
    void Destroy(void);

private:
    /// <summary>
    /// インスタンス用
    /// シングルトンパターンのためのインスタンス保持用変数
    /// </summary>
    static EffectManager* instance_;

    /// <summary>
    /// エフェクトデータ格納用マップ
    /// </summary>
    std::unordered_map<EFFECT, int> effectRes_;   /// <summary>初期データ</summary>
    std::unordered_map<EFFECT, int> effectPlay_;  /// <summary>再生データ</summary>
    //std::unordered_map<EFFECT,int[]> effectTest_;  //再生データ
    std::unordered_map<EFFECT, int> effectMax_;   /// <summary>再生データの最大所持数</summary>

    /// <summary>
    /// コンストラクタ
    /// シングルトンパターンのため外部からのインスタンス化を防止
    /// </summary>
    EffectManager(void);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~EffectManager() = default;
};