#pragma once
#include <map>
#include <Dxlib.h>
#include "../../Common/Vector2.h"

/// <summary>
/// 入力全般（キーボード、マウス、ゲームパッド）を管理するクラス（シングルトン）
/// </summary>
class InputManager
{

public:
	/// <summary>
	/// ゲームコントローラーの認識番号
	/// DxLib定数、DX_INPUT_PAD1等に対応
	/// </summary>
	enum class JOYPAD_NO
	{
		KEY_PAD1,			/// キー入力とパッド１入力
		PAD1,				/// パッド１入力
		PAD2,				/// パッド２入力
		PAD3,				/// パッド３入力
		PAD4,				/// パッド４入力
		INPUT_KEY = 4096	/// キー入力
	};

	/// <summary>
	/// ゲームコントローラータイプ
	/// DxLib定数、DX_OTHER等に対応
	/// </summary>
	enum class JOYPAD_TYPE
	{
		OTHER = 0,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK_4,
		DUAL_SENSE,
		SWITCH_JOY_CON_L,
		SWITCH_JOY_CON_R,
		SWITCH_PRO_CTRL,
		MAX
	};

	/// <summary>
	/// ゲームコントローラーボタン
	/// </summary>
	enum class JOYPAD_BTN
	{
		LEFT = 0,
		RIGHT,
		TOP,
		DOWN,
		R_TRIGGER,
		L_TRIGGER,
		R_BUTTON,
		L_BUTTON,
		START_BUTTON,
		SELECT_BUTTON,
		MAX
	};
	/// <summary>
	/// ゲームコントローラーの入力情報
	/// </summary>
	struct JOYPAD_IN_STATE
	{
		unsigned char ButtonsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		unsigned char ButtonsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgDown[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgUp[static_cast<int>(JOYPAD_BTN::MAX)];
		int AKeyLX;
		int AKeyLY;
		int AKeyRX;
		int AKeyRY;
	};

	/// <summary>
	/// インスタンスを明示的に生成
	/// </summary>
	static void CreateInstance(void);

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static InputManager& GetInstance(void);

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(void);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(void);

	/// <summary>
	/// リソースの破棄
	/// </summary>
	void Destroy(void);

	/// <summary>
	/// 判定を行うキーを追加
	/// </summary>
	void Add(int key);

	/// <summary>
	/// 判定を行うキーをクリア
	/// </summary>
	void Clear(void);

	/// <summary>
	/// キーの押下判定
	/// </summary>
	bool IsNew(int key) const;

	/// <summary>
	/// キーの押下判定(押しっぱなしはNG)
	/// </summary>
	bool IsTrgDown(int key) const;

	/// <summary>
	/// キーを離した時の判定
	/// </summary>
	bool IsTrgUp(int key) const;

	/// <summary>
	/// マウス座標の取得
	/// </summary>
	Vector2 GetMousePos(void) const;

	/// <summary>
	/// マウスのクリック状態を取得(MOUSE_INPUT_LEFT、RIGHT)
	/// </summary>
	int GetMouse(void) const;

	/// <summary>
	/// マウスが左クリックされたか
	/// </summary>
	bool IsClickMouseLeft(void) const;

	/// <summary>
	/// マウスが右クリックされたか
	/// </summary>
	bool IsClickMouseRight(void) const;

	/// <summary>
	/// マウスが左クリックされたか(押しっぱなしはNG)
	/// </summary>
	bool IsTrgMouseLeft(void) const;

	/// <summary>
	/// マウスが右クリックされたか(押しっぱなしはNG)
	/// </summary>
	bool IsTrgMouseRight(void) const;

	/// <summary>
	/// コントローラの入力情報を取得する
	/// </summary>
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);
	
	/// <summary>
	/// ボタンが押された
	/// </summary>
	bool IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const;

	/// <summary>
	/// パッドボタンが新しく押されたか
	/// </summary>
	bool IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const;
	
	/// <summary>
	/// パッドボタンが離されたか
	/// </summary>
	bool IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const;

private:

	/// <summary>
	/// キー情報
	/// </summary>
	struct Info
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	/// <summary>
	/// マウス情報
	/// </summary>
	struct MouseInfo
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	/// <summary>
	/// コントローラ情報
	/// </summary>
	DINPUT_JOYSTATE joyDInState_;

	/// <summary>
	/// コントローラ情報(XBOX)
	/// </summary>
	XINPUT_STATE joyXInState_;

	/// <summary>
	/// シングルトン用インスタンス
	/// </summary>
	static InputManager* instance_;

	///<summary>
	/// キー情報
	/// </summary>
	std::map<int, InputManager::Info> keyInfos_;
	InputManager::Info infoEmpty_;

	/// <summary>
	/// マウス情報
	/// </summary>
	std::map<int, InputManager::MouseInfo> mouseInfos_;
	InputManager::MouseInfo mouseInfoEmpty_;

	/// <summary>
	/// マウスカーソルの位置
	/// </summary>
	Vector2 mousePos_;
	
	/// <summary>
	/// マウスボタンの入力状態
	/// </summary>
	int mouseInput_;

	/// <summary>
	// パッド情報
	/// </summary>
	JOYPAD_IN_STATE padInfos_[5];

	/// <summary>
	/// デフォルトコンストラクタをprivateにして、
	/// 外部から生成できない様にする
	/// </summary>
	InputManager(void);
	InputManager(const InputManager& manager);
	~InputManager(void) = default;

	/// <summary>
	/// 配列の中からキー情報を取得する
	/// </summary>
	const InputManager::Info& Find(int key) const;

	/// <summary>
	/// 配列の中からマウス情報を取得する
	/// </summary>
	const InputManager::MouseInfo& FindMouse(int key) const;

	/// <summary>
	/// 接続されたコントローラの種別を取得する
	/// </summary>
	JOYPAD_TYPE GetJPadType(JOYPAD_NO no);

	/// <summary>
	/// コントローラの入力情報を取得する
	/// </summary>
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no);

	/// <summary>
	/// コントローラ(XBOX)の入力情報を取得する
	/// </summary>
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no);

	/// <summary>
	/// コントローラの入力情報を更新する
	/// </summary>
	void SetJPadInState(JOYPAD_NO jpNo);

};