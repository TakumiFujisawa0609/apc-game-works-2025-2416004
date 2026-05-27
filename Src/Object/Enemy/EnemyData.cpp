#include "../../Pch.h"
#include "EnemyData.h"

bool EnemyData::LoadCSV(const std::string& path)
{
    std::ifstream file(path);

    // ファイルオープン失敗チェック
    if (!file.is_open())
    {
        std::cerr << "Failed to open CSV: " << path << std::endl;
        return false;
    }

    std::string line;

    // ヘッダ行（項目名）を読み飛ばす
    std::getline(file, line); 

    // データ行を一行ずつ読み込む
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        EnemyInfo info;

        // カンマ区切りでトークンを取得し、各パラメータに格納
        // type
        std::getline(ss, token, ','); 
        info.type = Trim(token);

        // hp
        std::getline(ss, token, ','); 
        info.param.hp = std::stof(Trim(token));

        // maxHp
        std::getline(ss, token, ','); 
        info.param.maxHp = std::stof(Trim(token));

        // attack
        std::getline(ss, token, ','); 
        info.param.attack = std::stof(Trim(token));

        // defense
        std::getline(ss, token, ','); 
        info.param.defense = std::stof(Trim(token));

        // speed
        std::getline(ss, token, ','); 
        info.param.speed = std::stof(Trim(token));

        // level
        std::getline(ss, token, ','); 
        info.param.level = std::stoi(Trim(token));

        // maxLevel
        std::getline(ss, token, ','); 
        info.param.maxLevel = std::stoi(Trim(token));

        // jumpPower
        std::getline(ss, token, ','); 
        info.param.jumpPower = std::stof(Trim(token));

        // radius
        std::getline(ss, token, ','); 
        info.param.radius = std::stof(Trim(token));

        // 検索用マップ（lookup_）にインデックスを登録し、リストに追加
        lookup_[info.type] = enemies_.size();
        enemies_.push_back(info);

        // デバッグ用にロードした情報をログ出力
        std::cout << "Loaded Enemy: [" << info.type
            << "] HP=" << info.param.hp
            << " ATK=" << info.param.attack
            << " DEF=" << info.param.defense
            << " SPD=" << info.param.speed
            << std::endl;
    }

    return true;
}

const EnemyInfo* EnemyData::GetData(const std::string& type) const
{
    auto it = lookup_.find(type);

    // マップ内に存在するかチェック
    if (it != lookup_.end())
    {
        return &enemies_[it->second];
    }

    // 見つからなければnullptrを返す
    return nullptr;
}

const std::vector<EnemyInfo>& EnemyData::GetAll() const
{
    return enemies_;
}

std::string EnemyData::Trim(const std::string& str)
{
    // 対象とする空白文字の定義
    const char* ws = " \t\r\n";

    // 最初と最後の「空白文字以外」の位置を探す
    size_t start = str.find_first_not_of(ws);
    size_t end = str.find_last_not_of(ws);

    // 全て空白文字だった場合の処理
    if (start == std::string::npos)
    {
        return "";
    }

    // 該当範囲を抜き出す
    return str.substr(start, end - start + 1);
}