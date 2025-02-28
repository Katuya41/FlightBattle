#pragma once
#include <vector>
#include <list>
#include <deque>
#include <unordered_map>
#include <memory>
#include "direct3d.h"

class CEffect;
class CGameObject;
class CPlayer;
class CEnemy;
class CShottoBall;
class CMisile;
class CMisileUI;

/**
 * �����CPlayerAttack�N���X�̐����ł��B
 * ���̃N���X�̓v���C���[���U���̍ۂɂ���^�[�Q�b�g�ƒe�̐������s���܂��B
 *
 * @author �g������
 * @since 2024-10-23
 */
class CPlayerAttack
{
public:
	/**
	* �U���̏�Ԃ������Ă���񋓌^�ł��B
	* @param MISILE		�~�T�C��
	* @param GUN		�@�֏e
	* @param FOURMISILE	4�A�~�T�C��
	* @param MAX		�ő吔
	*/
	enum ATTACK_ID
	{
		MISILE,
		ONETARGET_FOURMISILE,
		FOURTARGET_FOURMISILE,
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

	//�����I�p�ϐ�
	CEnemy* pInitEnemy;

	//�e�ƃ~�T�C���̃G�t�F�N�g��UV�T�C�Y
	DirectX::XMFLOAT2 GunHitUVSize = { 0,0 };
	DirectX::XMFLOAT2 MisileHitUVSize = { 0,0 };

	//���݂̍U������
	ATTACK_ID AttackId = MISILE;

	//�ˌ��|�C���g
	DirectX::XMFLOAT3 ShotPos = { 0.0f,0.0f,0.0f };

	//�ˌ��|�C���g��\�����邩
	bool IsShotPos = false;

	//�U���L�[�������ꂽ�܂܂̏�Ԃ�
	bool IsPressAttackKey = false;

	//�^�[�Q�b�g���@�̐؂�ւ�(true:�ʏ�̃^�[�Q�b�g false:��ʂɉf���Ă���G�S�Ă��^�[�Q�b�g����)
	bool IsOneTarget = true;

	//�G�ϐ��p�u���C�N���邩
	bool IsBreak = false;

	//�������p�^�[�Q�b�g��ݒ肵����
	bool IsSetTarget = false;

	//�쐬�����^�[�Q�b�gUI��ID
	int EnemyFrame = 0;

	bool CreateEnemyFrame = false;

	//���b�N�I��SE��炵�Ă��邩
	bool IsPlayRockOnSE = false;

	//�~�T�C�����i�[����z��
	std::vector<std::unique_ptr<CMisile>> vuMisile;

	//�D��x�ɂ���
	// �^�[�Q�b�g�@�\�́A�D��x���������̂�����ɓ���Ă���
	// 0:�D��x����(�����l)
	// 1:�J�����ɉf���Ă��Ȃ��A�^�[�Q�b�g�\�����ȏ�
	// 2:�J�����ɉf���Ă��Ȃ����A�^�[�Q�b�g���������ȓ�
	// 3:�J�����ɉf���Ă��邩�A�^�[�Q�b�g�����ȏ�
	// 4:�J�����ɉf���Ă��邩�A�^�[�Q�b�g�����ȓ�
	// 5:���S�J�����Ɏʂ��Ă��邩�^�[�Q�b�g�����ȓ�
	// 5:�v���C���[�Ƃ̋�������������߂�
	//
	/**
	* �G��������ۂɎg�p����\����
	*
	* @param Enemy		�G
	* @param FastPoint	�D��x(0:�Ȃ�,1 < 2 < 3 < 4 < 5 : 5����ԗD��x������)
	*/
	struct RESENEMY
	{
		CEnemy* Enemy = nullptr;
		int FastPoint = 0;
	};
	//�G���i�[����ϐ�
	//std::vector<std::unique_ptr<RESENEMY>> vEnemys;
	std::vector<RESENEMY*> vEnemys;

	//�G�̍ō��X�s�[�h
	float EnemyMaxSpeed = 0.4f;
	float EnemyTargetMaxSpeed = 0.2f;

