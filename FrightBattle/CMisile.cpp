#include "CMisile.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CScene.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include "CCheckCollision.h"
#include "CPlayerAttack.h"
#include "xa2.h"

#include <random>
#include <iostream>

using namespace std;

using namespace DirectX;

void CMisile::InsertParticle()
{
    //�p�[�e�B�N���ǉ�
    PARTICLE Particle;
    Particle.Object = make_unique<CGameObject>();

    // ����������̏�����
    //�n�[�h�E�F�A�̗�����������g�p����
    //�����Z���k�E�c�C�X�^�@�Ɋ�Â��[������������
    std::random_device rd;
    std::mt19937 rng(rd());

    // �����͈̔͂��w��
    std::uniform_real_distribution<float> dist(0.0f, 0.4f);
    float Rnd = dist(rng) - 0.2f;

    Particle.Object->Position = { Position.x + GetRight().x * Rnd + GetUp().x * Rnd,
                                  Position.y + GetRight().y * Rnd + GetUp().y * Rnd,
                                  Position.z + GetRight().z * Rnd + GetUp().z * Rnd,};

    Particle.Object->Scaling = { 0.1f, 0.1f, 0.1f };
    Particle.Object->materialDiffuse.w = 1.0f;
    Particle.Object->SetActive(true);
    Particle.Object->SetLighting(false);
    Particle.Object->SetBillboard(true);
    Particle.Object->SetModel(ParticleModel);
    Particle.DeleteTime = CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time;
    Particle.StartTime = GetTickCount64();
    Particle.Object->SetTag(MISILE_SMOKE);
    Particles.push_back(move(Particle));

    //�p�[�e�B�N���ǉ�
    PARTICLE Particle2;
    Particle2.Object = make_unique<CGameObject>();

    Rnd = dist(rng) - 0.2f;

    Particle2.Object->Position = { Position.x + GetRight().x * Rnd + GetUp().x * Rnd,
                              Position.y + GetRight().y * Rnd + GetUp().y * Rnd,
                              Position.z + GetRight().z * Rnd + GetUp().z * Rnd, };

    Particle2.Object->Scaling = { 0.1f, 0.1f, 0.1f };
    Particle2.Object->materialDiffuse.w = 1.0f;
    Particle2.Object->SetActive(true);
    Particle2.Object->SetLighting(false);
    Particle2.Object->SetBillboard(true);
    Particle2.Object->SetModel(ParticleModel);
    Particle2.DeleteTime = CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time;
    Particle2.StartTime = GetTickCount64();
    Particle2.Object->SetTag(MISILE_SMOKE);
    Particles.push_back(move(Particle2));
}

