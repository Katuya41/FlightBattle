#pragma once
#include "CGameObject.h"
#include "CPlayerAttack.h"

class CMisile;
//class CPlayerAttack;

/**
 * �����CEnemy�N���X�̐����ł��B
 * ���̃N���X�͓G�̐���Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CEnemy :
    public CGameObject
{

private:
    //���L�ϐ�

    //�W�F�b�g�G�t�F�N�g
    CEffect* eJetEffect;

    //�^�[�Q�b�g����Ă���Ƃ��̃~�j�}�b�v�̃��f��
    D3D::Model TargetMiniMapModel;
    //�^�[�Q�b�g����Ă��Ȃ��~�j�}�b�v���f���̔��p�ϐ�
    D3D::Model NottargetMiniMapModel;
    //�^�[�Q�b�g�̃I�u�W�F�N�g�̍��W
    DirectX::XMFLOAT3 TargetPosition = { 0,0,0 };
    //�^�[�Q�b�g�̕����x�N�g��
    DirectX::XMVECTOR TargetDir;
    //�U���ł��邩
    bool CanAttack = true;
    //�^�[�Q�b�g���ǂ���
    bool IsTarget = false;
    //�v���C���[���^�[�Q�b�g�ł��Ă��邩
    bool IsAttackTarget = false;
    //�����ێ��p�ϐ�
    int OldRndNum = NULL;
    //�v���C���[�Ƃ̋���
    double PlayerDistance;
    //�v���C���[�̐��ʃx�N�g���Ƃ̊p�x
    float ToPlayerRadius = 0.0f;
    //�n�_�ړ��G�̈ړ��|�C���g������ϐ�
    DirectX::XMFLOAT3 MovePos[8];

    //��芪���G�p�ϐ�//

    //�U���w�����I�������
    bool EndChaseAndAttack = false;
    //�ŏ��Ɉʒu�������s������
    bool IsInitChengeBoidPos = false;
    //�{�X�����Ɍ���������
    bool IsDefenceMove = false;
    //��芪���̂��Ă�����̍��W
    DirectX::XMFLOAT3 FollowPos = { 0.0f,0.0f,0.0f };
    //�{�X��芪���p���W�ϐ�
    DirectX::XMFLOAT3 EntouragePos = { 0.0f,0.0f,0.0f };

    //�{�X�p�G���G�����t���O
    bool InsertEntourage = false;
    //��芪���U���t���O
    bool IsAttackEntourage = false;
    //�f�o�b�O�p�U���t���O
    bool DebugIsAttack = true;
    //�{�X�p�ϐ�
    //�p�j��Ԃ̎�芪���̈ړ����W������z��
    DirectX::XMFLOAT3 EntourageMovePos[8];

    //�@�֏e�G//
    //���̓G�̋�̓I�ȍs��
    //�z��ɉ��L�̍\���̂����A�ŏ����珇�Ԃɐݒ肵�Ă���

    /**
    * �ړ���̍��W�ƒʉ߂������̍\����
    */
    struct GUNENEMY_MOVEFLAG
    {
        DirectX::XMFLOAT3 MovePos = {};  //�ړ���̍��W
        bool EndRotate = false;          //�ړ���ւ̉�]�I��
        bool EndForwardRotate = false;   //���s��]�̏I��
        bool IsPassPos = false;          //���W��ʉ߂�����
        bool IsInsert = false;           //���̍��W��ݒ肵����
        ULONGLONG PassTime = 0;          //�ړ���ɓ��B��������
        ULONGLONG WaitTime = 5000;       //�ړ���̒�~����
    };
    std::deque<GUNENEMY_MOVEFLAG> qMoveArray;
    std::vector<DirectX::XMFLOAT3> qResArray;
    //���݂̍\����
    GUNENEMY_MOVEFLAG NowFlag;

    //�~�T�C���p�ϐ�//

    //�~�T�C�����ˋ���
    ULONGLONG ShotTime = 0;
    //�^�[�Q�b�g�J�n����
    ULONGLONG TargetStartTime = 0;
    //���˂����~�T�C��������ϐ�
    CMisile* pMisile;
    //���Ԍv���p�ϐ�
    ULONGLONG Times = 0;

    // �@�֏e�p�ϐ� //
    ULONGLONG GunShotTime = 0;

