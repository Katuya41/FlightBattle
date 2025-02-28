#include "CCamera.h"
using namespace DirectX;

#define ADD_RADIUS_NUM (1)

//��������p
#define INITFOVRAD (45)

void CCamera::SetVector()
{
    //�s��擾
    XMMATRIX matrixRotate = RotateMatrix;

    // �����O�����x�N�g��
    XMVECTOR iniForward = XMVectorSet(0, 0, 1, 0);

    // ����������x�N�g��
    XMVECTOR iniUp = XMVectorSet(0, 1, 0, 0);

    // �����E�����x�N�g��
    XMVECTOR iniRight = XMVectorSet(1, 0, 0, 0);

    //�v���C���[�̊e�����x�N�g���쐬
    // �����O�����x�N�g���Ɖ�]�s����|����
    newForward
        = XMVector3TransformCoord(iniForward, matrixRotate);

    // ����������x�N�g���Ɖ�]�s����|����
    newUP
        = XMVector3TransformCoord(iniUp, matrixRotate);

    // �����E�����x�N�g���Ɖ�]�s����|����
    newRIGHT
        = XMVector3TransformCoord(iniRight, matrixRotate);

    // XMVECTOR��XMFLOAT3�ϊ�
    Forward.x = XMVectorGetX(newForward);
    Forward.y = XMVectorGetY(newForward);
    Forward.z = XMVectorGetZ(newForward);

    //// XMVECTOR��XMFLOAT3�ϊ�
    //Up.x = XMVectorGetX(newUP);
    //Up.y = XMVectorGetY(newUP);
    //Up.z = XMVectorGetZ(newUP);

    // XMVECTOR��XMFLOAT3�ϊ�
    Right.x = XMVectorGetX(newRIGHT);
    Right.y = XMVectorGetY(newRIGHT);
    Right.z = XMVectorGetZ(newRIGHT);

}

CCamera::CCamera()
{
    XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);            //�J�����̍��W
    XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);    //�J�����̉f������
    XMVECTOR vUp = XMVectorSet(up.x, up.y, up.z, 0);                //�J�����̏����

    // View�ϊ��s��쐬�̊֐����Ăяo��
    matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);
}

void CCamera::InitMatrixRotate()
{
    //���݂̊p�x�����Ƃɍs����쐬
    XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(Rotate.y));
    XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(Rotate.x));
    XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(Rotate.z));
    RotateMatrix = matrixRotateX * matrixRotateY * matrixRotateZ;
}

void CCamera::MoveForwardDebugCamera(bool _plus)
{
    // �O�i���鏈��
    // XMVECTOR��XMFLOAT3�ϊ�
    Forward.x = XMVectorGetX(newForward);
    Forward.y = XMVectorGetY(newForward);
    Forward.z = XMVectorGetZ(newForward);

    if (_plus)
    {
        eye.x += Forward.x * MoveSpeed;
        eye.y += Forward.y * MoveSpeed;
        eye.z += Forward.z * MoveSpeed;
    }
    else
    {
        eye.x -= Forward.x * MoveSpeed;
        eye.y -= Forward.y * MoveSpeed;
        eye.z -= Forward.z * MoveSpeed;
    }
}

void CCamera::MoveRightDebugCamera(bool _plus)
{
    // �O�i���鏈��
    // XMVECTOR��XMFLOAT3�ϊ�
    Right.x = XMVectorGetX(newRIGHT);
    Right.y = XMVectorGetY(newRIGHT);
    Right.z = XMVectorGetZ(newRIGHT);

    if (_plus)
    {
        eye.x += Right.x * MoveSpeed;
        eye.y += Right.y * MoveSpeed;
        eye.z += Right.z * MoveSpeed;
    }
    else
    {
        eye.x -= Right.x * MoveSpeed;
        eye.y -= Right.y * MoveSpeed;
        eye.z -= Right.z * MoveSpeed;
    }
}

void CCamera::MoveUpDebugCamera(bool _plus)
{
    //// �O�i���鏈��
    //// XMVECTOR��XMFLOAT3�ϊ�
    //Up.x = XMVectorGetX(newUP);
    //Up.y = XMVectorGetY(newUP);
    //Up.z = XMVectorGetZ(newUP);

    //if (_plus)
    //{
    //    eye.x += Up.x * MoveSpeed;
    //    eye.y += Up.y * MoveSpeed;
    //    eye.z += Up.z * MoveSpeed;
    //}
    //else
    //{
    //    eye.x -= Up.x * MoveSpeed;
    //    eye.y -= Up.y * MoveSpeed;
    //    eye.z -= Up.z * MoveSpeed;
    //}
}

