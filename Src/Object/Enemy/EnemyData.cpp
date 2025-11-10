#include "EnemyData.h"



// CSV読み込み
bool EnemyData::LoadCSV(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open CSV: " << path << std::endl;
        return false;
    }

    std::string line;

    std::getline(file, line); // ヘッダ行スキップ

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        EnemyInfo info;

        std::getline(ss, token, ','); info.type = Trim(token);

        std::getline(ss, token, ','); info.param.hp = std::stof(Trim(token));

        std::getline(ss, token, ','); info.param.maxHp = std::stof(Trim(token));

        std::getline(ss, token, ','); info.param.attack = std::stof(Trim(token));

        std::getline(ss, token, ','); info.param.defense = std::stof(Trim(token));

        std::getline(ss, token, ','); info.param.speed = std::stof(Trim(token));

        std::getline(ss, token, ','); info.param.level = std::stoi(Trim(token));

        std::getline(ss, token, ','); info.param.maxLevel = std::stoi(Trim(token));

        std::getline(ss, token, ','); info.param.jumpPower = std::stof(Trim(token));

        std::getline(ss, token, ','); info.param.radius = std::stof(Trim(token));

        lookup_[info.type] = enemies_.size();
        enemies_.push_back(info);

        std::cout << "Loaded Enemy: [" << info.type
            << "] HP=" << info.param.hp
            << " ATK=" << info.param.attack
            << " DEF=" << info.param.defense
            << " SPD=" << info.param.speed
            << std::endl;
    }

    return true;
}

// type で取得
const EnemyInfo* EnemyData::GetData(const std::string& type) const
{
    auto it = lookup_.find(type);

    if (it != lookup_.end())
    {
        return &enemies_[it->second];
    }

    return nullptr;
}

// 全て取得
const std::vector<EnemyInfo>& EnemyData::GetAll() const
{
    return enemies_;
}

// 前後の空白削除 + BOM対応
std::string EnemyData::Trim(const std::string& str)
{
    const char* ws = " \t\r\n";

    size_t start = str.find_first_not_of(ws);

    size_t end = str.find_last_not_of(ws);

    if (start == std::string::npos)

        return "";

    return str.substr(start, end - start + 1);
}