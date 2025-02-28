#include "CMisileUI.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include "CPlayerAttack.h"

//ミサイルの装填時間
#define MISILE_TIME (6000)

CMisileUI::CMisileUI()
{
	pEffect = new CEffect();
	pEffect->SetBillboard(true);
	pEffect->SetLighting(false);
	//pEffect->Position = { 1.35f,-0.1f,-0.2f };
	pEffect->Scaling = { 1.0f,1.0f,1.0f };

	//AnimSpeed = 0.03f;

	AnimSpeed = CPlayerAttack::Get()->GetMisileParameter().Misile_Charge_Speed;
}

CMisileUI::~CMisileUI()
{
	delete pEffect;
	pEffect = nullptr;
}

void CMisileUI::Update()
{
	//現在の状態によってActiveを変更
	switch (MisileUIState)
	{
	case CMisileUI::READY:
		isActive = true;
		break;
	case CMisileUI::CHARGE:
		//チャージのアニメーションが終了したらREADYに変更
		if (pEffect->GetAnimPlay())
		{
			isActive = false;
		}
		else
		{
			isActive = true;
			MisileUIState = READY;
		}
		break;
	default:
		break;
	}
}

void CMisileUI::SetMisileUIStateCharge()
{
	MisileUIState = CHARGE;
	isActive = false;
	AnimSpeed = CPlayerAttack::Get()->GetMisileParameter().Misile_Charge_Speed;
	CEffectManager::Get()->SetvEffectMisileUI(pEffect, 4, AnimSpeed);
}

void CMisileUI::SetMisileUIInf(D3D::Model _model, DirectX::XMFLOAT2 _effectsize)
{
	pEffect->SetModel(_model);
	pEffect->SetAnimUVSize(_effectsize.x, _effectsize.y);
}

void CMisileUI::SetEffectPos(const DirectX::XMFLOAT3& _pos)
{
	pEffect->Position = _pos;
}

void CMisileUI::SetEffectScale(const DirectX::XMFLOAT3& _scale)
{
	pEffect->Scaling = _scale;
}

void CMisileUI::ResetMisileUI()
{
	MisileUIState = READY;
	pEffect->ResetAnimation();
}
