#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/// @brief 敵の基礎パラメータを保持する構造体
struct EnemyParam
{
    // 基本ステータス関連
    float hp = 10.0f;        // 現在HP
    float maxHp = 10.0f;     // 最大HP
    float attack = 1.0f;     // 攻撃力
    float defense = 0.0f;    // 防御力
    float speed = 0.03f;     // 移動速度

    // レベル関連
    int level = 1;           // 現在レベル
    int maxLevel = 10;       // 最大レベル

    // 物理・判定関連
    float jumpPower = 0.0f;  // ジャンプ力 (使う場合)
    float radius = 1.0f;     // 当たり判定半径
};

/// @brief 敵の種類名とパラメータを紐付ける構造体
struct EnemyInfo
{
    std::string type;        // 敵の種類（識別子）
    EnemyParam param;        // パラメータの実体
};

/// @brief エネミーのデータ（マスターデータ）を管理するクラス
/// @details CSVからの読み込みや、特定の敵データの検索を行う
class EnemyData
{
public:

    /// @brief コンストラクタ
    EnemyData(void) = default;

    /// @brief デストラクタ
    ~EnemyData(void) = default;

    /// @brief CSVファイルから敵データを読み込む
    /// @param path CSVファイルのパス
    /// @return 読み込みに成功すればtrue
    bool LoadCSV(const std::string& path);

    /// @brief 種類名を指定して敵データを取得する
    /// @param type 取得したい敵のタイプ名
    /// @return 見つかった場合はデータのポインタ、見つからない場合はnullptr
    const EnemyInfo* GetData(const std::string& type) const;

    /// @brief 保持している全ての敵データを取得する
    /// @return EnemyInfoのリスト（const参照）
    const std::vector<EnemyInfo>& GetAll() const;

private:

    /// @brief 文字列の前後の空白（トリミング）を削除する
    /// @param str 対象の文字列
    /// @return トリミング後の文字列
    static std::string Trim(const std::string& str);

    // データ管理関連
    std::vector<EnemyInfo> enemies_;                        // 全ての敵データを格納するリスト
    std::unordered_map<std::string, size_t> lookup_;       // type名からベクトルのインデックスを高速検索するためのマップ
};