#pragma once
#include "direct3d.h"
#include <DirectXCollision.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>
#include <memory>

//�N���X�^�̑O���錾
//���݂��ɃC���N���[�h�������󋵂��Ȃ����̂Ɏg��
//���̃N���X�^�̃|�C���^�ϐ���錾�������ꍇ�Ɏg��
class CGameObject;
class CPlayer;
class CCamera; 
class CEffect;
class CEnemy;
class CVector;

/**
 * �����CGame�N���X�̐����ł��B
 * ���̃N���X�̓Q�[������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CGame
{
private:
	/**
	* ���j���[���̏�Ԃ������񋓌^�ł�
	*/
	enum MAINMENU
	{
		MMENU_NONE,
		MMENU_GAMESTART,
		MMENU_QUITGAME,
		MMENU_MAX,
	};
	MAINMENU MainMenu = MMENU_GAMESTART;

	/**
	* ���C�����j���[�ŃQ�[���X�^�[�g��I���������̗񋓌^�ł��B
	*/
	enum GAMESTAGE
	{
		GMENU_NONE,
		GMENU_STAGE1,
		GMENU_STAGE2,
		GMENU_STAGE3,
		GMENU_STAGE4,
		GMENU_MAX,
	};
	GAMESTAGE GameStage = GMENU_NONE;

	//�J�����p�Â��v���C���[���W
	//�z��Ƀv���C���[�̐��ʕ������i�[���Ă���
	//�w��̃t���[���o������ɁA���Ԃɔz�񂩂甲���Ă���
	//DirectX::XMFLOAT3 CameraOldPos[3];
	std::deque<DirectX::XMFLOAT3> NewCameraPos;
	bool InsertNewCamFocus = false;

	//�Q�[�����I�����邩�̏���
	bool IsGameEnd = false;

	//�Q�[�����J�n���邩�̕ϐ�
	bool IsStartGame = false;

	//�X�e�[�W�J�E���g�_�E��
	bool IsStartCountDown = false;

	//�Q�[���I����I��������
	bool IsCheckGamequit = false;

	//���C�����j���[�őI��������
	bool IsCheckMainMenu = false;

	//�Q�[���X�e�[�W���j���[�őI��������
	bool IsCheckGameStage = false;

	//�v���C���[�X�s�[�h����p
	int MoveSpeedNum = 0;

	//�v���C���[���x����p
	int AltitudeNum = 0;

	//�X�e�[�W���烁�j���[�ɖ߂�ϐ�
	bool BackStage = false;

	//�_���o�������邽�߂̈ꎞ�ϐ�
	ULONGLONG CloudOldTime = 0;

	//�J�n�J�E���g�_�E����SE�p�ϐ�
	ULONGLONG CountDowntime = 0;
	int Cnt = 0;

	//�v���W�F�N�g�J�n����
	ULONGLONG StartProjTime = 0;
	//�v���W�F�N�g�o�ߎ���
	int NowMoveTime = 0;

	//�c�݂̋���
	float Dist = 0.01f;

	DirectX::XMFLOAT3 SunPosition = { 0.0f,60.0f,0.0f };

public:
	float& GetDist() { return Dist; }

	bool GetIsGAmeEnd() { return IsGameEnd; }

	void SetSunPosition(DirectX::XMFLOAT3 _sunpos) { SunPosition = _sunpos; }

	DirectX::XMFLOAT3& GetSunPosition() { return SunPosition; }

public:
	// �J�����̏����ʒu
	const DirectX::XMFLOAT3 iniCameraPos = { 0,1,-2 };
	// �J�����ʒu�����ϐ�
	DirectX::XMFLOAT3 cameraPos = { 0.0f,0.0f,0.0f };
	// �J�����̉�]�p�x
	DirectX::XMFLOAT3 cameraRotate = { 0,0,0 };

	DirectX::XMFLOAT3 MiniMapPos = { 0,0,0 };

	// �J�����ʒu�����ϐ�
	DirectX::XMFLOAT3 DebugCameraPos = { 0.0f,3.0f,0.0f };
	// �J�����ʒu�����ϐ�
	DirectX::XMFLOAT3 DebugCameraUp = { 0.0f,1.0f,0.0f };
	// �J�����̉�]�p�x
	DirectX::XMFLOAT3 DebugCameraFocus = { 0.0f,3.0f,10.0f };

	//�Q�[���J�����̐ݒ�
	enum CAMERA_ID
	{
		Third,
		First,
	};
	CAMERA_ID CameraId = First;

