#include "Input.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input::~Input()
{
	if (mDevMouse != nullptr)
	{
		mDevMouse->Release();
		mDevMouse = nullptr;
	}
	if (mDevKeyboad != nullptr)
	{
		mDevKeyboad->Release();
		mDevKeyboad = nullptr;
	}
	if (mDInput != nullptr)
	{
		mDInput->Release();
		mDInput = nullptr;
	}
}

Input* Input::Get()
{
    static Input instance;
    return &instance;
}

void Input::Initialize(HWND hWnd, HINSTANCE hInstance)
{
	HRESULT	hr;
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDInput, NULL);
	if (FAILED(hr)) {
		return;
	}

	// キーボードデバイス生成
	mDInput->CreateDevice(GUID_SysKeyboard, &mDevKeyboad, NULL);
	if (FAILED(hr)) {
		return;
	}

	// データフォーマットの設定
	hr = mDevKeyboad->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) {
		return;
	}

	// 協調レベルの設定
	hr = mDevKeyboad->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		return;
	}

	// マウスデバイス生成
	mDInput->CreateDevice(GUID_SysMouse, &mDevMouse, NULL);
	if (FAILED(hr)) {
		return;
	}

	// データフォーマットの設定
	hr = mDevMouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr)) {
		return;
	}

	// 協調レベルの設定
	hr = mDevMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		return;
	}

	mDevKeyboad->Acquire();
	mDevMouse->Acquire();
}

void Input::Update()
{
	// １フレーム前のデータを保存
	memcpy_s(mOldKeyBuffer, sizeof(mOldKeyBuffer), mKeyBuffer, sizeof(mKeyBuffer));

	HRESULT hr;

	// キーボードの状態を取得する
	hr = mDevKeyboad->GetDeviceState(sizeof(mKeyBuffer), &mKeyBuffer);

	if (FAILED(hr))
	{
		mDevKeyboad->Acquire();
		mDevKeyboad->GetDeviceState(sizeof(mKeyBuffer), &mKeyBuffer);
	}

	mOldMouseState = mMouseState;

	// マウスの状態を取得する
	hr = mDevMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);

	if (FAILED(hr))
	{
		mDevMouse->Acquire();
		mDevMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);
	}
}

bool Input::GetKeyPress(int key)
{
	return (mKeyBuffer[key] & 0x80) != 0;
}

bool Input::GetKeyTrigger(int key)
{
	return (mOldKeyBuffer[key] & 0x80) == 0 && (mKeyBuffer[key] & 0x80) != 0;
}

bool Input::GetKeyUp(int key)
{
	return (mOldKeyBuffer[key] & 0x80) == 1 && (mKeyBuffer[key] & 0x80) != 0;
}

bool Input::GetMousePress(int key)
{
	return (mMouseState.rgbButtons[key] & 0x80) != 0;
}

POINT Input::GetMouseMove()
{
	POINT pt;
	pt.x = mMouseState.lX;
	pt.y = mMouseState.lY;
	return pt;
}