	//�G�̍ő��]�\�p�x
	float EnemysMaxRotation = 1.5f;

	/**
	* �^�[�Q�b�g�̏������ꂽ�\����
	*/
	std::vector<CEnemy*> lTargetEnemys;

	//�^�[�Q�b�g��
	int TargetNum = 0;

	//�{�X��芪���G�p���G��
	int EntourageDefenceNum = 0;

	//��芪���G�̍U�����G��
	int EntourageAttackNum = 0;

	//�@�֏e�G�̑S�G�ړ��|�C���g���B����
	bool IsGunEnemysPass = false;

	//�@�֏e�G�̈ړ���ۑ��z��
	std::deque<DirectX::XMFLOAT3> ResArray;

	/**
	* �G�p�����[�^������\���̂ł��B
	*/
	struct ENEMY_PARAMETER
	{
		float ThresHold;            //���B���鋖�e�͈�
		float MaxSpeed;				//�ō����x
		float Fright_MaxSpeed;		//��s�G�̍ō����x
		float Follow_MaxSpeed;		//�Ǐ]�G�̍ō����x
		float Entourage_MaxSpeed;	//��芪���G�̍ō����x
		float Boss_MaxSpeed;		//�{�X�G�̍ō����x
		float AddSpeed;             //�������x
		float Fright_AddSpeed;		//��s�G�̉������x
		float Follow_AddSpeed;		//�Ǐ]�G�̉������x
		float Entourage_AddSpeed;   //��芪���G�̉������x
		float Boss_AddSpeed;        //�{�X�G�̉������x
		float Fright_MaxRotate;		//��s�G�̍ō���]�p�x
		float Follow_MaxRotate;		//��s�G�̍ō���]�p�x
		float Entourage_MaxRotate;	//��s�G�̍ō���]�p�x
		float Boss_MaxRotate;		//��s�G�̍ō���]�p�x
		int FrightEnemy_HP;         //��ԓG��HP
		int GroundEnemy_HP;         //�n�ʓG��HP
		int EntourageEnemy_HP;		//��芪���G��HP
		int BossEnemy_HP;			//�{�X�I��HP
		float GroundEnemy_AttackTime;   //�n�ʓG�̍U���Ԋu
		float Max_Attack_Distance;  //�ő�U���\����
		float Target_Time;          //�^�[�Q�b�g��������
		float Attack_CoolTime;      //�U���Ԋu
		int InsertEntourageNum;     //��芪���̏�����
		float Entourage_ChengePos_Time;    //��芪���̍��W�ύX�Ԋu
		float Entourage_Attack_CoolTime;   //��芪���̍U���Ԋu
		float Entourage_Move_Pos_Dis;	   //��芪���̈ړ��Ԋu
	};
	ENEMY_PARAMETER EnemyParameter;

	enum ENEMY_PARAMS_TAG
	{
		HOLD,
		ENEMYMAXSPEED,
		FRIGHT_MAX_SPEED,
		FOLLOW_MAX_SPEED,
		ENTOURAGE_MAX_SPEED,
		BOSS_MAX_SPEED,
		ADDSPEED,
		FRIGHTADDSPEED,
		FOLLOWADDSPEED,
		ENTOURAGEADDSPEED,
		BOSSADDSPEED,
		FRIGHTMAXROTATE,
		FOLLOWMAXROTATE,
		ENTOURAGEMAXROTATE,
		BOSSMAXROTATE,
		FRIGHTENEMYHP,
		GROUNDENEMYHP,
		ENTOURAGEENEMYHO,
		BOSSENEMYHP,
		GROUNDENEMY_ATTACKTIME,
		MAXATTACKDISTANCE,
		TARGETTIME,
		ATTACKCOOLTIME,
		INSERTENtoURAGENUM,
		ENTOURAGECHANGEPOSTIME,
		ENTOURAGEATTACKCOOLTIME,
		ENTOURAGEMOVEPOSDIS,
		ENEMYMAX,
	};
	ENEMY_PARAMS_TAG EnemyParamTag = HOLD;

