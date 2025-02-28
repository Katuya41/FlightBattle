#include "CMisileManager.h"
#include <DirectXMath.h>
#include "CMisile.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CEffect.h"
#include "xa2.h"

//�~�T�C����������܂ł̎���
#define DeleteMisileTime (6000)

//�~�T�C���̏������x
#define MISILE_SPEED (0.3)

//�~�T�C���̑��U����
#define MISILE_TIME (2000)
//#define MISILE_TIME (0)

using namespace DirectX;

CMisileManager::CMisileManager()
{

}

CMisileManager* CMisileManager::Get()
{
	static CMisileManager instance;
	return &instance;
}

void CMisileManager::Update()
{
	for (auto i = vMisile.begin(); i != vMisile.end(); i++)
	{
		if (GetTickCount64() >= (*i)->ShotTime + DeleteMisileTime || !(*i)->GetActive() || (*i)->GetMisileHit())
		{
			vMisile.erase(i);
			break;
		}
		else
		{
			(*i)->Update();
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (!MisileUi[i].Active)
		{
			if (GetTickCount64() >= MisileUi[i].OldTime + MISILE_TIME)
			{
				CGame::Get()->SetMisileUI(i, true);
				MisileUi[i].Active = CGame::Get()->GetMisileUIActive(i);
			}
		}
	}
}

void CMisileManager::Draw()
{
	for (auto i = vMisile.begin(); i != vMisile.end(); i++)
	{
		(*i)->Draw();
	}
}

void CMisileManager::SetUV_Size(DirectX::XMFLOAT2 _uvsize)
{
	UVSize = _uvsize;
}

void CMisileManager::SetMisileInf(CGameObject* _spornobj, CEnemy* _target, int _misileuinum)
{
	CMisile* res = new CMisile();
	res->SetModel(model);						//���f��
	res->Scaling = { 0.2f, 0.2f, 0.2f };		//�傫���ݒ�
	res->SetTag(CGameObject::MISILE);			//�^�O�̐ݒ�
	res->SetMisileState(CMisile::MISILE_STATE::READY);
	res->SetUv_Size(UVSize);
	res->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -1.0f, _spornobj->Position.y + _spornobj->GetUp().y * -1.0f, _spornobj->Position.z + _spornobj->GetUp().z * -1.0f });				//�X�|�[��������W
	res->SetSpornRotate(_spornobj->Rotation);			//�p�x�ݒ�
	res->SetInitRotateMatrix();						//�����s��ݒ�
	res->SetVector();							//���݂�vector�v�Z
	res->SetMoveSpeed((float)MISILE_SPEED);				//�������x
	res->SetTarget(_target);					//�^�[�Q�b�g���擾
	res->ShotTime = GetTickCount64();			//���Ԏ����̐ݒ�
	res->SetMisileParticleTime(GetTickCount64());
	res->SetHitMisileEffect(MisileHitModel);	//�~�T�C���̓����������̃G�t�F�N�g�e�N�X�`��
	//XA_Play(SE_MISILESHOTTO);

	//�~�T�C��UI�̔�A�N�e�B�u��
	CGame::Get()->SetMisileUI(_misileuinum, false);
	MisileUi[_misileuinum].Active = CGame::Get()->GetMisileUIActive(_misileuinum);
	MisileUi[_misileuinum].OldTime = GetTickCount64();

	vMisile.push_back(res);
}
