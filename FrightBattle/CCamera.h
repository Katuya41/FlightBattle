#pragma once
#include"direct3d.h"

/**
 * �����CCamera�N���X�̐����ł��B
 * ���̃N���X�̓J��������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CCamera
{
public:
	/**
	*�J�����̏�Ԃ�����\���̂ł��B
	*
	* @param NORMAL  �ʏ�J����
	* @param MINIMAP �~�j�}�b�v�J����
	*/
	enum CAMERA_MODE
	{
		NORMAL,
		MINIMAP,
	};

	/**
	*�J���������݂�����W��ݒ肷��֐��ł��B
	* @param �J�����̍��W���Z�b�g���܂��B
	*/
	enum PROJECTION_MODE
	{
		_3D, _2D
	};

private:
	// �����o�[�ϐ�
	// �v�Z�ς݃r���[�ϊ��s���ۑ�����ϐ�
	DirectX::XMMATRIX matrixView = {};

	// Eye��ۑ�����ϐ�
	DirectX::XMFLOAT3 eye = { 0,0,-1 };
	// Focus��ۑ�����ϐ�
	DirectX::XMFLOAT3 focus = { 0,0,0 };
	// Up��ۑ�����ϐ�
	DirectX::XMFLOAT3 up = { 0,1,0 };
	// Right��ۑ�����ϐ�
	DirectX::XMFLOAT3 right = { 1.0f,0.0f,0.0f };

	//�f�o�b�O�p//

	//�O�����x�N�g�� = �J�����������Ă��������\�����
	DirectX::XMFLOAT3 Forward = { 0,0,1 };

	//������x�N�g�� = �J�����̏������\�����
	DirectX::XMFLOAT3 Right = { 1,0,0 };

	//�J�����̊e�����x�N�g���쐬
	DirectX::XMVECTOR newForward = { 0,0,1,0 };
	DirectX::XMVECTOR newUP = { 0,1,0,0 };
	DirectX::XMVECTOR newRIGHT = { 1,0,0,0 };

	//�����p�s��
	DirectX::XMMATRIX RotateMatrix = {};

	//�p�x(�f�o�b�O)
	DirectX::XMFLOAT3 Rotate = { 0.0f,0.0f,0.0f };

	//�J�����̊e���_������\���̂ł��B
	struct Frustum
	{
		DirectX::XMFLOAT4 planes[6];
	};
	Frustum frus = {};

	//�J�����̃��[�h
	CAMERA_MODE CameraMode = NORMAL;
	
	// �f�t�H���g��3D �� �������e
	PROJECTION_MODE projectionMode = _3D;

	//�J������FOV�p�x
	float FovRadius = 45.0f;
	//float FovRadius = 60.0f;

	//�f�o�b�O�p�J������
	bool IsDebug = false;

	//�f�o�b�O�J�����̈ړ��X�s�[�h
	float MoveSpeed = 0.2f;

	//�f�o�b�O�J�����̉�]���x
	float RotateSpeed = 2.0f;

	//�J�����̃I�t�Z�b�g�ʒu
	DirectX::XMFLOAT3 OffsetCameraPos = { 0.0f,0.7f,2.5f };

	//�J�����̃A���t�@�l
	float FocusAlpha = 0.2f;
	float UpAlpha = 0.1f;
	float PosAlpha = 0.0f;

	/**
	* �J�����̕����v�Z
	*/
	void SetVector();

