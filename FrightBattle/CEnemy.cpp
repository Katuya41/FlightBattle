#include "CEnemy.h"
#include "CGame.h"
#include "CMisile.h"
#include "CScene.h"
#include "CCamera.h"
#include "CPlayer.h"
#include "CMisile.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include <random>
#include <iostream>
#include "CEnemyAttack.h"
//#include "CPlayerAttack.h"

#include "DebugUI.h"

using namespace DirectX;

void CEnemy::GetRandomNum(float _maxnum, float _minnum, float& _getnum)
{
	// ����������̏�����
	//�n�[�h�E�F�A�̗�����������g�p����
	//�����Z���k�E�c�C�X�^�@�Ɋ�Â��[������������
	std::random_device rd; 
	std::mt19937 rng(rd()); 

	// �����͈̔͂��w��
	std::uniform_real_distribution<float> dist(_maxnum, _minnum);

	_getnum = dist(rng);
}

void CEnemy::CheckRefCamera()
{
	XMMATRIX matrix = XMMatrixMultiply(CGame::Get()->GetCamera3D()->GetViewMatrix(),
		CGame::Get()->GetCamera3D()->GetProjectionMatrix());

	//�J�����̎�������\�z
	//�J�����̎�������\�z����6�̕��ʂ��`����
	Plane planes[6];

	// Left plane
	planes[0].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[0],
		0.0f);
	planes[0].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[0];
	NormalizePlane(planes[0]);

	// Right plane
	planes[1].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[0],
		0.0f);
	planes[1].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[0];
	NormalizePlane(planes[1]);

	// Top plane
	planes[2].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[1],
		0.0f);
	planes[2].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[1];
	NormalizePlane(planes[2]);

	// Bottom plane
	planes[3].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[1],
		0.0f);
	planes[3].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[1];
	NormalizePlane(planes[3]);

	// Near plane
	planes[4].normal = XMVectorSet(matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[2],
		0.0f);
	planes[4].distance = matrix.r[3].m128_f32[2];
	NormalizePlane(planes[4]);

	// Far plane
	planes[5].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[2],
		0.0f);
	planes[5].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[2];

	NormalizePlane(planes[5]);

	//�����蔻���OBB�ɐݒ�
	DirectX::BoundingOrientedBox obb = ColliderBoxOBB;

	//�����蔻��̒��_��ݒ�
	XMFLOAT3 fboxVertices[8];
	obb.GetCorners(fboxVertices);
	XMVECTOR boxVertices[8];
	for (int i = 0; i < 8; i++)
	{
		boxVertices[i] = XMLoadFloat3(&(fboxVertices[i]));
	}
	//XMVECTOR boxVertices[8] = XMLoadFloat3(fboxVertices);

	//�I�u�W�F�N�g����������ɂ��邩�ǂ����𔻒f
	int cnt = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			float distance = XMVectorGetX(XMPlaneDotCoord(planes[i].normal, boxVertices[j])) + planes[i].distance;
			if (distance <= 0.0f) {
				//�I�u�W�F�N�g����������ɂȂ�
				cnt++;
			}
		}
	}

	if (cnt >= 8)
	{
		//�I�u�W�F�N�g������������Ȃ�
		IsRefMainCamera = false;
	}
	else
	{
		//�I�u�W�F�N�g����������ɂ���
		IsRefMainCamera = true;
	}
}

void CEnemy::CheckRefCameraCenter()
{
	// ���݂̃v���W�F�N�V�����s����擾
	auto projMatrix = CGame::Get()->GetCamera3D()->GetProjectionMatrix();

	// �V��������p�iFOV�j��ݒ� (���݂�苷������)
	float fov = XMConvertToRadians(10); // 45�x����22.5�x�ɏk��
	

	// �A�X�y�N�g����/���N���b�v�ʂ̒l�͓���
	float aspectRatio = 16.0f / 9.0f;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	// �V�����v���W�F�N�V�����s����쐬
	projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

	XMMATRIX matrix = XMMatrixMultiply(CGame::Get()->GetCamera3D()->GetViewMatrix(),
		projMatrix);

	//�J�����̎�������\�z
	//�J�����̎�������\�z����6�̕��ʂ��`����
	Plane planes[6];

	// Left plane
	planes[0].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[0],
		0.0f);
	planes[0].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[0];
	NormalizePlane(planes[0]);

	// Right plane
	planes[1].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[0],
		0.0f);
	planes[1].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[0];
	NormalizePlane(planes[1]);

	// Top plane
	planes[2].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[1],
		0.0f);
	planes[2].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[1];
	NormalizePlane(planes[2]);

	// Bottom plane
	planes[3].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[1],
		0.0f);
	planes[3].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[1];
	NormalizePlane(planes[3]);

	// Near plane
	planes[4].normal = XMVectorSet(matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[2],
		0.0f);
	planes[4].distance = matrix.r[3].m128_f32[2];
	NormalizePlane(planes[4]);

	// Far plane
	planes[5].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[2],
		0.0f);
	planes[5].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[2];

	NormalizePlane(planes[5]);

	//�����蔻���OBB�ɐݒ�
	DirectX::BoundingOrientedBox obb = ColliderBoxOBB;

	//�����蔻��̒��_��ݒ�
	XMFLOAT3 fboxVertices[8];
	obb.GetCorners(fboxVertices);
	XMVECTOR boxVertices[8];
	for (int i = 0; i < 8; i++)
	{
		boxVertices[i] = XMLoadFloat3(&(fboxVertices[i]));
	}
	//XMVECTOR boxVertices[8] = XMLoadFloat3(fboxVertices);

	//�I�u�W�F�N�g����������ɂ��邩�ǂ����𔻒f
	int cnt = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			float distance = XMVectorGetX(XMPlaneDotCoord(planes[i].normal, boxVertices[j])) + planes[i].distance;
			if (distance <= 0.0f) {
				//�I�u�W�F�N�g����������ɂȂ�
				cnt++;
			}
		}
	}

	if (cnt >= 8)
	{
		//�I�u�W�F�N�g������������Ȃ�
		IsRefCenterCamera = false;
	}
	else
	{
		//�I�u�W�F�N�g����������ɂ���
		IsRefCenterCamera = true;
	}
}

void CEnemy::StateMoveUpdate(float _maxrotate)
{
	if (IsAttacked)
	{
		StateIsTargetUpdate();
	}
	else
	{
		//�ړ��悪�ݒ肳��Ă��邩
		if (OldRndNum != NULL)
		{
			//�ړ���ɓ��B�������ǂ���
			if (!CheckMovePos(MovePos[OldRndNum], CPlayerAttack::Get()->GetEnemyParameter().ThresHold))
			{
				//���Ă��Ȃ������ꍇ�A���̕����Ɍ����ĉ�]
				//�^�[�Q�b�g�̕����擾
				SetDirPos(MovePos[OldRndNum]);

				float Initangle = 0.0f;

				//��]������p�x���v�Z
				float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
				Initangle = acosf(dotProduct);

				//�p�x��x���Ōv�Z
				float an = XMConvertToDegrees(Initangle);

				XMVECTOR axis;

				if (an > 2.0f)
				{
					//��]�����鎲���v�Z
					axis = XMVector3Cross(newForward, TargetDir);
					if (XMVector3Equal(axis, XMVectorZero()))
					{
						axis.m128_f32[1] = 1.0f;
					}

					if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
					{
						//Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
						Initangle = XMConvertToRadians(_maxrotate);
					}

					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

					//��]�s��쐬
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//���݂̉�]�s��ɂ������킹��
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}
				else
				{
					//�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
					DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
					DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

					float angle = 0.0f;

					//��]������p�x���v�Z
					float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
					angle = acosf(dotProduct);

					//�p�x��x���Ōv�Z
					float an = XMConvertToDegrees(angle);

					if (an > 0.1f)
					{
						//��]�����鎲���v�Z
						XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

						if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
						{
							angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
						}

						XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

						//��]�s��쐬
						XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

						//���݂̉�]�s��ɂ������킹��
						RotateMatrix *= QMT;

						Rotation = ExtractRotationFromMatrix(RotateMatrix);
					}
				}
			}
			else
			{
				//�ړ���ύX
				SetNewMovePosition();
			}
		}
		else
		{
			//�ړ���ύX
			SetNewMovePosition();
		}
	}


}

