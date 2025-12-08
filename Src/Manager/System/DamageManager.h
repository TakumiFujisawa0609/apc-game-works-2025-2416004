#pragma once

class Player;

class DamageManager
{
public:
	
	// シングルトンの生成
	static void CreateInstance(void);

	// インスタンスを取得する
	static DamageManager& GetInstance(void);

	// インスタンスを破棄する
	static void DestroyInstance(void);

    // プレイヤーを登録
    void SetPlayer(Player* player);

private:

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
};

