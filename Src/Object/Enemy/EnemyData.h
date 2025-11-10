#pragma once

// 敵の構造体
struct EnemyParam
{
    float hp = 10.0f;        // 現在HP

    float maxHp = 10.0f;     // 最大HP

    float attack = 1.0f;     // 攻撃力

    float defense = 0.0f;    // 防御力

    float speed = 0.03f;     // 移動速度

    int level = 1;           // 現在レベル

    int maxLevel = 10;       // 最大レベル

    float jumpPower = 0.0f;  // ジャンプ力 (使う場合)

    float radius = 1.0f;     // 当たり判定半径
};

struct EnemyInfo
{
    std::string type;

    EnemyParam param;
};

// エネミーのデータ管理クラス
class EnemyData
{
public:
    EnemyData() = default;
    ~EnemyData() = default;

    // CSV読み込み
    bool LoadCSV(const std::string& path);

    // typeでデータを取得
    const EnemyInfo* GetData(const std::string& type) const;

    // 全ての敵タイプを取得
    const std::vector<EnemyInfo>& GetAll() const;

private:
    // 前後の空白を削除
    static std::string Trim(const std::string& str);

    // 全ての敵
    std::vector<EnemyInfo> enemies_;

    // type からインデックスへの検索
    std::unordered_map<std::string, size_t> lookup_;
};