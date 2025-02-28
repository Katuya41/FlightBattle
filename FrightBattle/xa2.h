//=============================================================================
//
// �T�E���h���� [xa2.h]
//
//=============================================================================
#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_

#include <xaudio2.h>

// �T�E���h�t�@�C��
typedef enum
{
	BGM_TITLE,				//�^�C�g��BGM
	BGM_STAGE,				//�X�e�[�WBGM
	BGM_RESULT,				//���U���g����BGM
	SE_STARTGAME,			//�^�C�g���ł̃{�^����������SE
	SE_MOVETITLE,			//���U���g�Ń{�^����������SE
	SE_MISILESHOTTO,		//�~�T�C����ł�������SE
	SE_MISILEHIT,			//�~�T�C����������������SE
	SE_LOCKON,				//���b�N�I����SE
	SE_ENEMYROCK,			//�G�Ƀ��b�N����Ă���
	SE_BUZZER,				//�v���C���[���b�N�I���u�U�[
	SE_COUNTDOWN,			//�J�E���g�_�E��SE
	SE_COUNTDOWN2,			//�J�E���g�_�E��SE
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// �Q�[�����[�v�J�n�O�ɌĂяo���T�E���h�̏���������
HRESULT XA_Initialize(void);

// �Q�[�����[�v�I����ɌĂяo���T�E���h�̉������
void XA_Release(void);

// �����Ŏw�肵���T�E���h���Đ�����
void XA_Play(SOUND_LABEL label , const float& _volume, const float& _pitch);

// �����Ŏw�肵���T�E���h���~����
void XA_Stop(SOUND_LABEL label);

// �����Ŏw�肵���T�E���h�̍Đ����ĊJ����
void XA_Resume(SOUND_LABEL label);

#endif
