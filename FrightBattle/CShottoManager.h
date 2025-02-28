#pragma once
#include<vector>
#include"direct3d.h"

class CShottoBall;
class CGameObject;
class CEnemy;

/**
 * �����CShottoManager�N���X�̐����ł��B
 * ���̃N���X�͒e��������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CShottoManager
{
private:
	//�~�T�C���̉ϒ��z��
	std::vector<CShottoBall*> vShottoBall;

	//�ˌ��|�C���g
	DirectX::XMFLOAT3 ShotPos = { 0.0f,0.0f,0.0f };

	//�ˌ��|�C���g��\�����邩
	bool IsShotPos = false;

	//�����o�[�ϐ�
	D3D::Model model;	//�`�悷��ׂ����f���f�[�^
	D3D::Model BallHitModel;

	DirectX::XMFLOAT2 UVSize = { 0,0 };

	//�ʂ�����������
	ULONGLONG BallShottoTime = 0;

	CShottoManager() {}
	~CShottoManager() {}

public:
	static CShottoManager* Get();

	//�X�V����
	void Update();

	//�`�揈��
	void Draw();

	//�ˌ��|�C���g�`�F�b�N
	void CheckShotPos(DirectX::XMFLOAT3 _spornpos, CEnemy* _target);

	//���f�����Z�b�g����
	void SetModel(D3D::Model _model) { model = _model; }

	//�e�������������̃G�t�F�N�g
	void SetBallHitModel(D3D::Model _model) { BallHitModel = _model; }

	//�G�t�F�N�g��uv�T�C�Y�ݒ�
	void SetUV_Size(DirectX::XMFLOAT2 _uvsize) { UVSize = _uvsize; }

	//�ˌ����W��Ԃ�
	DirectX::XMFLOAT3 GetShotPoint() { return ShotPos; }

	//�ˌ��|�C���g��\�����邩
	bool GetIsActiveShotpos() { return IsShotPos; }

	//�@�֏e���Z�b�g����
	//������:�X�s�[�h
	//������:�~�T�C���̃X�|�[��������W
	//��O����:�~�T�C���̏����p�x
	//��l����:�^�[�Q�b�g�̍��W
	void SetBallInf(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _spornrotate, CEnemy* _target);
};