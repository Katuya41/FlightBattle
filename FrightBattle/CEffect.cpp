#include "CEffect.h"
#include "Animation.h"

using namespace DirectX;

/**
*CEffectのコンストラクタです。
*/
CEffect::CEffect()
{
	anim = new Animation();
	anim->SetPattern(0);
	UV_Size = { 0.0f,0.0f };
}

/**
*CEffectのデストラクタです。
*/
CEffect::~CEffect()
{
	delete anim;
}

/**
*UVアニメーションの行列を返す関数です。
*
* @return UVアニメーションの行列
*/
DirectX::XMMATRIX CEffect::GetUvMatrix()
{
	XMFLOAT2 uv = anim->GetUV();
	return XMMatrixTranslation(uv.x, uv.y, 0.0f);
}

/**
*CEffectの更新処理を行う関数です。
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
*アニメーションの状態を取得する関数です。
*/
void CEffect::Play()
{
	anim->SetIsPlaying(true);
	SetActive(true);
}

/**
*アニメーションの状態を取得する関数です。
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
*アニメーションパターンを設定する関数です。
*
* @param アニメーションパターン番号
*/
void CEffect::SetAnimPattern(int _num)
{
	anim->SetPattern(_num);
}

/**
*アニメーションのUVサイズを設定する関数です。
*
* @param _u UVサイズのU
* @param _v UVサイズのV
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