void CEnemy::StateMoveForwardUpdate(float _maxrotate)
{
	////�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
	//DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
	//DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

	//float angle = 0.0f;

	////��]������p�x���v�Z
	//float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
	//// ���l�덷�h�~�̂��߁A�͈͂��N���b�v
	//dotProduct = max(-1.0f, min(1.0f, dotProduct));
	//angle = acosf(dotProduct);

	////�p�x��x���Ōv�Z
	//float an = XMConvertToDegrees(angle);

	//if (an > _maxrotate)
	//{
	//	//��]�����鎲���v�Z
	//	XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

	//	// �����[���x�N�g���̏ꍇ�������i���łɊ��S�ɐ����܂��͋t�����̏ꍇ�j
	//	if (XMVector3LengthSq(axisY).m128_f32[0] < 1e-6f) {
	//		axisY = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // �f�t�H���g�̉�]���iX���j
	//		return;
	//	}

	//	if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
	//	{
	//		angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
	//	}

	//	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

	//	//��]�s��쐬
	//	XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

	//	//���݂̉�]�s��ɂ������킹��
	//	RotateMatrix *= QMT;

	//	Rotation = ExtractRotationFromMatrix(RotateMatrix);
	//}

	//�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
	DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
	DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

	float Initangle = 0.0f;

	//��]������p�x���v�Z
	//���݂̐��ʂƈړ���̃x�N�g���v�Z
	float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
	if (dotProduct > 1.0f)
		Initangle = 0.0f;
	else
		Initangle = acosf(dotProduct);

	//�p�x��x���Ōv�Z
	float an = XMConvertToDegrees(Initangle);

	XMVECTOR axis;

	//�^�[�Q�b�g�܂ł̉�]�p�x���ő��]�p�x�𒴂��Ă���ꍇ
	if (an > _maxrotate)
	{
		//��]�����鎲���v�Z
		axis = XMVector3Cross(newForward, TargetDir);
		if (XMVector3Equal(axis, XMVectorZero()))
		{
			axis.m128_f32[1] = 1.0f;
		}

		an = _maxrotate;

		an = XMConvertToRadians(an);
		XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

		//��]�s��쐬
		XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

		//���݂̉�]�s��ɂ������킹��
		RotateMatrix *= QMT;

		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}
	else
	{
		//��]�����鎲���v�Z
		axis = XMVector3Cross(newForward, TargetDir);
		if (XMVector3Equal(axis, XMVectorZero()))
		{
			axis.m128_f32[1] = 1.0f;
		}

		an = XMConvertToRadians(an);
		XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

		//��]�s��쐬
		XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

		//���݂̉�]�s��ɂ������킹��
		RotateMatrix *= QMT;

		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}
}

void CEnemy::StateIsTargetUpdate()
{
	//�J�����ɉf���Ă���
	if (IsRefMainCamera || IsAttacked)
	{
		//�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
		DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
		DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

		float angle = 0.0f;

		//��]������p�x���v�Z
		float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
		angle = acosf(dotProduct);

		//�p�x��x���Ōv�Z
		float an = XMConvertToDegrees(angle);

		if (an > 0.1f)
		{
			//��]�����鎲���v�Z
			XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
			{
				angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
			}

			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

			//��]�s��쐬
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//���݂̉�]�s��ɂ������킹��
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);
		}
	}
	else
	{
		StateMoveUpdate(CPlayerAttack::Get()->GetEnemyMaxRotation());
	}

}

bool CEnemy::StateAttackUpdate()
{
	//�v���C���[�̍��W�擾
	XMFLOAT3 FollowPos = CGame::Get()->GetPlayer()->Position;

	//�^�[�Q�b�g�̕����擾
	SetDirPos(FollowPos);

	float Initangle = 0.0f;

	//��]������p�x���v�Z
	float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
	if (dotProduct > 1.0f)
		Initangle = 0.0f;
	else
		Initangle = acosf(dotProduct);

	//�p�x��x���Ōv�Z
	float an = XMConvertToDegrees(Initangle);

	if (an <= 40.0f)
	{
		//�ŏ��Ƀv���C���[�Ƃ̋����𔻒�
		if (PlayerDistance <= (float)CPlayerAttack::Get()->GetEnemyParameter().Max_Attack_Distance)
		{
			//�^�[�Q�b�g�J�n
			if (!IsAttackTarget)
			{
				IsAttackTarget = true;
				CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
				TargetStartTime = GetTickCount64();
			}

			//�^�[�Q�b�g���Ԃ��߂�����
			if (GetTickCount64() >= TargetStartTime + (ULONGLONG)CPlayerAttack::Get()->GetEnemyParameter().Target_Time && IsAttackTarget)
			{
				//�~�T�C�����󂶂�Ȃ��ꍇ
				if (pMisile != nullptr)
				{
					if (pMisile->GetActive() == false)
					{

						//CEnemyAttack::Get()->AttackTarget(this);
						pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * -1.0f,
																									   Position.y + GetUp().y * -1.0f,
																									   Position.z + GetUp().z * -1.0f }, Rotation);
						ShotTime = GetTickCount64();
						TargetStartTime = 0;
						IsAttackTarget = false;
						EndChaseAndAttack = true;
						Times = GetTickCount64();
						return true;
					}
				}
				else
				{
					//pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * -1.0f,
						//																		  Position.y + GetUp().y * -1.0f,
						//																		  Position.z + GetUp().z * -1.0f }, Rotation);
					CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * -1.0f,
																		  Position.y + GetUp().y * -1.0f,
																		  Position.z + GetUp().z * -1.0f }, Rotation);
					ShotTime = GetTickCount64();
					TargetStartTime = 0;
					IsAttackTarget = false;
					CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
					EndChaseAndAttack = true;
					Times = GetTickCount64();
					return true;
				}
			}
		}
		else
		{
			TargetStartTime = 0;
			IsAttackTarget = false;
			CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
		}
	}
	else
	{
		TargetStartTime = 0;
		IsAttackTarget = false;
		CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
	}

	return false;
}

void CEnemy::StateRunUpdate()
{
}

