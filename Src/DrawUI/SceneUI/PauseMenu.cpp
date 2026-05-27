#include "PauseMenu.h"
#include "../../Manager/Generic/InputManager.h"
#include "../../Manager/Decoration/SoundManager.h"
#include "../../Manager/Generic/SceneManager.h"
#include "../../Scene/SceneOption.h"
#include "../../Scene/SceneTutorial.h"
#include "../../DrawUI/Font.h"
#include "../../Application.h"

PauseMenu::PauseMenu(void)
	: currentIndex_(0)
	, visible_(false)
	, decisionMade_(false)
	, stickFree_(true)
	, howToPlayPage_(0)
	, mode_(MODE_POUSE::SELECT)
{
	// メインメニューの項目名設定
	menuItems_ = { "続ける", "遊び方", "設定", "ゲーム終了" };
}

void PauseMenu::Show(void)
{
	// メニューを表示状態にし、各フラグをリセット
	visible_ = true;
	currentIndex_ = 0;
	decisionMade_ = false;
	mode_ = MODE_POUSE::SELECT;

	// 操作のためにマウスを表示
	SetMouseDispFlag(TRUE);
}

void PauseMenu::Hide(void)
{
	visible_ = false;
	decisionMade_ = false;
	mode_ = MODE_POUSE::SELECT;

	// マウスを非表示にし、ゲーム中に邪魔にならないよう画面中央へ戻す
	SetMouseDispFlag(FALSE);
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
}

bool PauseMenu::IsVisible(void) const { return visible_; }

bool PauseMenu::IsDecisionMade(void) const { return decisionMade_; }

int PauseMenu::GetSelectedIndex(void) const { return currentIndex_; }

void PauseMenu::Initialize(void) {}

void PauseMenu::Update(void)
{
	// 非表示中は更新しない
	if (!visible_)
	{
		return;
	}

	SoundManager& sound = SoundManager::GetInstance();
	auto& input = InputManager::GetInstance();

	const float stickPush = 0.5f;
	const float stickRelease = 0.2f;

	// 説明ページ表示中の処理（Escキーか左クリックでメニューに戻る）
	if (mode_ == MODE_POUSE::HOW_TO_PLAY_PAGE || mode_ == MODE_POUSE::CONTROL)
	{
		bool isCancel = input.IsTrgDown(KEY_INPUT_ESCAPE) ||
			input.IsTrgMouseLeft() ||
			input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT);

		if (isCancel)
		{
			sound.Play(SoundManager::SOUND::SE_CANCEL);
			mode_ = MODE_POUSE::SELECT;
		}
		return;
	}

	// 描画位置から逆算して当たり判定用の矩形を計算
	const int screenW = Application::SCREEN_SIZE_X;
	const int screenH = Application::SCREEN_SIZE_Y;
	const int menuCount = static_cast<int>(menuItems_.size());
	const int boxHeight = menuCount *  KMENU_ITEM_HEIGHT + ( KMENU_PADDING_Y * 2);

	const int menuLeft = (screenW - KMENU_BOX_WIDTH) / 2;
	const int menuTop = (screenH - boxHeight) / 2;
	const int itemStartY = menuTop +  KMENU_PADDING_Y;

	InputManager::JOYPAD_IN_STATE padState;
	padState = input.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
	VECTOR padDir = input.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	if (stickFree_)
	{
		// キーボードによる項目選択（上下ループ対応）
		if (input.IsTrgDown(KEY_INPUT_UP) || padDir.z > stickPush)
		{
			sound.Play(SoundManager::SOUND::SE_SELECT);
			currentIndex_ = (currentIndex_ + menuCount - 1) % menuCount;
			stickFree_ = false;
		}
		if (input.IsTrgDown(KEY_INPUT_DOWN) || padDir.z < -stickPush)
		{
			sound.Play(SoundManager::SOUND::SE_SELECT);
			currentIndex_ = (currentIndex_ + 1) % menuCount;
			stickFree_ = false;
		}
	}

	if (fabs(padDir.z) < stickRelease)
	{
		stickFree_ = true;
	}

	// マウスカーソルによる項目選択判定
	Vector2 mousePos = input.GetMousePos();
	for (int i = 0; i < menuCount; ++i)
	{
		int itemTop = itemStartY + (i *  KMENU_ITEM_HEIGHT);

		// 項目矩形内に入っているかチェック
		bool isHover = (mousePos.x >= menuLeft && mousePos.x <= menuLeft + KMENU_BOX_WIDTH &&
			mousePos.y >= itemTop && mousePos.y <= itemTop +  KMENU_ITEM_HEIGHT);

		if (isHover && currentIndex_ != i)
		{
			sound.Play(SoundManager::SOUND::SE_SELECT);
			currentIndex_ = i;
		}
	}

	bool isDecision = input.IsTrgDown(KEY_INPUT_SPACE) ||
		input.IsTrgMouseLeft() ||
		input.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (isDecision)
	{
		sound.Play(SoundManager::SOUND::SE_PUSH);

		switch (currentIndex_)
		{
		case 0: // 「続ける」
			Hide();
			break;
		case 1: // 「遊び方」
			mode_ = MODE_POUSE::HOW_TO_PLAY_PAGE;
			howToPlayPage_ = 1;
			SceneManager::GetInstance().PushScene(std::make_shared<SceneTutorial>());
			Hide();
			break;
		case 2: // 「設定」
			mode_ = MODE_POUSE::CONTROL;
			SceneManager::GetInstance().PushScene(std::make_shared<SceneOption>());
			Hide();
			break;
		case 3: // 「ゲーム終了」
			SceneManager::GetInstance().GameEnd();
			Hide();
			break;
		}
	}
}

