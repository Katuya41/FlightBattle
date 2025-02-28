#pragma once
#include "CGameObject.h"
#include "deque"
class CEnemy;
class CEffect;

/**
 * �����CPlayer�N���X�̐����ł��B
 * ���̃N���X�̓v���C���[����Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CPlayer :
    public CGameObject
{
private:
    //�����Ă���G�t�F�N�g���
    CEffect* pEffect;
    CEffect* eJetEffect;

    //�G�Ƀ^�[�Q�b�g���ꂽ�Ƃ��Ɏg�p���郂�f��
    std::unique_ptr<CGameObject> pEnemyRock;
    std::unique_ptr<CGameObject> pRockModel;
    std::unique_ptr<CGameObject> pRockOnModel;

    //�x������p�̃I�u�W�F�N�g
    std::unique_ptr<CGameObject> pCaveatObject;

    //�V�[�����X�e�[�W
    bool IsStageScene = false;

    //���˔���
    bool IsMisileShotto = false;

    //�ʔ��˔���
    bool IsBallShotto = false;

    //�J�����𓮂����Ă��邩
    bool IsMoveCamera = false;

    //�^�[�Q�b�gUI��\�����邩
    bool IsDrawRockUI = false;

    //���b�N�I������Ă��邩
    //����͓G�Ƀ��b�N����Ă��邩�ŕύX�����
    bool IsEnemyRock = false;

    //����̓~�T�C���ɂ���ĕύX�����
    //�~�T�C���̐��ʕ����Ƀv���C���[������ꍇ��true�ɂȂ�
    bool IsRockOn = false;

    //�x��UI��`�悷�邩
    bool IsDrawCaveat = false;

    //�쐬�����^�[�Q�b�gUI��ID
    int TargetUIId = 0;

    //��]������p�x
    DirectX::XMFLOAT3 Angle = { 0.0f,0.0f,0.0f };

    //�^�[�Q�b�g�̍��W�i�[�p�ϐ�
    DirectX::XMFLOAT3 TargetPos = { 0,0,0 };

    //�J������]�p�ϐ�
    DirectX::XMFLOAT3 CameraMoveRotate = { 0.0f,0.0f,0.0f };

    //�U���̎��ԗp
    ULONGLONG BivTime;

    //�������Ă���t���O2
    bool OldFastSpeed = false;

    //�������Ă��鎞��
    ULONGLONG OldSpeedUpTime = 0;
    ULONGLONG SpeedUpTime = 0;

    //�x��SE�����I�ɖ炷�̂Ɏg���^�C�}�[
    ULONGLONG EnemyRockTime = 0;

    //�G���^�[�Q�b�g�����Ƃ��̈ړ�����UI
    struct MOVETARGET_UI
    {
        DirectX::XMFLOAT3 Position = {};    //UI�̍��W
        bool IsDraw = false;                //�`�悷�邩
        CEnemy* pTargetObject;              //�^�[�Q�b�g�I�u�W�F�N�g
    };
    std::list<MOVETARGET_UI> plMoveTargetUI;

    //�p�[�e�B�N���̍\���̂ł�
    struct PARTICLE
    {
        CGameObject* Object;        //�p�[�e�B�N���̃I�u�W�F�N�g
        float CoolTime;				//�N�[���^�C��
        ULONGLONG DeleteTime;		//��������
        ULONGLONG StartTime;		//�J�n����
    };

    //�p�[�e�B�N���̐����Ԋu����p�ϐ�
    LONGLONG OldInsertParticleTime;

    //�p�[�e�B�N���̃��f��
    D3D::Model ParticleModel;

    //�p�[�e�B�N���̃I�u�W�F�N�g���i�[����ϐ�
    std::deque<PARTICLE> qParticles;

public:
    /**
    * ������Ԃ̗񋓌^�ł�
    */
    enum SPEEDUP_MODE
    {
        LOW,
        NORMAL,
        HIGH,
    };

private:
    SPEEDUP_MODE SpeedUpMode = LOW;

private:

    /**
    * �p�����[�^������\���̂ł��B
    */
    struct PARAMETER
    {
        float MaxInitRotate;        //���s�p�x�ւ̍ő��]�p�x
        float Max_SlipZ;            //�ő�Z����
        float Max_SlipX;            //�ő�X����
        float Particle_DeleteTime;  //�p�[�e�B�N���̐�������
        float Particle_CoolTime_Low;    //LOW��Ԃ̃p�[�e�B�N���o���p�x
        float Particle_CoolTime_Normal; //NORMAL��Ԃ̃p�[�e�B�N���o���p�x
        float Particle_CoolTime_High;   //HIGH��Ԃ̃p�[�e�B�N���o���p�x
        float Move_Add_Speed;       //�ړ������x
        float NoMove_AddSpeed;      //�������ĂȂ���Ԃ̉�����
        float Right_Move_Speed;     //���E�ړ����x
        float Max_Right_Move_Speed; //���E�ō��ړ����x
        float Max_MoveSpeed;        //�ő呬�x
        float Min_MoveSpeed;        //�Œᑬ�x
        float RotateX_StickSpeed;    //�X�e�B�b�N�ł�X����]���x
        float RotateZ_StickSpeed;   //�X�e�B�b�N�ł�Z����]���x
        float Yow_Rotate_Speed;     //���[��]���x
    };
    PARAMETER Parameter;

    enum PARAM_TAG
    {
        MAXINITROTATE,
        MAXSLIPZ,
        MAXSLIPX,
        PARTICLEDELETETIME,
        PARTICLECOOLTIMELOW,
        PARTICLECOOLTIMENORMAL,
        PARTICLECOOLTIMEHIGH,
        ADDSPEED,
        NOMOVEADDSPEED,
        RIGHTMOVESPEED,
        MAXRIGHTMOVESPEED,
        MAXSPEED,
        MINSPEED,
        ROTATESTICKSPEED,
        ROTATEZSTICKSPEED,
        YOWROTATESPEED,
        MAX,
    };
    PARAM_TAG ParamTag = MAXINITROTATE;

    /**
    * �p�����[�^���t�@�C������ǂݍ���
    */
    void InsertParameter();

    /**
    * �v���C���[�̃p�[�e�B�N���X�V����
    */
    void PaeticleUpdate();

    /**
    * �p�[�e�B�N����ǉ�����֐��ł��B
    */
    void InsertParticle();

    //���͔���
    void CheckInput(DirectX::XMFLOAT3& _angle);

    /**
    * ��]�X�V����
    */
    void RotateUpdate();

    /**
    * �v���C���[��Z����n�ʂƕ��s�ɂ���֐��ł��B
    */
    void SetInitRotate();

    /**
    * �R���g���[���[�̐U���X�V�������s���֐��ł��B
    */
    void BivUpdate();

    /**
    * �����쎞�̋@�̂̎p������
    */
    void SetNotInputRotate();

