#pragma once
#include<vector>
#include"direct3d.h"

class CEffect;

/**
 * �����CEffectManager�N���X�̐����ł��B
 * ���̃N���X�̓G�t�F�N�g����Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CEffectManager
{
private: //�����o�[�ϐ�

	//�G�t�F�N�g�̉ϒ��z��
	std::vector<CEffect*> vEffect;
	std::vector<CEffect*> vEffect2D;

	//�`�悷��ׂ����f���f�[�^
	D3D::Model model;	

	CEffectManager() {}
	~CEffectManager();

public:
	static CEffectManager* Get();

	/**
	*CEffect�̍X�V�������s���֐��ł��B
	*/
	void Update();

	/**
	*CEffect�̕`�揈���s���֐��ł��B
	*/
	void Draw();

	/**
	* UI�p��Draw�֐�
	*/
	void Draw2D();

	/**
	*�G�t�F�N�g���Z�b�g����֐��ł��B
	* 
	* @param �Z�b�g����G�t�F�N�g
	*/
	void SetvEffect(CEffect* _effect, DirectX::XMFLOAT3 _position, int _animpattern, float _animspeed);

	void Set3DvEffect(CEffect* _effect, int _animpattern, float _animspeed);

	/**
	*�G�t�F�N�g���Z�b�g����֐��ł��B
	*
	* @param �Z�b�g����G�t�F�N�g
	*/
	void SetvEffectMisileUI(CEffect* _effect, int _animpattern, float _animspeed);

	/**
	* �G�t�F�N�g�̒��g���N���A����֐��ł�
	*/
	void ClearEffect();
};

