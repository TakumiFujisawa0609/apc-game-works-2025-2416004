#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// 敵の構造体
struct EnemyInfo
{
    std::string type;
    float hp;
    float speed;
    float radius;
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