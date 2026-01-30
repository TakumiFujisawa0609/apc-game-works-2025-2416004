#include "DamageManager.h"
#include "../../Object/Player/Player.h"
#include "../../Object/Enemy/EnemyBase.h"
#include "../../Utility/Utility.h"
#include <algorithm>
#include <cmath>

// 静的メンバの初期化
DamageManager* DamageManager::instance_ = nullptr;

// シングルトンの生成
void DamageManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new DamageManager();
        instance_->Init();
    }
}

// インスタンスを取得
DamageManager& DamageManager::GetInstance(void)
{
    if (instance_ == nullptr)
    {
        CreateInstance();
    }
    return *instance_;
}

// インスタンスを破棄
void DamageManager::DestroyInstance(void)
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

// コンストラクタ
DamageManager::DamageManager(void)
    : player_(nullptr)
    , criticalMultiplier_(1.5f)
    , defenseEfficiency_(1.0f)
    , minDamage_(1)
    , playerBaseCriticalRate_(0.05f)
{
}

// デストラクタ
DamageManager::~DamageManager(void)
{
}

// 初期化
void DamageManager::Init(void)
{
    player_ = nullptr;
    criticalMultiplier_ = 1.5f;
    defenseEfficiency_ = 1.0f;
    minDamage_ = 1;
    playerBaseCriticalRate_ = 0.05f;
}

// プレイヤーを登録
void DamageManager::SetPlayer(Player* player)
{
    player_ = player;
}

// ★プレイヤーからエネミーへの攻撃（属性対応）
DamageManager::DamageResult DamageManager::PlayerAttackEnemy(
    EnemyBase* enemy,
    float damageMultiplier,
    DamageElement element,
    DamageType type,
    float bonusCriticalChance)
{
    if (!player_ || !enemy)
    {
        DamageResult emptyResult = {};
        return emptyResult;
    }

    // プレイヤーのパラメータを取得
    const auto& playerParam = player_->GetParam();
    float playerAttack = static_cast<float>(playerParam.attack);

    // エネミーのパラメータを取得
    float enemyDefense = enemy->GetDefense();

    // 最終攻撃力 = プレイヤーの攻撃力 × 倍率
    float finalAttack = playerAttack * damageMultiplier;

    // クリティカル率 = 基本クリティカル率 + ボーナス
    float totalCriticalRate = playerBaseCriticalRate_ + bonusCriticalChance;

    // ★ダメージ計算（属性相性を考慮）
    // エネミーの属性（仮に物理属性とする、実装次第で変更可能）
    DamageElement enemyElement = DamageElement::PHYSICAL;

    DamageResult result = CalculateDamage(
        finalAttack,
        enemyDefense,
        element,        // 攻撃属性
        enemyElement,   // 防御側の属性
        type,
        totalCriticalRate
    );

    // ダメージを適用
    ApplyDamageToEnemy(enemy, result);

    return result;
}

// エネミーからプレイヤーへの攻撃（属性対応）
DamageManager::DamageResult DamageManager::EnemyAttackPlayer(
    EnemyBase* enemy,
    float damageMultiplier,
    DamageElement element,
    DamageType type,
    float bonusCriticalChance)
{
    if (!player_ || !enemy)
    {
        DamageResult emptyResult = {};
        return emptyResult;
    }

    // エネミーのパラメータを取得
    float enemyAttack = enemy->GetAttack();

    // プレイヤーのパラメータを取得
    const auto& playerParam = player_->GetParam();
    float playerDefense = static_cast<float>(playerParam.defensse);

    // 最終攻撃力 = エネミーの攻撃力 × 倍率
    float finalAttack = enemyAttack * damageMultiplier;

    // クリティカル率
    float totalCriticalRate = bonusCriticalChance;

    // ★ダメージ計算（属性相性を考慮）
    // プレイヤーの属性（仮に物理属性）
    DamageElement playerElement = DamageElement::PHYSICAL;

    DamageResult result = CalculateDamage(
        finalAttack,
        playerDefense,
        element,        // 攻撃属性
        playerElement,  // 防御側の属性
        type,
        totalCriticalRate
    );

    // ダメージを適用
    ApplyDamageToPlayer(result);


    return result;
}

