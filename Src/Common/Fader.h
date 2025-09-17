#pragma once

/// <summary>
/// フェード処理を管理するクラス
/// </summary>
class Fader
{
public:
	/// <summary>
	/// 透過最大値
	/// </summary>
	static constexpr int ALPHA_MAX = 255;

	/// <summary>
	/// 少し暗くする時のアルファ値
	/// </summary>
	static constexpr int LITTLE_ALPHA = 150;

	/// <summary>
	/// 少し暗くする時のフェードアウト速度
	/// </summary>
	static constexpr float LITTLE_FADE_OUT_SPEED = 2.0f;


	/// <summary>
	/// シーン遷移時のフェード速度
	/// </summary>
	static constexpr float SPEED_SCENE = 5.0f;

	/// <summary>
	/// フェーズ遷移時のフェード速度
	/// </summary>
	static constexpr float SPEED_PHASE = 2.0f;

	/// <summary>
	/// 汎用フェード操作時の速度
	/// </summary>
	static constexpr float SPEED_ALPHA = 3.0f;

	/// <summary>
	/// フェード状態
	/// </summary>
	enum class STATE
	{
		NONE,				//何もしていない
		FADE_OUT,			//徐々に暗転
		FADE_IN,			//徐々に明転
		FADE_KEEP,			//現在の状態を保つ
		SET_FADE_OUT		//アルファ値を決めて暗転
	};

	/// <summary>
	/// 現在のフェード状態を取得する
	/// </summary>
	STATE GetState(void) const;

	/// <summary>
	/// フェード処理が完了しているかどうかを判定する
	/// </summary>
	bool IsEnd(void)const;

	/// <summary>
	/// 指定されたフェード処理を開始する
	/// </summary>
	void SetFade(STATE state);

	/// <summary>
	/// 任意のアルファ値を設定する
	/// </summary>
	void SetAlpha(float alpha);

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init(void);

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(void);

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw(void);

protected:

	/// <summary>
	/// 現在のフェード状態
	/// </summary>
	STATE state_;

	///<summary>
	///暗転・明転用透過度
	/// </summary>
	float alpha_;

	/// <summary>
	///フェード速度
	/// </summary>
	float spped_;

	///<summary>
	///透明度の指定値用
	///</summary>
	float alphaMax_;

	///<summary>
	///状態(STATE)を保ったまま終了判定を行うため、
	///Update->Draw->Updateの1フレーム判定用
	///</summary>
	bool isPreEnd_;

	///<summary>
	///フェード処理の終了判定
	///</summary>
	bool isEnd_;

private:

	///<summary>
	///マスク画像領域
	///</summary>
	int tmpScene_;

	///<summary>
	///マスク描画処理
	///</summary>
	void CricleMask(void);
};