	/**
	* �~�T�C���p�����[�^������\���̂ł��B
	*/
	struct MISILE_PARAMETER
	{
		float MaxRotate;			//�v���C���[�~�T�C���̍Œ�p�x
		float MinRotate;			//�v���C���[�~�T�C���̍ō��p�x
		float Enemy_Min_Rotate;		//�G�~�T�C���̍Œ�p�x
		float Enemy_Max_Rotate;		//�G�~�T�C���̍ō��p�x
		float Min_Speed;			//�~�T�C���̒����x
		float Max_Speed;			//�ō����x
		float Min_Add_Speed;		//��l�ȉ����x���̉����x
		float Max_Add_Speed;		//��l�ȏ㑬�x���̉����x
		float Enemy_Min_Add_Speed;	//�G��l�ȉ����x���̉����x
		float Enemy_Max_Add_Speed;	//�G��l�ȏ㑬�x���̉����x
		int	  Delete_Misile_Time;	//�~�T�C���̐�������
		float Particle_Cooltime;	//�p�[�e�B�N���̏o���Ԋu
		float Particle_OutTime;		//�p�[�e�B�N���̏����鑬�x
		float Particle_AddScale;	//�p�[�e�B�N���̑傫���Ȃ鑬�x
		float Misile_Charge_Speed;	//�~�T�C���̃`���[�W���x
	};
	MISILE_PARAMETER MisileParameter;

	enum MISILE_PARAMS_TAG
	{
		MAXROTATE,
		MINROTATE,
		ENEMYMINROTATE,
		ENEMYMAXROTATE,
		MINSPEED,
		MISILEMAXSPEED,
		MINADDSPEED,
		MAXADDSPEED,
		ENEMYMINADDSPEED,
		ENEMYMAXADDSPEED,
		DELETEMISILETIME,
		PARTICLECOOLTIME,
		PARTICLEOUTTIME,
		PARTICLEADDSCALE,
		MISILECHARGESPEED,
		MISILEMAX,
	};
	MISILE_PARAMS_TAG MisileParamTag = MAXROTATE;

	/**
	* �G�p�����[�^�[���t�@�C������擾����֐��ł��B
	*/
	void InsertEnemyParameter();

	/**
	* �~�T�C���p�����[�^�[���t�@�C������擾����֐��ł��B
	*/
	void InsertMisileParameter();

	//CEnemy* TargetEnemy;

	CPlayerAttack();
	~CPlayerAttack();

	/**
	* �G�̃��C���������֐��ł��B
	*/
	void DrawTargetLine();

