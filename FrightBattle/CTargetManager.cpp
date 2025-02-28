#include "CTargetManager.h"
#include "CEnemy.h"
#include "CGame.h"
#include "CCamera.h"

//ターゲット可能距離
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
		//ターゲット有効
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

	//////敵を格納
	////vEnemy.push_back(_enemy);
	////MaxNum += 1;
}

void CTargetManager::SetTargetNum()
{
	//現在のターゲット状態をなくす
	//ターゲット時間をリセット
	vEnemy[TargetNum]->SetIsTarget(false);
	vEnemy[TargetNum]->SetTargetTime(0);
	int res = TargetNum;

	do
	{
		//次の敵を設定
		if ((TargetNum + 1) >= vEnemy.size())
			TargetNum = 0;
		else
			TargetNum++;

		//最初に設定した敵と一緒だったら
		//初期設定の敵をターゲット(現時点ではターゲット不可能な透明の敵)
		if (res == TargetNum)
		{
			if ((TargetNum + 1) >= vEnemy.size())
				TargetNum = 0;
			else
				TargetNum++;
			break;
		}

		//下の条件がtrueだったら繰り返す
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
