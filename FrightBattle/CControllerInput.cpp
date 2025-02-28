#include "CControllerInput.h"
#include <DirectXMath.h>
#include <iostream>

/**
    *CControllerInput�̃R���X�g���N�^�ł��B
    */
CControllerInput::CControllerInput()
{
    ZeroMemory(&State, sizeof(XINPUT_STATE));  // �\���̑S�̂��[���ŏ�����
}

CControllerInput* CControllerInput::Get()
{
	static CControllerInput instance;
	return &instance;
}

/**
*�R���g���[���[�{�^���̃{�^���������ꂽ�u�Ԃ��擾����֐��ł��B
*
* @param �R���g���[���[�{�^���̖��O
* @return �{�^���������ꂽ�Ƃ�����true
*/
bool CControllerInput::GetBottuanTrigger(BOTTUN_NAME _buttonname)
{
	//�R���g���[���[�̏�Ԃ��擾
	GetControllerState();

    //�{�^�����͔���
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
*�R���g���[���[���ڑ�����Ă��邩������擾����֐��ł��B
*/
void CControllerInput::GetControllerState()
{
	ZeroMemory(&State, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	DwResult = XInputGetState(0, &State);

	if (DwResult == ERROR_SUCCESS)
	{
		//�ڑ��ɐ��������Ƃ��̏���

	}
	else
	{
		//�ڑ��Ɏ��s�����Ƃ��̏���

	}
}

/**
*�R���g���[���[�{�^���̃{�^���������ꂽ�����擾����֐��ł��B
*/
void CControllerInput::DoControllerEvent()
{
    //�R���g���[���[�̏�����
    iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
    iPad_leftshoulder = 0, iPad_rightshoulder = 0;
    iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
    iPad_Start = 0;

    //�R���g���[���[�̃A�b�v�f�[�g
    //�Ή����鑀�삪���ꂽ�炳�ꂽ����̃t���O��1�ɂȂ�
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) 
        iPad_left = 1;//�Q�[���p�b�h�\���L�[��
    else OldLeftKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) 
        iPad_right = 1;//�Q�[���p�b�h�\���L�[�E
    else OldRightKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) 
        iPad_up = 1;//�Q�[���p�b�h�\���L�[��
    else OldUpKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
        iPad_down = 1;//�Q�[���p�b�h�\���L�[��
    else OldDownKey = false;

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) 
        iPad_leftshoulder = 1;//�Q�[���p�b�hL
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) 
        iPad_rightshoulder = 1;//�Q�[���p�b�hR

    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_A)
    {
        iPad_A = 1;//�Q�[���p�b�hA
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_B)
    {
        iPad_B = 1;//�Q�[���p�b�hB
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_X)
    {
        iPad_X = 1;//�Q�[���p�b�hX
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
    {
        iPad_Y = 1;//�Q�[���p�b�hY
    }
    if (State.Gamepad.wButtons & XINPUT_GAMEPAD_START)
    {
        iPad_Start = 1;//�Q�[���p�b�h�X�^�[�g
    }
    else
        OldStartKey = false;

    //�Q�[���p�b�h�A�i���O�X�e�B�b�N�̃f�b�h�]�[������
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
