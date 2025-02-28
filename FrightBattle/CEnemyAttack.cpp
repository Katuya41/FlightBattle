#include"CEnemyAttack.h"
#include <DirectXMath.h>
#include "CCamera.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CMisile.h"
#include "CShottoBall.h"
#include "CEffect.h"
#include "CScene.h"
#include "xa2.h"
#include "Input.h"

//�~�T�C���̏������x
#define MISILE_SPEED (0.2)

//�ʂ�������܂ł̎���
#define DeleteBallTime (5000)

//�ʂ̃X�s�[�h
#define GUN_SPEED (0.6f)

//�v���C���[�x���p����
#define MAX_PLAYER_MISILETARGET_DISTANCE (200)

CEnemyAttack::CEnemyAttack()
{
}

CEnemyAttack::~CEnemyAttack()
{
	Misiles.clear();

	for (auto ShotBall : vShottoBall)
	{
		delete ShotBall;
	}
	vShottoBall.clear();
}

void CEnemyAttack::MisileUpdate()
{
	IsDrawPlayerRock = false;
	IsClosePlayerDis = false;

	//�~�T�C���̍X�V����
	for (auto i = Misiles.begin(); i != Misiles.end(); i++)
	{
		if ((*i)->GetMisileEnd())
		{
			Misiles.erase(i);
			break;
		}
		else
		{
			(*i)->Update();

			if ((*i)->GetActive() && (*i)->GetIsMoveTarget())
			{
				//������100�ȏ�̏ꍇ�͉��F�x���ŁA
				//������100�ȓ����~�T�C�����v���C���[�̕����������Ă����ꍇ�ɐԐF�x���ɂȂ�
				if ((*i)->CheckDistance(CGame::Get()->GetPlayer(), {}) <= ((float)MAX_PLAYER_MISILETARGET_DISTANCE))
				{
					IsDrawPlayerRock = true;
					IsClosePlayerDis = false;
					if ((*i)->GetTargetDistance() <= ((float)MAX_PLAYER_MISILETARGET_DISTANCE / 2.0f) &&
						(*i)->GetIsMoveTarget())
					{
						IsClosePlayerDis = true;
					}
				}
			}
		}
	}

	//�v���C���[���b�N���f����\��
	if (IsDrawPlayerRock)
		CGame::Get()->GetPlayer()->SetIsDrawRockUI(true);
	else
		CGame::Get()->GetPlayer()->SetIsDrawRockUI(false);

	//���b�N���f���̐F��ݒ�
	if(IsClosePlayerDis)
		CGame::Get()->GetPlayer()->SetIsRockOn(true);		//��
	else
		CGame::Get()->GetPlayer()->SetIsRockOn(false);		//���F

}

void CEnemyAttack::GunUpdate()
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

void CEnemyAttack::TargetUpdate()
{
	//�G�̃^�[�Q�b�g�́A�n�ʓG�̏ꍇ�A�U���\�������ɂ��邩�̂�
}

void CEnemyAttack::MisileDraw()
{
	for (auto it = Misiles.begin(); it != Misiles.end(); it++)
	{
		(*it)->Draw();

		if (CScene::Get()->GetIsDrawCollider())
		{
			(*it)->DrawColliderObject();
		}
	}
}

void CEnemyAttack::MisileParticleDraw()
{
	for (auto it = Misiles.begin(); it != Misiles.end(); it++)
	{
		(*it)->DrawParticle();
	}

	//for (auto Misile : Misiles)
	//{
	//	Misile->DrawParticle();
	//}
}

void CEnemyAttack::GunDraw()
{
	for (auto ShotBall : vShottoBall)
	{
		ShotBall->Draw();

		if (CScene::Get()->GetIsDrawCollider())
		{
			ShotBall->DrawColliderObject();
		}
	}
}

