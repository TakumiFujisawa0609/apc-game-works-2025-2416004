#include "../../Pch.h"
#include "SceneUi.h"
#include "../../DrawUI/Font.h"
#include "../../Application.h"

SceneUi::SceneUi(void)
	: frameCount_(0)
	, isVisible_(true)
	, currentIndex_(0)
{
}

SceneUi::~SceneUi(void)
{
}

void SceneUi::Update(void)
{
	UpdateBlinking();
}

void SceneUi::Draw(int baseYOverride)
{
	// 引数がデフォルト値(-1)の場合はY座標500を基準にする
	int startY = (baseYOverride == -1) ? 500 : baseYOverride;
	DrawFontList(startY);
}

void SceneUi::UpdateBlinking(void)
{
	frameCount_++;

	isVisible_ = (frameCount_ / kBlinkInterval) % 2 == 0;
}

void SceneUi::DrawFontList(int baseY)
{
	for (int i = 0; i < static_cast<int>(fontList_.size()); ++i)
	{
		const auto& font = fontList_[i];
		bool isSelected = (i == currentIndex_);

		// 文字列の幅を取得し、画面中央に配置されるようX座標を計算
		int textWidth = Font::GetInstance().GetDefaultTextWidth(font.message.c_str());
		int xPos = (Application::SCREEN_SIZE_X / 2) - textWidth;

		// ループのインデックスに応じて表示Y座標をずらす
		int yPos = baseY + (i * kLineSpacing);

		// 選択状態に応じて文字色を変更（黄色 / グレー）
		int color = isSelected ? GetColor(255, 255, 0) : GetColor(170, 170, 170);

		// 現在選択されている項目かつ、点滅処理によって非表示期間である場合は描画をスキップ
		if (isSelected && !isVisible_)
		{
			continue;
		}

		Font::GetInstance().DrawDefaultText(
			xPos, yPos, font.message.c_str(),
			color, kFontSize, Font::FONT_TYPE_ANTIALIASING_EDGE
		);
	}
}

void SceneUi::AddCharacter(const char* message)
{
	if (message == nullptr)
	{
		return;
	}
	// リストに表示項目を追加
	fontList_.push_back(FontData{ message });
}

void SceneUi::SetCurrentIndex(int index)
{
	// 範囲外アクセス防止のガード
	if (index >= 0 && index < GetMaxIndex())
	{
		currentIndex_ = index;
	}
}

int SceneUi::GetCurrentIndex(void) const
{
	return currentIndex_;
}

int SceneUi::GetMaxIndex(void) const
{
	return static_cast<int>(fontList_.size());
}