public:
    /**
    * ����ԓG�̏�Ԃ������񋓌^�ł��B
    */
    enum ENEMY_STATE
    {
        MOVE,           //�w��|�C���g�𓮂����
        MOVE_FORWARD,   //�܂�������ё�����
        ISTARGET,       //�v���C���[�Ƀ^�[�Q�b�g����Ă���
        CHASE,          //�v���C���[��ǂ�������
        ATTACK,         //�v���C���[���U������
        RUN,            //�v���C���[���U�����Ă����Ƃ��ɓ�����
        ENTOURAGE_FOLLOW,   //��芪���{�X�Ǐ]
        ENTOURAGE_FRIGHT,   //��芪���̜p�j
        ENTOURAGE_DEFENCE,  //��芪���{�X�h��
        ENTOURAGE_ATTACK,   //��芪���v���C���[�U��
        GUN_ENEMT,          //�@�֏e�G
    };

    CPlayerAttack::GUNENEMYSTARTPOS GunENemyStartPos = CPlayerAttack::GUNENEMYSTARTPOS::NoneVerTex;

    //�D��x
    int Fastpoint = 0;

    //�^�[�Q�b�g�����܂ł̎��ԕϐ�
    LONGLONG TargetTime = 0;
    //�^�[�Q�b�g�ɂ��ꂽ����
    LONGLONG OldTargetTime = 0;

private:
    //���
    ENEMY_STATE EnemyState = MOVE;

    /**
    * ��������
    * 
    * @param   �Œ�͈�
    * @param   �ő�͈�
    * @param   �擾��������
    */
    void GetRandomNum(float _maxnum , float _minnum ,float& _getnum);

    /**
    * �J�����ɉf���Ă��邩���m���߂�֐��ł��B
    */
    void CheckRefCamera();

    /**
    * ���C���J�����̒��S�t�߂ɉf���Ă��邩���m���߂�֐��ł��B
    */
    void CheckRefCameraCenter();

    /**
    * ����ԓG��MOVE�X�V�֐��ł�
    */
    void StateMoveUpdate(float _maxrotate);

    /**
    * ����ԓG��MOVE_FORWARD�X�V�֐��ł��B
    */
    void StateMoveForwardUpdate(float _maxrotate);

    /**
    * ����ԓG��ISTARGET�X�V�֐��ł�
    */
    void StateIsTargetUpdate();

    /**
    * ����ԓG��ATTACK�X�V�֐��ł�
    */
    bool StateAttackUpdate();

    /**
    * ����ԓG��RUN�X�V�֐��ł�
    */
    void StateRunUpdate();

    /**
    * ��芪���G��ENTOURAGE_FOLLOW�֐��ł��B
    */
    void StateFollowEntourageUpdate(float _maxspeed);

    /**
    * ��芪���G��ENTOURAGE_DEFENCE�֐��ł��B
    */
    void StateDefenceEntourageUpdate();

    /**
    * ��芪���G��ENTOURAGE_ATTACK�֐��ł��B
    */
    void StateAttackEntourageUpdate();

