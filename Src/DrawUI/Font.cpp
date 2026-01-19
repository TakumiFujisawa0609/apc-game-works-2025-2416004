#include"Font.h"


//シングルトンインスタンスの初期化
Font* Font::instance_ = nullptr;

//コンストラクタ
Font::Font() : defaultFont_(""){}

//デストラクタ
Font::~Font(void)
{
	//全てのフォントハンドルを解放
	for (auto& outerPair : fontHandles_)
	{
		for (auto& innerPair : outerPair.second)
		{
			int fontHandle = innerPair.second;
			if (fontHandle != -1 && fontHandle != DX_DEFAULT_FONT_HANDLE)
			{
				DeleteFontToHandle(fontHandle);
			}
		}
	}

	//動的フォントハンドルを解放
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
	instance_->Init();
}

//シングルトンインスタンス取得
Font& Font::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		Font::CreateInstance();
	}
	return *instance_;
}

//フォントの初期化
void Font::Init(void)
{
}

//フォントの追加
bool Font::AddFont(const std::string& fontId, const std::string& internalFontName, const std::string& fontPath, int fontSize, int fontWeight, int fontType)
{
	// 1. フォントファイルをシステムに一時登録する
	// これを行わないと、ファイルパスがあっても「内部フォント名」で作成できない場合があります
	if (fontPath != "") {
		if (AddFontResourceEx(fontPath.c_str(), FR_PRIVATE, NULL) > 0) {
			// 登録成功（OSがinternalFontNameを認識できる状態になった）
		}
		else {
			// ファイルからの読み込み失敗
			return false;
		}
	}

	// 2. フォントハンドルの作成
	// 日本語名が含まれても良いように、DXライブラリの文字コード設定に準拠
	int handle = CreateFontToHandle(internalFontName.c_str(), fontSize, fontWeight, fontType);

	if (handle == -1)
	{
		return false;
	}

	// マップに保存
	fontHandles_[fontId][{fontSize, fontType}] = handle;

	// デフォルトフォントが空なら設定
	if (defaultFont_ == "")
	{
		defaultFont_ = fontId;
	}

	// 内部で使用する名前を保存（動的生成用）
	fontIdToInternalName_[fontId] = internalFontName;

	return true;
}

//フォントの削除
void Font::RemoveFont(const std::string& fontId)
{
	auto it = fontHandles_.find(fontId);
	if (it != fontHandles_.end())
	{
		// 内側のハンドルを全て削除
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

//デフォルトフォントの設定
void Font::SetDefaultFont(const std::string& fontId)
{
	auto it = fontHandles_.find(fontId);
	if (it != fontHandles_.end() && !it->second.empty())
	{
		defaultFont_ = fontId;
	}
	else
	{
		OutputDebugStringA(("SetDefaultFont: フォントID [" + fontId + "] は未登録またはサイズ情報がありません。\n").c_str());
	}
}

//テキスト描画
void Font::DrawText(const std::string& fontId, int x, int y, const char* text, int color, int fontSize, int fontType)
{
	int fontHandle = -1;
	int useFontType = (fontType >= 0) ? fontType : FONT_TYPE_NORMAL;

	// フォント名取得
	auto itName = fontNameMap_.find(fontId);
	std::string internalFontName = (itName != fontNameMap_.end()) ? itName->second : "";

	if (fontSize > 0)
	{
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

		if (fontHandle == -1)
		{
			// ここで登録済みフォント名を使う
 			fontHandle = GetDynamicFontHandle(internalFontName, fontSize, 3, useFontType);
		}
	}
	else
	{
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

	if (fontHandle == -1)
	{
		fontHandle = DX_DEFAULT_FONT_HANDLE;
	}

         	DrawFormatStringFToHandle(x, y, color, fontHandle, text);
}

// デフォルトフォントで描画
void Font::DrawDefaultText(int x, int y, const char* text, int color, int fontSize, int fontType)
{
	DrawText(defaultFont_, x, y, text, color, fontSize, fontType);
}

//文字の横幅を取得
int Font::GetDefaultTextWidth(const std::string& text) const
{
	return GetDrawStringWidth(text.c_str(), static_cast<int>(text.size()));
}

//一時的なフォントを取得または生成
int Font::GetDynamicFontHandle(const std::string& internalFontName, int fontSize, int fontWeight, int fontType)
{
	auto key = std::make_pair(fontSize, fontType);
	auto it = dynamicFontHandles_.find(key);

	if (it != dynamicFontHandles_.end())
	{
		return it->second;
	}

	int fontHandle = CreateFontToHandle(internalFontName.c_str(), fontSize, fontWeight, fontType);
	if (fontHandle != -1)
	{
		dynamicFontHandles_[key] = fontHandle;
	}
	return fontHandle;
}

//解放処理
void Font::Destroy(void)
{
	if (instance_ != nullptr)
	{
		delete instance_;       // デストラクタが呼ばれてフォントを解放
		instance_ = nullptr;
	}
}