#pragma once
#include "CGameObject.h"

class CEnemy;
class CEffect;

/**
 * �����CShottoBall�N���X�̐����ł��B
 * ���̃N���X�͒e��������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CShottoBall :
    public CGameObject
{
public:
      //���˂��ꂽ����
    ULONGLONG ShotTime;

private:
	//�^�[�Q�b�g�̃I�u�W�F�N�g�̍��W
	DirectX::XMFLOAT3 TargetPosition = { 0,0,0 };

	//�^�[�Q�b�g�̕���
	DirectX::XMFLOAT3 fDirection;
	DirectX::XMVECTOR vDirection;

	//�^�[�Q�b�g���
	CEnemy* Target;

	//�����Ă���G�t�F�N�g���
	CEffect* pEffect;

	//�G�̓����蔻��
	DirectX::BoundingOrientedBox EnemyCol;

	//�ʂ����˂��ꂽ����
	LONGLONG ShottoTime = 0;

	float ShotSpeed = 0.0f;

public:
	CShottoBall();
	~CShottoBall();

	/**
	* �X�V�������s���֐��ł�
	*/
	void Update() override;

	/**
	* �`�揈�����s���֐��ł�
	*/
	void Draw() override;

	/**
	* �e�̐i�s������ݒ肷��֐��ł��B
	*/
	void SetDiviatDir();

	/**
	* �^�[�Q�b�g��ݒ肷��֐��ł�
	* 
	* @param  �ݒ肵�����^�[�Q�b�g
	*/
	void SetTarget(CEnemy* _target) { Target = _target; }

	/**
	* �^�[�Q�b�g�ւ̕������v�Z����֐��ł��B
	* 
	* @return �v�Z�����^�[�Q�b�g�̕���
	*/
	DirectX::XMFLOAT3 SetDirect();

	/**
	* �e�̃��f����ݒ肷��֐��ł��B
	* 
	* @param  �ݒ肵�����e�̃��f��
	*/
	void SetModel(D3D::Model _model);

	/**
	* ��ԑ��x��ݒ肷��֐��ł��B
	* 
	* @param  �ݒ肵�����ړ����x
	*/
	void SetSpeed(float _speed) { MoveSpeed = _speed; }

	/**
	* �^�[�Q�b�g�̍��W��ݒ肷��֐��ł��B
	* 
	* @param  �ݒ肵�����^�[�Q�b�g�̍��W
	*/
	void TargetPos(DirectX::XMFLOAT3 _targetpos) { TargetPosition = _targetpos; }

	/**
	* �e���o��������W��ݒ肷��֐��ł��B
	* 
	* @param  �e���o�������������W
	*/
	void SetSpornPos(DirectX::XMFLOAT3 _spornpos) { Position = _spornpos; }

	/**
	* �~�T�C���̏����p�x��ݒ肷��֐��ł��B
	* 
	* @param  �e���o���������Ƃ��̏����p�x
	*/
	void SetSpornRotate(DirectX::XMFLOAT3 _spornrotate) { Rotation = _spornrotate; }

	/**
	* ������]�s���ݒ肷��֐��ł��B
	*/
	void SetRotateMatrix();

	/**
	* �e�̃q�b�g�G�t�F�N�g���f����ݒ肷��֐��ł��B
	* 
	* @param  �ݒ肵�����e�̃q�b�g�G�t�F�N�g���f��
	*/
	void SetHitBallColBoxEffect(D3D::Model _model);

	/**
	* �e�����˂��ꂽ���Ԃ�ݒ肷��֐��ł��B
	* 
	* @param  �e�����˂��ꂽ����
	*/
	void SetShottoTime(LONGLONG _time) { ShottoTime = _time; }

	/**
	* �A�j���[�V������UV�T�C�Y��ݒ肷��֐��ł��B
	* 
	* @param  _u	�ݒ肵����UV��U
	* @param  _v	�ݒ肵����UV��V
	*/
	void SetAnimUVSize(float _u,float _v);

};