void CEnemy::StateFollowEntourageUpdate(float _maxspeed)
{
	//���ψʒu�Ɍ�����
	//�ړ���ɓ��B�������ǂ���
	if (!CheckMovePos(TargetPosition, 0.5f))
	{
		//�^�[�Q�b�g�̕����擾
		SetDirPos(TargetPosition);

		float Initangle = 0.0f;

		//��]������p�x���v�Z
		float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
		if (dotProduct > 1.0f)
			Initangle = 0.0f;
		else
			Initangle = acosf(dotProduct);

		//�p�x��x���Ōv�Z
		float an = XMConvertToDegrees(Initangle);

		XMVECTOR axis;

		if (an >= 2.0f)
		{
			//��]�����鎲���v�Z
			axis = XMVector3Cross(newForward, TargetDir);
			if (XMVector3Equal(axis, XMVectorZero()))
			{
				axis.m128_f32[1] = 1.0f;
			}

			////��]�p�x��ݒ�
			//if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
			//{
			//	//Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
			//	Initangle = an;
			//}

			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
			{
				//angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
				an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;
				//angle = an;
			}

			an = XMConvertToRadians(an);
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

			//��]�����鎲�x�N�g���ݒ�
			//MVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

			//��]�s��쐬
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//���݂̉�]�s��ɂ������킹��
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);

			////�G�̃X�s�[�h�ݒ�
			//SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed / 2.0f));
			//if (MoveSpeed <= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
			//	MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;

			//�G�̃X�s�[�h�ݒ�
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
			if (MoveSpeed >= _maxspeed)
			{
				MoveSpeed = _maxspeed;
			}

			// 2�_�Ԃ̋������v�Z
			float distance = sqrt((TargetPosition.x - Position.x) * (TargetPosition.x - Position.x) +
				(TargetPosition.y - Position.y) * (TargetPosition.y - Position.y) +
				(TargetPosition.z - Position.z) * (TargetPosition.z - Position.z));

			if (distance >= 10)
			{
				//�G�̃X�s�[�h�ݒ�
				SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed * 2.0f);
				if (MoveSpeed >= _maxspeed + 0.3f)
				{
					MoveSpeed = _maxspeed + 0.3f;
				}
			}
		}
	}
	else
	{
		//�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
		DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
		DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

		float angle = 0.0f;

		//��]������p�x���v�Z
		float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
		// ���l�덷�h�~�̂��߁A�͈͂��N���b�v
		dotProduct = max(-1.0f, min(1.0f, dotProduct));
		angle = acosf(dotProduct);

		//�p�x��x���Ōv�Z
		float an = XMConvertToDegrees(angle);

		if (an > 5.0f)
		{
			//��]�����鎲���v�Z
			XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

			// �����[���x�N�g���̏ꍇ�������i���łɊ��S�ɐ����܂��͋t�����̏ꍇ�j
			if (XMVector3LengthSq(axisY).m128_f32[0] < 1e-6f) {
				axisY = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // �f�t�H���g�̉�]���iX���j
			}

			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
			{
				//angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
				an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;
				//angle = an;
			}

			an = XMConvertToRadians(an);
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, an);
			//XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

			//��]�s��쐬
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//���݂̉�]�s��ɂ������킹��
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);
		}

		//�G�̃X�s�[�h�ݒ�
		SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed));
		if (MoveSpeed < 0.1f)
			MoveSpeed = 0.1f;
		else if (MoveSpeed >= _maxspeed)
			MoveSpeed = _maxspeed;
		else if (MoveSpeed >= 0.2f)
			MoveSpeed = 0.2f;
	}
}

void CEnemy::StateDefenceEntourageUpdate()
{
	//�{�X�͐��̂̎�芪������Ɏ��点�Ă���
	//�v���C���[���U�����s�����Ƃ��ɔ���
	//���˂����~�T�C���̐��ɉ����Ėh�䂳����G�̐�������
	//�h�q�̎�芪�����g�p���ă~�T�C����h�䂷��
	//�����l�@
	//1:�~�T�C�����v���C���[���甭�˂���� (����擾�\)
	//2:���˂��ꂽ�~�T�C���̗\�z�O�����v�Z (�������)
	//3:���̋O���ɓ���悤�Ɏ�芪���Ɉړ��w�����o�� (�ړ���̍��W���킩��Ή\)
	//4:�~�T�C������芪���ɂ������Ėh�� (�����鏈���͉\)

	//�v���C���[���~�T�C����������������
	//�~�T�C���̃^�[�Q�b�g�ɂȂ��Ă��邩
	if (IsAttacked)
	{
		//�{�X���^�[�Q�b�g�Ƃ����~�T�C�������ׂĎ擾
		//std::deque<CMisile> Misiles;
		//Misiles.push_back()

		//�擾�����~�T�C���̋O���\�����v�Z����

		//���̌��ʂ����Ɏ������ړ�����ׂ����W���擾����
		//�����̍��W����O���\�z�̈ʒu�܂ł̒����������A1�b�قǈړ�������Ɍ��̏ꏊ�ɖ߂�
	}

	
}

void CEnemy::StateAttackEntourageUpdate()
{
	//�v���C���[���U�������ꍇ�A�I��
	if (StateAttackUpdate())
	{
		ShotTime = GetTickCount64();
		TargetStartTime = 0;
		IsAttackTarget = false;
		EndChaseAndAttack = true;
		CPlayerAttack::Get()->SetEntourageAddNum(-1);
		//EnemyState = ENTOURAGE_FOLLOW;
		EnemyState = ENTOURAGE_FRIGHT;
	}

	//�ړ���ɓ��B�������ǂ���
	if (!CheckMovePos(TargetPosition, 0.5f))
	{
		////�v���C���[���U�������ꍇ�A�I��
		//if (StateAttackUpdate())
		//{
		//	ShotTime = GetTickCount64();
		//	TargetStartTime = 0;
		//	IsAttackTarget = false;
		//	EndChaseAndAttack = true;
		//	EnemyState = ENTOURAGE_FOLLOW;
		//}

		//�^�[�Q�b�g�̕����擾
		SetDirPos(TargetPosition);

		float Initangle = 0.0f;

		//��]������p�x���v�Z
		float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
		if (dotProduct > 1.0f)
			Initangle = 0.0f;
		else
			Initangle = acosf(dotProduct);

		//�p�x��x���Ōv�Z
		float an = XMConvertToDegrees(Initangle);

		XMVECTOR axis;

		if (an >= 10.0f)
		{
			//��]�����鎲���v�Z
			axis = XMVector3Cross(newForward, TargetDir);
			if (XMVector3Equal(axis, XMVectorZero()))
			{
				axis.m128_f32[1] = 1.0f;
			}

			//��]�p�x��ݒ�
			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.0f)
			{
				Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.0f);
			}

			//��]�����鎲�x�N�g���ݒ�
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

			//��]�s��쐬
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//���݂̉�]�s��ɂ������킹��
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);

			////�G�̃X�s�[�h�ݒ�
			//SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed / 2.0f));
			//if (MoveSpeed <= 0.1f)
			//	MoveSpeed = 0.1f;

			//�G�̃X�s�[�h�ݒ�
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed);
			if (CGame::Get()->GetPlayer()->GetMoveSpeed())
			{
				MoveSpeed = CGame::Get()->GetPlayer()->GetMoveSpeed();
			}
		}
		else
		{
			//�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
			DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
			DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

			float angle = 0.0f;

			//��]������p�x���v�Z
			float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
			// ���l�덷�h�~�̂��߁A�͈͂��N���b�v
			dotProduct = max(-1.0f, min(1.0f, dotProduct));
			angle = acosf(dotProduct);

			//�p�x��x���Ōv�Z
			float an = XMConvertToDegrees(angle);

			if (an > 10.0f)
			{
				//��]�����鎲���v�Z
				XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

				// �����[���x�N�g���̏ꍇ�������i���łɊ��S�ɐ����܂��͋t�����̏ꍇ�j
				if (XMVector3LengthSq(axisY).m128_f32[0] < 1e-6f) {
					axisY = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // �f�t�H���g�̉�]���iX���j
				}

				if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
				{
					angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
				}

				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

				//��]�s��쐬
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//���݂̉�]�s��ɂ������킹��
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);
			}
		}
	}
}

CEnemy::CEnemy()
{
	ColliderObject = new CGameObject();

	//Init();
}