void CCamera::RotateDebugCamera(bool _right)
{
    if (_right)
    {
        //�C�ӎ��ŉ�]������
        //�N�H�[�^�j�I���ł̉�]
        XMVECTOR QrX = XMQuaternionRotationAxis(newRIGHT, XMConvertToRadians(0.0f));
        XMVECTOR QrY = XMQuaternionRotationAxis(newUP, XMConvertToRadians(RotateSpeed));
        XMVECTOR QrZ = XMQuaternionRotationAxis(newForward, XMConvertToRadians(0.0f));

        XMMATRIX QmtX = XMMatrixRotationQuaternion(QrX);
        XMMATRIX QmtY = XMMatrixRotationQuaternion(QrY);
        XMMATRIX QmtZ = XMMatrixRotationQuaternion(QrZ);

        //XMMATRIX QMT = QmtX * QmtY * QmtZ;
        XMMATRIX QMT = QmtY * QmtX * QmtZ;
        RotateMatrix *= QMT;
    }
    else
    {
        //�C�ӎ��ŉ�]������
        //�N�H�[�^�j�I���ł̉�]
        XMVECTOR QrX = XMQuaternionRotationAxis(newRIGHT, XMConvertToRadians(0.0f));
        XMVECTOR QrY = XMQuaternionRotationAxis(newUP, XMConvertToRadians(-RotateSpeed));
        XMVECTOR QrZ = XMQuaternionRotationAxis(newForward, XMConvertToRadians(0.0f));

        XMMATRIX QmtX = XMMatrixRotationQuaternion(QrX);
        XMMATRIX QmtY = XMMatrixRotationQuaternion(QrY);
        XMMATRIX QmtZ = XMMatrixRotationQuaternion(QrZ);

        //XMMATRIX QMT = QmtX * QmtY * QmtZ;
        XMMATRIX QMT = QmtY * QmtX * QmtZ;
        RotateMatrix *= QMT;
    }
}

/**
*�J���������݂�����W��ݒ肷��֐��ł��B
* @param �J�����̍��W���Z�b�g���܂��B
*/
void CCamera::SetEye(DirectX::XMFLOAT3 _eyeposition)
{
	eye = _eyeposition;
}

/**
 *�J�������_�����W��ݒ肷��֐��ł��B
 * @param �J�������_�����W���Z�b�g���܂��B
 */
void CCamera::SetFocus(DirectX::XMFLOAT3 _focusposition)
{
	focus = _focusposition;
}

/**
 *�J�����̏������ݒ肷��֐��ł��B
 * @param �J�����̏�������Z�b�g���܂��B
 */
void CCamera::SetUp(DirectX::XMFLOAT3 upPosition)
{
	up = upPosition;
}

/**
 *�J�����̏�����𑝉�����֐��ł��B
 * @param �J�����̏�����ɑ����������W���Z�b�g���܂��B
 */
void CCamera::AddUp(DirectX::XMFLOAT3 upPosition)
{
	up.x += upPosition.x;
	up.y += upPosition.y;
	up.z += upPosition.z;
}

/**
 *�r���[�ϊ��s����v�Z����֐�(�X�V����)�ł��B
 */
void CCamera::Update()
{
	//XMFLOAT3����XMVECTOR�Ɍ^�ϊ�����
    if (!IsDebug)
    {
        XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);            //�J�����̍��W
        XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);    //�J�����̉f������
        XMVECTOR vUp = XMVectorSet(up.x, up.y, up.z, 0);                //�J�����̏����

        // View�ϊ��s��쐬�̊֐����Ăяo��
        matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);

        ExtractFrustum(frus, matrixView);
    }
    else
    {
        //�����v�Z
        SetVector();

        focus.x = eye.x + Forward.x * 10.0f;
        focus.y = eye.y + Forward.y * 10.0f;
        focus.z = eye.z + Forward.z * 10.0f;

        XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);            //�J�����̍��W
        XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);    //�J�����̉f������
        XMVECTOR vUp = newUP;                                           //�J�����̏����

        // View�ϊ��s��쐬�̊֐����Ăяo��
        matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);

        ExtractFrustum(frus, matrixView);
    }

}

