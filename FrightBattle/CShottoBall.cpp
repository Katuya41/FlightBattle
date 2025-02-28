#include "CShottoBall.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include "CCheckCollision.h"

using namespace DirectX;

CShottoBall::CShottoBall()
{
    pEffect = new CEffect();
    pEffect->SetBillboard(true);
    pEffect->SetLighting(false);
    pEffect->Scaling = { 3.0f,3.0f,3.0f };

    ColliderObject = new CGameObject();
    SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
    ColliderObject->Position = Position;
    ColliderObject->Rotation = Rotation;
    ColliderObject->Scaling = { 0.1f,0.1f,0.1f };

    //�~�T�C���̓����蔻��쐬
    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

CShottoBall::~CShottoBall()
{
    delete pEffect;
    delete Target;
}

void CShottoBall::Update()
{

    //�G�̓����蔻��擾
    if (Target != nullptr)
    {
        if (Target->GetTag() != INITENEMY)
        {
            EnemyCol = Target->GetColliderObjectOBB();

            if (CCheckCollision::Get()->CheckColBoxOBBOBB(EnemyCol, ColliderBoxOBB))
            {
                isActive = false;
                Target->SetDamege(2);
                CEffectManager::Get()->SetvEffect(pEffect, this->Position, 3, 0.1f);
            }
        }
    }


    CGameObject::SetVector();

    //�G�Ɍ������Ă���
    //MoveEnemy();

    CGameObject::MoveUpdate();

    ColliderObject->Position = Position;
    ColliderObject->Rotation = Rotation;

    //�~�T�C���̓����蔻��쐬
    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

    EnemyCol = CGame::Get()->GetPlayer()->GetColliderObjectOBB();

    //�^�[�Q�b�g�Ƃ̓����蔻����`�F�b�N����
    if (CCheckCollision::Get()->CheckColBoxOBBOBB(ColliderBoxOBB, EnemyCol))
    {
        isActive = false;
        CGame::Get()->GetPlayer()->SetDamege(1);
        CEffectManager::Get()->SetvEffect(pEffect, this->Position, 2, 0.2f);
    }
}

void CShottoBall::Draw()
{
    CGameObject::Draw();
}

void CShottoBall::SetDiviatDir()
{
    //�@�֏e�ݒ�
    //�G�̃X�s�[�h�ƈʒu�ƈړ��������擾
    //�v���C���[�̈ʒu�ƃX�s�[�h���擾
    //�G�̃X�s�[�h�ƈʒu�ƈړ��������疢���̓G���W���v�Z
    //����ɒe�̃X�s�[�h�����Čv�Z�������W�ɒe�𔭎˂���
    //�����Ōv�Z����킯�ł͂Ȃ��A�ŏ��Ɍv�Z�������Ĕ�ԕ����Əꏊ�̂ݎ󂯎��A
    //��������ɒe���ړ�������悤�ɂ���

    ////�^�[�Q�b�g�ւ̕����x�N�g�����擾
    //XMFLOAT3 directionVector;
    //directionVector.x = Target->Position.x - Position.x;
    //directionVector.y = Target->Position.y - Position.y;
    //directionVector.z = Target->Position.z - Position.z;
    //// ���K�����ꂽ�����x�N�g�����擾
    //XMFLOAT3 normalizedDirection;
    //XMStoreFloat3(&normalizedDirection, XMVector3Normalize(XMLoadFloat3(&directionVector)));
    ////�G�̈ړ��X�s�[�h�ƈړ��������擾
    //float EnemySpeed = Target->GetMoveSpeed();
    //XMFLOAT3 EnemyPos = Target->Position;
    //XMFLOAT3 EnemyForward = Target->GetForward();
    //XMFLOAT3 EnemyVelocity = { EnemyForward.x * EnemySpeed,EnemyForward.y * EnemySpeed,EnemyForward.z * EnemySpeed };

    ////�^�[�Q�b�g�̍��W�ƈړ��������擾
    //XMVECTOR targetPosition = XMLoadFloat3(&Target->Position);
    //XMVECTOR targetVelocity = XMLoadFloat3(&EnemyVelocity);

    ////�^�[�Q�b�g�̕������v�Z
    //XMVECTOR toTarget = targetPosition - XMLoadFloat3(&Position);
    ////�^�[�Q�b�g�܂ł̋������v�Z
    //float distanceToTarget = XMVectorGetX(XMVector3Length(toTarget));

    //// �e�ۂ��^�[�Q�b�g�ɓ��B���鎞�Ԃ��v�Z
    //float timeToTarget = distanceToTarget / ShotSpeed;

    //// �\���ʒu���v�Z
    //XMVECTOR predictedPosition = targetPosition + targetVelocity * timeToTarget;

    //// �ˌ��������v�Z
    //XMVECTOR shootDirection = XMVector3Normalize(predictedPosition - XMLoadFloat3(&Position));

    //////���݂̋ʂ̕����ƃ^�[�Q�b�g�̕����̊p�x����ςŌv�Z
    //XMVECTOR res = newForward - shootDirection;
    //float angle = 0.0f;

    //float dotProduct = XMVectorGetX(XMVector3Dot(newForward, shootDirection));
    //angle = acosf(dotProduct);
    ////�p�x��x���Ōv�Z
    //float an = XMConvertToDegrees(angle);

    ////��]�����鎲���v�Z
    //if (an != 0)
    //{
    //    //��]�����鎲���v�Z
    //    XMVECTOR axis = XMVector3Cross(newForward, shootDirection);

    //    XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

    //    //��]�s��쐬
    //    XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

    //    //���݂̉�]�s��ɂ������킹��
    //    RotateMatrix *= QMT;
    //}

    Rotation = ExtractRotationFromMatrix(RotateMatrix);

    //MoveSpeed = ShotSpeed;
}

XMFLOAT3 CShottoBall::SetDirect()
{
    // �I�u�W�F�N�g�Ԃ̈ʒu�x�N�g�����v�Z
    //�����̓^�[�Q�b�g-�v���C���[�̈ʒu�ŋ��߂���
    XMFLOAT3 directionVector;
    directionVector.x = Target->Position.x - Position.x;
    directionVector.y = Target->Position.y - Position.y;
    directionVector.z = Target->Position.z - Position.z;

    // �ʒu�x�N�g���𐳋K��
    vDirection = XMVector3Normalize(XMLoadFloat3(&directionVector));

    // ���K�����ꂽ�����x�N�g�����擾
    XMFLOAT3 normalizedDirection;
    XMStoreFloat3(&normalizedDirection, vDirection);

    return normalizedDirection;
}

void CShottoBall::SetModel(D3D::Model _model)
{
    model = _model;
}

void CShottoBall::SetRotateMatrix()
{
    RotateMatrix = GetIsMatrix();
}

void CShottoBall::SetHitBallColBoxEffect(D3D::Model _model)
{
    pEffect->SetModel(_model);
}

void CShottoBall::SetAnimUVSize(float _u, float _v)
{
    pEffect->SetAnimUVSize(_u, _v);
}
