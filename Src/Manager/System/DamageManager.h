#pragma once
#include <DxLib.h>

class Player;
class EnemyBase;

class DamageManager
{
public:
    // ダメージ属性
    enum class DamageElement
    {
        PHYSICAL,   // 物理（無属性）
        FIRE,       // 火
        ICE,        // 氷
        LIGHTNING,  // 雷（追加可能）
        POISON,     // 毒（追加可能）
    };

    // ダメージ計算結果
    struct DamageResult
    {
        int rawDamage;              // 生のダメージ値
        int finalDamage;            // 防御力・属性相性適用後の最終ダメージ
        bool isCritical;            // クリティカルヒットかどうか
        bool isBlocked;             // ブロック（ダメージ無効化）されたか
        float damageReduction;      // ダメージ軽減率（0.0～1.0）
        float elementMultiplier;    // 属性相性倍率
        DamageElement element;      // ダメージ属性
    };

    // ダメージタイプ（確定ダメージ用）
    enum class DamageType
    {
        NORMAL,     // 通常ダメージ（防御力・属性相性適用）
        TRUE_DAMAGE // 確定ダメージ（防御・属性無視）
    };

    // シングルトンの生成
    static void CreateInstance(void);

    // インスタンスを取得する
    static DamageManager& GetInstance(void);

    // インスタンスを破棄する
    static void DestroyInstance(void);

    // 初期化
    void Initialize(void);

    // プレイヤーを登録
    void SetPlayer(Player* player);

    // ★プレイヤーからエネミーへの攻撃（属性対応）
    DamageResult PlayerAttackEnemy(
        EnemyBase* enemy,
        float damageMultiplier = 1.0f,          // ダメージ倍率
        DamageElement element = DamageElement::PHYSICAL, // ★属性
        DamageType type = DamageType::NORMAL,
        float bonusCriticalChance = 0.0f        // ボーナスクリティカル率
    );

    // ★エネミーからプレイヤーへの攻撃（属性対応）
    DamageResult EnemyAttackPlayer(
        EnemyBase* enemy,
        float damageMultiplier = 1.0f,          // ダメージ倍率
        DamageElement element = DamageElement::PHYSICAL, // ★属性
        DamageType type = DamageType::NORMAL,
        float bonusCriticalChance = 0.0f        // ボーナスクリティカル率
    );

    // ★属性相性を計算
    float GetElementMultiplier(DamageElement attackElement, DamageElement defenseElement) const;

    // ダメージをプレイヤーに適用
    void ApplyDamageToPlayer(const DamageResult& result);

    // ダメージをエネミーに適用
    void ApplyDamageToEnemy(EnemyBase* enemy, const DamageResult& result);

    // クリティカル倍率を設定
    void SetCriticalMultiplier(float multiplier);

    // プレイヤーの基本クリティカル率を設定
    void SetPlayerBaseCriticalRate(float rate);

    // 防御力によるダメージ軽減率を計算
    float CalculateDamageReduction(float defense) const;

    // 属性名を取得（デバッグ用）
    const char* GetElementName(DamageElement element) const;

private:
    // コンストラクタ（private）
    DamageManager(void);

    // デストラクタ
    ~DamageManager(void);

    // コピーコンストラクタ禁止
    DamageManager(const DamageManager&) = delete;

    // コピー代入演算子禁止
    DamageManager& operator=(const DamageManager&) = delete;

    // ムーブコンストラクタ禁止
    DamageManager(DamageManager&&) = delete;

    // ムーブ代入演算子禁止
    DamageManager& operator=(DamageManager&&) = delete;

    // アドレス取得演算子(参照演算子)禁止
    DamageManager* operator&() = delete;
    const DamageManager* operator&() const = delete;

    // ★内部ダメージ計算
    DamageResult CalculateDamage(
        float attackPower,
        float defense,
        DamageElement attackElement,
        DamageElement defenseElement,
        DamageType type,
        float criticalChance
    );

private:
    // シングルトンインスタンス
    static DamageManager* instance_;

    // プレイヤーへの参照
    Player* player_;

    // ダメージ計算パラメータ
    float criticalMultiplier_;          // クリティカル倍率（デフォルト: 1.5倍）
    float defenseEfficiency_;           // 防御力の効率（デフォルト: 1.0）
    int minDamage_;                     // 最小ダメージ
    float playerBaseCriticalRate_;      // プレイヤーの基本クリティカル率

    // 内部処理用
    bool RollCritical(float criticalChance) const;  // クリティカル判定
};