void PauseMenu::Draw(void)
{
	if (!visible_)
	{
		return;
	}

	// 画面中央配置のための計算
	const int screenW = Application::SCREEN_SIZE_X;
	const int screenH = Application::SCREEN_SIZE_Y;
	const int menuCount = static_cast<int>(menuItems_.size());
	const int boxHeight = menuCount *  KMENU_ITEM_HEIGHT + ( KMENU_PADDING_Y * 2);

	const int x = (screenW - KMENU_BOX_WIDTH) / 2;
	const int y = (screenH - boxHeight) / 2;

	const int sideMargin = 10;
	const int topOffset = -5;
	const int height = 40;
	const int kFontSize = 24;

	// 背景の黒ボックスと白の枠線を描画
	DrawBox(x, y, x + KMENU_BOX_WIDTH, y + boxHeight, GetColor(0, 0, 0), TRUE);
	DrawBox(x, y, x + KMENU_BOX_WIDTH, y + boxHeight, GetColor(255, 255, 255), FALSE);

	// 全メニュー項目の描画
	for (int i = 0; i < menuCount; ++i)
	{
		int itemCenterY = y +  KMENU_PADDING_Y + (i *  KMENU_ITEM_HEIGHT);

		// 現在選択されている項目の横にハイライト（黄色枠）を表示
		if (i == currentIndex_)
		{
			DrawBox(x + sideMargin,
				itemCenterY +topOffset,
				x + KMENU_BOX_WIDTH - sideMargin,
				itemCenterY + topOffset + height,
				0xFFFF00, FALSE);
		}

		// テキストが枠の中央に来るようにX座標を計算して描画
		int textW = Font::GetInstance().GetDefaultTextWidth(menuItems_[i]);
		int textX = x + (KMENU_BOX_WIDTH / 2) - (textW / 2);

		Font::GetInstance().DrawDefaultText(textX, itemCenterY, menuItems_[i].c_str(),
			0xffffff, kFontSize, Font::FONT_TYPE_ANTIALIASING_EDGE);
	}
}

void PauseMenu::Release(void) {}