private:
	enum FADE
	{
		NO_FADE,
		FADE_IN,
		FADE_OUT,
	};
	FADE Fade = NO_FADE;

	//�t�F�[�h�p�l��
	CGameObject* pFadePanel = nullptr;
	CGameObject* pWaitPanel = nullptr;

	//std::unique_ptr<CGameObject> pFadePanel;
	//std::unique_ptr<CGameObject> pWaitPanel;

	//UI
	CGameObject* pShotPosUI = nullptr;		//�e�e�Ǝ˗\��
	//�X�^�[�g�J�E���g�_�E��
	CEffect* pCountDownUI = nullptr;
	//�X�^�[�g���[��
	CGameObject* RuleUI = nullptr;

	ULONGLONG CameraShakeStartTime = 0;	//�J�������h��n�߂�J�n����
	bool IsDamegeShakeCamera = false;		//�J�������h��邩�̔���
	bool IsDamegeShakeright = true;			//�J�������h���Ƃ��̎��̗h������

	bool IsSpeedShakeCamera = false;		//�J�������h��邩�̔���
	bool IsSpeedShakeright = true;		//�J�������h���Ƃ��̎��̗h������

	//�J�����ϐ�
	CCamera* camera = nullptr;
	CCamera* camera3D = nullptr;		//��ʂ�3D�I�u�W�F�N�g�Ƃ��ĕ`��Ɏg��
	CCamera* camera2D = nullptr;		//2D�I�ȕ\�����������I�u�W�F�N�g(UI���Ɏg��)
	CCamera* MiniMapCam = nullptr;	    //�~�j�}�b�v�J����
	CCamera* DebugCamera = nullptr;		//�f�o�b�O�V�[���p�J����

	//�~�j�}�b�v�p�ϐ�
	DirectX::XMFLOAT3 MinimapDir = { 0.0f,0.0f,0.0f };
	DirectX::XMVECTOR minimapdir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&MinimapDir));;

	typedef std::unordered_map<std::wstring, D3D::Model> ModelHash;
	ModelHash* modelManager;

	D3D::Model model;

	bool WaitFadeSecne = false;
	double Length = 0.0f;

	//�~�j�}�b�v�؂�ւ��悤�ϐ�
	int MiniMapNum = 0;

	//�ꎞ��~�p�ϐ�
	bool IsStop = false;

	//�f�o�b�O�\���p
	bool IsDebug = false;

	CGame() {}
	~CGame();

	/**
	 *�I�u�W�F�N�g�̃����[�X����������֐��ł��B
	 */
	void ReleseObject();