CEnemy::~CEnemy()
{
	isActive = false;
}

void CEnemy::Init()
{
	switch (Tag)
	{
	case CGameObject::TAG::FRIGHT_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().FrightEnemy_HP;
		ColliderObject->Position = Position;

		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxOBB();

		break;
	}
	case CGameObject::TAG::GROUND_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().GroundEnemy_HP;
		ShotTime = 0;

		ColliderObject->Position = Position;
		ColliderObject->Scaling = { 0.6f, 0.7f, 0.6f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxAABB(ColliderObject->Position, ColliderObject->Scaling);
		break;
	}
	case CGameObject::TAG::FOLLOW_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().FrightEnemy_HP;

		ColliderObject->Position = Position;
		//ColliderObject->Scaling = Scaling;
		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxOBB();

		EndChaseAndAttack = true;
		Times = GetTickCount64();

		break;
	}
	case TAG::ENTOURAGE_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().EntourageEnemy_HP;

		//�{�X�̍��W
		DirectX::XMFLOAT3 BossPos = CPlayerAttack::Get()->GetBossEnemy()->Position;

		//�{�X�̌��
		//�ړ�����ꏊ�������_���Ȑ����Ŏ擾
		GetRandomNum(-4.0f, 4.0f, EntouragePos.x);
		GetRandomNum(-4.0f, 4.0f, EntouragePos.y);

		// �����_���Ȑ������擾
		if (EntouragePos.x < 0)
			EntouragePos.x -= 2.0f;
		else
			EntouragePos.x += 2.0f;
		if (EntouragePos.y < 0)
			EntouragePos.y -= 2.0f;
		else
			EntouragePos.y += 2.0f;
		GetRandomNum(1.0f, 3.0f, EntouragePos.z);
		//EntouragePos.z = 1.0f;

		TargetPosition = { BossPos.x - (Forward.x * EntouragePos.z) + (RIGHT.x * EntouragePos.x) + (UP.x * EntouragePos.y)  ,
						   BossPos.y - (Forward.y * EntouragePos.z) + (RIGHT.y * EntouragePos.x) + (UP.y * EntouragePos.y) ,
						   BossPos.z - (Forward.z * EntouragePos.z) + (RIGHT.z * EntouragePos.x) + (UP.z * EntouragePos.y) };

		IsInitChengeBoidPos = true;
		Times = GetTickCount64();

		Position = TargetPosition;

		MoveSpeed = (CPlayerAttack::Get()->GetBossEnemy()->GetMoveSpeed());

		RotateMatrix = CPlayerAttack::Get()->GetBossEnemy()->GetRotateMatrix();

		EndChaseAndAttack = true;

		//�h�q����5�̈ȉ��Ȃ�h�q�ɐݒ�
		if (CPlayerAttack::Get()->GetEntourageDefNum() < 5)
		{
			EnemyState = ENTOURAGE_FOLLOW;
			CPlayerAttack::Get()->SetEntourageDefAddNum(1);
		}
		else
		{
			EnemyState = ENTOURAGE_FRIGHT;
		}

		//EnemyState = ENTOURAGE_FOLLOW;
		//EnemyState = ENTOURAGE_FRIGHT;
		//EnemyState = ENTOURAGE_ATTACK;

		ColliderObject->Position = Position;

		//ColliderObject->Position = { Position.x + -0.1f * GetForward().x,
		//							Position.y + -0.1f * GetForward().y,
		//							Position.z + -0.1f * GetForward().z };

		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxOBB();
		break;
	}
	case TAG::BOSS_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().BossEnemy_HP;

		ShotTime = GetTickCount64();

		ColliderObject->Position = Position;

		//ColliderObject->Position = { Position.x + -0.1f * GetForward().x,
		//							Position.y + -0.1f * GetForward().y,
		//							Position.z + -0.1f * GetForward().z };

		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetMovePos({ 200, 0, 10 }, 0);		//�E�O
		SetMovePos({ -200, 0, 10 }, 1);		//���O
		SetMovePos({ -200, 0,-10 }, 2);		//�����
		SetMovePos({ 200, 0,-10 }, 3);		//�E���

		SetColliderBoxOBB();
		break;
	}
	case TAG::GUN_ENEMY:
	{
		Hp = 10;

		SetColliderBoxOBB();
		break;
	}
	}

	if (Tag != BOSS_ENEMY)
	{
		SetMovePos({ 80,50,50 }, 0);		//�E�O
		SetMovePos({ -80,-10,50 }, 1);		//���O
		SetMovePos({ -80,50,-50 }, 2);		//�����
		SetMovePos({ 80,-10,-50 }, 3);		//�E���
	}

	isActive = true;
	//�^�[�Q�b�g�ł��邩
	CanAttack = true;

	//�^�[�Q�b�g���ǂ���
	IsTarget = false;

	//�v���C���[�̍U���ɒǂ��Ă��邩
	IsAttacked = false;

	SetCanAttack(false);
}

void CEnemy::Update()
{
	if (GetTag() == INITENEMY)
	{
		Position = { CGame::Get()->GetPlayer()->Position.x + 10.0f * CGame::Get()->GetPlayer()->GetForward().x,
					 CGame::Get()->GetPlayer()->Position.y + 10.0f * CGame::Get()->GetPlayer()->GetForward().y,
					 CGame::Get()->GetPlayer()->Position.z + 10.0f * CGame::Get()->GetPlayer()->GetForward().z
		};
		return;
	}

	if (!isActive)
		return;

	if (Hp <= 0)
	{
		isActive = false;
		return;
	}

	//�J�����ɉf���Ă��邩����
	CheckRefCamera();

	//���S�J�����ɒʂ��Ă��邩����
	CheckRefCameraCenter();

	//���݂̃x�N�g���v�Z
	CGameObject::SetVector();

	//�v���C���[�Ƃ̋������v�Z
	PlayerDistance = CheckDistance(CGame::Get()->GetPlayer(), {});

	switch (Tag)
	{
	case CGameObject::TAG::FRIGHT_ENEMY:	UpdateFrightEnemy(); break;
	case CGameObject::TAG::GROUND_ENEMY:	UpdateGroundEnemy(); break;
	case CGameObject::TAG::FOLLOW_ENEMY:	UpdateFollowEnemy(); break;
	case CGameObject::TAG::ENTOURAGE_ENEMY:	UpdateEntourageEnemy(); break;
	case CGameObject::TAG::GUN_ENEMY:		UpdateGunEnemy();	 break;
	case CGameObject::TAG::BOSS_ENEMY:		UpdateBossEnemy();	 break;
	default:
		break;
	}

	//�~�j�}�b�v�X�V����
	MiniMapObjectUpdate();
}

