#pragma once
#include "direct3d.h"
#include <DirectXCollision.h>

class CGameObject;

/**
 * �����CCheckCollision�N���X�̐����ł��B
 * ���̃N���X�͓����蔻��̐���Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-10-21
 */
class CCheckCollision
{
private:
	/**
	* CCheckCollision�̃R���X�g���N�^�ł��B
	*/
	CCheckCollision();

	/**
	* CCheckCollision�̃f�X�g���N�^�ł��B
	*/
	~CCheckCollision();

public:

	static CCheckCollision* Get();

	/**
	* �����蔻���Ԃ��֐��ł��B
	* 
	* @param _bc1 ���肷�铖���蔻��1(AABB)
	* @param _bc2 ���肷�铖���蔻��2(AABB)
	* @return �Փ˂��Ă��邩
	*/
	bool CheckColBoxAABBAABB(DirectX::BoundingBox& _bc1, DirectX::BoundingBox& _bc2);

	/**
	* �����蔻���Ԃ��֐��ł��B
	*
	* @param _bc1 ���肷�铖���蔻��1(OBB)
	* @param _bc2 ���肷�铖���蔻��2(AABB)
	* @return �Փ˂��Ă��邩
	*/
	bool CheckColBoxOBBAABB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingBox& _bc2);

	/**
	* �����蔻���Ԃ��֐��ł��B
	*
	* @param _bc1 ���肷�铖���蔻��1(OBB)
	* @param _bc2 ���肷�铖���蔻��2(OBB)
	* @return �Փ˂��Ă��邩
	*/
	bool CheckColBoxOBBOBB(DirectX::BoundingOrientedBox& _bc1, DirectX::BoundingOrientedBox& _bc2);

};

#include "CCheckCollision.inl"