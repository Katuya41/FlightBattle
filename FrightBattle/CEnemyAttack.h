#pragma once
#include "direct3d.h"
#include<list>
#include <vector>
#include <memory>

class CEffect;
class CGameObject;
class CPlayer;
class CEnemy;
class CShottoBall;
class CMisile;

/**
 * �����CEnemyAttack�N���X�̐����ł��B
 * ���̃N���X�͓G���U���̍ۂɂ���^�[�Q�b�g�ƒe�̐������s���܂��B
 *
 * @author �g������
 * @since 2024-10-28
 */
class CEnemyAttack
{
public:
	/**
	* �U���̏�Ԃ������Ă���񋓌^�ł��B
	* @param MISILE		�~�T�C��
	* @param GUN		�@�֏e
	* @param MAX		�ő吔
	*/
	enum ATTACK_ID
	{
		MISILE,
		GUN,
		MAX,
	};

private:
	//�����o�[�ϐ�
	D3D::Model MisileModel;		//�~�T�C���̃��f��
	D3D::Model MisileHitModel;	//�~�T�C���������������̃G�t�F�N�g���f��

	D3D::Model GunModel;		//�e�̃��f��
	D3D::Model GunHitModel;		//�e�������������̃G�t�F�N�g���f��

	D3D::Model MisileParticleModel;	//�~�T�C���̃p�[�e�B�N�����f��

	D3D::Model MiniMapMisileModel;	//�~�j�}�b�v�̃~�T�C�����f��

	//�e�ƃ~�T�C���̃G�t�F�N�g��UV�T�C�Y
	DirectX::XMFLOAT2 GunHitUVSize = { 0,0 };
	DirectX::XMFLOAT2 MisileHitUVSize = { 0,0 };

	//���݂̍U������
	ATTACK_ID AttackId = MISILE;

	//�e���i�[����z��
	//std::deque<CShottoBall*> vShottoBall;
	std::vector<CShottoBall*> vShottoBall;

	//�~�T�C�����i�[����z��
	//std::deque<CMisile*> vMisile;
	//std::vector<CMisile*> Misiles;
	std::vector<std::unique_ptr<CMisile>> Misiles;

	//�v���C���[�̃��b�N�I��UI��\�����邩
	bool IsDrawPlayerRock = false;

	//�v���C���[�Ƃ̋������w�苗���ȓ���
	bool IsClosePlayerDis = false;

	CEnemyAttack();
	~CEnemyAttack();

	/**
	* �~�T�C���̍X�V�������s���֐��ł��B
	*/
	void MisileUpdate();

	/**
	* �@�֏e�̍X�V�������s���֐��ł��B
	*/
	void GunUpdate();

	/**
	* �^�[�Q�b�g�̍X�V�������s���֐��ł��B
	*/
	void TargetUpdate();

	/**
	* �~�T�C���̕`�揈��
	*/
	void MisileDraw();

	/**
	* �~�T�C���̃p�[�e�B�N���`��
	*/
	void MisileParticleDraw();

	/**
	* �e�̕`�揈��
	*/
	void GunDraw();

	/**
	*�~�T�C���̔��ˏ������s���֐��ł��B
	*
	* @param _sppornobj		�X�|�[������ۂ̊�̃I�u�W�F�N�g
	* @param _target		�^�[�Q�b�g�̃I�u�W�F�N�g
	* @param _moveenemy		�G��ǂ������邩
	*/
	CMisile* AttackMisile(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation ,CPlayer* _target, bool _moveenemy);

	/**
	* �@�֏e�̔��ˏ������s������
	* 
	* @param _sppornobj		�X�|�[������ۂ̊�̃I�u�W�F�N�g
	*/
	void AttackShotBall(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation);

public:

	static CEnemyAttack* Get();

	/**
	* �X�V�������s���֐��ł��B
	*/
	void Update();

	/**
	* �`�揈�����s���֐��ł��B
	*/
	void Draw();

	/**
	* �~�j�}�b�v�̃~�T�C����`�悷��֐��ł��B
	*/
	void MiniMapMisileDraw();

	/**
	* �v���C���[�̍U���������s���֐��ł��B
	*/
	CMisile* AttackTarget(ATTACK_ID _attackid , DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation);

	/**
	*�~�T�C���̃��f����UV�T�C�Y��ݒ肷��֐��ł��B
	*
	* @param _misilemodel		�ݒ肵�����~�T�C���̃��f��
	* @param _misilhitemodel	�ݒ肵�����~�T�C���̃q�b�g�G�t�F�N�g���f��
	* @param _minimapmisilemodel	�ݒ肵�����~�j�}�b�v���f��
	* @param _misilehituvsize	�ݒ肵�����~�T�C���̃G�t�F�N�gUV�T�C�Y
	*/
	void SetMisileInf(D3D::Model _misilemodel, D3D::Model _misilhitemodel, D3D::Model _minimapmisilemodel, DirectX::XMFLOAT2 _misilehituvsize);

	/**
	*�@�֏e�̃��f����UV�T�C�Y��ݒ肷��֐��ł��B
	*
	* @param _misilemodel			�ݒ肵�����~�T�C���̃��f��
	* @param _misilhitemodel		�ݒ肵�����~�T�C���̃q�b�g�G�t�F�N�g���f��
	*/
	void SetGunInf(D3D::Model _gunmodel, D3D::Model _gunhitmodel, DirectX::XMFLOAT2 _ballhituvsize);

	/**
	* �~�T�C���̃p�[�e�B�N����ݒ肷��֐��ł��B
	*
	* @param �~�T�C���̃p�[�e�B�N�����f��
	*/
	void SetMisileParticleModel(D3D::Model _model) { MisileParticleModel = _model; }

	/**
	* �e�̏����擾����֐��ł��B
	*/
	void GetShotBalls(std::list<CShottoBall*>& _shotballs);
};