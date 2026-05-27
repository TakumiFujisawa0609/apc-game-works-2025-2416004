#pragma once
#include "../../Common/Vector2.h"
#include <DxLib.h>
#include <map>

/// @brief 入力全般（キーボード、マウス、ゲームパッド）を管理するクラス（シングルトン）
class InputManager
{

public:

	/// @brief ゲームコントローラーの認識番号
	enum class JOYPAD_NO
	{
		KEY_PAD1,			/// キー入力とパッド１入力
		PAD1,				/// パッド１入力
		PAD2,				/// パッド２入力
		PAD3,				/// パッド３入力
		PAD4,				/// パッド４入力
		INPUT_KEY = 4096	/// キー入力
	};

	/// @brief ゲームコントローラータイプ
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

	/// @brief ゲームコントローラーボタン
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

	/// @brief ゲームコントローラーの入力情報
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

	/// @brief インスタンスを明示的に生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得
	/// @return InputManagerの参照
	static InputManager& GetInstance(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief リソースの破棄
	void  DestroyInstance(void);

	/// @brief 判定を行うキーを追加
	/// @param key キーID（KEY_INPUT_...）
	void Add(int key);

	/// @brief 判定を行うキーをクリア
	void Clear(void);

	/// @brief キーの押下判定
	/// @param key キーID
	/// @return 押されていればtrue
	bool IsNew(int key) const;

	/// @brief キーが押されているか（押しっぱなしOK）
	/// @param key キーID
	/// @return 押されていればtrue
	bool IsPress(int key) const;

	/// @brief キーの押下判定(押しっぱなしはNG)
	/// @param key キーID
	/// @return 押した瞬間ならtrue
	bool IsTrgDown(int key) const;

	/// @brief キーを離した時の判定
	/// @param key キーID
	/// @return 離した瞬間ならtrue
	bool IsTrgUp(int key) const;

	/// @brief マウス座標の取得
	/// @return Vector2形式のマウス座標
	Vector2 GetMousePos(void) const;

	/// @brief マウスのクリック状態を取得
	/// @return マウス入力ビットフラグ
	int GetMouse(void) const;

	/// @brief マウスが左クリックされたか
	/// @return クリックされていればtrue
	bool IsClickMouseLeft(void) const;

	/// @brief マウスが右クリックされたか
	/// @return クリックされていればtrue
	bool IsClickMouseRight(void) const;

	/// @brief マウスが左クリックされたか(押しっぱなしはNG)
	/// @return 押した瞬間ならtrue
	bool IsTrgMouseLeft(void) const;

	/// @brief マウスが右クリックされたか(押しっぱなしはNG)
	/// @return 押した瞬間ならtrue
	bool IsTrgMouseRight(void) const;

	/// @brief マウスボタンが押されているか（押しっぱなしOK）
	/// @param key マウスボタンID（MOUSE_INPUT_...）
	/// @return 押されていればtrue
	bool IsMousePress(int key) const;

	/// @brief コントローラの入力情報を取得する
	/// @param no パッド番号
	/// @return JOYPAD_IN_STATE構造体
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);
	
	/// @brief ボタンが押された
	/// @param no パッド番号
	/// @param btn ボタン種類
	/// @return 押されていればtrue
	bool IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const;

	/// @brief パッドボタンが新しく押されたか
	/// @param no パッド番号
	/// @param btn ボタン種類
	/// @return 押した瞬間ならtrue
	bool IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const;
	
	/// @brief パッドボタンが離されたか
	/// @param no パッド番号
	/// @param btn ボタン種類
	/// @return 離した瞬間ならtrue
	bool IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const;

	/// @brief パッドボタンが押されているか（押しっぱなし判定）
	/// @param no パッド番号
	/// @param btn ボタン種類
	/// @return 押されていればtrue
	bool IsPadBtnPress(JOYPAD_NO no, JOYPAD_BTN btn) const;

