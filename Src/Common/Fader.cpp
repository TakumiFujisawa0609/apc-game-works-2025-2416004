#include"Fader.h"

#include<DxLib.h>

#include"../Application.h"


//状態の取得
Fader::STATE Fader::GetState(void) const
{
	return state_;
}

//フェード処理が終了しているかどうか
//true : フェード終了, false :フェード未終了
bool Fader::IsEnd(void) const
{
	return isEnd_;
}

//指定フェードを開始する
void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)		//NONE以外の状態が指定された場合
	{
		isPreEnd_ = false;			//終了フラグのリセット
		isEnd_ = false;				//終了フラグのリセット
	}
}

//フェードのアルファ値を設定する
//alpha: 暗さの最大値(0.0～1.0)
void Fader::SetAlpha(float alpha)
{
	alphaMax_ = alpha;
}

//初期化
void Fader::Init(void)
{
	state_ = STATE::NONE;
	alpha_ = 0;
	alphaMax_ = 0;
	isPreEnd_ = true;
	isEnd_ = true;

	//デバック時とリリース時で画面サイズを切り替える
	tmpScene_ = MakeScreen(
		Application::DEFA_SCREEN_SIZE_X,	//リリース時の横幅
		Application::DEFA_SCREEN_SZIE_Y,	//リリース時の縦幅
		true
	);
}

//更新処理
void Fader::Update(void)
{
	//フェード終了時は処理をスキップ
	if (isEnd_ == true)
	{
		return;
	}

	switch (state_)
	{
	//フェードなし
	case STATE::NONE:

	//フェードの維持
	case STATE::FADE_KEEP:
		return;

	//フェードアウト処理
	case STATE::FADE_OUT:
		alpha_ += SPEED_ALPHA;		//アルファ値を増加
		if (alpha_ > ALPHA_MAX)
		{
			//フェードの終了
			alpha_ = ALPHA_MAX;		//最大値でクリップ
			if (isPreEnd_ == true)
			{
				//1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}

			isPreEnd_ = true;
		}

		break;
	
	//フェードイン処理
	case STATE::FADE_IN:
		alpha_ -= SPEED_SCENE;		//アルファ値を減少
		if (alpha_ < 0)
		{
			//フェード終了
			alpha_ = 0;				//最小でクリップ
			if (isPreEnd_ == true)
			{
				//1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;

	//特定の暗さまでフェードアウト
	case STATE::SET_FADE_OUT:
		alpha_ += SPEED_ALPHA;		//アルファ値を増加
		if (alpha_ > alphaMax_)
		{
			//フェーズ終了
			alpha_ = LITTLE_ALPHA;	//設定された最大値でクリップ
			if (isPreEnd_ == true)
			{
				//1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}

		break;

	default:		//そのほかの状態は処理しない
		return;
	}
}


//描画処理
void Fader::Draw(void)
{
	switch (state_)
	{
	case STATE::NONE:	//描画なし
		return;
	case STATE::FADE_KEEP:
	case STATE::SET_FADE_OUT:
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		CricleMask();	//マスク呼び出し
		break;
	}
}

//マスク描画処理
void Fader::CricleMask(void)
{
	//描画領域をマスク画像領域に切り替える
	//元は、背面スクリーンになっている
	SetDrawScreen(tmpScene_);

	//画面全体を黒に塗る
	DrawBox(
		0, 0,
		Application::DEFA_SCREEN_SIZE_X,//リリース時の横幅
		Application::DEFA_SCREEN_SZIE_Y,//リリース時の縦幅
		0x000000, true
	);

	//白色の円を描画する
	//alpha値を利用して大きさを制御
	DrawCircle(
		(Application::DEFA_SCREEN_SIZE_X / 2),//リリース時の中心x
		Application::DEFA_SCREEN_SZIE_Y / 2,//リリース時の中心y
		(ALPHA_MAX - alpha_) * 4,			//円の半径(アルファ値で変化)
		0xffffff, true
	);
	
	//描画領域を元に戻す
	SetDrawScreen(DX_SCREEN_BACK);

	//描画を色の乗算モードにする
	SetDrawBlendMode(DX_BLENDMODE_MUL, 0);

	//元のゲーム画面にマスク画像を描画
	DrawGraph(0, 0, tmpScene_, false);

	//描画モードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

