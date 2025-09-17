#include "ImageFader.h"

#include<DxLib.h>

#include"../Application.h"


// 画像をフェード状態に応じて描画する関数
void ImageFader::Draw(int img, Vector2 pos, float scl, float angle, bool trans, bool reverse)
{
	// フェードの状態に応じて処理を分岐
	switch (state_)
	{
	case Fader::STATE::NONE:
		// フェード処理がない場合は何も描画しない
		return;
	case Fader::STATE::FADE_OUT:
	case Fader::STATE::FADE_IN:

		// アルファブレンドを設定（alpha_の値で透明度を調整）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)alpha_);
		
		// 指定された位置・スケール・角度で画像を回転描画
		DrawRotaGraph(
			pos.x, pos.y,		 // 描画位置（中心座標）
			scl,				 // 拡大率
			angle,				 // 回転角度（ラジアン）
			img,				 // 画像ハンドル
			trans,				 // 透過色を有効にするか
			reverse				 // 左右反転するか
		);

		// ブレンドモードを元に戻す（通常描画）
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	}
}
