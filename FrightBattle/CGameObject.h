#pragma once
#include"direct3d.h"
#include <DirectXCollision.h>
#include <xinput.h>
#include <unordered_map>
#include <list>
#include <deque>
#include "string"
#include <memory>
#pragma comment (lib, "xinput.lib")
class CCamera;
class CEffect;
class CEnemy;

/**
 * �����CGameObject�N���X�̐����ł��B
 * ���̃N���X�̓Q�[���I�u�W�F�N�g����Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CGameObject
{
private:


protected:
	//�~�j�}�b�v�p�I�u�W�F�N�g
	CGameObject* pMinimapObject;

	//�~�j�}�b�v�X�V���s����
	bool IsUpdateMiniMap = false;

	//���̃I�u�W�F�N�g�̃~�j�}�b�v���f����`�悷�邩
	bool IsDrawMiniMapModel = false;

	//�~�T�C��UI�p�ϐ�
	ULONGLONG MisileShotTime = 0;

	//�����蔻��̃I�u�W�F�N�g
	CGameObject* ColliderObject;

	//�I�u�W�F�N�g�����蔻��
	DirectX::BoundingBox ColliderBoxAABB;
	DirectX::BoundingOrientedBox ColliderBoxOBB;

	//�����蔻��̎�ނ�AABB��
	bool IsColAABB = true;

	//�����蔻������ꂽ�z��
	static std::unordered_map<int, std::list<DirectX::BoundingBox>> mColliderBoxsAABB;

	//�����o�[�ϐ�
	D3D::Model model;	//�`�悷��ׂ����f���f�[�^
	std::wstring ModelName;	//�`�悷�郂�f���f�[�^�̖��O

	D3D::Model NoiseModel;

	//�O�����x�N�g�� = �L�����������Ă��������\�����
	DirectX::XMFLOAT3 Forward = { 0,0,1 };

	//������x�N�g�� = �L�����̏������\�����
	DirectX::XMFLOAT3 UP = { 0,1,0 };

	//������x�N�g�� = �L�����̏������\�����
	DirectX::XMFLOAT3 RIGHT = { 1,0,0 };

	//�v���C���[�̊e�����x�N�g���쐬
	DirectX::XMVECTOR newForward = { 0,0,1,0 };
	DirectX::XMVECTOR newUP = { 0,1,0,0 };
	DirectX::XMVECTOR newRIGHT = { 1,0,0,0 };

	//�����p�s��
	DirectX::XMMATRIX RotateMatrix;

	//�I�u�W�F�N�g�̗̑�
	int Hp = 0;

	//�ړ����x�����ϐ�
	float MoveSpeed = 0.0f;

	//�E�ړ��X�s�[�h
	float RightMoveSpeed = 0.0f;

	//�~�j�}�b�v�ŕ`�悷�邩
	bool IsDrawMiniMap = false;

	// �r���{�[�h���ǂ��������ϐ�
	bool isBillboard = false;

	//�`��\����\���ϐ�
	bool isActive = true;

	//���C�e�B���O���s������\���ϐ�
	bool isLighting = true;

	//�e��`�悷�邩��\���ϐ�
	bool IsShadow = false;

	//�v���C���[�����͂�����
	bool IsInput = false;

	//��ɒn�ʂƕ��s�ɂȂ�悤�ɂ��邩�̔���
	bool IsAutoPilot = false;

	//�}���񃂁[�h���ǂ���
	//�}���񃂁[�h�ɂ���
	//�}���񒆂ł́A�󑤂ւ�X����]�͑��x�������A
	//�n�ʑ��ւ�X����]�͑��x���オ��
	//����Ɖ�]�p�x�����̊Ԃ�2�{�ɂȂ�
	bool IsVeryRotate = false;

	//����������
	bool IsFastSpeed = false;

	//�I�u�W�F�N�g���J�����ɉf���Ă��邩
	bool IsRefMainCamera = false;

	//�I�u�W�F�N�g�����S�J�����Ɏʂ��Ă��邩
	bool IsRefCenterCamera = false;

	//UI��
	bool IsUi = false;

	//�U������Ă��邩
	bool IsAttacked = false;

	//�v���XUI�pFade�ϐ�
	bool IsFadeOut = false;

	//�~�T�C���q�b�gUI�p�Z�b�g����
	ULONGLONG SetTime = 0;

	//UV���W��ݒ肷��֐�
	virtual DirectX::XMMATRIX GetUvMatrix();

	DWORD DwResult = ERROR_DEVICE_NOT_CONNECTED;	//�R���g���[���[�̐ڑ��m�F�Ɏg��
	XINPUT_STATE State;			//���݂̃R���g���[���[�̏�ԊǗ�

	//�R���g���[���[�̏�����
	//�\���{�^��
	int iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
	//LR�g���K�[�{�^��
	int iPad_leftshoulder = 0, iPad_rightshoulder = 0;
	//LR�X�e�B�b�N�������݃{�^��
	int iPad_leftthumb = 0, iPad_rightthumb = 0;
	//A,B,X,Y�{�^��
	int iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
	//���j���[�{�^��,�r���[�{�^��
	int iPad_Start = 0, iPad_Back = 0;
	//���X�e�B�b�N
	int iPad_LStickX = 0, iPad_LStickY = 0;
	int iPad_RStickX = 0, iPad_RStickY = 0;

	//�R���g���[���[�g���K�[�p�ϐ�
	int OldAKey = 0;		//A
	int OldBKey = 0;		//B
	int OldXKey = 0;		//X
	int OldYKey = 0;		//Y
	int OldRightKey = 0;	//RIGHT
	int OldLeftKey = 0;		//LEFT
	int OldUpKey = 0;		//UP
	int OldDownKey = 0;		//DOWN
	int OldStartKey = 0;	//START
	int OldBackKey = 0;	//BACK
	int OldRightThumb = 0;
	int OldLeftThumb = 0;
	int LeftThumbLX = 0;	//
	int LeftThumbLY = 0;	//

	//������\����
	struct Plane {
		DirectX::XMVECTOR normal;
		float distance;
	};

	///**
	//* �v���C���[�p�^�[�Q�b�g���C��������
	//*/
	//void DrawPlayerTargetLine();

