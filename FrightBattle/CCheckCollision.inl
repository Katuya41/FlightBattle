#pragma once
#include "CCheckCollision.h"
#include "CGameObject.h"

/**
* CCheckCollisionのコンストラクタです。
*/
inline CCheckCollision::CCheckCollision()
{
}

/**
* CCheckCollisionのデストラクタです。
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
* 当たり判定を返す関数です。
*
* @param _bc1 判定する当たり判定1(AABB)
* @param _bc2 判定する当たり判定2(AABB)
* @return 衝突しているか
*/
inline bool CCheckCollision::CheckColBoxAABBAABB(DirectX::BoundingBox& _bc1, DirectX::BoundingBox& _bc2)
{
	return _bc1.Intersects(_bc2);
}

/**
* 当たり判定を返す関数です。
*
* @param _bc1 判定する当たり判定1(OBB)
* @param _bc2 判定する当たり判定2(AABB)
* @return 衝突しているか
*/
inline bool CCheckCollision::CheckColBoxOBBAABB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingBox& _bc2)
{
	return _bc1.Intersects(_bc2);
}

/**
* 当たり判定を返す関数です。
*
* @param _bc1 判定する当たり判定1(OBB)
* @param _bc2 判定する当たり判定2(OBB)
* @return 衝突しているか
*/
inline bool CCheckCollision::CheckColBoxOBBOBB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingOrientedBox& _bc2)
{
	return _bc1.Intersects(_bc2);
}