CMisile::CMisile()
{
    pEffect = new CEffect();
    pEffect->SetBillboard(true);
    pEffect->SetLighting(false);

    IsShotto = false;

    pEffect->Scaling = { 0.5f,0.5f,0.5f };

    ColliderObject = new CGameObject();
    SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
    ColliderObject->Position = Position;
    ColliderObject->Rotation = Rotation;
    ColliderObject->Scaling = Scaling;

    //�~�T�C���̓����蔻��쐬
    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

CMisile::~CMisile()
{
    if (pEffect != nullptr)
        delete pEffect;

    //�p�[�e�B�N���̍폜
    //�X�}�[�g�|�C���^�̈׉�������͕s�v
    Particles.clear();
}

void CMisile::Update()
{
    //�J�����ɉf���Ă��邩
    ChackRefCamera();

    //�p�[�e�B�N���̍X�V����
    ParticleUpdate();

    if (Particles.size() == 0 && isActive == false)
    {
        Particles.clear();
        IsEnd = true;
    }

    if (GetTickCount64() >= ShotTime + CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time)
    {
        isActive = false;
        Target->SetIsAttacked(false);
    }

    if (isActive == false)
        return;


    //�p�[�e�B�N���쐬
    if (GetTickCount64() >= OldParticleTime + (ULONGLONG)CPlayerAttack::Get()->GetMisileParameter().Particle_Cooltime)
    {
        //�p�[�e�B�N����ǉ�
        InsertParticle();
        OldParticleTime = GetTickCount64();

        //CParticleManager::Get()->SetParticle(CParticleManager::MODEL_NAME::MISILE_PARTICLE, Position);
    }

    CGameObject::SetVector();

    //�G�Ɍ������Ă���
    MoveUpdate();

    CGameObject::MoveUpdate();

    if (IsMoveEnemy)
    {
        //�����蔻��
        //�G�������G����Ȃ����
        if (Target->GetTag() != INITENEMY && isActive)
        {
            //�G�̓����蔻��擾
            if (Target->GetIsColAABB())
            {
                EnemyColAABB = Target->GetColliderObjectAABB();
            }
            else
            {
                EnemyColOBB = Target->GetColliderObjectOBB();
            }

            Target->SetIsAttacked(true);

            //�G���A�N�e�B�u�Ȃ�
            if (Target->GetActive())
            {
                //�^�[�Q�b�g�Ƃ̋������v�Z
                TargetDistance = CheckDistance(Target, {});

                ColliderObject->Position = Position;
                ColliderObject->Rotation = Rotation;
                ColliderObject->Scaling = Scaling;

                //�~�T�C���̓����蔻��쐬
                SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

                if (Target->GetIsColAABB())
                {
                    //�^�[�Q�b�g�Ƃ̓����蔻����`�F�b�N����
                    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, EnemyColAABB))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        Target->SetIsAttacked(false);
                        Target->SetDamege(10);
                        CEffectManager::Get()->SetvEffect(pEffect, this->Position, 2, 0.1f);
                        MisileState = WAIT;

                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });

                        //UI��\�����邩

                        if (IsDrawHitUI || Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY || 
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->MisileHitUIActive();

                        //�R���g���[���[��U�������邩
                        if (Target->GetTag() == CGameObject::TAG::PLAYER)
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);
                        else if (Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY ||
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);

                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(),1.0f);
                    }
                }
                else
                {
                    ColliderObject->Position = Position;
                    ColliderObject->Rotation = Rotation;
                    ColliderObject->Scaling = Scaling;

                    //�~�T�C���̓����蔻��쐬
                    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

                    //�^�[�Q�b�g�Ƃ̓����蔻����`�F�b�N����
                    if (CCheckCollision::Get()->CheckColBoxOBBOBB(ColliderBoxOBB, EnemyColOBB))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        Target->SetIsAttacked(false);
                        Target->SetDamege(10);
                        CEffectManager::Get()->SetvEffect(pEffect, this->Position, 2, 0.1f);
                        MisileState = WAIT;

                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });

                        //UI��\�����邩
                        if (IsDrawHitUI || Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY || 
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->MisileHitUIActive();

                        //�R���g���[���[��U�������邩
                        if (Target->GetTag() == CGameObject::TAG::PLAYER)
                        {
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);
                            CGame::Get()->ShakeCamera(1);
                        }
                        else if (Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY ||
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);
                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
                    }
                }

                //�n�ʂƂ̓����蔻��
                for (auto Collider : mColliderBoxsAABB[TAG::GROUND])
                {
                    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        Target->SetIsAttacked(false);
                        CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                        //�~�T�C���̓����蔻��쐬
                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
                    }
                }

                //�����Ƃ̓����蔻��
                for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
                {
                    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
                    }
                }

            }
        }
    }
    else
    {
        ColliderObject->Position = Position;
        ColliderObject->Rotation = Rotation;
        ColliderObject->Scaling = Scaling;

        //�~�T�C���̓����蔻��쐬
        SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

        if (CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND) != nullptr)
        {
            DirectX::BoundingBox GroundCol = CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->GetColliderObjectAABB();
            if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, GroundCol))
            {
                isActive = false;
                IsDrawMiniMapModel = false;
                Target->SetIsAttacked(false);
                CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                //�~�T�C���̓����蔻��쐬
                SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
            }
        }

        ////�n�ʂƂ̓����蔻��
        //for (auto Collider : mColliderBoxsAABB[TAG::GROUND])
        //{
        //    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
        //    {
        //        isActive = false;
        //        IsDrawMiniMapModel = false;
        //        Target->SetIsAttacked(false);
        //        CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
        //        //�~�T�C���̓����蔻��쐬
        //        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
        //        //XA_Play(SE_MISILEHIT);
        //    }
        //}

        //�����Ƃ̓����蔻��
        for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
        {
            if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
            {
                isActive = false;
                IsDrawMiniMapModel = false;
                Target->SetIsAttacked(false);
                CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
            }
        }
    }

    //�~�j�}�b�v�̃A�b�v�f�[�g
    if (isActive)
        MiniMapObjectUpdate();

}

void CMisile::Draw()
{
    if (isActive)
        CGameObject::Draw();
}

