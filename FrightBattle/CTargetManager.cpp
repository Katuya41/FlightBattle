#include "CTargetManager.h"
#include "CEnemy.h"
#include "CGame.h"
#include "CCamera.h"

//�^�[�Q�b�g�\����
#define MAX_TARGET_DISTANCE (100)

CTargetManager* CTargetManager::Get()
{
	static CTargetManager instance;
	return &instance;
}

void CTargetManager::Update()
{
	if (vEnemy[TargetNum]->GetHp() > 0)
	{
		//�^�[�Q�b�g�L��
		vEnemy[TargetNum]->SetIsTarget(true);
	}
	else
	{
		auto it = vEnemy.begin() + TargetNum;
		vEnemy.erase(it);
		TargetNum = 0;
	}
}

void CTargetManager::SetEnemy()
{
	vEnemy = CGame::Get()->GetEnemys();

	//////�G���i�[
	////vEnemy.push_back(_enemy);
	////MaxNum += 1;
}

void CTargetManager::SetTargetNum()
{
	//���݂̃^�[�Q�b�g��Ԃ��Ȃ���
	//�^�[�Q�b�g���Ԃ����Z�b�g
	vEnemy[TargetNum]->SetIsTarget(false);
	vEnemy[TargetNum]->SetTargetTime(0);
	int res = TargetNum;

	do
	{
		//���̓G��ݒ�
		if ((TargetNum + 1) >= vEnemy.size())
			TargetNum = 0;
		else
			TargetNum++;

		//�ŏ��ɐݒ肵���G�ƈꏏ��������
		//�����ݒ�̓G���^�[�Q�b�g(�����_�ł̓^�[�Q�b�g�s�\�ȓ����̓G)
		if (res == TargetNum)
		{
			if ((TargetNum + 1) >= vEnemy.size())
				TargetNum = 0;
			else
				TargetNum++;
			break;
		}

		//���̏�����true��������J��Ԃ�
	} while (CheckIsTarget());
}

bool CTargetManager::CheckIsTarget()
{
	if (vEnemy[TargetNum]->GetIsCameraRef() && vEnemy[TargetNum]->GetPlayerDistance() > MAX_TARGET_DISTANCE)
		return true;
	else
		return false;
}

CEnemy* CTargetManager::GetTargetObj()
{
	return vEnemy[TargetNum];

	//return nullptr;
}
