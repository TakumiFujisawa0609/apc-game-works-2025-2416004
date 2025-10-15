#include "EnemyData.h"
#include <fstream>
#include <sstream>
#include <iostream>

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

    // ヘッダ行を読み飛ばす
    if (!std::getline(file, line)) return false;

    // ヘッダ行のBOMを削除
    if (line.size() >= 3 &&
        (unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF)
    {
        line = line.substr(3);
    }

    while (std::getline(file, line))
    {
        // データ行のBOMを削除（必要なら）
        if (line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF &&
            (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF)
        {
            line = line.substr(3);
        }

        std::stringstream ss(line);
        std::string token;
        EnemyInfo info;

        // type
        if (!std::getline(ss, token, ',')) continue;
        info.type = Trim(token);

        // hp
        if (!std::getline(ss, token, ',')) continue;
        info.hp = std::stof(Trim(token));

        // speed
        if (!std::getline(ss, token, ',')) continue;
        info.speed = std::stof(Trim(token));

        // radius
        if (!std::getline(ss, token, ',')) continue;
        info.radius = std::stof(Trim(token));

        // 登録（インデックスを保存）
        size_t index = enemies_.size();
        enemies_.push_back(info);
        lookup_[info.type] = index;

        // デバッグ
        std::cout << "Loaded Enemy: [" << info.type << "] HP=" << info.hp
            << " Speed=" << info.speed << " Radius=" << info.radius << std::endl;
    }

    return true;
}

// type で取得
const EnemyInfo* EnemyData::GetData(const std::string& type) const
{
    auto it = lookup_.find(type);
    if (it != lookup_.end())
    {
        return &enemies_[it->second];  // インデックスから要素へのポインタを返す
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
    std::string s = (start == std::string::npos) ? "" : str.substr(start, end - start + 1);

    // 先頭BOM削除
    if (!s.empty() && (unsigned char)s[0] == 0xEF) s = s.substr(3);

    return s;
}