public:
	static CGame* Get();

	/**
	*�Q�[���I�u�W�F�N�g�쐬�ƃA�Z�b�g�ǂݍ��݂�����֐��ł��B
	*/
	void SetGameObjectInit();

	/**
	 *�Q�[���I�u�W�F�N�g�̏���������������֐��ł��B
	 */
	void Initialize();

	/**
	 *�Q�[���̍X�V����������֐��ł��B
	 */
	void Update();

	/**
	 *�Q�[���̕`�揈��������֐��ł��B
	 */
	void Draw();

	/**
	 *�`��ݒ肳��Ă���J�������擾����֐��ł��B
	 * 
	 * @return �`��ɐݒ肳��Ă���J������Ԃ��܂��B
	 */
	CCamera* GetCamera() { return camera; }

	/**
	 *�`��ݒ�̃J������ݒ肷��֐��ł��B
	 *
	 * @param �`��ݒ�ɂ������J����
	 */
	void SetCamera(CCamera* cam) { camera = cam; }

	/**
	 *3d�̃J�������擾����֐��ł��B
	 *
	 * @return 3d�̃J������Ԃ��܂��B
	 */
	CCamera* GetCamera3D() { return camera3D; }

	/**
	* �f�o�b�O�J�������擾����
	* 
	* @return �f�o�b�O�J����
	*/
	CCamera* GetDebugCamera() { return DebugCamera; }

	////3D�̃J�����̍��W�擾
	//DirectX::XMFLOAT3 GetCameraPos() { return camera3D->GetCameraPos(); }

	/**
	 *2d�̃J�������擾����֐��ł��B
	 *
	 * @return 2d�̃J������Ԃ��܂��B
	 */
	CCamera* GetCamera2D() { return camera2D; }

	/**
	 *�~�j�}�b�v�J�������擾����֐��ł��B
	 *
	 * @return �~�j�}�b�v�J������Ԃ��܂��B
	 */
	CCamera* GetMiniCamera() { return MiniMapCam; }

	/**
	 *�v���C���[�I�u�W�F�N�g���擾����֐��ł��B
	 *
	 * @return �v���C���[�I�u�W�F�N�g��Ԃ��܂��B
	 */
	CPlayer* GetPlayer();

	/**
	 *���C���J�����̐��ʕ������擾����֐��ł��B
	 *
	 * @param  ��ƂȂ�v���C���[�ʒu
	 * @return ���C���J�����̐��ʕ�����Ԃ��܂��B
	 */
	DirectX::XMFLOAT3 GetMainCameraForward(CGameObject* _player);

	////�O�l�̃J�����̐ݒ�
	//DirectX::XMFLOAT3 SetMainCamera(CPlayer* _player);

	/**
	 *�I�u�W�F�N�g��Relese��Delete���s���֐��ł��B
	 *
	 * @param  ����������I�u�W�F�N�g
	 */
	void EndObject(CGameObject* _obj);

	/**
	 *�t�F�[�h�̐ݒ���s���֐��ł��B
	 *
	 * @param  �ύX�������t�F�[�h�̏��
	 */
	void SetFade(FADE _fade) { Fade = _fade; }

	/**
	* �Q�[�����J�n���邩�̕ϐ���ݒ肷��֐��ł��B
	*/
	void SetIsGameStageStart(bool _isstart) { IsStartGame = _isstart; IsStartCountDown = _isstart; }

	/**
	* �X�e�[�W���J�n�������̕ϐ����擾����֐��ł��B
	*/
	bool GetIsStartStage() { return IsStartGame; }

	/**
	 *�~�T�C�����q�b�g��������UI������s���֐��ł��B
	 */
	void MisileHitUIActive();

	/**
	* �v���C���[�Ƀ~�T�C���������������ɃJ������h�炷�֐��ł��B
	* 
	* @param id
	* 1:�_���[�W�U��
	* 2:�X�s�[�h�A�b�v�U��
	* 3:
	* 4:
	* 
	*/
	void ShakeCamera(const int& _id);

	/**
	 *�J�����̎��_�̐ݒ���s���֐��ł��B
	 * 
	 * @param �ύX�������J�����̎��_
	 */
	void SetCameraID(CAMERA_ID _id) { CameraId  = _id; }

	/**
	 *�J�������W�̐ݒ���s���֐��ł��B
	 */
	void SetCameraPos();

	/**
	 *�J�������W�̎擾���s���֐��ł��B
	 * 
	 * @return �J�������W��Ԃ��܂��B
	 */
	DirectX::XMFLOAT3 GetCameraPos() { return cameraPos; }

	/**
	* �~�j�}�b�v���W��Ԃ�
	*/
	DirectX::XMFLOAT3& GetMiniMapCameraPos() { return MiniMapPos; }

	/**
	 *�J��������l�̎��_����Ԃ��֐��ł��B
	 *
	 * @return �J��������l�̎��_����Ԃ��܂��B
	 */
	bool GetIsFirst();

	/**
	* �G���o��������֐��ł��B
	*
	* @param _tag		  �G�̃^�OID
	* @param _newposition �o��������I�u�W�F�N�g�̃|�W�V����
	* @param _newrotate   �o��������I�u�W�F�N�g�̉�]�p�x
	* @param _newscaling  �o��������I�u�W�F�N�g�̃X�P�[��
	*/
	void InsertEnemy(int _tag ,DirectX::XMFLOAT3 _newposition, DirectX::XMFLOAT3 _newrotate, DirectX::XMFLOAT3 _newscaling);

	/**
	* �~�j�}�b�v�؂�ւ��ϐ���Ԃ�
	* 
	* @return �~�j�}�b�v�؂�ւ��ϐ�
	*/
	int GetMiniMapNum() { return MiniMapNum; }

	/**
	* �ꎞ��~�p�ϐ���ύX����֐��ł��B
	*/
	void SetIsGameStop() { IsStop = !IsStop; }

	/**
	* �ꎞ��~�ϐ����擾����֐��ł��B
	* 
	* @return  �ꎞ��~�ϐ�
	*/
	bool GetIsStop() { return IsStop; }

	/**
	* �o�ߎ��Ԃ�ݒ肷��֐��ł��B
	*/
	void SetNowMoveTime();

	/**
	* �o�ߎ��Ԃ�Ԃ��֐��ł��B
	*/
	int GetNowMoveTime();

	int GetStartTime() { return (int)StartProjTime; }

	/**
	* ���f������ꂽ�֐����擾����
	* 
	* @param  ���f���e�N�X�`���̖��O
	* @return �e�N�X�`���̖��O�����ƂɎ擾�������f����Ԃ�
	*/
	D3D::Model& GetModel(const std::wstring _moedlname);

	/**
	* �f�o�b�O���[�h�����擾
	*/
	bool GetIsDebug() { return IsDebug; }
};

