#pragma once
#include "direct3d.h"
#include <DirectXCollision.h>

class CGameObject;

/**
 * これはCCheckCollisionクラスの説明です。
 * このクラスは当たり判定の制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-10-21
 */
class CCheckCollision
{
private:
	/**
	* CCheckCollisionのコンストラクタです。
	*/
	CCheckCollision();

	/**
	* CCheckCollisionのデストラクタです。
	*/
	~CCheckCollision();

public:

	static CCheckCollision* Get();

	/**
	* 当たり判定を返す関数です。
	* 
	* @param _bc1 判定する当たり判定1(AABB)
	* @param _bc2 判定する当たり判定2(AABB)
	* @return 衝突しているか
	*/
	bool CheckColBoxAABBAABB(DirectX::BoundingBox& _bc1, DirectX::BoundingBox& _bc2);

	/**
	* 当たり判定を返す関数です。
	*
	* @param _bc1 判定する当たり判定1(OBB)
	* @param _bc2 判定する当たり判定2(AABB)
	* @return 衝突しているか
	*/
	bool CheckColBoxOBBAABB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingBox& _bc2);

	/**
	* 当たり判定を返す関数です。
	*
	* @param _bc1 判定する当たり判定1(OBB)
	* @param _bc2 判定する当たり判定2(OBB)
	* @return 衝突しているか
	*/
	bool CheckColBoxOBBOBB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingOrientedBox& _bc2);

};

#include "CCheckCollision.inl"