	/// @brief 左スティックのX軸の値を取得
	/// @param padNo パッド番号
	/// @return -1.0f ～ 1.0f の値
	float GetPadStickLX(int padNo) const;

	/// @brief 左スティックのY軸の値を取得
	/// @param padNo パッド番号
	/// @return -1.0f ～ 1.0f の値
	float GetPadStickLY(int padNo) const;

	/// @brief 右スティックのX軸の値を取得
	/// @param padNo パッド番号
	/// @return -1.0f ～ 1.0f の値
	float GetPadStickRX(int padNo) const;

	/// @brief 右スティックのY軸の値を取得
	/// @param padNo パッド番号
	/// @return -1.0f ～ 1.0f の値
	float GetPadStickRY(int padNo) const;

	/// @brief 左スティック入力から方向ベクトルを取得
	/// @param aKeyX X軸入力値
	/// @param aKeyY Y軸入力値
	/// @return 正規化された方向ベクトル
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY);

	/// @brief マウス感度の設定
	/// @param sensitivity 感度
	void SetMouseSensitivity(float sensitivity);

	/// @brief マウス感度の取得
	/// @return 現在の感度
	float GetMouseSensitivity(void) const { return mouseSensitivity_; }

private:

	/// @brief キー情報構造体
	struct Info
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	/// @brief マウス情報構造体
	struct MouseInfo
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// スティック関連
	static constexpr float AKEY_VAL_MAX = 32767.0f; // スティック入力の最大値
	static constexpr float THRESHOLD = 0.01f;        // スティックの無効範囲

	// マウス関連
	static constexpr float DEFAULT_SENSITIVITY = 0.2f; // デフォルトマウス感度

	// コントローラの情報関連
	DINPUT_JOYSTATE joyDInState_; // コントローラ情報
	XINPUT_STATE joyXInState_;    // コントローラ情報(XBOX)

	// シングルトン用インスタンス
	static InputManager* instance_; 

	// キー情報関連
	std::map<int, InputManager::Info> keyInfos_;  // キー情報管理マップ
	InputManager::Info infoEmpty_;                // 空のキー情報
	
	// マウス情報関連
	std::map<int, InputManager::MouseInfo> mouseInfos_; // マウス情報管理マップ
	InputManager::MouseInfo mouseInfoEmpty_;            // 空のマウス情報

	// マウス関連
	Vector2 mousePos_;           // マウスカーソルの位置
	int mouseInput_;             // マウスボタンの入力状態
	float mouseSensitivity_;     // マウス感度

	// パッド情報（全5スロット）
	JOYPAD_IN_STATE padInfos_[5]; 

	/// @brief コンストラクタ
	InputManager(void);

	/// @brief コピーコンストラクタ（使用禁止）
	InputManager(const InputManager& manager);

	/// @brief デストラクタ
	~InputManager(void) = default;

	/// @brief 配列の中からキー情報を取得する
	/// @param key キーID
	/// @return キー情報への参照
	const InputManager::Info& Find(int key) const;

	/// @brief 配列の中からマウス情報を取得する
	/// @param key マウスボタンID
	/// @return マウス情報への参照
	const InputManager::MouseInfo& FindMouse(int key) const;

	/// @brief 接続されたコントローラの種別を取得する
	/// @param no パッド番号
	/// @return JOYPAD_TYPE
	JOYPAD_TYPE GetJPadType(JOYPAD_NO no);

	/// @brief コントローラの入力情報を取得する（DirectInput）
	/// @param no パッド番号
	/// @return DINPUT_JOYSTATE
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no);

	/// @brief コントローラの入力情報を取得する（XInput）
	/// @param no パッド番号
	/// @return XINPUT_STATE
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no);

	/// @brief コントローラの入力情報を更新する
	/// @param jpNo パッド番号
	void SetJPadInState(JOYPAD_NO jpNo);

};