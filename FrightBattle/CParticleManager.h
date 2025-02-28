#pragma once

#include"direct3d.h"
#include <vector>

class CGameObject;

/**
 * これはCParticleManagerクラスの説明です。
 * このクラスはパーティクル制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CParticleManager
{
	struct PARTICLE
	{
		CGameObject* Object;
		float CoolTime;				//クールタイム
		ULONGLONG DeleteTime;			//消去時間
		ULONGLONG StartTime;			//開始時間
	};

public:
	enum MODEL_NAME
	{
		MISILE_PARTICLE,
		CONTRAIL_PARTICLE,
		SMOKE_PARTICLE,
		MAX,
	};

private:
	struct MODEL_STR
	{
		D3D::Model model;
		MODEL_NAME name;
	};

	//パーティクルのモデル
	D3D::Model Models[MAX];
	//std::vector<MODEL_STR> vParticleModel;

	//パーティクルを入れる変数
	std::vector<PARTICLE> vParticle;

	CParticleManager() {}
	~CParticleManager() {}

public:

	static CParticleManager* Get();

	//更新処理
	void Update();

	//描画処理
	void Draw();

	//パーティクルセット
	void SetParticle(MODEL_NAME _name, DirectX::XMFLOAT3 _position);

	void ClearvParticle() { vParticle.clear(); }

	//パーティクルのモデルセット
	void SetParticleModel(D3D::Model _model, MODEL_NAME _name);

	//パーティクルのモデル取得
	D3D::Model GetParticleModel(MODEL_NAME _name);
};

