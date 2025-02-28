#pragma once
#include <list>
#include <unordered_map>
#include <iostream>
#include <DirectXMath.h>
#include "CGameObject.h"
#include <string>

class CCamera;
class CPlayer;
class CMisileUI;

/**
 * �����CScene�N���X�̐����ł��B
 * ���̃N���X�̓V�[������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CScene
{
public:

	/**
	* �V�[���̏�Ԃ�\���񋓌^�ł��B
	*/
	enum SCENE
	{
		TITLE,			//�^�C�g��
		MENU,			//���j���[
		STAGE,			//�e�X�g�X�e�[�W
		STAGE_01,		//�X�e�[�W1
		STAGE_02,		//�X�e�[�W2
		STAGE_03,		//�X�e�[�W3
		STAGE_04,		//�X�e�[�W4
		TEST_STAGE,		//�e�X�g�V�[��
		RESULT,			//���U���g
		GAMEOVER,		//�Q�[���I�[�o�[
		GAMECLEAR,		//�Q�[���N���A
		CREATE_STAGE,	//�X�e�[�W�쐬
	};
	SCENE Scene;

private:

	enum ENUM_GET_TEXT
	{
		TAG,
		BILLBORD,
		LIGHTING,
		POSX,
		POSY,
		POSZ,
		ROTX,
		ROTY,
		ROTZ,
		SCAX,
		SCAY,
		SCAZ,
		MATERIAL,
		ISUI,
		END,
	};
	ENUM_GET_TEXT EGetText = TAG;

	/**
	* �e�L�X�g����擾����������␔�����i�[����\���̂ł�
	*/
	struct STR_GET_TEXT
	{
		int TagId;
		int IsBillbord;
		int IsLighting;
		float PositionX;
		float PositionY;
		float PositionZ;
		float RotationX;
		float RotationY;
		float RotationZ;
		float ScalingX;
		float ScalingY;
		float ScalingZ;
		float MaterialDiffuseW;
		int Isui;
	};
	STR_GET_TEXT SGetText;

	std::list<CMisileUI*> plMisileUI;

	//CGameObject�^�̕ϐ�������z��
	std::list<CGameObject*> lGameObject;

	//UI������z��
	std::list<CGameObject*> lGameUI;

	//�v���C���[
	std::list<CPlayer*>lPlayer;

	//���x����������z��
	std::unordered_map<int, CGameObject*> lGameSpeedNums;

	//���x����������z��
	std::unordered_map<int, CGameObject*> lGameAltNums;

	//�{�X��HP�o�[������z��
	std::list<CGameObject*> lBossHPUI;

	//�v���C���[��\�����邩
	bool IsDrawPlayer = true;

	//�����蔻���`�悳���邩
	bool IsDrawCollider = false;

	//�X�e�[�W�f�o�b�O�E�B���h�E���쐬������
	bool IsCreateStageDebugWindoe = false;

	//�X�e�[�W�쐬�f�o�b�O�E�B���h�E���쐬������
	bool IsCreateCreateStageWindow = false;

	//BGM�̉���
	float BGM_Volume = 0.005f;
	//SE�̉���
	float SE_Volume = 0.02f;

	//�V�����ǉ�����I�u�W�F�N�g�p�ϐ�
	DirectX::XMFLOAT3 NewObjectPos = { 0.0f,-40.0f,0.0f };
	DirectX::XMFLOAT3 NewObjectRot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewObjectSca = { 10.0f,20.0f,10.0f };

	//�V�����ǉ�����n�ʓG�p�ϐ�
	DirectX::XMFLOAT3 NewGroundEnemyPos = { 0.0f,-58.0f,0.0f };
	DirectX::XMFLOAT3 NewGroundEnemyRot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewGroundEnemySca = { 0.3f,0.3f,0.3f };

	//�V�����ǉ�����󒆓G�p�ϐ�
	DirectX::XMFLOAT3 NewFrightEnemyPos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewFrightEnemyRot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewFrightEnemySca = { 1.0f,1.0f,1.0f };

	//�V�����ǉ�����v���C���[�p�ϐ�
	DirectX::XMFLOAT3 NewPlayerPos;
	DirectX::XMFLOAT3 NewPlayerRot;

	//�e�L�X�g����擾����������e�X�g�p
	std::string TestText;

	CScene();
	~CScene();

	/**
	* �쐬�����Q�[���I�u�W�F�N�g���폜����
	*/
	void ClearGameObject();

	/**
	* �^�C�g���V�[���Z�b�g����֐��ł��B
	*/
	void SetTitleScene();

	/**
	* ���j���[�V�[���Z�b�g����֐��ł��B
	*/
	void SetMenuScene();

	/**
	* �X�e�[�W�V�[����ݒ肷��֐��ł��B
	*/
	void SetStageScene(std::string _stagename);

	/**
	* �e�X�g�X�e�[�W�V�[����ݒ肷��֐��ł��B
	*/
	void SetTestStageScene();

	/**
	* �Q�[���N���A��ʂ��Z�b�g����֐��ł��B
	*/
	void SetGameClearScene();

	/**
	* �Q�[���I�[�o�[��ʂ��Z�b�g����֐��ł��B
	*/
	void SetGameOverScene();

	/**
	* �X�e�[�W�쐬��ʂ��Z�b�g����֐��ł��B
	*/
	void SetCreateStageScene();


	/**
	* ���x�����I�u�W�F�N�gUI���쐬���Ē��̏���ݒ肵�Ĕz��Ɋi�[����֐��ł��B
	*
	* @param _moedlname ���f���̖��O
	* @param _isbillboar _islighting �r���{�[�g,���C�e�B���O���邩
	* @param _position _rotation _scaling _materialw ���W,�p�x,�傫��,�A���t�@�l
	* @param _tag �^�O
	* @param _gameobject �ݒ肷��I�u�W�F�N�g�̕ϐ�
	*/
	void SetGameObjectSppedNumsInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, CGameObject* _gameobject, int _num);

	/**
	* ���x�����I�u�W�F�N�gUI���쐬���Ē��̏���ݒ肵�Ĕz��Ɋi�[����֐��ł��B
	*
	* @param _moedlname ���f���̖��O
	* @param _isbillboar _islighting �r���{�[�g,���C�e�B���O���邩
	* @param _position _rotation _scaling _materialw ���W,�p�x,�傫��,�A���t�@�l
	* @param _tag �^�O
	* @param _gameobject �ݒ肷��I�u�W�F�N�g�̕ϐ�
	*/
	void SetGameObjectAltNumsInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, CGameObject* _gameobject, int _num);

