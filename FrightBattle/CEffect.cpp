#include "CEffect.h"
#include "Animation.h"

using namespace DirectX;

/**
*CEffect�̃R���X�g���N�^�ł��B
*/
CEffect::CEffect()
{
	anim = new Animation();
	anim->SetPattern(0);
	UV_Size = { 0.0f,0.0f };
}

/**
*CEffect�̃f�X�g���N�^�ł��B
*/
CEffect::~CEffect()
{
	delete anim;
}

/**
*UV�A�j���[�V�����̍s���Ԃ��֐��ł��B
*
* @return UV�A�j���[�V�����̍s��
*/
DirectX::XMMATRIX CEffect::GetUvMatrix()
{
	XMFLOAT2 uv = anim->GetUV();
	return XMMatrixTranslation(uv.x, uv.y, 0.0f);
}

/**
*CEffect�̍X�V�������s���֐��ł��B
*/
void CEffect::Update()
{
	anim->Update();

	if (!anim->GetIsPlaying())
	{
		SetActive(false);
	}

	//CGameObject::Update();
}

/**
*�A�j���[�V�����̏�Ԃ��擾����֐��ł��B
*/
void CEffect::Play()
{
	anim->SetIsPlaying(true);
	SetActive(true);
}

/**
*�A�j���[�V�����̏�Ԃ��擾����֐��ł��B
*/
bool CEffect::GetAnimPlay()
{
	return anim->GetIsPlaying();
}

void CEffect::ResetAnimation()
{
	anim->ResetAnimation();
}

/**
*�A�j���[�V�����p�^�[����ݒ肷��֐��ł��B
*
* @param �A�j���[�V�����p�^�[���ԍ�
*/
void CEffect::SetAnimPattern(int _num)
{
	anim->SetPattern(_num);
}

/**
*�A�j���[�V������UV�T�C�Y��ݒ肷��֐��ł��B
*
* @param _u UV�T�C�Y��U
* @param _v UV�T�C�Y��V
*/
void CEffect::SetAnimUVSize(float _u, float _v)
{
	UV_Size = { _u,_v };
	anim->SetUv(UV_Size);
}

void CEffect::SetAnimSpeed(float _animspeed)
{
	anim->SetAnimSpeed(_animspeed);
}
