#include "CEffectManager.h"
#include "CEffect.h"

CEffectManager::~CEffectManager()
{
	vEffect.clear();
	vEffect2D.clear();
}

CEffectManager* CEffectManager::Get()
{
	static CEffectManager instance;
	return &instance;
}

/**
*CEffectの更新処理を行う関数です。
*/
void CEffectManager::Update()
{
	for (auto i = vEffect.begin() ; i != vEffect.end(); i++)
	{
		if ((*i)->GetAnimPlay())
		{
			(*i)->Update();
		}
		else
		{
			delete (*i);
			(*i) = nullptr;
			vEffect.erase(i);
			return;
		}
	}

	for (auto i = vEffect2D.begin(); i != vEffect2D.end(); i++)
	{
		if ((*i)->GetAnimPlay())
		{
			(*i)->Update();
		}
		else
		{
			vEffect2D.erase(i);
			return;
		}
	}
}

/**
*CEffectの描画処理行う関数です。
*/
void CEffectManager::Draw()
{
	for (auto i = vEffect.begin(); i < vEffect.end(); i++)
	{
		(*i)->Draw();
	}
}

void CEffectManager::Draw2D()
{
	for (auto i = vEffect2D.begin(); i < vEffect2D.end(); i++)
	{
		(*i)->Draw();
	}
}

/**
*エフェクトをセットする関数です。
*
* @param セットするエフェクト
*/
void CEffectManager::SetvEffect(CEffect* _effect, DirectX::XMFLOAT3 _position, int _animpattern, float _animspeed)
{
	CEffect* s;
	s = new CEffect();
	s->SetBillboard(true);
	s->SetLighting(false);
	s->SetModel(_effect->GetModel());
	s->SetAnimPattern(_animpattern);
	s->SetAnimSpeed(_animspeed);
	s->Position = _position;
	s->SetAnimUVSize(_effect->GetAnimUV().x, _effect->GetAnimUV().y);
	s->Play();

	vEffect.push_back(s);
}

void CEffectManager::Set3DvEffect(CEffect* _effect, int _animpattern, float _animspeed)
{
	_effect->SetAnimPattern(_animpattern);
	_effect->SetAnimSpeed(_animspeed);
	_effect->Play();

	vEffect.push_back(_effect);
}

void CEffectManager::SetvEffectMisileUI(CEffect* _effect, int _animpattern, float _animspeed)
{
	//CEffect* s;
	//s = new CEffect();
	//s->SetBillboard(true);
	//s->SetLighting(false);
	//s->SetModel(_effect->GetModel());
	//s->SetAnimPattern(_animpattern);
	//s->SetAnimSpeed(_animspeed);
	//s->Position = _effect->Position;
	//s->SetAnimUVSize(_effect->GetAnimUV().x, _effect->GetAnimUV().y);
	//s->Play();

	//vEffect.push_back(s);

	_effect->SetAnimPattern(_animpattern);
	_effect->SetAnimSpeed(_animspeed);
	_effect->Play();

	vEffect2D.push_back(_effect);
}

void CEffectManager::ClearEffect()
{
	for (auto Effect : vEffect)
	{
		if (Effect != nullptr)
		{
			delete Effect;
			Effect = nullptr;
		}
	}

	vEffect.clear(); 
	vEffect2D.clear();
}
