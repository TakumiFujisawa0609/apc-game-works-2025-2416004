#pragma once
#include"Fader.h"
#include"Vector2.h"

// 画像のフェードイン／フェードアウト描画を担当するクラス
// Fader クラスを基底クラスとして継承
class ImageFader : public Fader
{
public:

	/// <summary>
	/// 描画メソッド
	/// 指定された画像を画面上に描画します。位置、スケール、回転角度、透過、反転の各パラメータを指定できます。
	/// </summary>
	/// <param name="imag">描画する画像のリソースID</param>
	/// <param name="pos">画像の描画位置（2Dベクトル）</param>
	/// <param name="scl">画像のスケール（1.0 は元のサイズ）</param>
	/// <param name="angle">画像の回転角度（度単位）</param>
	/// <param name="trans">透過処理を行うかどうか</param>
	/// <param name="reverse">画像を反転させるかどうか</param>
	void Draw(int img, Vector2 pos, float scl, float angle, bool trans, bool reverse);
};

