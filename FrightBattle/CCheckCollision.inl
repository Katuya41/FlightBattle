#pragma once
#include "CCheckCollision.h"
#include "CGameObject.h"

/**
* CCheckCollision�̃R���X�g���N�^�ł��B
*/
inline CCheckCollision::CCheckCollision()
{
}

/**
* CCheckCollision�̃f�X�g���N�^�ł��B
*/
inline CCheckCollision::~CCheckCollision()
{
}

inline CCheckCollision* CCheckCollision::Get()
{
	static CCheckCollision instance;
	return &instance;
}

/**
* �����蔻���Ԃ��֐��ł��B
*
* @param _bc1 ���肷�铖���蔻��1(AABB)
* @param _bc2 ���肷�铖���蔻��2(AABB)
* @return �Փ˂��Ă��邩
*/
inline bool CCheckCollision::CheckColBoxAABBAABB(DirectX::BoundingBox& _bc1, DirectX::BoundingBox& _bc2)
{
	return _bc1.Intersects(_bc2);
}

/**
* �����蔻���Ԃ��֐��ł��B
*
* @param _bc1 ���肷�铖���蔻��1(OBB)
* @param _bc2 ���肷�铖���蔻��2(AABB)
* @return �Փ˂��Ă��邩
*/
inline bool CCheckCollision::CheckColBoxOBBAABB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingBox& _bc2)
{
	return _bc1.Intersects(_bc2);
}

/**
* �����蔻���Ԃ��֐��ł��B
*
* @param _bc1 ���肷�铖���蔻��1(OBB)
* @param _bc2 ���肷�铖���蔻��2(OBB)
* @return �Փ˂��Ă��邩
*/
inline bool CCheckCollision::CheckColBoxOBBOBB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingOrientedBox& _bc2)
{
	return _bc1.Intersects(_bc2);
}