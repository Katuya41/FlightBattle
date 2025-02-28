#pragma once
#include"direct3d.h"
#include <DirectXCollision.h>
#include <xinput.h>
#pragma comment (lib, "xinput.lib")

/**
 * これはCControllerInputクラスの説明です。
 * このクラスはコントローラー制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CControllerInput
{
public:

	/**
	*コントローラーボタンの名前を入れる列挙型です。
	*
	* @param コントローラーボタンの名前
	*/
	enum BOTTUN_NAME
	{
		NONE,
		A,
		B,
		X,
		Y,
		RIGHT,
		LEFT,
		UP,
		DOWN,
		RB,
		LB,
		RT,
		LT,
		MENU,
	};

private:
	/**
	*CControllerInputのコンストラクタです。
	*/
	CControllerInput();

	/**
	*CControllerInputのデストラクタです。
	*/
	~CControllerInput() {}

	//設定するコントローラーボタンの変数
	BOTTUN_NAME BottunName = NONE;

	//コントローラーの接続確認に使う
	DWORD DwResult = ERROR_DEVICE_NOT_CONNECTED;	
	//現在のコントローラーの状態管理
	XINPUT_STATE State;		

	//コントローラーの初期化
	int iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
	int iPad_leftshoulder = 0, iPad_rightshoulder = 0;
	int iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
	int iPad_Start = 0;
	int iPad_LStickX = 0, iPad_LStickY = 0;

	//コントローラートリガー用変数
	bool OldAKey = false;
	bool OldBKey = false;
	bool OldXKey = false;
	bool OldYKey = false;
	bool OldRightKey = false;
	bool OldLeftKey = false;
	bool OldUpKey = false;
	bool OldDownKey = false;
	bool OldStartKey = false;
	bool LeftThumbLX = false;
	bool LeftThumbLY = false;

public:
	static CControllerInput* Get();

	/**
	*コントローラーボタンのボタンが押された瞬間を取得する関数です。
	*
	* @param コントローラーボタンの名前
	* @return ボタンが押されたときだけtrue
	*/
	bool GetBottuanTrigger(BOTTUN_NAME _buttonname);

	/**
	*コントローラーが接続されているか判定を取得する関数です。
	*/
	void GetControllerState();

	/**
	*コントローラーボタンのボタンが押されたかを取得する関数です。
	*/
	void DoControllerEvent();
};