/**
 *�v�Z�ς݃r���[�ϊ��s����擾����֐��ł��B
 * @return �v�Z�ς݃r���[�ϊ��s����擾���܂��B
 */
DirectX::XMMATRIX CCamera::GetViewMatrix()
{
	return matrixView;
}

/**
*�I�u�W�F�N�g�̓��e���@���擾����֐��ł��B
* @return �I�u�W�F�N�g�̓��e���@���擾���܂��B
*/
DirectX::XMMATRIX CCamera::GetProjectionMatrix()
{
	FXMMATRIX matrixProj3D = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(FovRadius), 16.0f / 9.0f, 1.0f, 3000.0f);

	// ���s���e�̍s��쐬
	// �����@�A�F�f���o���ʂ̉��c�̒���
	// �����B�C�F�f���o����Ԃ̉��s�i��O�ƍŉ��j
	FXMMATRIX matrixProj2D = XMMatrixOrthographicLH(
		16.0f * 0.3f, 9.0f * 0.3f, 0.0f, 3000.0f);

	if (projectionMode == _3D)
	{
		return matrixProj3D;
	}
	else
	{
		return matrixProj2D;
	}
}

/**
 *�J�����̊e���_���v�Z����֐��ł��B
 * @param �J�����̊e���_���i�[����ꏊ�ł��B
 * @param ���݂̃v���W�F�N�V�����s��ł�
 */
void CCamera::ExtractFrustum(Frustum& frustum, XMMATRIX viewProj)
{
    // Transpose the view-projection matrix
    XMMATRIX mat = XMMatrixTranspose(viewProj);

    // Left plane
    frustum.planes[0] = XMFLOAT4(
        mat.r[3].m128_f32[0] + mat.r[0].m128_f32[0],
        mat.r[3].m128_f32[1] + mat.r[0].m128_f32[1],
        mat.r[3].m128_f32[2] + mat.r[0].m128_f32[2],
        mat.r[3].m128_f32[3] + mat.r[0].m128_f32[3]);

    // Right plane
    frustum.planes[1] = XMFLOAT4(
        mat.r[3].m128_f32[0] - mat.r[0].m128_f32[0],
        mat.r[3].m128_f32[1] - mat.r[0].m128_f32[1],
        mat.r[3].m128_f32[2] - mat.r[0].m128_f32[2],
        mat.r[3].m128_f32[3] - mat.r[0].m128_f32[3]);

    // Bottom plane
    frustum.planes[2] = XMFLOAT4(
        mat.r[3].m128_f32[0] + mat.r[1].m128_f32[0],
        mat.r[3].m128_f32[1] + mat.r[1].m128_f32[1],
        mat.r[3].m128_f32[2] + mat.r[1].m128_f32[2],
        mat.r[3].m128_f32[3] + mat.r[1].m128_f32[3]);

    // Top plane
    frustum.planes[3] = XMFLOAT4(
        mat.r[3].m128_f32[0] - mat.r[1].m128_f32[0],
        mat.r[3].m128_f32[1] - mat.r[1].m128_f32[1],
        mat.r[3].m128_f32[2] - mat.r[1].m128_f32[2],
        mat.r[3].m128_f32[3] - mat.r[1].m128_f32[3]);

    // Near plane
    frustum.planes[4] = XMFLOAT4(
        mat.r[2].m128_f32[0],
        mat.r[2].m128_f32[1],
        mat.r[2].m128_f32[2],
        mat.r[2].m128_f32[3]);

    // Far plane
    frustum.planes[5] = XMFLOAT4(
        mat.r[3].m128_f32[0] - mat.r[2].m128_f32[0],
        mat.r[3].m128_f32[1] - mat.r[2].m128_f32[1],
        mat.r[3].m128_f32[2] - mat.r[2].m128_f32[2],
        mat.r[3].m128_f32[3] - mat.r[2].m128_f32[3]);
}

void CCamera::MoveInitFovRadius(float _moverad)
{
    if (FovRadius > INITFOVRAD)
        FovRadius -= _moverad;
    else if (FovRadius < INITFOVRAD)
        FovRadius += _moverad;
    else
        FovRadius = INITFOVRAD;
}