void CEnemy::UpdateFrightEnemy()
{
	//�v���C���[�Ƃ̋������v�Z
	PlayerDistance = CheckDistance(CGame::Get()->GetPlayer(), {});

	//��Ԃ�ݒ�
	//
	if (IsTarget && IsRefMainCamera && PlayerDistance <= 100)
	{
		EnemyState = MOVE_FORWARD;
	}
	else
	{
		EnemyState = MOVE;
	}

	if (IsAttacked)
	{
		EnemyState = MOVE_FORWARD;
	}

	switch (EnemyState)
	{
	case CEnemy::MOVE:
		StateMoveUpdate(CPlayerAttack::Get()->GetEnemyMaxRotation());
		break;
	case CEnemy::MOVE_FORWARD:
		StateMoveForwardUpdate(2.0f);
		break;
	default:
		break;
	}

	//�G�̃X�s�[�h�ݒ�
	SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Fright_AddSpeed);
	if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Fright_MaxSpeed)
	{
		MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Fright_MaxSpeed;
	}

	//�ړ�����
	CGameObject::MoveUpdate();

	//eJetEffect->Position = { Position.x + GetForward().x * -0.7f + GetUp().x * -0.35f + GetRight().x * 0.02f,
	//					 Position.y + GetForward().y * -0.7f + GetUp().y * -0.35f + GetRight().x * 0.02f,
	//					 Position.z + GetForward().z * -0.7f + GetUp().z * -0.35f + GetRight().x * 0.02f };
	//eJetEffect->Rotation = Rotation;
	//eJetEffect->SetInitRotateMatrix();

	//�����蔻��ݒ�
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//�G�̓����蔻��쐬
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateGroundEnemy()
{
	//�J�����ɉf���Ă��邩����
	ChackRefCamera();

	//�ŏ��Ƀv���C���[�Ƃ̋����𔻒�
	if (CheckDistance(CGame::Get()->GetPlayer(), {}) <= (float)CPlayerAttack::Get()->GetEnemyParameter().Max_Attack_Distance
		&& GetTickCount64() >= ShotTime + (ULONGLONG)CPlayerAttack::Get()->GetEnemyParameter().GroundEnemy_AttackTime)
	{
		//�^�[�Q�b�g�J�n
		if (!IsAttackTarget)
		{
			if (pMisile != nullptr)
			{
				if (pMisile->GetActive() == false)
				{
					IsAttackTarget = true;
					CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
					TargetStartTime = GetTickCount64();
				}
			}
			else
			{
				IsAttackTarget = true;
				CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
				TargetStartTime = GetTickCount64();
			}
		}
		else
		{
			CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
		}

		//�^�[�Q�b�g���Ԃ��߂�����
		if (GetTickCount64() >= TargetStartTime + (ULONGLONG)CPlayerAttack::Get()->GetEnemyParameter().Target_Time && IsAttackTarget)
		{
			//�~�T�C������
			if (pMisile != nullptr)
			{
				//�~�T�C�����󂶂�Ȃ��ꍇ
				if (pMisile->GetActive() == false)
				{
					//CEnemyAttack::Get()->AttackTarget(this);
					pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * 1.0f,
																								  Position.y + GetUp().y * 1.0f,
																								  Position.z + GetUp().z * 1.0f }, { -90.0f,0.0f,0.0f });
					ShotTime = GetTickCount64();
					TargetStartTime = 0;
					IsAttackTarget = false;
					CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
				}
			}
			else
			{
				//�~�T�C������̎�
				pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * 1.0f,
																							   Position.y + GetUp().y * 1.0f,
																							   Position.z + GetUp().z * 1.0f }, { -90.0f,0.0f,0.0f });
				ShotTime = GetTickCount64();
				TargetStartTime = 0;
				IsAttackTarget = false;
				CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
			}
		}
	}
	else
	{
		TargetStartTime = 0;
		IsAttackTarget = false;
		CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
	}

}

