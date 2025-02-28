#pragma once
#include"direct3d.h"
#include <DirectXCollision.h>
#include <xinput.h>
#pragma comment (lib, "xinput.lib")

/**
 * �����CControllerInput�N���X�̐����ł��B
 * ���̃N���X�̓R���g���[���[����Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CControllerInput
{
public:

	/**
	*�R���g���[���[�{�^���̖��O������񋓌^�ł��B
	*
	* @param �R���g���[���[�{�^���̖��O
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
	*CControllerInput�̃R���X�g���N�^�ł��B
	*/
	CControllerInput();

	/**
	*CControllerInput�̃f�X�g���N�^�ł��B
	*/
	~CControllerInput() {}

	//�ݒ肷��R���g���[���[�{�^���̕ϐ�
	BOTTUN_NAME BottunName = NONE;

	//�R���g���[���[�̐ڑ��m�F�Ɏg��
	DWORD DwResult = ERROR_DEVICE_NOT_CONNECTED;	
	//���݂̃R���g���[���[�̏�ԊǗ�
	XINPUT_STATE State;		

	//�R���g���[���[�̏�����
	int iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
	int iPad_leftshoulder = 0, iPad_rightshoulder = 0;
	int iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
	int iPad_Start = 0;
	int iPad_LStickX = 0, iPad_LStickY = 0;

	//�R���g���[���[�g���K�[�p�ϐ�
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
	*�R���g���[���[�{�^���̃{�^���������ꂽ�u�Ԃ��擾����֐��ł��B
	*
	* @param �R���g���[���[�{�^���̖��O
	* @return �{�^���������ꂽ�Ƃ�����true
	*/
	bool GetBottuanTrigger(BOTTUN_NAME _buttonname);

	/**
	*�R���g���[���[���ڑ�����Ă��邩������擾����֐��ł��B
	*/
	void GetControllerState();

	/**
	*�R���g���[���[�{�^���̃{�^���������ꂽ�����擾����֐��ł��B
	*/
	void DoControllerEvent();
};