public:

	CCamera();
	~CCamera(){}

	/**
	 *�J���������݂�����W��ݒ肷��֐��ł��B
	 * @param �J�����̍��W���Z�b�g���܂��B
	 */
	void SetEye(DirectX::XMFLOAT3 eyePosition);

	/**
	 *�J�������_�����W��ݒ肷��֐��ł��B
	 * @param �J�������_�����W���Z�b�g���܂��B
	 */
	void SetFocus(DirectX::XMFLOAT3 focusPosition);

	/**
	 *�J�����̏������ݒ肷��֐��ł��B
	 * @param �J�����̏�������Z�b�g���܂��B
	 */
	void SetUp(DirectX::XMFLOAT3 upPosition);

	/**
	 *�J�����̏�����𑝉�����֐��ł��B
	 * @param �J�����̏�����ɑ����������W���Z�b�g���܂��B
	 */
	void AddUp(DirectX::XMFLOAT3 upPosition);

	/**
	 *�J�����̍��W���擾����֐��ł��B
	 * @return �J�����̍��W��Ԃ��܂��B
	 */
	DirectX::XMFLOAT3 GetCameraPos() { return eye; }

	/**
	* �J�����̃t�H�[�J�X���擾����֐��ł�
	* 
	* @return �J�����̃t�H�[�J�X���W
	*/
	DirectX::XMFLOAT3 GetCameraFocus() { return focus; }

	/**
	* �J�����̏�������擾����֐��ł��B
	*/
	DirectX::XMFLOAT3 GetCameraUp() { return up; }

	/**
	 *�r���[�ϊ��s����v�Z����֐�(�X�V����)�ł��B
	 */
	void Update();

	/**
	 *�v�Z�ς݃r���[�ϊ��s����擾����֐��ł��B
	 * @return �v�Z�ς݃r���[�ϊ��s����擾���܂��B
	 */
	DirectX::XMMATRIX GetViewMatrix();
	
	 /**
	 *�I�u�W�F�N�g�̓��e���@���擾����֐��ł��B
	 * @return �I�u�W�F�N�g�̓��e���@���擾���܂��B
	 */
	DirectX::XMMATRIX GetProjectionMatrix();

	/**
	 *�I�u�W�F�N�g�̓��e���@��ݒ肷��֐��ł��B
	 * @param �ݒ肵�����I�u�W�F�N�g�̓��e���@�ł��B
	 */
	void SetProjectionMode(PROJECTION_MODE mode) { projectionMode = mode; }

	/**
	 *�I�u�W�F�N�g�̓��e���@��ݒ肷��֐��ł��B
	 * @param �ݒ肵�����I�u�W�F�N�g�̓��e���@�ł��B
	 */
	void SetCameraMode(CAMERA_MODE _cameramode) { CameraMode = _cameramode; }

	/**
	 *�J�����̊e���_���v�Z����֐��ł��B
	 * @param �J�����̊e���_���i�[����ꏊ�ł��B
	 * @param ���݂̃v���W�F�N�V�����s��ł�
	 */
	void ExtractFrustum(Frustum& frustum, DirectX::XMMATRIX viewProj);

	/**
	* �J������FOV�p�x���Z�b�g����֐��ł��B
	* 
	* @param �ݒ肵����FOV�p�x
	*/
	void SetFovRadius(float _fovradius) { FovRadius = _fovradius; }

	/**
	* �J������FOV�p�x���v���X����֐��ł��B
	* 
	* @param ���������l
	*/
	void AddFovRadius(float _addrad) { FovRadius += _addrad; }

	/**
	* ���X�ɏ�����FOV�p�x�ɖ߂�
	*/
	void MoveInitFovRadius(float _moverad);

	/**
	* �����̎���p�ɐݒ肷��
	*/
	void InitFovRadius() { FovRadius = 45.0f; }

	/**
	* ���݂̎���p���擾����
	*/
	float GetFovRadius() { return FovRadius; }

	/**
	* �J�����̃I�t�Z�b�g�ʒu��Ԃ��֐��ł��B
	*/ 
	DirectX::XMFLOAT3& GetOffsetCameraPos() { return OffsetCameraPos; }

	/**
	* �J�����̃t�H�[�J�X�⊮��Ԃ�
	* �J�����̃A�b�v�⊮��Ԃ�
	*/
	float& GetFocusAlpha() { return FocusAlpha; }
	float& GetUpAlpha() { return UpAlpha; }

	/**
	* �f�o�b�O�p�J�����ɕύX
	*/
	void SetIsDebug() { IsDebug = true; }

	/**
	* ������]�s��쐬(�f�o�b�O�p)
	*/
	void InitMatrixRotate();

	/**
	* �J�������W�O��ړ�(�f�o�b�O�p)
	*/
	void MoveForwardDebugCamera(bool _plus);

	/**
	* �J�������W���E�ړ�(�f�o�b�O�p)
	*/
	void MoveRightDebugCamera(bool _plus);

	/**
	* �J�������W�㉺�ړ�(�f�o�b�O�p)
	*/
	void MoveUpDebugCamera(bool _plus);

	/**
	* �f�o�b�O�J������]
	*/
	void RotateDebugCamera(bool _right);

	/**
	* �f�o�b�O�J�����̈ړ��X�s�[�h���擾����֐��ł��B
	*/
	float& GetDebugMoveSpeed() { return MoveSpeed; }

	/**
	* �f�o�b�O�J�����̉�]���x���擾����֐��ł��B
	*/
	float& GetDebugRotateSpeed() { return RotateSpeed; }
};