void CEnemy::UpdateFollowEnemy()
{

	//�v���C���[�̏������̍��W�擾
	XMFLOAT3 FollowPos = CGame::Get()->GetPlayer()->Position;
	//FollowPos = { FollowPos.x - (CGame::Get()->GetPlayer()->GetForward().x) * 4.0f,
	//			  FollowPos.y - (CGame::Get()->GetPlayer()->GetForward().y) * 4.0f,
	//			  FollowPos.z - (CGame::Get()->GetPlayer()->GetForward().z) * 4.0f, };

	//���������ȉ��Ȃ�MOVEFORWARD�Ɉڍs
	if (PlayerDistance <= 5.0f)
	{
		ShotTime = GetTickCount64();
		TargetStartTime = 0;
		IsAttackTarget = false;
		CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
		EndChaseAndAttack = true;
		Times = GetTickCount64();
	}

	if (!EndChaseAndAttack)
	{
		//�ړ���ɓ��B�������ǂ���
		if (!CheckMovePos(FollowPos, 5.0f))
		{
			if (StateAttackUpdate())
			{
				EndChaseAndAttack = true;
				Times = GetTickCount64();
			}

			//�^�[�Q�b�g�̕����擾
			SetDirPos(FollowPos);

			float Initangle = 0.0f;

			//��]������p�x���v�Z
			float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
			if (dotProduct > 1.0f)
				Initangle = 0.0f;
			else
				Initangle = acosf(dotProduct);

			//�p�x��x���Ōv�Z
			float an = XMConvertToDegrees(Initangle);

			XMVECTOR axis;

			if (an > 0.1f)
			{
				//��]�����鎲���v�Z
				axis = XMVector3Cross(newForward, TargetDir);
				if (XMVector3Equal(axis, XMVectorZero()))
				{
					axis.m128_f32[1] = 1.0f;
				}

				//��]�p�x��ݒ�
				if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
				{
					Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
				}

				//��]�����鎲�x�N�g���ݒ�
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

				//��]�s��쐬
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//���݂̉�]�s��ɂ������킹��
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);
			}
			else
			{
				//�����@�̂�Z���p�x���X���Ă�����A�����ɖ߂�
				DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
				DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

				float angle = 0.0f;

				//��]������p�x���v�Z
				float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
				angle = acosf(dotProduct);

				//�p�x��x���Ōv�Z
				float an = XMConvertToDegrees(angle);

				if (an > 0.1f)
				{
					//��]�����鎲���v�Z
					XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

					if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
					{
						angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
					}

					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

					//��]�s��쐬
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//���݂̉�]�s��ɂ������킹��
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}

				if (StateAttackUpdate())
				{
					EndChaseAndAttack = true;
					Times = GetTickCount64();
				}
			}

			//�G�̃X�s�[�h�ݒ�
			SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed / 2.0f));
			if (MoveSpeed <= 0.1f)
				MoveSpeed = 0.1f;
		}
	}
	else
	{
		if (GetTickCount64() < Times + CPlayerAttack::Get()->GetEnemyParameter().Attack_CoolTime)
			StateMoveForwardUpdate(2.0f);
		else
			EndChaseAndAttack = false;

		//if (GetTickCount64() >= Times + FOLLOW_CCOLTIME)
		//	AAA = false;
	}

	if (EndChaseAndAttack)
	{
		//�G�̃X�s�[�h�ݒ�
		SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
		if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
		{
			MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;
		}
	}
	else
	{
		if (PlayerDistance >= 200)
		{
			//�G�̃X�s�[�h�ݒ�
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
			if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
			{
				MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;
			}
		}
		else
		{
			//�G�̃X�s�[�h�ݒ�
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
			if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
			{
				MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;
			}
		}
	}

	//�ړ�����
	CGameObject::MoveUpdate();

	//�����蔻��ݒ�
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//�G�̓����蔻��쐬
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateEntourageEnemy()
{
	//�{�X���|���ꂽ�ꍇ�AFOLLOW_ENEMY���[�h�ɕύX
	if (CPlayerAttack::Get()->GetBossEnemy() == nullptr || !CPlayerAttack::Get()->GetBossEnemy()->GetActive())
	{
		Tag = FOLLOW_ENEMY;
		return;

		if (!EndChaseAndAttack)
		{
			EndChaseAndAttack = true;
			CPlayerAttack::Get()->SetEntourageAddNum(-1);
		}
	}

	//��莞�Ԍ�ɁA�ʒu��ύX����
	if (!IsInitChengeBoidPos || (((GetTickCount64() >= Times + CPlayerAttack::Get()->GetEnemyParameter().Entourage_ChengePos_Time) && EndChaseAndAttack)))
	{
		switch (EnemyState)
		{
		case CEnemy::ENTOURAGE_FOLLOW:
		{
			break;
		}
		case CEnemy::ENTOURAGE_FRIGHT:

			break;
		case CEnemy::ENTOURAGE_DEFENCE:
		{
			break;
		}
		case CEnemy::ENTOURAGE_ATTACK:
			//�ړ�����ꏊ�������_���Ȑ����Ŏ擾(3�`-3)
			GetRandomNum(-2.0f, 2.0f, EntouragePos.x);
			GetRandomNum(-2.0f, 2.0f, EntouragePos.y);
			GetRandomNum(10.0f, 20.0f, EntouragePos.z);
			break;
		default:
			break;
		}

		IsInitChengeBoidPos = true;
		Times = GetTickCount64();
	}

	//�v���C���[�U����Ԃ̂݁A��Ƀv���C���[�̈ʒu���擾����
	if (EnemyState == ENTOURAGE_ATTACK)
	{
		FollowPos = CGame::Get()->GetPlayer()->Position;

		//�G�̃X�s�[�h�ݒ�
		SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed);
		if (MoveSpeed >= CGame::Get()->GetPlayer()->GetMoveSpeed() * 0.8f)
		{
			MoveSpeed = CGame::Get()->GetPlayer()->GetMoveSpeed() * 0.8f;
		}
	}
	else if (EnemyState == ENTOURAGE_FRIGHT)
	{
		//�{�X�𒆐S�Ƃ����ړ��|�C���g�擾
		for (int i = 0; i < 8; i++)
		{
			MovePos[i] = CPlayerAttack::Get()->GetBossEnemy()->GetEntourageMovePos(i);
		}

		//�G�̃X�s�[�h�ݒ�
		SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed);
		if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed)
		{
			MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed;
		}
	}
	else
	{
		FollowPos = CPlayerAttack::Get()->GetBossEnemy()->Position;
	}

	//�ړ��|�C���g�ݒ�
	TargetPosition = { FollowPos.x - (Forward.x * EntouragePos.z) + (RIGHT.x * EntouragePos.x) + (UP.x * EntouragePos.y),
					   FollowPos.y - (Forward.y * EntouragePos.z) + (RIGHT.y * EntouragePos.x) + (UP.y * EntouragePos.y),
					   FollowPos.z - (Forward.z * EntouragePos.z) + (RIGHT.z * EntouragePos.x) + (UP.z * EntouragePos.y) };

	//�X�V����
	switch (EnemyState)
	{
	case CEnemy::ENTOURAGE_DEFENCE: StateFollowEntourageUpdate((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed); break;
	case CEnemy::ENTOURAGE_FRIGHT: StateMoveUpdate(CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxRotate); break;
	case CEnemy::ENTOURAGE_FOLLOW: StateFollowEntourageUpdate((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed); break;
	case CEnemy::ENTOURAGE_ATTACK: StateAttackEntourageUpdate(); break;
	default:
		break;
	}

	//�ړ�����
	CGameObject::MoveUpdate();

	//�����蔻��ݒ�
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//�G�̓����蔻��쐬
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateBossEnemy()
{
	//�{�X�̎��͂̍��W�Ɉړ��|�C���g��ݒ�
	float PointNum = CPlayerAttack::Get()->GetEnemyParameter().Entourage_Move_Pos_Dis;
	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * PointNum) + (Forward.z * PointNum) }, 0);		//�E��O

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * PointNum) + (Forward.z * PointNum) }, 1);	//����O

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * PointNum) + (Forward.z * -PointNum) }, 2);	//������

	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * PointNum) + (Forward.z * -PointNum) }, 3);	//�E����

	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * -PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * -PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * -PointNum) + (Forward.z * PointNum) }, 4);	//�E���O

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * -PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * -PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * -PointNum) + (Forward.z * PointNum) }, 5);	//�����O

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * -PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * -PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * -PointNum) + (Forward.z * -PointNum) }, 6);	//�������

	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * -PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * -PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * -PointNum) + (Forward.z * -PointNum) }, 7);	//�E�����

	if (InsertEntourage)
	{
		StateMoveUpdate(CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxRotate);
	}
	else
	{
		if (GetTickCount64() >= Times)
		{
			//��芪��������
			for (int i = 0; i < CPlayerAttack::Get()->GetEnemyParameter().InsertEntourageNum; i++)
			{
				// �����_���Ȑ������擾
				float randomValueX/* = dist(rng)*/;
				GetRandomNum(-6.0f,6.0f, randomValueX);
				float randomValueY/* = dist(rng)*/;
				GetRandomNum(-6.0f, 6.0f, randomValueY);
				float randomValueZ/* = dist2(rng)*/;
				GetRandomNum(2.0f, 4.0f, randomValueZ);

				DirectX::XMFLOAT3 InsertPos = { Position.x - (Forward.x * randomValueZ) + (RIGHT.x * randomValueX) + (UP.x * randomValueY)  ,
												Position.y - (Forward.y * randomValueZ) + (RIGHT.y * randomValueX) + (UP.y * randomValueY) ,
												Position.z - (Forward.z * randomValueZ) + (RIGHT.z * randomValueX) + (UP.z * randomValueY)  };

				//��芪��������
				CGame::Get()->InsertEnemy(CGameObject::TAG::ENTOURAGE_ENEMY, InsertPos, Rotation, { 1.5f,1.5f,1.5f });
			}

			//�@�֏e�G������
			for (int j = 0; j < 5; j++)
			{
				CGame::Get()->InsertEnemy(CGameObject::TAG::GUN_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 0.5f, 0.5f, 0.5f });
			}

			InsertEntourage = true;
			//Times = 0;
			CPlayerAttack::Get()->SetTrueBreak();
		}
	}

	//�G�̃X�s�[�h�ݒ�
	SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Boss_AddSpeed);
	if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxSpeed)
	{
		MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxSpeed;
	}

	//��芪���ɍU���x�����s��
	//�S�Ă̓G�̍U���x�����I�����Ă��邩��
	//�U���x���̃N�[���^�C�����o�߂�����
	if (CPlayerAttack::Get()->GetEntourageAttackNum() == 0 && DebugIsAttack)
	{
		//�U���x�����o����Ă��Ȃ��Ƃ�
		if (!IsAttackEntourage)
		{
			//�U���̂��߂̃N�[���^�C���J�n
			ShotTime = GetTickCount64();
			IsAttackEntourage = true;
		}

		if (GetTickCount64() >= ShotTime + CPlayerAttack::Get()->GetEnemyParameter().Entourage_Attack_CoolTime)
		{
			CPlayerAttack::Get()->SetAttackEntourageEnemy();
			//ShotTime = GetTickCount64();
			IsAttackEntourage = false;
		}
	}

	//��芪���̖h�q�w���ݒ�
	if (PlayerDistance < 30 && IsTarget)
	{
		//�v���C���[�Ƃ̋������߂����^�[�Q�b�g����Ă���
		CPlayerAttack::Get()->SetDefenceEntourageEnemy(true);
	}
	else
	{
		//�v���C���[�Ƃ̋����������܂��̓^�[�Q�b�g����Ă��Ȃ�
		CPlayerAttack::Get()->SetDefenceEntourageEnemy(false);
	}

	//�v���C���[�Ƃ̋������߂��Ƃ����^�[�Q�b�g����Ă���Ƃ�
	//��芪���̈ꕔ���v���C���[�̌��ɔz�u����

	OldPosition = Position;

	//�ړ�����
	CGameObject::MoveUpdate();

	//�ړ��x�N�g���v�Z
	// deltaTime: �t���[���Ԃ̌o�ߎ���
	float deltaTime = 0.016f; // ��: 60FPS

	// ���݈ʒu�ƑO�t���[���̈ʒu���擾
	XMVECTOR currentTargetPosition = XMLoadFloat3(&Position);
	XMVECTOR previousTargetPosition = XMLoadFloat3(&OldPosition);

	// �ړ��x�N�g�����v�Z
	XMVECTOR movementVector = XMVectorSubtract(currentTargetPosition, previousTargetPosition);

	// ���x�x�N�g�����v�Z (�ړ����� �� ����)
	XMVECTOR velocity = XMVectorScale(movementVector, 1.0f / deltaTime);

	// ���x�x�N�g����ۑ����Ă���
	DirectX::XMStoreFloat3(&MoveVelocity, velocity);

	//�����蔻��ݒ�
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//�G�̓����蔻��쐬
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateGunEnemy()
{
	//�ړ������x�N�g��
	XMFLOAT3 MoveVector = { 0.0f,0.0f,0.0f };

	//���g���󂩂ǂ���
	MoveSpeed = 0.5f;

	//�ʉߍ��W��ʉ߂��Ă��Ȃ�������
	if (!NowFlag.IsPassPos)
	{
		//�ړ���̃x�N�g�����v�Z
		SetDirPos(NowFlag.MovePos);

		//XMFLOAT3�ɕϊ�
		MoveVector.x = XMVectorGetX(TargetDir);
		MoveVector.y = XMVectorGetY(TargetDir);
		MoveVector.z = XMVectorGetZ(TargetDir);

		//�ړ���ɓ���������
		if (CheckMovePos(NowFlag.MovePos, 1.0f))
		{
			MoveSpeed = 0.0f;
			NowFlag.EndForwardRotate = true;

			if (CPlayerAttack::Get()->GetGunEnemysPass())
			{
				NowFlag.IsPassPos = true;
				NowFlag.PassTime = GetTickCount64();
			}
		}

		//�ݒ肵�����W�̕����������Ă��Ȃ��ꍇ
		if (!NowFlag.EndRotate)
		{
			//�^�[�Q�b�g�̕����擾
			SetDirPos(NowFlag.MovePos);

			float Initangle = 0.0f;

			//��]������p�x���v�Z
			//���݂̐��ʂƈړ���̃x�N�g���v�Z
			float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
			if (dotProduct > 1.0f)
				Initangle = 0.0f;
			else
				Initangle = acosf(dotProduct);

			//�p�x��x���Ōv�Z
			float an = XMConvertToDegrees(Initangle);

			XMVECTOR axis;

			//�^�[�Q�b�g�܂ł̉�]�p�x���ő��]�p�x�𒴂��Ă���ꍇ
			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
			{
				//��]�����鎲���v�Z
				axis = XMVector3Cross(newForward, TargetDir);
				if (XMVector3Equal(axis, XMVectorZero()))
				{
					axis.m128_f32[1] = 1.0f;
				}

				an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;

				an = XMConvertToRadians(an);
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

				//��]�s��쐬
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//���݂̉�]�s��ɂ������킹��
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);
			}
			else
			{
				//��]�����鎲���v�Z
				axis = XMVector3Cross(newForward, TargetDir);
				if (XMVector3Equal(axis, XMVectorZero()))
				{
					axis.m128_f32[1] = 1.0f;
				}

				an = XMConvertToRadians(an);
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

				//��]�s��쐬
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//���݂̉�]�s��ɂ������킹��
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);

				NowFlag.EndRotate = true;
			}
		}
	}
	else
	{
		//�z��̒��g����̏ꍇ�A�ړ��I��
		if (qMoveArray.empty())
		{
			MoveSpeed = 0.0f;
		}
		else
		{
			//��~���Ԃ𒴂�����
			if (GetTickCount64() >= NowFlag.PassTime + NowFlag.WaitTime)
			{
				//�ړ���z��̐擪�v�f���擾
				NowFlag = qMoveArray.front();
				qMoveArray.push_back(qMoveArray.front());
				qMoveArray.pop_front();
				NowFlag.IsPassPos = false;
				XMFLOAT3 ResFloat;
				ResFloat = CPlayerAttack::Get()->GetBossEnemy()->Position;
				ResFloat = { ResFloat.x + (CPlayerAttack::Get()->GetBossEnemy()->GetForward().x * NowFlag.MovePos.z) + (CPlayerAttack::Get()->GetBossEnemy()->GetRight().x * NowFlag.MovePos.x) + (CPlayerAttack::Get()->GetBossEnemy()->GetUp().x * NowFlag.MovePos.y),
						ResFloat.y + (CPlayerAttack::Get()->GetBossEnemy()->GetForward().y * NowFlag.MovePos.z) + (CPlayerAttack::Get()->GetBossEnemy()->GetRight().y * NowFlag.MovePos.x) + (CPlayerAttack::Get()->GetBossEnemy()->GetUp().y * NowFlag.MovePos.y),
						ResFloat.z + (CPlayerAttack::Get()->GetBossEnemy()->GetForward().z * NowFlag.MovePos.z) + (CPlayerAttack::Get()->GetBossEnemy()->GetRight().z * NowFlag.MovePos.x) + (CPlayerAttack::Get()->GetBossEnemy()->GetUp().z * NowFlag.MovePos.y) };
				NowFlag.MovePos = ResFloat;
			}
			else
			{
				//�^�[�Q�b�g�̕����擾
				SetDirPos(CGame::Get()->GetPlayer()->Position);

				float Initangle = 0.0f;

				//��]������p�x���v�Z
				//���݂̐��ʂƈړ���̃x�N�g���v�Z
				float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
				if (dotProduct > 1.0f)
					Initangle = 0.0f;
				else
					Initangle = acosf(dotProduct);

				//�p�x��x���Ōv�Z
				float an = XMConvertToDegrees(Initangle);

				XMVECTOR axis;

				//�^�[�Q�b�g�܂ł̉�]�p�x���ő��]�p�x�𒴂��Ă���ꍇ
				if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
				{
					//��]�����鎲���v�Z
					axis = XMVector3Cross(newForward, TargetDir);
					if (XMVector3Equal(axis, XMVectorZero()))
					{
						axis.m128_f32[1] = 1.0f;
					}

					an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;

					an = XMConvertToRadians(an);
					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

					//��]�s��쐬
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//���݂̉�]�s��ɂ������킹��
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}
				else
				{
					//��]�����鎲���v�Z
					axis = XMVector3Cross(newForward, TargetDir);
					if (XMVector3Equal(axis, XMVectorZero()))
					{
						axis.m128_f32[1] = 1.0f;
					}

					an = XMConvertToRadians(an);
					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

					//��]�s��쐬
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//���݂̉�]�s��ɂ������킹��
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}

				//�e�𔭎�
				if (an <= 30 && CPlayerAttack::Get()->GetBossEnemy()->GetDebugIsAttack())
				{
					if (GetTickCount64() >= GunShotTime + 1000)
					{
						//�e��������
						CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::GUN, Position, Rotation);

						GunShotTime = GetTickCount64();
					}
				}
			}
		}
	}

	//�ړ�����
	CGameObject::MoveVectorUpdate(MoveVector);

	//�����蔻��ݒ�
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//�G�̓����蔻��쐬
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::Draw()
{
	if (Tag == INITENEMY)
	{
		return;
	}

	if (Hp <= 0)
	{
		isActive = false;
		return;
	}

	if (IsRefMainCamera)
		CGameObject::Draw();


}

