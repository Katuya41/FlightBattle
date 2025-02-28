#include "CMisileManager.h"
#include <DirectXMath.h>
#include "CMisile.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CEffect.h"
#include "xa2.h"

//ミサイルが消えるまでの時間
#define DeleteMisileTime (6000)

//ミサイルの初期速度
#define MISILE_SPEED (0.3)

//ミサイルの装填時間
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
	res->SetModel(model);						//モデル
	res->Scaling = { 0.2f, 0.2f, 0.2f };		//大きさ設定
	res->SetTag(CGameObject::MISILE);			//タグの設定
	res->SetMisileState(CMisile::MISILE_STATE::READY);
	res->SetUv_Size(UVSize);
	res->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -1.0f, _spornobj->Position.y + _spornobj->GetUp().y * -1.0f, _spornobj->Position.z + _spornobj->GetUp().z * -1.0f });				//スポーンする座標
	res->SetSpornRotate(_spornobj->Rotation);			//角度設定
	res->SetInitRotateMatrix();						//初期行列設定
	res->SetVector();							//現在のvector計算
	res->SetMoveSpeed((float)MISILE_SPEED);				//初期速度
	res->SetTarget(_target);					//ターゲット情報取得
	res->ShotTime = GetTickCount64();			//発車時刻の設定
	res->SetMisileParticleTime(GetTickCount64());
	res->SetHitMisileEffect(MisileHitModel);	//ミサイルの当たった時のエフェクトテクスチャ
	//XA_Play(SE_MISILESHOTTO);

	//ミサイルUIの非アクティブ化
	CGame::Get()->SetMisileUI(_misileuinum, false);
	MisileUi[_misileuinum].Active = CGame::Get()->GetMisileUIActive(_misileuinum);
	MisileUi[_misileuinum].OldTime = GetTickCount64();

	vMisile.push_back(res);
}
