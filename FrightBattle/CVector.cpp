#include "CVector.h"
#include "CPlayerAttack.h"
#include "CEnemy.h"
#include "CGame.h"
#include "CPlayer.h"
#include "CCamera.h"
#include "CScene.h"

using namespace DirectX;

CVector::CVector()
{
	isActive = false;
}

void CVector::Update()
{
	CGameObject* Player = CScene::Get()->GetPlayer();
	XMFLOAT3 PlayerPos = CScene::Get()->GetPlayer()->Position;

	if (!CPlayerAttack::Get()->GetIsOnetarget())
	{
		return;
	}

	//矢印の座標を設定
	if (!CGame::Get()->GetIsFirst())	//三人称カメラ
	{
		Scaling = { 0.1f, 0.1f, 0.1f };

		//Position = { CGame::Get()->GetPlayer()->Position.x + (CGame::Get()->GetPlayer()->GetUp().x * 1.0f),
		//			 CGame::Get()->GetPlayer()->Position.y + (CGame::Get()->GetPlayer()->GetUp().y * 1.0f),
		//			 CGame::Get()->GetPlayer()->Position.z + (CGame::Get()->GetPlayer()->GetUp().z * 1.0f) };

		Position = { PlayerPos.x + (Player->GetUp().x * 1.0f),
					PlayerPos.y + (Player->GetUp().y * 1.0f),
					PlayerPos.z + (Player->GetUp().z * 1.0f) };

	}
	else if (CGame::Get()->GetIsFirst())	//一人称カメラ
	{
		Scaling = { 0.05f, 0.05f, 0.05f };

		Position.x = CGame::Get()->GetPlayer()->Position.x + (CGame::Get()->GetPlayer()->GetForward().x * 2.0f) + (CGame::Get()->GetPlayer()->GetUp().x * 0.5f);
		Position.y = CGame::Get()->GetPlayer()->Position.y + (CGame::Get()->GetPlayer()->GetForward().y * 2.0f) + (CGame::Get()->GetPlayer()->GetUp().y * 0.5f);
		Position.z = CGame::Get()->GetPlayer()->Position.z + (CGame::Get()->GetPlayer()->GetForward().z * 2.0f) + (CGame::Get()->GetPlayer()->GetUp().z * 0.5f);

		Position.x = Player->Position.x + (Player->GetForward().x * 2.0f) + (Player->GetUp().x * 0.5f);
		Position.y = Player->Position.y + (Player->GetForward().y * 2.0f) + (Player->GetUp().y * 0.5f);
		Position.z = Player->Position.z + (Player->GetForward().z * 2.0f) + (Player->GetUp().z * 0.5f);

	}

	//ターゲットを設定
	SetTarget(CPlayerAttack::Get()->GetTargetObj());

	//現在のオブジェクトの軸を計算
	CGameObject::SetVector();

	if (Target != nullptr)
	{
		//ターゲットへの方向ベクトル計算
		fDirect = SetDirect();

		//それをもとに角度を計算
		float dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(newForward), vDirect));
		float angle = acosf(dotProduct);
		float an = XMConvertToDegrees(angle);

		if (an > 0.0f)
		{
			//プレイヤーの正面とターゲットの方向の外積を計算
			//これにより回転する軸を取得
			XMVECTOR axis = XMVector3Cross(newForward, vDirect);

			//回転ベクトル作成
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

			//回転行列作成
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//オブジェクトの回転行列に加算
			RotateMatrix *= QMT;
		}

		//回転更新
		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}


}

void CVector::Draw()
{
	if (!CPlayerAttack::Get()->GetIsOnetarget())
	{
		return;
	}

	//ターゲットがいるかつターゲットが画面外のときに描画
	if (Target != nullptr)
	{
		if (!Target->GetIsCameraRef())
			CGameObject::Draw();
	}
}

XMFLOAT3 CVector::SetDirect()
{
	// オブジェクト間の位置ベクトルを計算
	//方向はターゲット-プレイヤーの位置で求められる
	XMFLOAT3 directionVector;
	directionVector.x = TargetPos.x - Position.x;
	directionVector.y = TargetPos.y - Position.y;
	directionVector.z = TargetPos.z - Position.z;

	// 位置ベクトルを正規化
	vDirect = XMVector3Normalize(XMLoadFloat3(&directionVector));

	// 正規化された方向ベクトルを取得
	XMFLOAT3 normalizedDirection;
	XMStoreFloat3(&normalizedDirection, vDirect);

	return normalizedDirection;
}

void CVector::SetTarget(CEnemy* _target)
{
	Target = _target;

	if (Target != nullptr)
		TargetPos = Target->Position;
	else
		TargetPos = { 0,0,0 };

	if (Target->GetTag() == INITENEMY)
	{
		isActive = false;
	}
	else
	{
		isActive = true;
	}
}