void CEnemy::SetDirPos(XMFLOAT3 _targetpos)
{
	// �I�u�W�F�N�g�Ԃ̈ʒu�x�N�g�����v�Z
	//�����̓^�[�Q�b�g-�v���C���[�̈ʒu�ŋ��߂���
	XMFLOAT3 directionVector;
	directionVector.x = _targetpos.x - Position.x;
	directionVector.y = _targetpos.y - Position.y;
	directionVector.z = _targetpos.z - Position.z;

	// �ʒu�x�N�g���𐳋K��
	TargetDir = XMVector3Normalize(XMLoadFloat3(&directionVector));
}

bool CEnemy::CheckMovePos(DirectX::XMFLOAT3 _targetpos, float _allowdis)
{
	// 2�_�Ԃ̋������v�Z
	float distance = sqrt((_targetpos.x - Position.x) * (_targetpos.x - Position.x) +
		(_targetpos.y - Position.y) * (_targetpos.y - Position.y) +
		(_targetpos.z - Position.z) * (_targetpos.z - Position.z));

	return distance <= _allowdis;

	//return distance <= THRESHOLD;
}

void CEnemy::SetTargetMiniMapObjectModel(bool _istarget)
{
	if (Tag == INITENEMY)
		return;

	//�^�[�Q�b�g���ꂽ�Ƃ�
	if (_istarget)
	{
		NottargetMiniMapModel = pMinimapObject->GetModel();
		pMinimapObject->SetModel(TargetMiniMapModel);
	}
	else
	{
		//�^�[�Q�b�g����O�ꂽ
		pMinimapObject->SetModel(NottargetMiniMapModel);
	}
}

