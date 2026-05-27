#pragma once
#include"../../Common/Quaternion.h"

/// @brief モデル制御の基本情報（大きさ・回転・位置）を管理するクラス
/// @details 大きさ：VECTOR基準 / 回転：Quaternion基準 / 位置：VECTOR基準
class Transform
{
public:

	/// @brief コンストラクタ
	Transform(void);

	/// @brief モデルを指定するコンストラクタ
	/// @param model モデルハンドル
	Transform(int model);

	/// @brief デストラクタ
	~Transform(void);

	// モデル関連
	int modelId;	// モデルのハンドルID

	// スケール関連
	VECTOR scl;		// 大きさ
	MATRIX matScl;	// スケール行列

	// 回転関連
	VECTOR rot;					// 回転（オイラー角）
	Quaternion quaRot;			// 回転（クォータニオン）
	Quaternion quaRotLocal;		// ローカル回転（クォータニオン）
	MATRIX matRot;				// 回転行列

	// 座標関連
	VECTOR pos;			// 位置（ワールド座標）
	VECTOR localPos;	// ローカル位置
	MATRIX matPos;		// 位置行列

	/// @brief モデル制御の基本情報更新
	void Update(void);

	/// @brief 解放処理
	void Release(void);

	/// @brief モデルの設定
	/// @param modelHId セットするモデルハンドル
	void SetModel(int modelHId);

	/// @brief 前方方向を取得
	/// @return 前方ベクトル
	VECTOR GetForward(void) const;

	/// @brief 後方方向を取得
	/// @return 後方ベクトル
	VECTOR GetBack(void) const;

	/// @brief 右方向を取得
	/// @return 右方向ベクトル
	VECTOR GetRight(void) const;

	/// @brief 左方向を取得
	/// @return 左方向ベクトル
	VECTOR GetLeft(void) const;

	/// @brief 上方向を取得
	/// @return 上方向ベクトル
	VECTOR GetUp(void) const;

	/// @brief 下方向を取得
	/// @return 下方向ベクトル
	VECTOR GetDown(void) const;

	/// @brief 指定したベクトルを現在の回転に合わせて変換し、方向を取得
	/// @param vec 元となる方向ベクトル
	/// @return 回転適用後の方向ベクトル
	VECTOR GetDir(const VECTOR& vec) const;

	/// @brief 座標のポインタを取得
	/// @return VECTOR型の座標ポインタ
	VECTOR* GetPosPtr(void);

	/// @brief 読み取り専用の座標ポインタを取得
	/// @return const VECTOR型の座標ポインタ
	const VECTOR* GetPosPtr(void) const;

};