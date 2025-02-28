#pragma once
#include "CGameObject.h"

/**
 * �����CMisileUI�N���X�̐����ł��B
* ���̃N���X�̓~�T�C��UI����Ŏg�p����܂��B
 *
* @author �g������
* @since 2024-7-10
*/
class CMisileUI :
	public CGameObject
{
private:

	//�����Ă���G�t�F�N�g���
	CEffect* pEffect;

	/**
	* ���݂̃~�T�C��UI�̏�Ԃ�\���\���̂ł�
	*/
	enum MISILEUI_STATE
	{
		READY,		//���ˏ�������
		CHARGE,		//�����[�h��
	};
	MISILEUI_STATE MisileUIState = READY;

	//�~�T�C���𔭎˂�������
	ULONGLONG MisileShotTime = 0;

	//�����[�h���Ƀ~�T�C���̒��g���B���p�l��
	CGameObject* MisileUIPanel;

	//�A�j���[�V�����̃X�s�[�h
	float AnimSpeed = 0.0f;

public:
	//�R���X�g���N�^
	CMisileUI();
	~CMisileUI();

	/**
	*�X�V�������s���֐��ł��B
	*/
	void Update() override;

	/**
	* ���݂̃~�T�C��UI�̏�Ԃ�Ԃ��܂�
	* 
	* @return �~�T�C��UI�̏�Ԃ�Ԃ��܂��B
	*/
	MISILEUI_STATE& GetMisileUIState() { return MisileUIState; }

	/**
	* ���݂̃~�T�C��UI�̏�Ԃ�ݒ肵�܂��B
	* 
	* @param  �ݒ肵�����~�T�C��UI�̏��
	*/
	void SetMisileUIStateCharge();

	/**
	* �~�T�C��UI�̏����Z�b�g���܂��B
	*/
	void SetMisileUIInf(D3D::Model _model,DirectX::XMFLOAT2 _effectsize);

	/**
	* �G�t�F�N�g�̍��W��ݒ肷��֐��ł��B
	*/
	void SetEffectPos(const DirectX::XMFLOAT3& _pos);

	/**
	* �G�t�F�N�g�̃T�C�Y��ݒ肷��֐��ł��B
	*/
	void SetEffectScale(const DirectX::XMFLOAT3& _scale);

	/**
	* �~�T�C���̏�Ԃ����Z�b�g����
	*/
	void ResetMisileUI();

	/**
	* �A�j���[�V�����̃X�s�[�h��ݒ肷��֐��ł��B
	* 
	* @param  �ݒ肵�����A�j���[�V�����̃X�s�[�h
	*/
	void SetAnimSped(const float& _speed) { AnimSpeed = _speed; }

	/**
	* �A�j���[�V�����̃X�s�[�h���擾����֐��ł��B
	* 
	* @return �A�j���[�V�����̃X�s�[�h
	*/
	const float& GetAnimSpeed() { return AnimSpeed; }

};