	/**
	* �~�T�C���̍X�V�������s���֐��ł��B
	*/
	void MisileUpdate();

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
	*�~�T�C���̔��ˏ������s���֐��ł��B
	*
	* @param _sppornobj		�X�|�[������ۂ̊�̃I�u�W�F�N�g
	* @param _target		�^�[�Q�b�g�̃I�u�W�F�N�g
	* @param _moveenemy		�G��ǂ������邩
	*/
	void AttackMisile(CGameObject* _spornobj, CEnemy* _target,bool _moveenemy);

	/**
	*4�̃~�T�C���̔��ˏ������s���֐��ł��B
	*
	* @param _sppornobj		�X�|�[������ۂ̊�̃I�u�W�F�N�g
	* @param _target		�^�[�Q�b�g�̃I�u�W�F�N�g
	* @param _misilenum		�~�T�C���̏���
	* @param _moveenemy		�G��ǂ������邩
	*/
	void AttackFourMisile(CGameObject* _spornobj, CEnemy* _target, int _misilenum, bool _moveenemy,bool _checkmisileui);

public:
	static CPlayerAttack* Get();

	//�p�����[�^//
	enum GUNENEMYSTARTPOS
	{
		VerTex1,
		VerTex2,
		VerTex3,
		VerTex4,
		VerTex5,
		MAXVerTex,
		NoneVerTex,
	};
	GUNENEMYSTARTPOS GunENemyStartPos = VerTex1;

	GUNENEMYSTARTPOS GetGunENemyStartPos();
	void AddGunEnemyStartPos();

	/**
	* �X�V�������s���֐��ł��B
	*/
	void Update();

	/**
	* �`�揈�����s���֐��ł��B
	*/
	void Draw();

	/**
	* �G�̍X�V�������s���֐��ł��B
	*/
	void EnemyUpdates();

	/**
	* �G�̕`�揈�����s���֐��ł��B
	*/
	void EnemysDraw();

	/**
	* �~�j�}�b�v�̓G��`�悷��֐��ł�
	*/
	void MiniMapEnemyDraw();

	/**
	* �~�j�}�b�v�̃~�T�C����`�悷��֐��ł��B
	*/
	void MiniMapMisileDraw();

	/**
	* �v���C���[�̍U���������s���֐��ł��B
	*/
	void AttackTarget(CGameObject* _spornobj);

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
	* �~�T�C���̃p�[�e�B�N����ݒ肷��֐��ł��B
	* 
	* @param �~�T�C���̃p�[�e�B�N�����f��
	*/
	void SetMisileParticleModel(D3D::Model _model) { MisileParticleModel = _model; }

	/**
	*�@�֏e�̋ʂ̃��f����UV�T�C�Y��ݒ肷��֐��ł��B
	*
	* @param _misilemodel		�ݒ肵�����~�T�C���̃��f��
	* @param _misilhitemodel	�ݒ肵�����~�T�C���̃q�b�g�G�t�F�N�g���f��
	* @param _misilehituvsize	�ݒ肵�����~�T�C���̃G�t�F�N�gUV�T�C�Y
	*/
	void SetBallInf(D3D::Model _ballmodel, D3D::Model _ballhitemodel, DirectX::XMFLOAT2 _ballhituvsize);

	/**
	* �G�̏���CGame�N���X����擾����֐��ł��B
	*/
	void InsertEnemys(CEnemy* _enemy);

	/**
	* �G���i�[����Ă���z����N���A����
	*/
	void ClearEnemys();

	/**
	* �^�[�Q�b�g�����X�V����
	* 
	* @param �^�[�Q�b�g���Ԃ����Z�b�g���邩
	*/
	void SetTarget(bool _resettime);

	/**
	* �����ɍ����G���^�[�Q�b�g���Ɋi�[����֐��ł�
	*/
	void InsertResEnemys();
	void InsertResEnemys(std::vector<RESENEMY*>& _enemys);
	//void InsertResEnemys(std::vector<std::unique_ptr<RESENEMY>>& _enemys);

	/**
	* �^�[�Q�b�g��Ԃ��܂��B
	* 
	* @return �^�[�Q�b�g�I�u�W�F�N�g
	*/
	CEnemy* GetTargetObj() {
		if (lTargetEnemys.empty())
		{
			return pInitEnemy;
		}
		else
		{
			return (*lTargetEnemys.begin());
		}

		//if (lTargetEnemys.empty()) { return pInitEnemy; }
		//else { return lTargetEnemys.begin()->Enemy;}
	}

	/**
	* ���ׂĂ̔z�������������֐��ł�
	*/
	void ResetPlayerAttack();

	/**
	* �i�[����Ă���G�̐����擾����֐��ł��B
	* 
	* @return �i�[����Ă���G�̐�
	*/
	size_t GetEnemyNum() { return vEnemys.size(); }

	/**
	* �U������̎�ޕύX
	*/
	void ChengeAttackweapon();

	/**
	* �U���L�[�������ꂽ�܂܂̏�Ԃ����擾����֐��ł��B
	* 
	* @param �U���L�[�������ꂽ�܂܂��̔���
	*/
	void SetIsPressAttackKey(bool _ispressattackkey) { IsPressAttackKey = _ispressattackkey; }

	/**
	* �~�T�C���p�[�e�B�N���z��̒��g���N���A�ɂ���
	*/
	void ClearMisileParticles();

	/**
	* ���݂̕����̍U���\������Ԃ�
	* 
	* @return ���݂̕����̍U���\����
	*/
	float GetWeaponMaxDistance();

	/**
	* ���݂̕����̎�ނ�Ԃ�
	* 
	* @return  ���݂̕����̎��
	*/
	ATTACK_ID GetAttackId() { return AttackId; }

	/**
	* �^�[�Q�b�g�\������̂�������Ԃ�
	*/
	bool GetIsOnetarget() { return IsOneTarget; }

	/**
	* �{�X�I�u�W�F�N�g��Ԃ��֐��ł��B
	*/
	CEnemy* GetBossEnemy();

	/**
	* ��芪���G�̍U�����̐��ݒ�
	*/
	void SetEntourageAddNum(int _addnum) { EntourageAttackNum += _addnum; }

	/**
	* ��芪���I�u�W�F�N�g�̍U���w�����o����Ă���G���m�F
	*/
	int GetEntourageAttackNum() { return EntourageAttackNum; }

	/**
	* ��芪���G�̖h�q�̐�
	*/
	void SetEntourageDefAddNum(int _addnum) { EntourageDefenceNum += _addnum; }

	/**
	* ��芪���G�̖h�q��Ԃ̐���Ԃ�
	* 
	* @return �h�q��Ԃ̎�芪���̐�
	*/
	int GetEntourageDefNum() { return EntourageDefenceNum; }

	/**
	* ��芪���I�u�W�F�N�g�̍U���w��
	*/
	void SetAttackEntourageEnemy();

	/**
	* ��芪���I�u�W�F�N�g�̖h�q�w���ݒ�֐��ł��B
	*/
	void SetDefenceEntourageEnemy(bool _isdefence);

	/**
	* �G�ϐ��Ńu���C�N����
	*/
	void SetTrueBreak() { IsBreak = true; }

	/**
	* �G�̍ő��]�p�x���擾����֐��ł��B
	*
	*  @return �G�̍ő��]�p�x
	*/
	float& GetEnemyMaxRotation() { return EnemysMaxRotation; }

	/**
	* �@�֏e�G�̍\���̂�Ԃ��֐��ł��B
	*/
	void GetGunEnemyFlag(std::deque<DirectX::XMFLOAT3>& _qgunflagarray);

	/**
	* �@�֏e�G�̍\���̂̒��g�����炷
	*/
	void ShiftGunEnemyArray();

	/**
	* �@�֏e�G�S�Ă��ړ��|�C���g�ɓ��B��������Ԃ�
	*/
	bool GetGunEnemysPass() { return IsGunEnemysPass; }

	/**
	* �@�֏e�̓G���S�Ĉړ��������Ă��邩
	*/
	void CheckGunEnemysPass();

	/**
	* �~�T�C���̃p�����[�^�̍\���̂�n���֐��ł��B
	* 
	* @return �~�T�C���̃p�����[�^�[�\����
	*/
	MISILE_PARAMETER& GetMisileParameter() { return MisileParameter; }

	/**
	* �G�̃p�����[�^�̍\���̂�n���֐��ł��B
	*
	* @return �G�̃p�����[�^�[�\����
	*/
	ENEMY_PARAMETER& GetEnemyParameter() { return EnemyParameter; }

	/**
	* �G�̂��ׂĂ̈ړ����ύX����(�f�o�b�O�p)
	*/
	void ChengeEnemysMovePos();

	/**
	* �G�̏���z��ɓ���ĕԂ��֐�(�f�o�b�O�p)
	* 
	* @param  �G�̏�������z��
	*/
	void GetEnemys(std::list<CEnemy*>& _enemys);

	/**
	* �^�[�Q�b�g�������ĕԂ��֐�(�f�o�b�O�p)
	* 
	* @param  �^�[�Q�b�g�̏�������z��
	*/
	void GetTargets(std::list<CEnemy*>& _targets);

	/**
	* �p�����[�^���Z�[�u����֐��ł��B
	*/
	void SaveEnemyParameter();

	/**
	* �p�����[�^���Z�[�u����֐��ł��B
	*/
	void SaveMisileParameter();
};

