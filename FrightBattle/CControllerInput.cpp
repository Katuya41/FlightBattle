#include "CControllerInput.h"
#include <DirectXMath.h>
#include <iostream>

/**
    *CControllerInputのコンストラクタです。
    */
CControllerInput::CControllerInput()
{
    ZeroMemory(&State, sizeof(XINPUT_STATE));  // 構造体全体をゼロで初期化
}

CControllerInput* CControllerInput::Get()
{
	static CControllerInput instance;
	return &instance;
}

/**
*コントローラーボタンのボタンが押された瞬間を取得する関数です。
*
* @param コントローラーボタンの名前
* @return ボタンが押されたときだけtrue
*/
bool CControllerInput::GetBottuanTrigger(BOTTUN_NAME _buttonname)
{
	//コントローラーの状態を取得
	GetControllerState();

    //ボタン入力判定
    DoControllerEvent();

    switch (_buttonname)
    {
    case CControllerInput::NONE:
        break;
    case CControllerInput::A:
        if (iPad_A)
        {
            return true;
        }
        break;
    case CControllerInput::B:
        if (iPad_B)
        {
            return true;
        }
        break;
    case CControllerInput::X:
        if (iPad_X)
        {
            return true;
        }
        break;
    case CControllerInput::Y:
        if (iPad_Y)
        {
            return true;
        }
        break;
    case CControllerInput::RIGHT:
        if (iPad_right)
        {
            return true;
        }
        break;
    case CControllerInput::LEFT:
        if (iPad_left)
        {
            return true;
        }
        break;
    case CControllerInput::UP:
        if (iPad_up)
        {
            return true;
        }
        break;
    case CControllerInput::DOWN:
        if (iPad_down)
        {
            return true;
        }
        break;
    case CControllerInput::RB:
        if (iPad_rightshoulder)
        {
            return true;
        }
        break;
    case CControllerInput::LB:
        if (iPad_leftshoulder)
        {
            return true;
        }
        break;
    case CControllerInput::RT:
        if (State.Gamepad.bRightTrigger)
        {
            return true;
        }
        break;
    case CControllerInput::LT:
        if (State.Gamepad.bLeftTrigger)
        {
            return true;
        }
        break;
    case CControllerInput::MENU:
        if (iPad_Start)
        {
            return true;
        }
        break;
    default:
        break;
    }

	return false;
}

/**
*コントローラーが接続されているか判定を取得する関数です。
*/
void CControllerInput::GetControllerState()
{
	ZeroMemory(&State, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	DwResult = XInputGetState(0, &State);

	if (DwResult == ERROR_SUCCESS)
	{
		//接続に成功したときの処理

	}
	else
	{
		//接続に失敗したときの処理

	}
}

/**
*コントローラーボタンのボタンが押されたかを取得する関数です。
*/
void CControllerInput::DoControllerEvent()
{
    //コントローラーの初期化
    iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
    iPad_leftshoulder = 0, iPad_rightshoulder = 0;
    iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
    iPad_Start = 0;

    //コントローラーのアップデート
    //対応する操作がされたらされた操作のフラグが1になる
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) 
        iPad_left = 1;//ゲームパッド十字キー左
    else OldLeftKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) 
        iPad_right = 1;//ゲームパッド十字キー右
    else OldRightKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) 
        iPad_up = 1;//ゲームパッド十字キー上
    else OldUpKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
        iPad_down = 1;//ゲームパッド十字キー下
    else OldDownKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) 
        iPad_leftshoulder = 1;//ゲームパッドL
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) 
        iPad_rightshoulder = 1;//ゲームパッドR

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_A)
    {
        iPad_A = 1;//ゲームパッドA
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_B)
    {
        iPad_B = 1;//ゲームパッドB
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_X)
    {
        iPad_X = 1;//ゲームパッドX
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
    {
        iPad_Y = 1;//ゲームパッドY
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_START)
    {
        iPad_Start = 1;//ゲームパッドスタート
    }
    else
        OldStartKey = false;

    //ゲームパッドアナログスティックのデッドゾーン処理
    if ((State.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && State.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
        (State.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && State.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
    {
        State.Gamepad.sThumbLX = 0;
        State.Gamepad.sThumbLY = 0;
    }

    if (State.Gamepad.sThumbLX != 0)
    {
        LeftThumbLX = false;
    }

    if (State.Gamepad.sThumbLY < 0)
    {
        LeftThumbLY = false;
    }
}
