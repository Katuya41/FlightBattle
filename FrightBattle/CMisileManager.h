#pragma once
#include<vector>
#include"direct3d.h"

class CMisile;
class CGameObject;
class CEnemy;
class CEffect;

/**
 * �����CMisileManager�N���X�̐����ł��B
 * ���̃N���X�̓~�T�C������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CMisileManager
{
	//�~�T�C���̔z��
	std::vector<CMisile*> vMisile;

	//�����o�[�ϐ�
	D3D::Model model;	//�`�悷��ׂ����f���f�[�^
	D3D::Model MisileHitModel;

	DirectX::XMFLOAT2 UVSize = { 0,0 };

	CMisileManager();
	~CMisileManager() {}

	struct MISILE_UI
	{
		ULONGLONG OldTime = 0;
		bool Active = true;
	};

	MISILE_UI MisileUi[2];

	//�~�T�C���̍ő働�b�N�I����
	int MaxRockonNuum = 1;

public:
	static CMisileManager* Get();
	
	/**
	*�X�V�������s���֐��ł��B
	*/
	void Update();

	/**
	*�`�揈�����s���֐��ł��B
	*/
	void Draw();

	/**
	*���f����ݒ肷��֐��ł��B
	* 
	* @param �ݒ肵�����~�T�C���̃��f��
	*/
	void SetModel(D3D::Model _model) { model = _model; }
	
	/**
	*�~�T�C����������������UI�̃��f���ݒ���s���֐��ł��B
	* 
	* @param �ݒ肵�����~�T�C����������������UI���f��
	*/
	void SetMisileHitModel(D3D::Model _model) { MisileHitModel = _model; }

	/**
	*�G�t�F�N�g��UV�T�C�Y�̐ݒ���s���֐��ł��B
	* 
	* @param �ݒ肵����UV�T�C�Y
	*/
	void SetUV_Size(DirectX::XMFLOAT2 _uvsize); 

	/**
	*�~�T�C���̐ݒ���s���֐��ł��B
	* 
	* @param _sppornobj		�X�|�[������ۂ̊�̃I�u�W�F�N�g
	* @param _target		�^�[�Q�b�g�̃I�u�W�F�N�g
	* @param _misileuinum	�w�肷��~�T�C����UI
	*/
	void SetMisileInf(CGameObject* _spornobj, CEnemy* _target, int _misileuinum);
};