public:

    CPlayer();
    ~CPlayer();

    //�i�[�֐�//

    /**
    * �~�T�C���̃q�b�g�e�N�X�`����ݒ肷��֐��ł��B
    */
    void SetHitMisileEffect(D3D::Model _model);

    /**
    * �^�[�Q�b�g���ꂽ�Ƃ���UI���i�[����֐��ł��B
    */
    void SetRockUI(D3D::Model& _rockmodel, D3D::Model& _rockonmodel);

    /**
    * �p�[�e�B�N���̃��f����ݒ肷��֐��ł��B
    *
    * @param �ݒ肷��p�[�e�B�N���̃��f��
    */
    void SetParticleModel(D3D::Model _model) { ParticleModel = _model; }

    void SetAnimUvSize(DirectX::XMFLOAT2 _uv);

    ////////

    /**
    * �X�V�������s���֐��ł��B
    */
    void Update() override;

    /**
    * �ړ��������s���֐��ł��B
    */
    void MoveUpdate() override;

    /**
    * �p�[�e�B�N���̕`�揈�����s���֐��ł��B
    */
    void DrawParticle();

    /**
    * �^�[�Q�b�g���ꂽ�Ƃ���UI��\������֐��ł��B
    */
    void DrawTargetUI();

    /**
    * �J�����𓮂����Ă���(���͂����n���Ă���)�����擾����֐��ł��B
    * 
    * @return �J�����𓮂����Ă��邩
    */
    bool GetIsMoveCamera() { return IsMoveCamera; }

    /**
    * �R���g���[���[�̐U�������I������֐��ł��B
    */
    void BivStop();

    /**
    * �U�����Ԃ�ݒ肷��֐��ł��B
    * 
    * @param  �U���I������
    */
    void SetBivTime(ULONGLONG _stoptime);

    /**
    * �^�[�Q�b�g��Ԑݒ�
    */
    void SetIsDrawRockUI(bool _isdrawui) { IsDrawRockUI = _isdrawui; }

    /**
    * �^�[�Q�b�g���b�N��Ԑݒ�
    */
    void SetIsRockOn(bool _isrock) { IsRockOn = _isrock; }

    /**
    * �G�Ƀ��b�N����Ă��邩�̏�ԕύX
    */
    void SetIsEnemyRock(bool _isenemyrock) { IsEnemyRock = _isenemyrock; }

    /**
    * �x����`�悷�邩��Ԃ��֐��ł��B
    */
    bool GetIsDrawCaveat() { return IsDrawCaveat; }

    /**
    * �V�[�����^�C�g������ݒ肷��֐��ł��B
    */
    void SetIsStageScene(bool _isstage) { IsStageScene = _isstage; }

    /**
    * �ړ�����^�[�Q�b�gUi��V�����쐬����֐��ł��B
    */
    void InsertMovetargetUI(CEnemy* _targetobject);

    /**
    * �^�[�Q�b�gUI���X�V����֐��ł��B
    */
    void TargetLineUpdate();

    /**
    * �^�[�Q�b�gUI��`�悷��֐��ł��B
    */
    void DrawTargetLine();

    /**
    * �ړ�����^�[�Q�b�gUI�̃��X�g���N���A����֐��ł��B
    */
    void ClearMoveTargetList();

    /**
    * �ړ�����^�[�Q�b�g�̒��̓���̃I�u�W�F�N�g���폜����֐��ł��B
    */
    void DeleteMoveTargetList(CEnemy* _enemy);

    /**
    * ���݂̃X�s�[�h��Ԃ��擾����֐��ł��B
    */
    SPEEDUP_MODE& GetSpeedUpMode() { return SpeedUpMode; }

    /**
    * ���݂̃p�����[�^��Ԃ��֐��ł�
    * 
    * @return ���݂̃p�����[�^�̍\����
    */
    PARAMETER& GetPlayerParameter() { return Parameter; }

    /**
    * �p�����[�^���Z�[�u����֐��ł��B
    */
    void SaveParameter();

    //void CheckControllerTrigger();
};

