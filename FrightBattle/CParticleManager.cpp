#include "CParticleManager.h"
#include "CGameObject.h"

//消える速度
#define OUTSPEED (0.6)

//パーティクルが大きくなる速度
#define BIGSPEED (0.0016)

//ミサイルのパーティクルの維持時間
#define PARTICLE_DELETETIME (1500)

CParticleManager* CParticleManager::Get()
{
	static CParticleManager instance;
	return &instance;
}

void CParticleManager::Update()
{
	//トレイル(ミサイルなどの軌道のこと)
	//1ミサイルごとに格納できるパーティクル数を設定しておく
	//最大数になったら、最初のパーティクルを消去して新しく追加する

	for (auto i = vParticle.begin(); i != vParticle.end(); i++)
	{
		if (GetTickCount64() >= i->StartTime + i->DeleteTime)
		{
			i->Object->materialDiffuse.w -= (float)OUTSPEED;

			if (i->Object->materialDiffuse.w <= 0.3)
			{
				vParticle.erase(i);
				break;
			}
		}
	}
}

void CParticleManager::Draw()
{
	for (auto i = vParticle.begin(); i != vParticle.end(); i++)
	{
		i->Object->Draw();
	}
}

void CParticleManager::SetParticle(MODEL_NAME _name, DirectX::XMFLOAT3 _position)
{
	PARTICLE particle;
	//CGameObject* Res = new CGameObject();
	//Res->Position = _position;
	//Res->Scaling = { 0.6f, 0.6f, 0.6f };



	particle.Object = new CGameObject();
	particle.Object->Position = _position;
	particle.Object->Scaling = { 0.2f, 0.2f, 0.2f };
	//particle.Object->SetBillboard(true);
	particle.Object->SetActive(true);
	particle.Object->SetLighting(false);
	particle.Object->SetModel(Models[_name]);

	particle.DeleteTime = PARTICLE_DELETETIME;
	particle.StartTime = GetTickCount64();

	vParticle.push_back(particle);
	//delete Res;
}

void CParticleManager::SetParticleModel(D3D::Model _model, MODEL_NAME _name)
{
	Models[_name] = _model;
}

D3D::Model CParticleManager::GetParticleModel(MODEL_NAME _name)
{
	return Models[_name];
}
