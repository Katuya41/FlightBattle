#pragma once

// Direct Inputを使ってキーボードとマウス操作を取得するクラス
#define DIRECTINPUT_VERSION 0x0800
#include	<dinput.h>

class Input
{

private:
	IDirectInput8* mDInput = nullptr;
	IDirectInputDevice8* mDevKeyboad = nullptr;
	IDirectInputDevice8* mDevMouse = nullptr;

	unsigned char mKeyBuffer[256] = {};
	unsigned char mOldKeyBuffer[256] = {};

	DIMOUSESTATE2 mMouseState = {};
	DIMOUSESTATE2 mOldMouseState = {};

	Input() {};

	~Input();
	

public:
	// シングルトンのインスタンスを取得する関数
	static Input* Get();

	// DirectInputの初期化を行う関数
	// ゲームループが始まる前に呼び出す
	void Initialize(HWND hWnd, HINSTANCE hInstance);

	// 毎ループ呼び出しキーの押下状態をバッファに保存する
	void Update();

	// 現在のキーボードの押下状態を取得する
	// key: DIK_で始まるキー定数を使う
	bool GetKeyPress(int key);

	//キーを押した瞬間を取得する
	bool GetKeyTrigger(int key);

	/**
	* キーが離れた瞬間を取得する
	* 
	* @param  判定するキー
	* @return キーが離れたか
	*/
	bool GetKeyUp(int key);

	// マウスボタンの押下状態を取得する
	// key: 0-4
	bool GetMousePress(int key);

	// マウスの移動量を取得する
	POINT GetMouseMove();

};

