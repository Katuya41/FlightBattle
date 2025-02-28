#pragma once
#include "CGameObject.h"
#include <list>
#include <deque>
#include <vector>

class CEnemy;
class CEffect;

/**
 * �����CMisile�N���X�̐����ł��B
* ���̃N���X�̓~�T�C������Ŏg�p����܂��B
 *
* @author �g������
* @since 2024-7-10
*/
class CMisile :
	public CGameObject
{
public:
	//�~�T�C���̏��
	enum MISILE_STATE
	{
		SHOTTO,		//���˒�
		READY,		//���ˉ\
		WAIT,		//���ˎ��ԑҋ@��
	};

private:
	//���˂��ꂽ����
	ULONGLONG ShotTime;

	//���݂̃~�T�C���̏��
	MISILE_STATE MisileState = READY;

	//�^�[�Q�b�g�̃I�u�W�F�N�g�̍��W
	DirectX::XMFLOAT3 TargetPosition = { 0,0,0 };

	//�^�[�Q�b�g�̕���
	DirectX::XMFLOAT3 fDirection;
	DirectX::XMVECTOR vDirection;

	//�^�[�Q�b�g���
	//����ɂ͓G�܂��̓v���C���[�̃A�h���X�����邽�߁A
	//CPlayer����CplayerAttack�ŊJ�����s��
	CGameObject* Target;

	//�����Ă���G�t�F�N�g���
	CEffect* pEffect;

	//�G�̓����蔻��
	DirectX::BoundingOrientedBox EnemyColOBB;
	DirectX::BoundingBox EnemyColAABB;

	//���ˉ\���ǂ���
	bool IsShotto = true;

	//�G��ǂ������邩
	bool IsMoveEnemy = false;

	//�������I�u�W�F�N�g���v���C���[��
	bool IsShotPlayer = false;

	//�p�[�e�B�N���̍\���̂ł�
	struct PARTICLE
	{
		std::unique_ptr<CGameObject> Object;	//���j�[�N�|�C���^�g�p
		float CoolTime;				//�N�[���^�C��
		ULONGLONG DeleteTime;			//��������
		ULONGLONG StartTime;			//�J�n����
	};

	//�p�[�e�B�N���̃I�u�W�F�N�g���i�[����ϐ�
	//vector��erase���s�����Ƀ������ړ����s��
	//�ړ����s��Ȃ�deque���g�p
	std::deque<PARTICLE> Particles;

	//�p�[�e�B�N���̔��莞��
	LONGLONG OldParticleTime;

	//�p�[�e�B�N���̃��f��
	D3D::Model ParticleModel;

	//�G�Ɍ������Ă�������
	void MoveUpdate();

	//��������
	bool IsEnd = false;

	//�^�[�Q�b�g�ɓ�����������UI��\�����邩
	bool IsDrawHitUI = false;

	//�^�[�Q�b�g�Ƃ̋���
	float TargetDistance = 0.0f;

	//��]�p�x
	float RotateAngle = 0.0f;

	//�����x
	float AddSpeed = 0.0f;

	/**
	* �p�[�e�B�N���𐶐�����֐��ł��B
	*/
	void InsertParticle();

public:
	//�R���X�g���N�^
	CMisile();
	~CMisile();

	/**
	*�X�V�������s���֐��ł��B
	*/
	void Update() override;

	/**
	*�`�揈���s���֐��ł��B
	*/
	void Draw() override;

	/**
	* �p�[�e�B�N���̍X�V����
	*/
	void ParticleUpdate();

	/**
	* �p�[�e�B�N���̕`�揈�����s���֐��ł��B
	*/
	void DrawParticle();

	/**
	*�^�[�Q�b�g�ݒ���s���֐��ł��B
	*
	* @param �^�[�Q�b�g�������I�u�W�F�N�g
	*/
	void SetTarget(CGameObject* _target) { Target = _target; }

	/**
	*�^�[�Q�b�g�ւ̕����̌v�Z���s���֐��ł��B
	*
	* @return ���̃I�u�W�F�N�g���猩���^�[�Q�b�g�I�u�W�F�N�g�ւ̕���
	*/
	DirectX::XMFLOAT3 SetDirect();

	/**
	*�~�T�C�����o��������W�̐ݒ���s���֐��ł��B
	*
	* @return �I�u�W�F�N�g���o��������W
	*/
	void SetSpornPos(DirectX::XMFLOAT3 _spornpos) { Position = _spornpos; }

	//�~�T�C���̏����p�x���Z�b�g����
	void SetSpornRotate(DirectX::XMFLOAT3 _spornrotate) { Rotation = _spornrotate; }

	//�~�T�C���̃q�b�g�e�N�X�`���ݒ�
	void SetHitMisileEffect(D3D::Model _model);

	void SetUv_Size(DirectX::XMFLOAT2 _uv);

	//�~�T�C���̏�Ԃ�Ԃ�
	MISILE_STATE GetMisileState() { return MisileState; }

	//�~�T�C���̏�Ԃ�ݒ肷��
	void SetMisileState(MISILE_STATE _state) { MisileState = _state; }

	//�~�T�C���̃p�[�e�B�N�����Ԑݒ�
	void SetMisileParticleTime(LONGLONG _time) { OldParticleTime = _time; }

	/**
	* �������I�u�W�F�N�g���v���C���[����ݒ肷��֐��ł��B
	*/
	void SetIsShotPlayer() { IsShotPlayer = true; }

	/**
	* �������邩��Ԃ�
	* 
	* @return �~�T�C���Ƃ��Ă̏������I������
	*/
	bool GetMisileEnd() { return IsEnd; }

	/**
	* �p�[�e�B�N���̃��f����ݒ肷��֐��ł��B
	* 
	* @param �ݒ肷��p�[�e�B�N���̃��f��
	*/
	void SetParticleModel(D3D::Model _model) { ParticleModel = _model; }

	/**
	* �p�[�e�B�N���z��̒��g���N���A����
	*/
	void ClearPareticles() { Particles.clear(); }

	/**
	* �G��ǂ������邩�̔����ݒ肷��֐��ł��B
	* 
	* @param �@�G��ǂ������邩
	*/
	void SetIsMoveEnemy(bool _ismoveenemy) { IsMoveEnemy = _ismoveenemy; }

	/**
	* �G��ǂ������邩�̔�����擾����֐��ł��B
	* 
	* @return  �G��ǂ������邩�̔���
	*/
	bool GetIsMoveTarget() { return IsMoveEnemy; }

	/**
	* �~�T�C�������˂��ꂽ���Ԃ�ݒ肷��
	*/
	void SetShotTime(ULONGLONG _time) { ShotTime = _time; }

	/**
	* �~�T�C����������������UI��\�����邩
	*/
	void SetIsDrawHitUI(bool _isdrawui) { IsDrawHitUI = _isdrawui; }

	/**
	* �^�[�Q�b�g�Ƃ̋�����Ԃ�
	* 
	* @return �^�[�Q�b�g�Ƃ̋���
	*/
	float GetTargetDistance() { return TargetDistance; }

	/**
	* �����x��Ԃ�
	*/
	float GetAddSpeed() { return AddSpeed; }
};