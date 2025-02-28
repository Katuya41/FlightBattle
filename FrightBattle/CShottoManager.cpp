#include "CShottoManager.h"
#include <DirectXMath.h>
#include "CShottoBall.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CCamera.h"

//�ʂ�������܂ł̎���
#define DeleteBallTime (1000)

//�ʂ̃N�[���^�C��
#define ShottoBallCoolTime (100)

//�ʂ̃X�s�[�h
#define GUN_SPEED (0.3f)

using namespace DirectX;

CShottoManager* CShottoManager::Get()
{
	static CShottoManager instance;
	return &instance;
}

void CShottoManager::Update()
{
	for (auto i = vShottoBall.begin(); i != vShottoBall.end(); i++)
	{
		if (GetTickCount64() >= (*i)->ShotTime + DeleteBallTime || !(*i)->GetActive())
		{
			vShottoBall.erase(i);
			break;
		}
		else
		{
			(*i)->Update();
		}
	}
}

void CShottoManager::Draw()
{
	for (auto i = vShottoBall.begin(); i != vShottoBall.end(); i++)
	{
		(*i)->Draw();
	}
}

void CShottoManager::CheckShotPos(DirectX::XMFLOAT3 _spornpos, CEnemy* _target)
{
    //�^�[�Q�b�g�ւ̕����x�N�g�����擾
    XMFLOAT3 directionVector;
    directionVector.x = _target->Position.x - _spornpos.x;
    directionVector.y = _target->Position.y - _spornpos.y;
    directionVector.z = _target->Position.z - _spornpos.z;
    // ���K�����ꂽ�����x�N�g�����擾
    XMFLOAT3 normalizedDirection;
    XMStoreFloat3(&normalizedDirection, XMVector3Normalize(XMLoadFloat3(&directionVector)));
    //�G�̈ړ��X�s�[�h�ƈړ��������擾
    float EnemySpeed = _target->GetMoveSpeed();
    XMFLOAT3 EnemyPos = _target->Position;
    XMFLOAT3 EnemyForward = _target->GetForward();
    XMFLOAT3 EnemyVelocity = { EnemyForward.x * EnemySpeed,EnemyForward.y * EnemySpeed,EnemyForward.z * EnemySpeed };
    XMFLOAT3 CameraPos = CGame::Get()->GetCamera3D()->GetCameraPos();

    //�^�[�Q�b�g�̍��W�ƈړ��������擾
    XMVECTOR targetPosition = XMLoadFloat3(&_target->Position);
    XMVECTOR targetVelocity = XMLoadFloat3(&EnemyVelocity);

    //�^�[�Q�b�g�̕������v�Z
    XMVECTOR toTarget = targetPosition - XMLoadFloat3(&_spornpos);
    //XMVECTOR toTarget = targetPosition - XMLoadFloat3(&CameraPos);
    //�^�[�Q�b�g�܂ł̋������v�Z
    float distanceToTarget = XMVectorGetX(XMVector3Length(toTarget));

    // �e�ۂ��^�[�Q�b�g�ɓ��B���鎞�Ԃ��v�Z
    float timeToTarget = distanceToTarget / GUN_SPEED;

    // �\���ʒu���v�Z
    XMVECTOR predictedPosition = targetPosition + targetVelocity * timeToTarget;

    // �ˌ��������v�Z
    //XMVECTOR shootDirection = XMVector3Normalize(predictedPosition - XMLoadFloat3(&CameraPos));
    XMVECTOR shootDirection = XMVector3Normalize(predictedPosition - XMLoadFloat3(&_spornpos));

    XMStoreFloat3(&ShotPos ,predictedPosition);
}

void CShottoManager::SetBallInf(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _spornrotate, CEnemy* _target)
{
	//�ʂ����Ă邩�`�F�b�N����
	if (BallShottoTime == 0 || GetTickCount64() >= BallShottoTime + (ULONGLONG)ShottoBallCoolTime)
	{
		CShottoBall* s = new CShottoBall();
		s->SetModel(model);						//���f��
		s->Scaling = { 0.1f, 0.1f, 0.1f };		//�傫���ݒ�
		s->SetShotSpeed(GUN_SPEED);
		s->SetSpornPos(_spornpos);				//�X�|�[��������W
		s->SetSpornRotate(_spornrotate);		//�p�x�ݒ�
		s->SetRotateMatrix();					//�����s��ݒ�
		s->SetVector();							//���݂�vector�v�Z
		s->SetTarget(_target);					//�^�[�Q�b�g���擾
		s->SetTag(CGameObject::MISILE);			//�^�O�̐ݒ�
		s->ShotTime = GetTickCount64();			//���Ԏ����̐ݒ�
		s->SetHitBallColBoxEffect(BallHitModel);		//�ʂ̓����������̃G�t�F�N�g�e�N�X�`��
		s->SetAnimUVSize(UVSize.x, UVSize.y);
		s->SetDiviatDir();

		vShottoBall.push_back(s);

		//�ʂ����������Ԃ��Z�b�g����
		BallShottoTime = GetTickCount64();
	}
}

