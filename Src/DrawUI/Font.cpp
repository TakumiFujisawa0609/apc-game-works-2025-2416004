#include "../Pch.h"
#include "Font.h"

// シングルトンインスタンスの静的メンバ初期化
Font* Font::instance_ = nullptr;

std::size_t Font::PairHash::operator()(const std::pair<int, int>& p) const noexcept
{
	return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
}

Font::Font(void) : defaultFont_("")
{
}

Font::~Font(void)
{
	// 登録されている全てのフォントハンドルをループで解放
	for (auto& outerPair : fontHandles_)
	{
		for (auto& innerPair : outerPair.second)
		{
			int fontHandle = innerPair.second;
			// 有効かつデフォルト以外のハンドルを削除
			if (fontHandle != -1 && fontHandle != DX_DEFAULT_FONT_HANDLE)
			{
				DeleteFontToHandle(fontHandle);
			}
		}
	}

	// キャッシュされている動的フォントハンドルも同様に解放
	for (const auto& dynamicFont : dynamicFontHandles_)
	{
		if (dynamicFont.second != -1 && dynamicFont.second != DX_DEFAULT_FONT_HANDLE)
		{
			DeleteFontToHandle(dynamicFont.second);
		}
	}
}

void Font::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Font();
	}
	instance_->Initialize();
}

Font& Font::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		Font::CreateInstance();
	}
	return *instance_;
}

void Font::Initialize(void)
{
	// 必要に応じて初期化処理を記述
}

bool Font::AddFont(const std::string& fontId, const std::string& internalFontName, const std::string& fontPath, int fontSize, int fontWeight, int fontType)
{
	// OSのフォントシステムにファイルを一時登録（FR_PRIVATE: このプロセスのみ有効）
	if (fontPath != "")
	{
		if (AddFontResourceEx(fontPath.c_str(), FR_PRIVATE, NULL) <= 0)
		{
			// ファイルの読み込みまたは登録に失敗
			return false;
		}
	}

	// 指定されたパラメータでDXライブラリのフォントハンドルを作成
	int handle = CreateFontToHandle(internalFontName.c_str(), fontSize, fontWeight, fontType);

	if (handle == -1)
	{
		return false;
	}

	// IDと(サイズ, タイプ)のペアをキーにしてハンドルを保存
	fontHandles_[fontId][{fontSize, fontType}] = handle;

	// 最初に登録されたフォントを自動的にデフォルトに設定
	if (defaultFont_ == "")
	{
		defaultFont_ = fontId;
	}

	// 後続のサイズ変更描画（動的生成）のために内部フォント名を記録
	fontIdToInternalName_[fontId] = internalFontName;

	return true;
}

void Font::RemoveFont(const std::string& fontId)
{
	auto it = fontHandles_.find(fontId);
	if (it != fontHandles_.end())
	{
		// このIDに紐付いている全てのサイズバリエーションを解放
		for (auto& innerPair : it->second)
		{
			int fontHandle = innerPair.second;
			if (fontHandle != -1 && fontHandle != DX_DEFAULT_FONT_HANDLE)
			{
				DeleteFontToHandle(fontHandle);
			}
		}
		fontHandles_.erase(it);
	}
}

void Font::SetDefaultFont(const std::string& fontId)
{
	auto it = fontHandles_.find(fontId);
	// 存在するIDかつ有効なハンドルリストを持っているか確認
	if (it != fontHandles_.end() && !it->second.empty())
	{
		defaultFont_ = fontId;
	}
	else
	{
		OutputDebugStringA(("SetDefaultFont: フォントID [" + fontId + "] は未登録またはサイズ情報がありません。\n").c_str());
	}
}

void Font::DrawText(const std::string& fontId, int x, int y, const char* text, int color, int fontSize, int fontType)
{
	int fontHandle = -1;
	int useFontType = (fontType >= 0) ? fontType : FONT_TYPE_NORMAL;

	// IDから内部フォント名を検索
	auto itName = fontIdToInternalName_.find(fontId);
	std::string internalFontName = (itName != fontIdToInternalName_.end()) ? itName->second : "";

	if (fontSize > 0)
	{
		// 指定されたサイズ・タイプのハンドルが既存か確認
		auto itFont = fontHandles_.find(fontId);
		if (itFont != fontHandles_.end())
		{
			auto& sizeMap = itFont->second;
			auto itSize = sizeMap.find({ fontSize, useFontType });
			if (itSize != sizeMap.end())
			{
				fontHandle = itSize->second;
			}
		}

		// 未作成のサイズなら動的に生成を試みる
		if (fontHandle == -1)
		{
			fontHandle = GetDynamicFontHandle(internalFontName, fontSize, 3, useFontType);
		}
	}
	else
	{
		// サイズ未指定の場合は登録済みの中から最初に見つかったものを使用
		auto itFont = fontHandles_.find(fontId);
		if (itFont != fontHandles_.end())
		{
			auto& sizeMap = itFont->second;
			if (!sizeMap.empty())
			{
				fontHandle = sizeMap.begin()->second;
			}
		}
	}

	// 最終的に取得できなかった場合は標準ハンドルを使用
	if (fontHandle == -1)
	{
		fontHandle = DX_DEFAULT_FONT_HANDLE;
	}

	// DXライブラリの描画関数を呼び出し
	DrawFormatStringToHandle(x, y, color, fontHandle, text);
}

void Font::DrawDefaultText(int x, int y, const char* text, int color, int fontSize, int fontType)
{
	DrawText(defaultFont_, x, y, text, color, fontSize, fontType);
}

int Font::GetDefaultTextWidth(const std::string& text) const
{
	// 描画時の文字列のピクセル幅を計算
	return GetDrawStringWidth(text.c_str(), static_cast<int>(text.size()));
}

int Font::GetDynamicFontHandle(const std::string& internalFontName, int fontSize, int fontWeight, int fontType)
{
	// サイズとタイプの組み合わせでキャッシュを検索
	auto key = std::make_pair(fontSize, fontType);
	auto it = dynamicFontHandles_.find(key);

	if (it != dynamicFontHandles_.end())
	{
		return it->second;
	}

	// キャッシュにない場合は新規作成して登録
	int fontHandle = CreateFontToHandle(internalFontName.c_str(), fontSize, fontWeight, fontType);
	if (fontHandle != -1)
	{
		dynamicFontHandles_[key] = fontHandle;
	}
	return fontHandle;
}

void Font:: DestroyInstance(void)
{
	if (instance_ != nullptr)
	{
		// インスタンスの削除。デストラクタ内で各リソースのDelete処理が走る
		delete instance_;
		instance_ = nullptr;
	}
}