void CEnemy::SetNewMovePosition()
{	
	int randomValue = 0;

	while (true)
	{
		// �����͈̔͂��w��
		if (Tag != ENTOURAGE_ENEMY)
		{
			// ����������̏�����
			std::random_device rd;  // �n�[�h�E�F�A�̗�����������g�p����
			std::mt19937 rng(rd()); // �����Z���k�E�c�C�X�^�@�Ɋ�Â��[������������

			std::uniform_int_distribution<int> dist(0, 3); // 0����3�͈̔͂̐����𐶐�

			// �����_���Ȑ������擾
			randomValue = dist(rng);

			if (OldRndNum != randomValue)
			{
				OldRndNum = randomValue;
				break;
			}
		}
		else
		{
			// ����������̏�����
			std::random_device rd;  // �n�[�h�E�F�A�̗�����������g�p����
			std::mt19937 rng(rd()); // �����Z���k�E�c�C�X�^�@�Ɋ�Â��[������������

			std::uniform_int_distribution<int> dist(0, 7); // 0����3�͈̔͂̐����𐶐�

			// �����_���Ȑ������擾
			randomValue = dist(rng);

			if (OldRndNum != randomValue)
			{
				OldRndNum = randomValue;
				break;
			}
		}


	}

}

void CEnemy::SetEnemyState(ENEMY_STATE _newstate)
{
	if (EnemyState != _newstate)
	{
		SetNewEntouragePos();
	}

	EnemyState = _newstate;
}

float CEnemy::GetToPlayerRadius()
{
	//���C���J�������炱�̓G�ւ̕����x�N�g�������߂�
	XMFLOAT3 directionVector;
	XMFLOAT3 CameraPos = CGame::Get()->GetCamera3D()->GetCameraPos();
	directionVector.x = Position.x - CameraPos.x;
	directionVector.y = Position.y - CameraPos.y;
	directionVector.z = Position.z - CameraPos.z;

	// �ʒu�x�N�g���𐳋K��
	DirectX::XMVECTOR vDirection = XMVector3Normalize(XMLoadFloat3(&directionVector));

	//���C���J�������璍�����W�ւ̃x�N�g�������߂�
	XMFLOAT3 directionVector2;
	XMFLOAT3 CameraFocusPos = CGame::Get()->GetCamera3D()->GetCameraFocus();
	directionVector2.x = CameraFocusPos.x - CameraPos.x;
	directionVector2.y = CameraFocusPos.y - CameraPos.y;
	directionVector2.z = CameraFocusPos.z - CameraPos.z;

	// �ʒu�x�N�g���𐳋K��
	DirectX::XMVECTOR vDirection2 = XMVector3Normalize(XMLoadFloat3(&directionVector2));


	//�p�x��x���Ŏ擾
	float dotProduct = XMVectorGetX(XMVector3Dot(vDirection2, vDirection));
	dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);
	ToPlayerRadius = XMConvertToDegrees(acosf(dotProduct));

	return ToPlayerRadius;
}

void CEnemy::ChengeFollowPos(DirectX::XMFLOAT3& _addpos)
{
	GetRandomNum(0.0f, _addpos.x, EntouragePos.x);
	GetRandomNum(0.0f, _addpos.y, EntouragePos.y);
	GetRandomNum(0.0f, _addpos.z, EntouragePos.z);

	if (EntouragePos.x < _addpos.x / 2.0f)
	{
		EntouragePos.x -= _addpos.x;
	}
	if (EntouragePos.y < _addpos.y / 2.0f)
	{
		EntouragePos.y -= _addpos.y;
	}
}

void CEnemy::SetGunEnemyNewPos(CPlayerAttack::GUNENEMYSTARTPOS _startpos)
{
	//�l������z��ɐݒ肵�A������擾���Čv�Z�������ʂ�z��ɓ���Ȃ���

	//�@�֏e�̓G�́A�{�X�𒆐S�Ɉړ�����
	if (GunENemyStartPos == CPlayerAttack::GUNENEMYSTARTPOS::NoneVerTex)
		GunENemyStartPos = _startpos;

	GUNENEMY_MOVEFLAG ResPos;
	XMFLOAT3 MovePos;

	bool res = true;
	//�@�֏e�̓G�̈ړ����ݒ�(�{�X�𒆐S)
	switch (GunENemyStartPos)
	{
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex1:
		ResPos.MovePos = { 10,0,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex2:
		ResPos.MovePos = { 0,10,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex3:
		ResPos.MovePos = { -10,0,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex4:
		ResPos.MovePos = { -5,-10,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex5:
		ResPos.MovePos = { 5,-10,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::MAXVerTex:
		break;
	default:
		break;
	}

	GunENemyStartPos = static_cast<CPlayerAttack::GUNENEMYSTARTPOS>(static_cast<int>(GunENemyStartPos) + 1);

	while (GunENemyStartPos != _startpos)
	{
		//�@�֏e�̓G�̈ړ����ݒ�(�{�X�𒆐S)
		switch (GunENemyStartPos)
		{
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex1:
			ResPos.MovePos = { 10,0,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex2:
			ResPos.MovePos = { 0,10,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex3:
			ResPos.MovePos = { -10,0,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex4:
			ResPos.MovePos = { -5,-10,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex5:
			ResPos.MovePos = { 5,-10,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::MAXVerTex:
			break;
		default:
			break;
		}

		//����炷
		GunENemyStartPos = static_cast<CPlayerAttack::GUNENEMYSTARTPOS>(static_cast<int>(GunENemyStartPos) + 1);
		if (GunENemyStartPos == MAX)
		{
			//MAX�̏ꍇ�A�ŏ��ɖ߂�
			GunENemyStartPos = CPlayerAttack::GUNENEMYSTARTPOS::VerTex1;
		}
	}

	//�ړ���z��̐擪�v�f���擾
	NowFlag = qMoveArray.front();

	return;
}