CMisile* CEnemyAttack::AttackMisile(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation, CPlayer* _target, bool _moveenemy)
{
	std::unique_ptr<CMisile> res = std::make_unique<CMisile>();
	//CMisile* res = new CMisile();
	res->SetModel(MisileModel);							//���f��
	res->SetMiniMapModel(MiniMapMisileModel);			//�~�j�}�b�v���f��
	res->SetParticleModel(MisileParticleModel);			//�~�T�C���̃p�[�e�B�N���̃��f��
	res->Scaling = { 0.2f, 0.2f, 0.2f };				//�傫���ݒ�
	res->SetTag(CGameObject::MISILE);					//�^�O�̐ݒ�
	res->SetMisileState(CMisile::MISILE_STATE::READY);
	res->SetUv_Size(MisileHitUVSize);
	res->SetIsMoveEnemy(_moveenemy);
	res->SetSpornPos(_spornpos);

	res->SetSpornRotate(_rotation);						//�p�x�ݒ�
	//res->SetSpornRotate(_spornobj->Rotation);			//�p�x�ݒ�
	res->SetInitRotateMatrix();							//�����s��ݒ�
	res->SetVector();									//���݂�vector�v�Z
	//res->SetMoveSpeed(0.0f);							//�������x
	res->SetMoveSpeed((float)MISILE_SPEED);
	res->SetTarget(_target);							//�^�[�Q�b�g���擾
	res->SetShotTime(GetTickCount64());					//���Ԏ����̐ݒ�
	res->SetMisileParticleTime(GetTickCount64());
	res->SetHitMisileEffect(MisileHitModel);			//�~�T�C���̓����������̃G�t�F�N�g�e�N�X�`��

	Misiles.push_back(std::move(res));

	return res.get();
}

void CEnemyAttack::AttackShotBall(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation)
{
	CShottoBall* s = new CShottoBall();
	s->SetModel(GunModel);							//���f��
	s->Scaling = { 0.1f, 0.1f, 0.1f };				//�傫���ݒ�
	s->SetMoveSpeed(GUN_SPEED);						//�ړ����x��ݒ肷��
	s->SetSpornPos(_spornpos);						//�X�|�[��������W
	s->SetSpornRotate(_rotation);					//�p�x�ݒ�
	s->SetRotateMatrix();							//�����s��ݒ�
	s->SetVector();									//���݂�vector�v�Z
	//s->SetTarget(CGame::Get()->GetPlayer());		//�^�[�Q�b�g���擾
	s->SetTag(CGameObject::GUN);					//�^�O�̐ݒ�
	s->ShotTime = GetTickCount64();					//���Ԏ����̐ݒ�
	s->SetHitBallColBoxEffect(GunHitModel);			//�ʂ̓����������̃G�t�F�N�g�e�N�X�`��
	s->SetAnimUVSize(GunHitUVSize.x, GunHitUVSize.y);
	s->SetDiviatDir();

	vShottoBall.push_back(s);
}

CEnemyAttack* CEnemyAttack::Get()
{
	static CEnemyAttack instance;
	return &instance;
}

void CEnemyAttack::Update()
{
	//�~�T�C���X�V����
	MisileUpdate();

	//�@�֏e�X�V����
	GunUpdate();
}

void CEnemyAttack::Draw()
{
	MisileDraw();

	GunDraw();

	MisileParticleDraw();
}

void CEnemyAttack::MiniMapMisileDraw()
{
	for (auto it = Misiles.begin(); it != Misiles.end(); it++)
	{
		(*it)->DrawMiniMapObject();
	}
}

CMisile* CEnemyAttack::AttackTarget(ATTACK_ID _attackid , DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation)
{
	switch (_attackid)
	{
	case CEnemyAttack::MISILE:
		return AttackMisile(_spornpos, _rotation, CGame::Get()->GetPlayer(),  true);
		break;
	case CEnemyAttack::GUN:
		AttackShotBall(_spornpos, _rotation);
		break;
	case CEnemyAttack::MAX:
		break;
	default:
		break;
	}

	return nullptr;

}

void CEnemyAttack::SetMisileInf(D3D::Model _misilemodel, D3D::Model _misilhitemodel, D3D::Model _minimapmisilemodel, DirectX::XMFLOAT2 _misilehituvsize)
{
	MisileModel = _misilemodel;
	MisileHitModel = _misilhitemodel;
	MisileHitUVSize = _misilehituvsize;
	MiniMapMisileModel = _minimapmisilemodel;
}

void CEnemyAttack::SetGunInf(D3D::Model _gunmodel, D3D::Model _gunhitmodel, DirectX::XMFLOAT2 _ballhituvsize)
{
	GunModel = _gunmodel;
	GunHitModel = _gunhitmodel;
	GunHitUVSize = _ballhituvsize;
}

void CEnemyAttack::GetShotBalls(std::list<CShottoBall*>& _shotballs)
{
	_shotballs.clear();

	for (auto ShotBall : vShottoBall)
	{
		_shotballs.push_back(ShotBall);
	}
}