public:
    /**
    *CEnemy�̃R���X�g���N�^�ł��B
    */
    CEnemy();

    /**
    *CEnemy�̃f�X�g���N�^�ł��B
    */
    ~CEnemy();

    /**
    *�G���̏��������s���֐��ł��B
    */
    void Init();

    /**
    *�X�V�������s���֐��ł��B
    */
    void Update() override;

    /**
    * �����ł���G�̍X�V����
    */
    void UpdateFrightEnemy();

    /**
    * �n��̓G�̍X�V����
    */
    void UpdateGroundEnemy();

    /**
    * �v���C���[�Ǐ]�̓G�̍X�V����
    */
    void UpdateFollowEnemy();

    /**
    * �{�X�̎�芪���G�̍X�V����
    */
    void UpdateEntourageEnemy();

    /**
    * �{�X�̍X�V����
    */
    void UpdateBossEnemy();

    /**
    * �@�֏e�G�̍X�V����
    */
    void UpdateGunEnemy();

    /**
    *�`�揈�����s���֐��ł��B
    */
    void Draw() override;

    /**
    *�^�[�Q�b�g���ǂ�����ݒ肷��֐��ł��B
    * 
    * @param �^�[�Q�b�g���ǂ���
    */
    void SetIsTarget(bool _istarget) { IsTarget = _istarget; }

    /**
    *�U���ł��邩��ݒ肷��֐��ł��B
    *
    * @param �^�[�Q�b�g�ɂł��邩
    */
    void SetCanAttack(bool _cantarget) { CanAttack = _cantarget; }

    /**
    *�^�[�Q�b�g�ɍU���ł��邩���擾����֐��ł��B
    *
    * @return �^�[�Q�b�g�ɍU���ł��邩
    */
    bool GetCanAttack() { return CanAttack; }

    /**
    * �G���^�[�Q�b�g���Ă��邩��Ԃ��֐��ł��B
    */
    bool GetIsAttackTarget() { return IsAttackTarget; }

    /**
    *�^�[�Q�b�g����Ă��邩�Ԃ����擾����֐��ł��B
    *
    * @return �^�[�Q�b�g����Ă��邩
    */
    bool GetIsTarget() { return IsTarget; }

    /**
    *�^�[�Q�b�g�̍��W��ݒ肷��֐��ł��B
    *
    * @return �^�[�Q�b�g�̍��W
    */
    void TargetPos(DirectX::XMFLOAT3 _targetpos) { TargetPosition = _targetpos; }

    /**
    *�ړ���̍��W��ݒ肷��֐��ł��B
    *
    * @param  �ړ��|�C���g
    * @param  �ԍ�
    */
    void SetMovePos(const DirectX::XMFLOAT3& _movepos, const int& _num) { MovePos[_num] = _movepos; }

    /**
    * ��芪���̈ړ���̍��W��ݒ肷��֐��ł��B
    *
    * @param  �ړ��|�C���g
    * @param  �ԍ�
    */
    void SetEntourageMovePos(const DirectX::XMFLOAT3& _movepos, const int& _num) { EntourageMovePos[_num] = _movepos; }

    /**
    * ��芪���̈ړ�����W�̔z����擾����֐��ł��B
    * 
    * @param  �擾������W��ID
    */
    DirectX::XMFLOAT3& GetEntourageMovePos(int _id) { return EntourageMovePos[_id]; }

    /**
    *�ړ���̕������v�Z����֐��ł��B
    *
    * @param �ړ���̍��W
    */
    void SetDirPos(DirectX::XMFLOAT3 _targetpos);

    /**
    *�ړ���̍��W�ɓ��B���������m�F����֐��ł��B
    *
    * @param  _targetpos  �ړ���̍��W
    * @param  _allowdis   ���e���鋗��
    * @return �ړ���̍��W�ɓ��B������
    */
    bool CheckMovePos(DirectX::XMFLOAT3 _targetpos,float _allowdis);

    /**
    *�v���C���[�Ƃ̋�����Ԃ��֐��ł��B
    *
    * @return �v���C���[�Ƃ̋���
    */
    double GetPlayerDistance() { return PlayerDistance; }

    /**
    * �^�[�Q�b�g����Ă���Ƃ��̃~�j�}�b�v�̃��f����ݒ肷��֐��ł��B
    * 
    * @param  �^�[�Q�b�g����Ă���Ƃ��̃~�j�}�b�v�̃��f��
    */
    void SetTargetMiniMapModel(D3D::Model _model) { TargetMiniMapModel = _model; }

    /**
    * �~�j�}�b�v�p�I�u�W�F�N�g�̃��f����ύX����֐��ł��B
    * 
    * @param  �^�[�Q�b�g����Ă��邩
    */
    void SetTargetMiniMapObjectModel(bool _istarget);

    /**
    * �~�T�C���̔��ˎ��Ԃ�ݒ肷��
    */
    void SetMisileShotTime(ULONGLONG _time) { ShotTime = _time; }

    /**
    * �V�����ړ�����擾
    */
    void SetNewMovePosition();

    /**
    * ���݂̏�Ԃ��擾����֐��ł��B
    */
    ENEMY_STATE& GetEnemyState() { return EnemyState; }

    /**
    * ���݂̏�Ԃ�ݒ肷��֐��ł��B
    */
    void SetEnemyState(ENEMY_STATE _newstate);

    /**
    * ��芪���G�̈ʒu�𒲐�������֐��ł��B
    */
    void SetNewEntouragePos() { IsInitChengeBoidPos = false; }

    /**
    * �^�[�Q�b�g�^�C����Ԃ�
    */
    ULONGLONG& GetTargetTime() { return TargetStartTime; }

    /**
    * ���ԗp�ϐ��Ɏ��Ԃ�ݒ肷��
    */
    void SetTimes(ULONGLONG _time) { Times = _time; }

    /**
    * �U�����s���G�ɍU���x����ݒ�
    */
    void SetIsEndChaseAttack() { EndChaseAndAttack = false; }

    /**
    * �U���x�����łĂ��邩�Ԃ�
    */
    bool GetIsEndChaseAttack() { return EndChaseAndAttack; }

    /**
    * �v���C���[�̐��ʃx�N�g���ƃv���C���[����G�ւ̃x�N�g���̊p�x��Ԃ��֐��ł��B
    */
    float GetToPlayerRadius();

    /**
    * ��芪���悤�֐�
    * 
    * �ړ��\����W��ύX����֐��ł��B
    */
    void ChengeFollowPos(DirectX::XMFLOAT3& _addpos);

    /**
    * �@�֏e�G�̈ړ����I�����Ă��邩��Ԃ��֐�
    */
    bool GetGunEnemyIsPass() { return NowFlag.EndForwardRotate; }

    /**
    * �@�֏e�G�̐V�������W�ݒ�
    * 
    * @param  _startpos �J�n���_�ԍ�
    */
    void SetGunEnemyNewPos(CPlayerAttack::GUNENEMYSTARTPOS _startpos);

    /**
    * �f�o�b�O�p�U���t���O�擾�֐�
    */
    bool& GetDebugIsAttack() { return DebugIsAttack; }

    ///**
    //* �ō����x��Ԃ��֐��ł��B
    //*/
    //float& GetMaxSpeed() { return MaxSpeed; }

    ///**
    //* �����x��Ԃ��֐��ł��B
    //*/
    //float& GetAddSpeed() { return AddSpeed; }
};