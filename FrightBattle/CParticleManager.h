#pragma once

#include"direct3d.h"
#include <vector>

class CGameObject;

/**
 * �����CParticleManager�N���X�̐����ł��B
 * ���̃N���X�̓p�[�e�B�N������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CParticleManager
{
	struct PARTICLE
	{
		CGameObject* Object;
		float CoolTime;				//�N�[���^�C��
		ULONGLONG DeleteTime;			//��������
		ULONGLONG StartTime;			//�J�n����
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

	//�p�[�e�B�N���̃��f��
	D3D::Model Models[MAX];
	//std::vector<MODEL_STR> vParticleModel;

	//�p�[�e�B�N��������ϐ�
	std::vector<PARTICLE> vParticle;

	CParticleManager() {}
	~CParticleManager() {}

public:

	static CParticleManager* Get();

	//�X�V����
	void Update();

	//�`�揈��
	void Draw();

	//�p�[�e�B�N���Z�b�g
	void SetParticle(MODEL_NAME _name, DirectX::XMFLOAT3 _position);

	void ClearvParticle() { vParticle.clear(); }

	//�p�[�e�B�N���̃��f���Z�b�g
	void SetParticleModel(D3D::Model _model, MODEL_NAME _name);

	//�p�[�e�B�N���̃��f���擾
	D3D::Model GetParticleModel(MODEL_NAME _name);
};