void CMisile::ParticleUpdate()
{
    //�p�[�e�B�N���̒��g��nullptr�̃I�u�W�F�N�g���폜����
    //remove_if�ɂ����Object�̒��g��nullptr�̕���erase���Ă���
    Particles.erase(std::remove_if(Particles.begin(), Particles.end(), [](const PARTICLE& p) {
        return p.Object == nullptr;
        }), Particles.end());

    //�p�[�e�B�N������ёւ���(�J��������ʒu���������̂��ɕ`��)
    if (!Particles.empty())
    {
        std::sort(Particles.begin(), Particles.end(), [](const PARTICLE& a, const PARTICLE& b) {
            return a.Object->CheckDistance(nullptr, CGame::Get()->GetCameraPos()) > b.Object->CheckDistance(nullptr, CGame::Get()->GetCameraPos());
            });
    }

    //�p�[�e�B�N���̃A�b�v�f�[�g
    //���X�ɑ傫�����Ă����A���X�ɔ������Ă���
    for (auto it = Particles.begin(); it != Particles.end(); ) // `it++` �͍Ō�ɏ���
    {
        //���g�����邩
        if (it->Object)
        {
            //�J�����ɉf���Ă��邩����
            it->Object->ChackRefCamera();

            //���X�ɔ������Ă���
            it->Object->materialDiffuse.w -= (float)CPlayerAttack::Get()->GetMisileParameter().Particle_OutTime;

            //���̑傫���܂ő傫������
            if (it->Object->Scaling.x < 0.4f)
                it->Object->Scaling = { it->Object->Scaling.x + 0.001f, it->Object->Scaling.y + 0.001f ,it->Object->Scaling.z + 0.001f };

            //�A���t�@�l��0�ɂȂ�����A�폜
            if (it->Object->materialDiffuse.w <= 0.0f) {
                it = Particles.erase(it);       
                continue;
            }
        }

        ++it; // �����Ȃ������ꍇ�����i�߂� 
    }
}

void CMisile::DrawParticle()
{
    for (auto it = Particles.begin(); it != Particles.end(); it++)
    {
        //�J�����ɉf���Ă���ꍇ�̂ݕ`��
        if ((*it).Object->GetIsCameraRef())
            (*it).Object->Draw();
    }

    //ChengeShader(0);

}

void CMisile::MoveUpdate()
{
    //�G�������G����Ȃ����
    if (IsMoveEnemy)
    {
        //�����x�N�g�����擾
        fDirection = SetDirect();

        //���݂̃~�T�C���̕����ƃ^�[�Q�b�g�̕����̊p�x����ςŌv�Z
        XMVECTOR res = newForward - vDirection;
        float angle = 0.0f;

        float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vDirection));
        angle = acosf(dotProduct);
        //�p�x��x���Ōv�Z
        float an = XMConvertToDegrees(angle);
        //angle = XMConvertToRadians(angle);

        //MoveSpeed = MISILE_SPEED;

        if (an >= 60 && MoveSpeed >= CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            IsMoveEnemy = false;
        }

        //��]�����鎲���v�Z
        if (an > 0.0f)
        {
            //��]�����鎲���v�Z
            XMVECTOR axis = XMVector3Cross(newForward, vDirection);

            //�������I�u�W�F�N�g���v���C���[
            if (IsShotPlayer)
            {
                //�X�s�[�h�������K�葬�x�ȉ��̏ꍇ�A��]�p�x�͍ő�p�x�Őݒ�
                if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().MaxRotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().MaxRotate);
                    }
                }
                else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().MinRotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().MinRotate);
                    }
                }
            }
            else
            {
                //�X�s�[�h�������K�葬�x�ȉ��̏ꍇ�A��]�p�x�͍ő�p�x�Őݒ�
                if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Rotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Rotate);
                    }
                }
                else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Rotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Rotate);
                    }
                }
            }



            XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

            //��]�s��쐬
            XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

            //���݂̉�]�s��ɂ������킹��
            RotateMatrix *= QMT;
        }
    }

    Rotation = ExtractRotationFromMatrix(RotateMatrix);

    //�������I�u�W�F�N�g���v���C���[
    //�����x�ݒ�
    if (IsShotPlayer)
    {
        if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Min_Add_Speed;
            MoveSpeed += AddSpeed;
        }
        else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Max_Add_Speed;
            MoveSpeed += AddSpeed;
        }

        if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Max_Speed)
        {
            MoveSpeed = CPlayerAttack::Get()->GetMisileParameter().Max_Speed;
        }
    }
    else
    {
        if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Add_Speed;
            MoveSpeed += AddSpeed;
        }
        else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Add_Speed;
            MoveSpeed += AddSpeed;
        }

        if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Max_Speed)
        {
            MoveSpeed = CPlayerAttack::Get()->GetMisileParameter().Max_Speed;
        }
    }
}

XMFLOAT3 CMisile::SetDirect()
{
    // �I�u�W�F�N�g�Ԃ̈ʒu�x�N�g�����v�Z
    //�����̓^�[�Q�b�g - �����̈ʒu�ŋ��߂���
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

void CMisile::SetHitMisileEffect(D3D::Model _model)
{
    pEffect->SetModel(_model);
}

void CMisile::SetUv_Size(DirectX::XMFLOAT2 _uv)
{
    pEffect->SetAnimUVSize(_uv.x, _uv.y);
}