public:
	static CScene* Get();

	/**
	* �X�V����
	*/
	void Update();

	/**
	* 3D��ʂ̕`��
	*/
	void Draw3DCamera();

	/**
	* 2D��ʂł̕`��
	*/
	void Draw2DCamera();

	/**
	* �V�[�����Z�b�g����֐��ł��B
	*
	* @param  �ݒ肵�����V�[��
	*/
	void SetScene(SCENE _scene);

	/**
	* ���݂̃V�[�����擾����֐��ł��B
	*
	* @return  ���݂̃V�[����Ԃ��܂��B
	*/
	SCENE GetNowScene() { return Scene; }

	/**
	* 3D�I�u�W�F�N�g�̔z��̒��g���擾����
	*
	* @param  �擾����I�u�W�F�N�g�̃^�O
	* @return �����̃^�O�ɉ������I�u�W�F�N�g
	*/
	CGameObject* Get3DGameObject(CGameObject::TAG _objecttag);

	/**
	* 2D�I�u�W�F�N�g�̔z��̒��g���擾����
	*
	* @param  �擾����I�u�W�F�N�g�̃^�O
	* @return �����̃^�O�ɉ������I�u�W�F�N�g
	*/
	CGameObject* Get2DGameObject(CGameObject::TAG _objecttag);

	/**
	* �X�e�[�W�V�[���ł̃v���C���[���擾����
	*
	* @return �v���C���[��Ԃ�
	*/
	CPlayer* GetPlayer();

	/**
	* �_�I�u�W�F�N�g���擾����֐��ł��B
	*
	* @param  �_�I�u�W�F�N�g���i�[���铮�I�z��
	*/
	void GetCloudObject(std::list<CGameObject*>& _cloudlist);

	/**
	* �_�I�u�W�F�N�g��ǉ�����֐��ł��B
	*/
	void InsertCloudObject();

	/**
	* �X�e�[�W�V�[���ł̑��x����UI���擾����
	*
	* @param  �擾�������ԍ���ID(0�`9)
	* @return �����ɉ����������I�u�W�F�N�g��Ԃ�
	*/
	CGameObject* GetStageSpeedNums(int _numsid);

	/**
	* �X�e�[�W�V�[���ł̍��x����UI���擾����
	*
	* @param  �擾�������ԍ���ID(0�`9)
	* @return �����ɉ����������I�u�W�F�N�g��Ԃ�
	*/
	CGameObject* GetStageAltNums(int _numsid);

	/**
	* �A�N�e�B�u�ȃ~�T�C��UI���擾����
	*
	* @return  �A�N�e�B�u�ȃ~�T�C��UI��Ԃ�
	*/
	CGameObject* GetActiveMisileUI();

	/**
	* �A�N�e�B�u�ȃ~�T�C��UI���擾����
	*
	* @return  �A�N�e�B�u�ȃ~�T�C��UI��Ԃ�
	*/
	CMisileUI* GetActiveMisileUITest(bool _isfour);

	/**
	* FourMisile�p�m�F
	*/
	int CheckFourMisileActive();

	/**
	* �w�肵�����̃~�T�C��UI�̏�Ԃ�ύX
	*/
	void SetMisileUICharge(const int _num);

	/**
	* �~�T�C��UI�����Z�b�g����
	*/
	void ResetMisileUI();

	/**
	* �����蔻���`�悳���邩��Ԃ�
	*/
	bool GetIsDrawCollider() { return IsDrawCollider; }

	/**
	* �����蔻���`�悳���邩��ݒ肷��
	*/
	void SetIsDrawCollider() { IsDrawCollider = !IsDrawCollider; }

	/**
	* �v���C���[��z��ŕԂ�(�f�o�b�O�p)
	*
	* @param  b3D�I�u�W�F�N�g������z��
	*/
	void Get3DObject(std::list<CGameObject*>& _3dobject);

	/**
	* �I�u�W�F�N�g���쐬���Ē��̏���ݒ肵�Ĕz��Ɋi�[����֐��ł��B
	*
	* @param _moedlname ���f���̖��O
	* @param _isbillboar _islighting �r���{�[�g,���C�e�B���O���邩
	* @param _position _rotation _scaling _materialw ���W,�p�x,�傫��,�A���t�@�l
	* @param _tag �^�O
	* @param _gameobject �ݒ肷��I�u�W�F�N�g�̕ϐ�
	* @param _isui		 UI���ǂ���
	*/
	void SetGameObjectInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, CGameObject* _gameobject, bool _isui);
	void SetGameObjectInfTest(bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, bool _isui);

	/**
	* �V�����I�u�W�F�N�g�쐬�p���W���擾����
	*/
	DirectX::XMFLOAT3& GetNewObjectPos() { return NewObjectPos; }
	DirectX::XMFLOAT3& GetNewObjectRot() { return NewObjectRot; }
	DirectX::XMFLOAT3& GetNewObjectSca() { return NewObjectSca; }

	/**
	* �V�����n��G�I�u�W�F�N�g�쐬�p���W���擾����
	*/
	DirectX::XMFLOAT3& GetNewGroundEnemyPos() { return NewGroundEnemyPos; }
	DirectX::XMFLOAT3& GetNewGroundEnemyRot() { return NewGroundEnemyRot; }
	DirectX::XMFLOAT3& GetNewGroundEnemySca() { return NewGroundEnemySca; }

	/**
	* �V�����󒆓G�I�u�W�F�N�g�쐬�p���W���擾����
	*/
	DirectX::XMFLOAT3& GetNewFrightEnemyPos() { return NewFrightEnemyPos; }
	DirectX::XMFLOAT3& GetNewFrightEnemyRot() { return NewFrightEnemyRot; }
	DirectX::XMFLOAT3& GetNewFrightEnemySca() { return NewFrightEnemySca; }

	/**
	* �V�����v���C���[�쐬�p�ϐ����擾����
	*/
	DirectX::XMFLOAT3& GetNewPlayerPos() { return NewPlayerPos; }
	DirectX::XMFLOAT3& GetNewPlayerRot() { return NewPlayerRot; }


	/**
	* �v���C���[��\�����邩��ݒ肷��֐�
	*/
	void SetDrawPlayer(bool _isdraw) { IsDrawPlayer = _isdraw; }

	/**
	* �v���C���[��z��Ɋi�[����֐��ł�(�f�o�b�O�p)
	*/
	void InsertPlayer(CPlayer* _player) { lPlayer.push_back(_player); }

	/**
	* �v���C���[�z����N���A����֐��ł��B
	*/
	void ClearLPlayer() { lPlayer.clear(); }

	/**
	* �e�L�X�g����l���擾����֐��ł��B
	*/
	void GetTextValue(std::string _stagename);

	/**
	* �e�L�X�g�ɒl��ݒ肷��֐��ł��B
	*/
	void SetTextValue(std::string _stagename);

	/**
	* �e�L�X�g�Ƀp�����[�^�[���Z�b�g����
	*
	* @param  �Z�b�g�������t�@�C����
	* @param  �Z�b�g�������p�����[�^�̃I�u�W�F�N�g�ԍ�
	* 1:Player 2:Enemy 3:Misile
	*/
	void SetTextFileParameter(std::string _stagename, int _num);

	/**
	* BGM�̉��ʂ�Ԃ�
	*/
	float& GetBGMVolume() { return BGM_Volume; }

	/**
	* SE�̉��ʂ�Ԃ�
	*/
	float& GetSeVolume() { return SE_Volume; }
};