public:
	enum TAG
	{
		NONE,				//�����Ȃ�
		BACK_SKY_FLAT,		//�^�C�g�����U���g�̔w�i�̋�
		BACK_SKY_CIRCLE,	//�Q�[���w�i�̋�
		GROUND,				//�n��
		PLAYER,				//�v���C���[
		VECTOR,				//���
		INITENEMY,			//�����G
		FRIGHT_ENEMY,		//��s�G
		GROUND_ENEMY,		//�n�ʓG
		GUN_ENEMY,			//�@�֏e�G
		FOLLOW_ENEMY,		//�Ǐ]�G
		ENTOURAGE_ENEMY,	//��芪���G
		BOSS_ENEMY,			//�{�X�G
		MISILE,				//�~�T�C��
		BACKFIRE,			//�o�b�N�t�@�C�A
		MISILE_SMOKE,		//�~�T�C���̉�
		GUN,				//�@�֏e
		OBJECT,				//��Q��
		CLOUD,				//�_
		SUN,				//���z
		COLLIDER,			//�����蔻��
		CARSOL,				//�J�[�\��
		UI,					//UI
		PRESS_UI,			//�v���XUI
		TARGET_UI,			//�G���^�[�Q�b�g����Ƃ��Ɉړ�����UI
		CENTER_UI,			//��ʒ���UI
		CAVEAT_UI,			//�x��UI
		ATTACKINF_UI,		//�U�����
		MISILE_UI,			//�~�T�C���̒e��\��UI
		HITMISILE_UI,		//�~�T�C���q�b�g��\��UI
		PLAYER_UI,			//�v���C���[�̏�Ԃ�\��UI
		BOSSNAME_UI,		//�{�X�̖��O������UI
		BOSSHP_UI,			//�{�X��HP���o��UI
		MINIMAP_UI,			//�~�j�}�b�v���UI��\��
		SPEED_NUM,			//�X�s�[�h��\������
		ALT_NUM,			//���x��\������
		WAIT_UI,			//��U��~UI
		MAX,				//�ő吔
	};

	enum BOTTUN_NAME
	{
		NO_Key,
		CONTROLLER_A,
		CONTROLLER_B,
		CONTROLLER_X,
		CONTROLLER_Y,
		CONTROLLER_RIGHT,
		CONTROLLER_LEFT,
		CONTROLLER_UP,
		CONTROLLER_DOWN,
		CONTROLLER_RB,
		CONTROLLER_LB,
		CONTROLLER_RT,
		CONTROLLER_LT,
		CONTROLLER_MENU,
		CONTROLLER_BACK,
		CONTROLLER_RIGHT_THUMB,
		CONTROLLER_LEFT_THUMB,
	};


	protected:
		BOTTUN_NAME BottunName = NO_Key;
		TAG Tag = NONE;

	public:

	CGameObject();
	~CGameObject();

	//�@�\
	//�X�V�֐�
	virtual void Update();

	//�ړ�����
	virtual void MoveUpdate();

	//���͂��ꂽ�x�N�g���̕����Ɉړ�����
	virtual void MoveVectorUpdate(DirectX::XMFLOAT3 _movevector);

	///**
	//* �^�[�Q�b�g�����Ƃ��Ɉړ�����^�[�Q�b�gUI�̍X�V����
	//*/
	//void MoveTargetUpdate();

	/**
	* �~�j�}�b�v�ł̃T�C�Y�X�V����
	*/
	void MiniMapObjectUpdate();

	//���݂̊p�����x�N�g�����擾����
	DirectX::XMMATRIX GetIsMatrix();

	//��]�s��ݒ�
	void SetIsMatrix(DirectX::XMMATRIX _matriox) { RotateMatrix = _matriox; }

	//�����̉�]�s��ݒ�
	void SetInitRotateMatrix();

	/**
	* ���݂̉�]�s����擾
	* 
	* @return �I�u�W�F�N�g�̉�]�s��
	*/
	DirectX::XMMATRIX GetRotateMatrix() { return RotateMatrix; }

	//���݂̃I�u�W�F�N�g�̊p�x���猩�������v�Z
	void SetVector();

	//�`��֐�
	virtual void Draw();

	/**
	* �~�j�}�b�v�̃I�u�W�F�N�g��`��
	*/
	void DrawMiniMapObject();

	/**
	* �����蔻��̃I�u�W�F�N�g��`��
	*/
	void DrawColliderObject();

	/**
	* �~�j�}�b�v�I�u�W�F�N�g�̐ݒ���s���֐��ł��B
	*/
	void SetMiniMapModel(D3D::Model _model);

	/**
	* HP�����炷�֐��ł�
	* @param ���݂�HP���猸�炵�����l
	*/
	void SetDamege(int _num) { Hp -= _num; }

	/**
	* ���݂�HP��Ԃ��֐��ł�
	* @return ���݂�HP��Ԃ��܂��B
	*/
	int GetHp() { return Hp; }

	/**
	* ������������Ԃ��܂��B
	* 
	* @return ����������
	*/
	bool GetIsFastSpeed() { return IsFastSpeed; }

	/**
	* �~�j�}�b�v�̃I�u�W�F�N�g�Ƃ��ĕ`�悷��悤�ɕύX����֐��ł��B
	*/
	void SetIsMiniMap() { IsDrawMiniMap = true; }

	//�^�O�̐ݒ�
	void SetTag(TAG _tag) { Tag = _tag; }

	//�^�O�擾
	TAG GetTag() { return Tag; }

	//���f���ɃZ�b�g����֐�
	void SetModel(D3D::Model& _model);

	//���f�����擾
	D3D::Model& GetModel() { return model; }

	/**
	* ���f���̖��O��ݒ�
	* 
	* @param  �ݒ肵�������f���̖��O
	*/
	void SetModelName(std::wstring _modelname) { ModelName = _modelname; }

	/**
	* ���f���̖��O���擾����
	* 
	* @return �擾���������f���̖��O
	*/
	std::wstring GetModelName() { return ModelName; }

	//�����蔻����擾(AABB��OBB�̓������)
	DirectX::BoundingBox GetColliderObjectAABB(){ return ColliderBoxAABB; }
	DirectX::BoundingOrientedBox GetColliderObjectOBB() { return ColliderBoxOBB; }

	/**
	* �����蔻����쐬����֐��ł��B(AABB��OBB�̓��)
	*/
	void SetColliderBoxAABB();
	void SetColliderBoxOBB();
	void SetColliderBoxAABB(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scaling);
	void SetColliderBoxOBB(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scaling);

	/**
	* �I�u�W�F�N�g�Ƃ̋������v�Z����֐�
	* 
	* @param  �����𑪂肽���I�u�W�F�N�g
	* @return �����ɐݒ肵���I�u�W�F�N�g�Ƃ̋���
	*/
	float CheckDistance(const CGameObject* _obj,const DirectX::XMFLOAT3& _pos) const;

	//����GameObject�̉��z���E�ł̈ʒu(���[���h���W)
	DirectX::XMFLOAT3 Position = {};

	//1�t���[���O�̍��W
	DirectX::XMFLOAT3 OldPosition = {};

	//�I�C���[�p
	DirectX::XMFLOAT3 Rotation = {};

	//�N�H�[�^�j�I���p
	DirectX::XMFLOAT3 QuaternionRotation = {};

	//�ړ��x�N�g��
	DirectX::XMFLOAT3 MoveVelocity = {};

	//�g��k����
	DirectX::XMFLOAT3 Scaling = { 1,1,1 };

		//���̃L�����N�^�[�̃}�e���A���F
	DirectX::XMFLOAT4 materialDiffuse = { 1,1,1,1 };

	//�ړ��X�s�[�h��ݒ肷��֐�
	void SetMoveSpeed(float _speed) { MoveSpeed = _speed; }

	//�ړ��X�s�[�h������������
	void SetAddMoveSpeed(float _speed) { MoveSpeed += _speed; }

	//�ړ��X�s�[�h���擾����
	float GetMoveSpeed() { return MoveSpeed; }

	//�O�����x�N�g����Ԃ�
	DirectX::XMFLOAT3 GetForward() { return Forward; }

	//������x�N�g����Ԃ�
	DirectX::XMFLOAT3 GetUp() { return UP; }

	//�E�����x�N�g����Ԃ�
	DirectX::XMFLOAT3 GetRight() { return RIGHT; }

	//�I�u�W�F�N�g�̊e�����x�N�g����Ԃ�
	DirectX::XMVECTOR& GetNewForward() { return newForward; }
	DirectX::XMVECTOR& GetNewUp() { return newUP; }
	DirectX::XMVECTOR& GetNewRight() { return newRIGHT; }

	// �r���{�[�h���ǂ�����ݒ肷��֐�
	void SetBillboard(bool isBillboard);
	/**
	* �r���{�[�h���擾����
	*/
	bool GetIsbillBord() { return isBillboard; }

	/**
	* UI����ݒ肷��
	*/
	void SetIsUI(bool _isui) { IsUi = _isui; }

	/**
	* UI�����擾����
	*/
	bool GetIsUI() { return IsUi; }

	void SetActive(bool _Active) { isActive = _Active; }
	bool GetActive() { return isActive; }


	void SetLighting(bool bLighting) { isLighting = bLighting; }

	/**
	* �v���C���[�ɍU������Ă��邩(�~�T�C���ɒǂ��Ă��邩)
	*
	* @param  �v���C���[�̍U���ɒǂ��Ă��邩
	*/
	void SetIsAttacked(bool _isatacked) { IsAttacked = _isatacked; }

	/**
	* �U������Ă��邩(�~�T�C���ɒǂ��Ă��邩)�̕ϐ����擾����֐��ł��B
	* 
	* @param  �~�T�C���Ƀ^�[�Q�b�g����Ă��邩��Ԃ��܂��B
	*/
	bool GetIsAttacked() { return IsAttacked; }

	/**
	* ���C�e�B���O���邩���擾����
	*/
	bool GetIsLighting() { return isLighting; }

	void SetShadow(bool _isshadow);

	//void GetCameraTexture(CCamera* _camera);

	////���͔���
	//DirectX::XMFLOAT3 CheckInput(DirectX::XMFLOAT3 _angle);

	//���͉������ݔ���
	bool PadInputTrigget(int _num);

	//��]�s�񂩂�I�C���[�p�x���擾
	DirectX::XMFLOAT3 ExtractRotationFromMatrix(const DirectX::XMMATRIX& matrix);

	//�R���g���[���[�̓���
	void DoControllerEvent();

	//�R���g���[�^�[�̃g���K�[�`�F�b�N
	bool CheckControllerTrigger(BOTTUN_NAME _buttonname);

	/**
	* �R���g���[���[�̃{�^����������Ă��邩�m�F����֐��ł��B
	* 
	* @param  �m�F�������{�^���̖��O
	* @return �{�^����������Ă��邩
	*/
	bool CheckControllerPress(BOTTUN_NAME _buttonname);

	//�R���g���[���[���ڑ�����Ă��邩
	void GetControllerState();

	//�I�u�W�F�N�g���f���Ă���J�������擾
	bool GetIsCameraRef();

	//�I�u�W�F�N�g���f���Ă��钆�S�J�������擾
	bool GetIsCenterCameraRef();

	//�J�����ɉf���Ă��邩����
	void ChackRefCamera();

	//���K��
	void NormalizePlane(Plane& plane);

	//�����蔻��I�u�W�F�N�g��ݒ�
	void SetColliderObject(CGameObject* _colliderobject);

	/**
	* �����蔻��̃��f����ݒ�
	* 
	* @param �ݒ肵���������蔻��̃��f��
	*/
	void SetColliderModel(D3D::Model _model) { ColliderObject->SetModel(_model); }

	//�J�����O�𖳌��ɂ���
	void SetCullingMode(bool _cullmode);

	/**
	* ���j�A�⊮��L���ɂ���
	*/
	void SetLinear();

	/**
	* �V�F�[�_�[��ύX����
	*/
	void ChengeShader(const int& _id);

	/**
	* �����蔻��̋��L�ϐ����N���A����
	*/
	void ClearColliderAABBBox();

	/**
	* �~�j�}�b�v�`��p�̃I�u�W�F�N�g���擾����֐��ł��B
	* 
	* @return  �~�j�}�b�v�`��p�I�u�W�F�N�g
	*/
	CGameObject* GetMiniMapObject() { return pMinimapObject; }

	/**
	* �����蔻�肪AABB�����擾����֐��ł��B
	*/
	bool GetIsColAABB() { return IsColAABB; }

	/**
	* �~�j�}�b�v�X�V���s�����̐ݒ�
	* 
	* @param  �~�j�}�b�v�X�V���s����
	*/
	void SetIsMiniMapUpdate(bool _isminimapupdate) { IsUpdateMiniMap = _isminimapupdate; }

	/**
	* �~�T�C���q�b�g�p�̎��Ԑݒ�
	*/
	void SetMisileHitSetTime(ULONGLONG _settime) { SetTime  = _settime; }

	/**
	* �~�T�C���𔭎˂������Ԃ�ݒ肷��֐��ł��B
	*/
	void SetMisileShotTime(ULONGLONG _shottime) { MisileShotTime = _shottime; }
};