// 内部ダメージ計算
DamageManager::DamageResult DamageManager::CalculateDamage(
    float attackPower,
    float defense,
    DamageElement attackElement,
    DamageElement defenseElement,
    DamageType type,
    float criticalChance)
{
    DamageResult result;
    result.rawDamage = static_cast<int>(attackPower);
    result.isCritical = false;
    result.isBlocked = false;
    result.damageReduction = 0.0f;
    result.elementMultiplier = 1.0f;
    result.element = attackElement;

    // 確定ダメージの場合は防御・属性無視
    if (type == DamageType::TRUE_DAMAGE)
    {
        result.finalDamage = result.rawDamage;
        return result;
    }

    // クリティカル判定
    if (RollCritical(criticalChance))
    {
        result.isCritical = true;
        attackPower *= criticalMultiplier_;
        result.rawDamage = static_cast<int>(attackPower);
    }

    // ★属性相性を計算
    result.elementMultiplier = GetElementMultiplier(attackElement, defenseElement);
    attackPower *= result.elementMultiplier;

    // 防御力によるダメージ軽減率を計算
    result.damageReduction = CalculateDamageReduction(defense);

    // 最終ダメージ = 攻撃力 × 属性相性 × (1 - 軽減率)
    float reducedDamage = attackPower * (1.0f - result.damageReduction);
    result.finalDamage = static_cast<int>(reducedDamage);

    // 最小ダメージ保証
    if (result.finalDamage < minDamage_)
    {
        result.finalDamage = minDamage_;
    }

    return result;
}

// ★属性相性を計算
float DamageManager::GetElementMultiplier(DamageElement attackElement, DamageElement defenseElement) const
{
    // 物理属性は相性なし
    if (attackElement == DamageElement::PHYSICAL)
    {
        return 1.0f;
    }

    // 火 vs 氷 = 有利（1.5倍）
    if (attackElement == DamageElement::FIRE && defenseElement == DamageElement::ICE)
    {
        return 1.5f;
    }

    // 氷 vs 火 = 不利（0.5倍）
    if (attackElement == DamageElement::ICE && defenseElement == DamageElement::FIRE)
    {
        return 0.5f;
    }

    // 氷 vs 氷 = 抵抗（0.75倍）
    if (attackElement == DamageElement::ICE && defenseElement == DamageElement::ICE)
    {
        return 0.75f;
    }

    // 火 vs 火 = 抵抗（0.75倍）
    if (attackElement == DamageElement::FIRE && defenseElement == DamageElement::FIRE)
    {
        return 0.75f;
    }

    // 火 vs 物理、氷 vs 物理 = 通常（1.0倍）
    if (defenseElement == DamageElement::PHYSICAL)
    {
        return 1.0f;
    }

    // デフォルト
    return 1.0f;
}

// ダメージをプレイヤーに適用
void DamageManager::ApplyDamageToPlayer(const DamageResult& result)
{
    if (!player_) return;
    player_->TakeDamage(result.finalDamage);
}

// ダメージをエネミーに適用
void DamageManager::ApplyDamageToEnemy(EnemyBase* enemy, const DamageResult& result)
{
    if (!enemy) return;
    enemy->TakeDamage(static_cast<float>(result.finalDamage));
}

// クリティカル倍率を設定
void DamageManager::SetCriticalMultiplier(float multiplier)
{
    criticalMultiplier_ = multiplier;
}

// プレイヤーの基本クリティカル率を設定
void DamageManager::SetPlayerBaseCriticalRate(float rate)
{
    playerBaseCriticalRate_ = rate;
}

// 防御力によるダメージ軽減率を計算
float DamageManager::CalculateDamageReduction(float defense) const
{
    float reduction = (defense / (defense + 100.0f)) * defenseEfficiency_;
    if (reduction > 0.9f)
    {
        reduction = 0.9f;
    }
    return reduction;
}

// クリティカル判定
bool DamageManager::RollCritical(float criticalChance) const
{
    if (criticalChance <= 0.0f) return false;
    float roll = Utility::RandRangeF(0.0f, 1.0f);
    return roll < criticalChance;
}

// 属性名を取得（デバッグ用）
const char* DamageManager::GetElementName(DamageElement element) const
{
    switch (element)
    {
    case DamageElement::PHYSICAL:  return "[物理]";
    case DamageElement::FIRE:      return "[火]";
    case DamageElement::ICE:       return "[氷]";
    case DamageElement::LIGHTNING: return "[雷]";
    case DamageElement::POISON:    return "[毒]";
    default:                       return "[不明]";
    }
}