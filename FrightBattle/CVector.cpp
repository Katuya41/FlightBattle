#include "CVector.h"
#include "CPlayerAttack.h"
#include "CEnemy.h"
#include "CGame.h"
#include "CPlayer.h"
#include "CCamera.h"
#include "CScene.h"

using namespace DirectX;

CVector::CVector()
{
	isActive = false;
}

void CVector::Update()
{
	CGameObject* Player = CScene::Get()->GetPlayer();
	XMFLOAT3 PlayerPos = CScene::Get()->GetPlayer()->Position;

	if (!CPlayerAttack::Get()->GetIsOnetarget())
	{
		return;
	}

	//���̍��W��ݒ�
	if (!CGame::Get()->GetIsFirst())	//�O�l�̃J����
	{
		Scaling = { 0.1f, 0.1f, 0.1f };

		//Position = { CGame::Get()->GetPlayer()->Position.x + (CGame::Get()->GetPlayer()->GetUp().x * 1.0f),
		//			 CGame::Get()->GetPlayer()->Position.y + (CGame::Get()->GetPlayer()->GetUp().y * 1.0f),
		//			 CGame::Get()->GetPlayer()->Position.z + (CGame::Get()->GetPlayer()->GetUp().z * 1.0f) };

		Position = { PlayerPos.x + (Player->GetUp().x * 1.0f),
					PlayerPos.y + (Player->GetUp().y * 1.0f),
					PlayerPos.z + (Player->GetUp().z * 1.0f) };

	}
	else if (CGame::Get()->GetIsFirst())	//��l�̃J����
	{
		Scaling = { 0.05f, 0.05f, 0.05f };

		Position.x = CGame::Get()->GetPlayer()->Position.x + (CGame::Get()->GetPlayer()->GetForward().x * 2.0f) + (CGame::Get()->GetPlayer()->GetUp().x * 0.5f);
		Position.y = CGame::Get()->GetPlayer()->Position.y + (CGame::Get()->GetPlayer()->GetForward().y * 2.0f) + (CGame::Get()->GetPlayer()->GetUp().y * 0.5f);
		Position.z = CGame::Get()->GetPlayer()->Position.z + (CGame::Get()->GetPlayer()->GetForward().z * 2.0f) + (CGame::Get()->GetPlayer()->GetUp().z * 0.5f);

		Position.x = Player->Position.x + (Player->GetForward().x * 2.0f) + (Player->GetUp().x * 0.5f);
		Position.y = Player->Position.y + (Player->GetForward().y * 2.0f) + (Player->GetUp().y * 0.5f);
		Position.z = Player->Position.z + (Player->GetForward().z * 2.0f) + (Player->GetUp().z * 0.5f);

	}

	//�^�[�Q�b�g��ݒ�
	SetTarget(CPlayerAttack::Get()->GetTargetObj());

	//���݂̃I�u�W�F�N�g�̎����v�Z
	CGameObject::SetVector();

	if (Target != nullptr)
	{
		//�^�[�Q�b�g�ւ̕����x�N�g���v�Z
		fDirect = SetDirect();

		//��������ƂɊp�x���v�Z
		float dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(newForward), vDirect));
		float angle = acosf(dotProduct);
		float an = XMConvertToDegrees(angle);

		if (an > 0.0f)
		{
			//�v���C���[�̐��ʂƃ^�[�Q�b�g�̕����̊O�ς��v�Z
			//����ɂ���]���鎲���擾
			XMVECTOR axis = XMVector3Cross(newForward, vDirect);

			//��]�x�N�g���쐬
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

			//��]�s��쐬
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//�I�u�W�F�N�g�̉�]�s��ɉ��Z
			RotateMatrix *= QMT;
		}

		//��]�X�V
		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}


}

void CVector::Draw()
{
	if (!CPlayerAttack::Get()->GetIsOnetarget())
	{
		return;
	}

	//�^�[�Q�b�g�����邩�^�[�Q�b�g����ʊO�̂Ƃ��ɕ`��
	if (Target != nullptr)
	{
		if (!Target->GetIsCameraRef())
			CGameObject::Draw();
	}
}

XMFLOAT3 CVector::SetDirect()
{
	// �I�u�W�F�N�g�Ԃ̈ʒu�x�N�g�����v�Z
	//�����̓^�[�Q�b�g-�v���C���[�̈ʒu�ŋ��߂���
	XMFLOAT3 directionVector;
	directionVector.x = TargetPos.x - Position.x;
	directionVector.y = TargetPos.y - Position.y;
	directionVector.z = TargetPos.z - Position.z;

	// �ʒu�x�N�g���𐳋K��
	vDirect = XMVector3Normalize(XMLoadFloat3(&directionVector));

	// ���K�����ꂽ�����x�N�g�����擾
	XMFLOAT3 normalizedDirection;
	XMStoreFloat3(&normalizedDirection, vDirect);

	return normalizedDirection;
}

void CVector::SetTarget(CEnemy* _target)
{
	Target = _target;

	if (Target != nullptr)
		TargetPos = Target->Position;
	else
		TargetPos = { 0,0,0 };

	if (Target->GetTag() == INITENEMY)
	{
		isActive = false;
	}
	else
	{
		isActive = true;
	}
}
