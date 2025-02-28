#include "CEnemy.h"
#include "CGame.h"
#include "CMisile.h"
#include "CScene.h"
#include "CCamera.h"
#include "CPlayer.h"
#include "CMisile.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include <random>
#include <iostream>
#include "CEnemyAttack.h"
//#include "CPlayerAttack.h"

#include "DebugUI.h"

using namespace DirectX;

void CEnemy::GetRandomNum(float _maxnum, float _minnum, float& _getnum)
{
	// 乱数生成器の初期化
	//ハードウェアの乱数生成器を使用する
	//メルセンヌ・ツイスタ法に基づく擬似乱数生成器
	std::random_device rd; 
	std::mt19937 rng(rd()); 

	// 乱数の範囲を指定
	std::uniform_real_distribution<float> dist(_maxnum, _minnum);

	_getnum = dist(rng);
}

void CEnemy::CheckRefCamera()
{
	XMMATRIX matrix = XMMatrixMultiply(CGame::Get()->GetCamera3D()->GetViewMatrix(),
		CGame::Get()->GetCamera3D()->GetProjectionMatrix());

	//カメラの視錐台を構築
	//カメラの視錐台を構築する6つの平面を定義する
	Plane planes[6];

	// Left plane
	planes[0].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[0],
		0.0f);
	planes[0].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[0];
	NormalizePlane(planes[0]);

	// Right plane
	planes[1].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[0],
		0.0f);
	planes[1].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[0];
	NormalizePlane(planes[1]);

	// Top plane
	planes[2].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[1],
		0.0f);
	planes[2].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[1];
	NormalizePlane(planes[2]);

	// Bottom plane
	planes[3].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[1],
		0.0f);
	planes[3].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[1];
	NormalizePlane(planes[3]);

	// Near plane
	planes[4].normal = XMVectorSet(matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[2],
		0.0f);
	planes[4].distance = matrix.r[3].m128_f32[2];
	NormalizePlane(planes[4]);

	// Far plane
	planes[5].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[2],
		0.0f);
	planes[5].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[2];

	NormalizePlane(planes[5]);

	//当たり判定をOBBに設定
	DirectX::BoundingOrientedBox obb = ColliderBoxOBB;

	//当たり判定の頂点を設定
	XMFLOAT3 fboxVertices[8];
	obb.GetCorners(fboxVertices);
	XMVECTOR boxVertices[8];
	for (int i = 0; i < 8; i++)
	{
		boxVertices[i] = XMLoadFloat3(&(fboxVertices[i]));
	}
	//XMVECTOR boxVertices[8] = XMLoadFloat3(fboxVertices);

	//オブジェクトが視錐台内にあるかどうかを判断
	int cnt = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			float distance = XMVectorGetX(XMPlaneDotCoord(planes[i].normal, boxVertices[j])) + planes[i].distance;
			if (distance <= 0.0f) {
				//オブジェクトが視錐台内にない
				cnt++;
			}
		}
	}

	if (cnt >= 8)
	{
		//オブジェクトが視錐台内がない
		IsRefMainCamera = false;
	}
	else
	{
		//オブジェクトが視錐台内にある
		IsRefMainCamera = true;
	}
}

void CEnemy::CheckRefCameraCenter()
{
	// 現在のプロジェクション行列を取得
	auto projMatrix = CGame::Get()->GetCamera3D()->GetProjectionMatrix();

	// 新しい視野角（FOV）を設定 (現在より狭くする)
	float fov = XMConvertToRadians(10); // 45度から22.5度に縮小
	

	// アスペクト比や近/遠クリップ面の値は同じ
	float aspectRatio = 16.0f / 9.0f;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	// 新しいプロジェクション行列を作成
	projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

	XMMATRIX matrix = XMMatrixMultiply(CGame::Get()->GetCamera3D()->GetViewMatrix(),
		projMatrix);

	//カメラの視錐台を構築
	//カメラの視錐台を構築する6つの平面を定義する
	Plane planes[6];

	// Left plane
	planes[0].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[0],
		0.0f);
	planes[0].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[0];
	NormalizePlane(planes[0]);

	// Right plane
	planes[1].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[0],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[0],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[0],
		0.0f);
	planes[1].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[0];
	NormalizePlane(planes[1]);

	// Top plane
	planes[2].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[1],
		0.0f);
	planes[2].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[1];
	NormalizePlane(planes[2]);

	// Bottom plane
	planes[3].normal = XMVectorSet(matrix.r[0].m128_f32[3] + matrix.r[0].m128_f32[1],
		matrix.r[1].m128_f32[3] + matrix.r[1].m128_f32[1],
		matrix.r[2].m128_f32[3] + matrix.r[2].m128_f32[1],
		0.0f);
	planes[3].distance = matrix.r[3].m128_f32[3] + matrix.r[3].m128_f32[1];
	NormalizePlane(planes[3]);

	// Near plane
	planes[4].normal = XMVectorSet(matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[2],
		0.0f);
	planes[4].distance = matrix.r[3].m128_f32[2];
	NormalizePlane(planes[4]);

	// Far plane
	planes[5].normal = XMVectorSet(matrix.r[0].m128_f32[3] - matrix.r[0].m128_f32[2],
		matrix.r[1].m128_f32[3] - matrix.r[1].m128_f32[2],
		matrix.r[2].m128_f32[3] - matrix.r[2].m128_f32[2],
		0.0f);
	planes[5].distance = matrix.r[3].m128_f32[3] - matrix.r[3].m128_f32[2];

	NormalizePlane(planes[5]);

	//当たり判定をOBBに設定
	DirectX::BoundingOrientedBox obb = ColliderBoxOBB;

	//当たり判定の頂点を設定
	XMFLOAT3 fboxVertices[8];
	obb.GetCorners(fboxVertices);
	XMVECTOR boxVertices[8];
	for (int i = 0; i < 8; i++)
	{
		boxVertices[i] = XMLoadFloat3(&(fboxVertices[i]));
	}
	//XMVECTOR boxVertices[8] = XMLoadFloat3(fboxVertices);

	//オブジェクトが視錐台内にあるかどうかを判断
	int cnt = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			float distance = XMVectorGetX(XMPlaneDotCoord(planes[i].normal, boxVertices[j])) + planes[i].distance;
			if (distance <= 0.0f) {
				//オブジェクトが視錐台内にない
				cnt++;
			}
		}
	}

	if (cnt >= 8)
	{
		//オブジェクトが視錐台内がない
		IsRefCenterCamera = false;
	}
	else
	{
		//オブジェクトが視錐台内にある
		IsRefCenterCamera = true;
	}
}

void CEnemy::StateMoveUpdate(float _maxrotate)
{
	if (IsAttacked)
	{
		StateIsTargetUpdate();
	}
	else
	{
		//移動先が設定されているか
		if (OldRndNum != NULL)
		{
			//移動先に到達したかどうか
			if (!CheckMovePos(MovePos[OldRndNum], CPlayerAttack::Get()->GetEnemyParameter().ThresHold))
			{
				//していなかった場合、その方向に向けて回転
				//ターゲットの方向取得
				SetDirPos(MovePos[OldRndNum]);

				float Initangle = 0.0f;

				//回転させる角度を計算
				float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
				Initangle = acosf(dotProduct);

				//角度を度数で計算
				float an = XMConvertToDegrees(Initangle);

				XMVECTOR axis;

				if (an > 2.0f)
				{
					//回転させる軸を計算
					axis = XMVector3Cross(newForward, TargetDir);
					if (XMVector3Equal(axis, XMVectorZero()))
					{
						axis.m128_f32[1] = 1.0f;
					}

					if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
					{
						//Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
						Initangle = XMConvertToRadians(_maxrotate);
					}

					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

					//回転行列作成
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//現在の回転行列にかけ合わせる
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}
				else
				{
					//もし機体のZ軸角度が傾いていたら、水平に戻す
					DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
					DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

					float angle = 0.0f;

					//回転させる角度を計算
					float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
					angle = acosf(dotProduct);

					//角度を度数で計算
					float an = XMConvertToDegrees(angle);

					if (an > 0.1f)
					{
						//回転させる軸を計算
						XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

						if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
						{
							angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
						}

						XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

						//回転行列作成
						XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

						//現在の回転行列にかけ合わせる
						RotateMatrix *= QMT;

						Rotation = ExtractRotationFromMatrix(RotateMatrix);
					}
				}
			}
			else
			{
				//移動先変更
				SetNewMovePosition();
			}
		}
		else
		{
			//移動先変更
			SetNewMovePosition();
		}
	}


}

void CEnemy::StateMoveForwardUpdate(float _maxrotate)
{
	////もし機体のZ軸角度が傾いていたら、水平に戻す
	//DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
	//DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

	//float angle = 0.0f;

	////回転させる角度を計算
	//float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
	//// 数値誤差防止のため、範囲をクリップ
	//dotProduct = max(-1.0f, min(1.0f, dotProduct));
	//angle = acosf(dotProduct);

	////角度を度数で計算
	//float an = XMConvertToDegrees(angle);

	//if (an > _maxrotate)
	//{
	//	//回転させる軸を計算
	//	XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

	//	// 軸がゼロベクトルの場合を処理（すでに完全に水平または逆方向の場合）
	//	if (XMVector3LengthSq(axisY).m128_f32[0] < 1e-6f) {
	//		axisY = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // デフォルトの回転軸（X軸）
	//		return;
	//	}

	//	if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
	//	{
	//		angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
	//	}

	//	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

	//	//回転行列作成
	//	XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

	//	//現在の回転行列にかけ合わせる
	//	RotateMatrix *= QMT;

	//	Rotation = ExtractRotationFromMatrix(RotateMatrix);
	//}

	//もし機体のZ軸角度が傾いていたら、水平に戻す
	DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
	DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

	float Initangle = 0.0f;

	//回転させる角度を計算
	//現在の正面と移動先のベクトル計算
	float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
	if (dotProduct > 1.0f)
		Initangle = 0.0f;
	else
		Initangle = acosf(dotProduct);

	//角度を度数で計算
	float an = XMConvertToDegrees(Initangle);

	XMVECTOR axis;

	//ターゲットまでの回転角度が最大回転角度を超えている場合
	if (an > _maxrotate)
	{
		//回転させる軸を計算
		axis = XMVector3Cross(newForward, TargetDir);
		if (XMVector3Equal(axis, XMVectorZero()))
		{
			axis.m128_f32[1] = 1.0f;
		}

		an = _maxrotate;

		an = XMConvertToRadians(an);
		XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

		//回転行列作成
		XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

		//現在の回転行列にかけ合わせる
		RotateMatrix *= QMT;

		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}
	else
	{
		//回転させる軸を計算
		axis = XMVector3Cross(newForward, TargetDir);
		if (XMVector3Equal(axis, XMVectorZero()))
		{
			axis.m128_f32[1] = 1.0f;
		}

		an = XMConvertToRadians(an);
		XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

		//回転行列作成
		XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

		//現在の回転行列にかけ合わせる
		RotateMatrix *= QMT;

		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}
}

void CEnemy::StateIsTargetUpdate()
{
	//カメラに映っている
	if (IsRefMainCamera || IsAttacked)
	{
		//もし機体のZ軸角度が傾いていたら、水平に戻す
		DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
		DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

		float angle = 0.0f;

		//回転させる角度を計算
		float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
		angle = acosf(dotProduct);

		//角度を度数で計算
		float an = XMConvertToDegrees(angle);

		if (an > 0.1f)
		{
			//回転させる軸を計算
			XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
			{
				angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
			}

			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

			//回転行列作成
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//現在の回転行列にかけ合わせる
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);
		}
	}
	else
	{
		StateMoveUpdate(CPlayerAttack::Get()->GetEnemyMaxRotation());
	}

}

bool CEnemy::StateAttackUpdate()
{
	//プレイヤーの座標取得
	XMFLOAT3 FollowPos = CGame::Get()->GetPlayer()->Position;

	//ターゲットの方向取得
	SetDirPos(FollowPos);

	float Initangle = 0.0f;

	//回転させる角度を計算
	float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
	if (dotProduct > 1.0f)
		Initangle = 0.0f;
	else
		Initangle = acosf(dotProduct);

	//角度を度数で計算
	float an = XMConvertToDegrees(Initangle);

	if (an <= 40.0f)
	{
		//最初にプレイヤーとの距離を判定
		if (PlayerDistance <= (float)CPlayerAttack::Get()->GetEnemyParameter().Max_Attack_Distance)
		{
			//ターゲット開始
			if (!IsAttackTarget)
			{
				IsAttackTarget = true;
				CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
				TargetStartTime = GetTickCount64();
			}

			//ターゲット時間を過ぎたら
			if (GetTickCount64() >= TargetStartTime + (ULONGLONG)CPlayerAttack::Get()->GetEnemyParameter().Target_Time && IsAttackTarget)
			{
				//ミサイルが空じゃない場合
				if (pMisile != nullptr)
				{
					if (pMisile->GetActive() == false)
					{

						//CEnemyAttack::Get()->AttackTarget(this);
						pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * -1.0f,
																									   Position.y + GetUp().y * -1.0f,
																									   Position.z + GetUp().z * -1.0f }, Rotation);
						ShotTime = GetTickCount64();
						TargetStartTime = 0;
						IsAttackTarget = false;
						EndChaseAndAttack = true;
						Times = GetTickCount64();
						return true;
					}
				}
				else
				{
					//pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * -1.0f,
						//																		  Position.y + GetUp().y * -1.0f,
						//																		  Position.z + GetUp().z * -1.0f }, Rotation);
					CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * -1.0f,
																		  Position.y + GetUp().y * -1.0f,
																		  Position.z + GetUp().z * -1.0f }, Rotation);
					ShotTime = GetTickCount64();
					TargetStartTime = 0;
					IsAttackTarget = false;
					CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
					EndChaseAndAttack = true;
					Times = GetTickCount64();
					return true;
				}
			}
		}
		else
		{
			TargetStartTime = 0;
			IsAttackTarget = false;
			CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
		}
	}
	else
	{
		TargetStartTime = 0;
		IsAttackTarget = false;
		CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
	}

	return false;
}

void CEnemy::StateRunUpdate()
{
}

void CEnemy::StateFollowEntourageUpdate(float _maxspeed)
{
	//平均位置に向かう
	//移動先に到達したかどうか
	if (!CheckMovePos(TargetPosition, 0.5f))
	{
		//ターゲットの方向取得
		SetDirPos(TargetPosition);

		float Initangle = 0.0f;

		//回転させる角度を計算
		float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
		if (dotProduct > 1.0f)
			Initangle = 0.0f;
		else
			Initangle = acosf(dotProduct);

		//角度を度数で計算
		float an = XMConvertToDegrees(Initangle);

		XMVECTOR axis;

		if (an >= 2.0f)
		{
			//回転させる軸を計算
			axis = XMVector3Cross(newForward, TargetDir);
			if (XMVector3Equal(axis, XMVectorZero()))
			{
				axis.m128_f32[1] = 1.0f;
			}

			////回転角度を設定
			//if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
			//{
			//	//Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
			//	Initangle = an;
			//}

			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
			{
				//angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
				an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;
				//angle = an;
			}

			an = XMConvertToRadians(an);
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

			//回転させる軸ベクトル設定
			//MVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

			//回転行列作成
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//現在の回転行列にかけ合わせる
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);

			////敵のスピード設定
			//SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed / 2.0f));
			//if (MoveSpeed <= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
			//	MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;

			//敵のスピード設定
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
			if (MoveSpeed >= _maxspeed)
			{
				MoveSpeed = _maxspeed;
			}

			// 2点間の距離を計算
			float distance = sqrt((TargetPosition.x - Position.x) * (TargetPosition.x - Position.x) +
				(TargetPosition.y - Position.y) * (TargetPosition.y - Position.y) +
				(TargetPosition.z - Position.z) * (TargetPosition.z - Position.z));

			if (distance >= 10)
			{
				//敵のスピード設定
				SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed * 2.0f);
				if (MoveSpeed >= _maxspeed + 0.3f)
				{
					MoveSpeed = _maxspeed + 0.3f;
				}
			}
		}
	}
	else
	{
		//もし機体のZ軸角度が傾いていたら、水平に戻す
		DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
		DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

		float angle = 0.0f;

		//回転させる角度を計算
		float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
		// 数値誤差防止のため、範囲をクリップ
		dotProduct = max(-1.0f, min(1.0f, dotProduct));
		angle = acosf(dotProduct);

		//角度を度数で計算
		float an = XMConvertToDegrees(angle);

		if (an > 5.0f)
		{
			//回転させる軸を計算
			XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

			// 軸がゼロベクトルの場合を処理（すでに完全に水平または逆方向の場合）
			if (XMVector3LengthSq(axisY).m128_f32[0] < 1e-6f) {
				axisY = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // デフォルトの回転軸（X軸）
			}

			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
			{
				//angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
				an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;
				//angle = an;
			}

			an = XMConvertToRadians(an);
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, an);
			//XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

			//回転行列作成
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//現在の回転行列にかけ合わせる
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);
		}

		//敵のスピード設定
		SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed));
		if (MoveSpeed < 0.1f)
			MoveSpeed = 0.1f;
		else if (MoveSpeed >= _maxspeed)
			MoveSpeed = _maxspeed;
		else if (MoveSpeed >= 0.2f)
			MoveSpeed = 0.2f;
	}
}

void CEnemy::StateDefenceEntourageUpdate()
{
	//ボスは数体の取り巻きを常に侍らせておく
	//プレイヤーが攻撃を行ったときに発火
	//発射したミサイルの数に応じて防御させる敵の数を決定
	//防衛の取り巻きを使用してミサイルを防御する
	//やり方考察
	//1:ミサイルがプレイヤーから発射される (判定取得可能)
	//2:発射されたミサイルの予想軌道を計算 (分からん)
	//3:その軌道に入るように取り巻きに移動指示を出す (移動先の座標がわかれば可能)
	//4:ミサイルが取り巻きにあたって防御 (当たる処理は可能)

	//プレイヤーがミサイルを撃ったか判定
	//ミサイルのターゲットになっているか
	if (IsAttacked)
	{
		//ボスをターゲットとしたミサイルをすべて取得
		//std::deque<CMisile> Misiles;
		//Misiles.push_back()

		//取得したミサイルの軌道予測を計算する

		//その結果を元に自分が移動するべき座標を取得する
		//自分の座標から軌道予想の位置までの直線を引き、1秒ほど移動した後に元の場所に戻る
	}

	
}

void CEnemy::StateAttackEntourageUpdate()
{
	//プレイヤーを攻撃した場合、終了
	if (StateAttackUpdate())
	{
		ShotTime = GetTickCount64();
		TargetStartTime = 0;
		IsAttackTarget = false;
		EndChaseAndAttack = true;
		CPlayerAttack::Get()->SetEntourageAddNum(-1);
		//EnemyState = ENTOURAGE_FOLLOW;
		EnemyState = ENTOURAGE_FRIGHT;
	}

	//移動先に到達したかどうか
	if (!CheckMovePos(TargetPosition, 0.5f))
	{
		////プレイヤーを攻撃した場合、終了
		//if (StateAttackUpdate())
		//{
		//	ShotTime = GetTickCount64();
		//	TargetStartTime = 0;
		//	IsAttackTarget = false;
		//	EndChaseAndAttack = true;
		//	EnemyState = ENTOURAGE_FOLLOW;
		//}

		//ターゲットの方向取得
		SetDirPos(TargetPosition);

		float Initangle = 0.0f;

		//回転させる角度を計算
		float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
		if (dotProduct > 1.0f)
			Initangle = 0.0f;
		else
			Initangle = acosf(dotProduct);

		//角度を度数で計算
		float an = XMConvertToDegrees(Initangle);

		XMVECTOR axis;

		if (an >= 10.0f)
		{
			//回転させる軸を計算
			axis = XMVector3Cross(newForward, TargetDir);
			if (XMVector3Equal(axis, XMVectorZero()))
			{
				axis.m128_f32[1] = 1.0f;
			}

			//回転角度を設定
			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.0f)
			{
				Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.0f);
			}

			//回転させる軸ベクトル設定
			XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

			//回転行列作成
			XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

			//現在の回転行列にかけ合わせる
			RotateMatrix *= QMT;

			Rotation = ExtractRotationFromMatrix(RotateMatrix);

			////敵のスピード設定
			//SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed / 2.0f));
			//if (MoveSpeed <= 0.1f)
			//	MoveSpeed = 0.1f;

			//敵のスピード設定
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed);
			if (CGame::Get()->GetPlayer()->GetMoveSpeed())
			{
				MoveSpeed = CGame::Get()->GetPlayer()->GetMoveSpeed();
			}
		}
		else
		{
			//もし機体のZ軸角度が傾いていたら、水平に戻す
			DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
			DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

			float angle = 0.0f;

			//回転させる角度を計算
			float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vTargetDir));
			// 数値誤差防止のため、範囲をクリップ
			dotProduct = max(-1.0f, min(1.0f, dotProduct));
			angle = acosf(dotProduct);

			//角度を度数で計算
			float an = XMConvertToDegrees(angle);

			if (an > 10.0f)
			{
				//回転させる軸を計算
				XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

				// 軸がゼロベクトルの場合を処理（すでに完全に水平または逆方向の場合）
				if (XMVector3LengthSq(axisY).m128_f32[0] < 1e-6f) {
					axisY = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // デフォルトの回転軸（X軸）
				}

				if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
				{
					angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f);
				}

				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

				//回転行列作成
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//現在の回転行列にかけ合わせる
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);
			}
		}
	}
}

CEnemy::CEnemy()
{
	ColliderObject = new CGameObject();

	//Init();
}

CEnemy::~CEnemy()
{
	isActive = false;
}

void CEnemy::Init()
{
	switch (Tag)
	{
	case CGameObject::TAG::FRIGHT_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().FrightEnemy_HP;
		ColliderObject->Position = Position;

		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxOBB();

		break;
	}
	case CGameObject::TAG::GROUND_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().GroundEnemy_HP;
		ShotTime = 0;

		ColliderObject->Position = Position;
		ColliderObject->Scaling = { 0.6f, 0.7f, 0.6f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxAABB(ColliderObject->Position, ColliderObject->Scaling);
		break;
	}
	case CGameObject::TAG::FOLLOW_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().FrightEnemy_HP;

		ColliderObject->Position = Position;
		//ColliderObject->Scaling = Scaling;
		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxOBB();

		EndChaseAndAttack = true;
		Times = GetTickCount64();

		break;
	}
	case TAG::ENTOURAGE_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().EntourageEnemy_HP;

		//ボスの座標
		DirectX::XMFLOAT3 BossPos = CPlayerAttack::Get()->GetBossEnemy()->Position;

		//ボスの後ろ
		//移動する場所をランダムな数字で取得
		GetRandomNum(-4.0f, 4.0f, EntouragePos.x);
		GetRandomNum(-4.0f, 4.0f, EntouragePos.y);

		// ランダムな整数を取得
		if (EntouragePos.x < 0)
			EntouragePos.x -= 2.0f;
		else
			EntouragePos.x += 2.0f;
		if (EntouragePos.y < 0)
			EntouragePos.y -= 2.0f;
		else
			EntouragePos.y += 2.0f;
		GetRandomNum(1.0f, 3.0f, EntouragePos.z);
		//EntouragePos.z = 1.0f;

		TargetPosition = { BossPos.x - (Forward.x * EntouragePos.z) + (RIGHT.x * EntouragePos.x) + (UP.x * EntouragePos.y)  ,
						   BossPos.y - (Forward.y * EntouragePos.z) + (RIGHT.y * EntouragePos.x) + (UP.y * EntouragePos.y) ,
						   BossPos.z - (Forward.z * EntouragePos.z) + (RIGHT.z * EntouragePos.x) + (UP.z * EntouragePos.y) };

		IsInitChengeBoidPos = true;
		Times = GetTickCount64();

		Position = TargetPosition;

		MoveSpeed = (CPlayerAttack::Get()->GetBossEnemy()->GetMoveSpeed());

		RotateMatrix = CPlayerAttack::Get()->GetBossEnemy()->GetRotateMatrix();

		EndChaseAndAttack = true;

		//防衛数が5体以下なら防衛に設定
		if (CPlayerAttack::Get()->GetEntourageDefNum() < 5)
		{
			EnemyState = ENTOURAGE_FOLLOW;
			CPlayerAttack::Get()->SetEntourageDefAddNum(1);
		}
		else
		{
			EnemyState = ENTOURAGE_FRIGHT;
		}

		//EnemyState = ENTOURAGE_FOLLOW;
		//EnemyState = ENTOURAGE_FRIGHT;
		//EnemyState = ENTOURAGE_ATTACK;

		ColliderObject->Position = Position;

		//ColliderObject->Position = { Position.x + -0.1f * GetForward().x,
		//							Position.y + -0.1f * GetForward().y,
		//							Position.z + -0.1f * GetForward().z };

		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetColliderBoxOBB();
		break;
	}
	case TAG::BOSS_ENEMY:
	{
		Hp = CPlayerAttack::Get()->GetEnemyParameter().BossEnemy_HP;

		ShotTime = GetTickCount64();

		ColliderObject->Position = Position;

		//ColliderObject->Position = { Position.x + -0.1f * GetForward().x,
		//							Position.y + -0.1f * GetForward().y,
		//							Position.z + -0.1f * GetForward().z };

		ColliderObject->Scaling = { 0.6f, 0.2f, 0.5f };

		ColliderObject->materialDiffuse.w = 0.7f;
		ColliderObject->SetLighting(false);

		SetMovePos({ 200, 0, 10 }, 0);		//右前
		SetMovePos({ -200, 0, 10 }, 1);		//左前
		SetMovePos({ -200, 0,-10 }, 2);		//左後ろ
		SetMovePos({ 200, 0,-10 }, 3);		//右後ろ

		SetColliderBoxOBB();
		break;
	}
	case TAG::GUN_ENEMY:
	{
		Hp = 10;

		SetColliderBoxOBB();
		break;
	}
	}

	if (Tag != BOSS_ENEMY)
	{
		SetMovePos({ 80,50,50 }, 0);		//右前
		SetMovePos({ -80,-10,50 }, 1);		//左前
		SetMovePos({ -80,50,-50 }, 2);		//左後ろ
		SetMovePos({ 80,-10,-50 }, 3);		//右後ろ
	}

	isActive = true;
	//ターゲットできるか
	CanAttack = true;

	//ターゲットかどうか
	IsTarget = false;

	//プレイヤーの攻撃に追われているか
	IsAttacked = false;

	SetCanAttack(false);
}

void CEnemy::Update()
{
	if (GetTag() == INITENEMY)
	{
		Position = { CGame::Get()->GetPlayer()->Position.x + 10.0f * CGame::Get()->GetPlayer()->GetForward().x,
					 CGame::Get()->GetPlayer()->Position.y + 10.0f * CGame::Get()->GetPlayer()->GetForward().y,
					 CGame::Get()->GetPlayer()->Position.z + 10.0f * CGame::Get()->GetPlayer()->GetForward().z
		};
		return;
	}

	if (!isActive)
		return;

	if (Hp <= 0)
	{
		isActive = false;
		return;
	}

	//カメラに映っているか判定
	CheckRefCamera();

	//中心カメラに通っているか判定
	CheckRefCameraCenter();

	//現在のベクトル計算
	CGameObject::SetVector();

	//プレイヤーとの距離を計算
	PlayerDistance = CheckDistance(CGame::Get()->GetPlayer(), {});

	switch (Tag)
	{
	case CGameObject::TAG::FRIGHT_ENEMY:	UpdateFrightEnemy(); break;
	case CGameObject::TAG::GROUND_ENEMY:	UpdateGroundEnemy(); break;
	case CGameObject::TAG::FOLLOW_ENEMY:	UpdateFollowEnemy(); break;
	case CGameObject::TAG::ENTOURAGE_ENEMY:	UpdateEntourageEnemy(); break;
	case CGameObject::TAG::GUN_ENEMY:		UpdateGunEnemy();	 break;
	case CGameObject::TAG::BOSS_ENEMY:		UpdateBossEnemy();	 break;
	default:
		break;
	}

	//ミニマップ更新処理
	MiniMapObjectUpdate();
}

void CEnemy::UpdateFrightEnemy()
{
	//プレイヤーとの距離を計算
	PlayerDistance = CheckDistance(CGame::Get()->GetPlayer(), {});

	//状態を設定
	//
	if (IsTarget && IsRefMainCamera && PlayerDistance <= 100)
	{
		EnemyState = MOVE_FORWARD;
	}
	else
	{
		EnemyState = MOVE;
	}

	if (IsAttacked)
	{
		EnemyState = MOVE_FORWARD;
	}

	switch (EnemyState)
	{
	case CEnemy::MOVE:
		StateMoveUpdate(CPlayerAttack::Get()->GetEnemyMaxRotation());
		break;
	case CEnemy::MOVE_FORWARD:
		StateMoveForwardUpdate(2.0f);
		break;
	default:
		break;
	}

	//敵のスピード設定
	SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Fright_AddSpeed);
	if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Fright_MaxSpeed)
	{
		MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Fright_MaxSpeed;
	}

	//移動処理
	CGameObject::MoveUpdate();

	//eJetEffect->Position = { Position.x + GetForward().x * -0.7f + GetUp().x * -0.35f + GetRight().x * 0.02f,
	//					 Position.y + GetForward().y * -0.7f + GetUp().y * -0.35f + GetRight().x * 0.02f,
	//					 Position.z + GetForward().z * -0.7f + GetUp().z * -0.35f + GetRight().x * 0.02f };
	//eJetEffect->Rotation = Rotation;
	//eJetEffect->SetInitRotateMatrix();

	//当たり判定設定
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//敵の当たり判定作成
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateGroundEnemy()
{
	//カメラに映っているか判定
	ChackRefCamera();

	//最初にプレイヤーとの距離を判定
	if (CheckDistance(CGame::Get()->GetPlayer(), {}) <= (float)CPlayerAttack::Get()->GetEnemyParameter().Max_Attack_Distance
		&& GetTickCount64() >= ShotTime + (ULONGLONG)CPlayerAttack::Get()->GetEnemyParameter().GroundEnemy_AttackTime)
	{
		//ターゲット開始
		if (!IsAttackTarget)
		{
			if (pMisile != nullptr)
			{
				if (pMisile->GetActive() == false)
				{
					IsAttackTarget = true;
					CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
					TargetStartTime = GetTickCount64();
				}
			}
			else
			{
				IsAttackTarget = true;
				CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
				TargetStartTime = GetTickCount64();
			}
		}
		else
		{
			CGame::Get()->GetPlayer()->SetIsEnemyRock(true);
		}

		//ターゲット時間を過ぎたら
		if (GetTickCount64() >= TargetStartTime + (ULONGLONG)CPlayerAttack::Get()->GetEnemyParameter().Target_Time && IsAttackTarget)
		{
			//ミサイル発射
			if (pMisile != nullptr)
			{
				//ミサイルが空じゃない場合
				if (pMisile->GetActive() == false)
				{
					//CEnemyAttack::Get()->AttackTarget(this);
					pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * 1.0f,
																								  Position.y + GetUp().y * 1.0f,
																								  Position.z + GetUp().z * 1.0f }, { -90.0f,0.0f,0.0f });
					ShotTime = GetTickCount64();
					TargetStartTime = 0;
					IsAttackTarget = false;
					CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
				}
			}
			else
			{
				//ミサイルが空の時
				pMisile = CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::MISILE, { Position.x + GetUp().x * 1.0f,
																							   Position.y + GetUp().y * 1.0f,
																							   Position.z + GetUp().z * 1.0f }, { -90.0f,0.0f,0.0f });
				ShotTime = GetTickCount64();
				TargetStartTime = 0;
				IsAttackTarget = false;
				CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
			}
		}
	}
	else
	{
		TargetStartTime = 0;
		IsAttackTarget = false;
		CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
	}

}

void CEnemy::UpdateFollowEnemy()
{

	//プレイヤーの少し後ろの座標取得
	XMFLOAT3 FollowPos = CGame::Get()->GetPlayer()->Position;
	//FollowPos = { FollowPos.x - (CGame::Get()->GetPlayer()->GetForward().x) * 4.0f,
	//			  FollowPos.y - (CGame::Get()->GetPlayer()->GetForward().y) * 4.0f,
	//			  FollowPos.z - (CGame::Get()->GetPlayer()->GetForward().z) * 4.0f, };

	//距離が一定以下ならMOVEFORWARDに移行
	if (PlayerDistance <= 5.0f)
	{
		ShotTime = GetTickCount64();
		TargetStartTime = 0;
		IsAttackTarget = false;
		CGame::Get()->GetPlayer()->SetIsEnemyRock(false);
		EndChaseAndAttack = true;
		Times = GetTickCount64();
	}

	if (!EndChaseAndAttack)
	{
		//移動先に到達したかどうか
		if (!CheckMovePos(FollowPos, 5.0f))
		{
			if (StateAttackUpdate())
			{
				EndChaseAndAttack = true;
				Times = GetTickCount64();
			}

			//ターゲットの方向取得
			SetDirPos(FollowPos);

			float Initangle = 0.0f;

			//回転させる角度を計算
			float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
			if (dotProduct > 1.0f)
				Initangle = 0.0f;
			else
				Initangle = acosf(dotProduct);

			//角度を度数で計算
			float an = XMConvertToDegrees(Initangle);

			XMVECTOR axis;

			if (an > 0.1f)
			{
				//回転させる軸を計算
				axis = XMVector3Cross(newForward, TargetDir);
				if (XMVector3Equal(axis, XMVectorZero()))
				{
					axis.m128_f32[1] = 1.0f;
				}

				//回転角度を設定
				if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
				{
					Initangle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
				}

				//回転させる軸ベクトル設定
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, Initangle);

				//回転行列作成
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//現在の回転行列にかけ合わせる
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);
			}
			else
			{
				//もし機体のZ軸角度が傾いていたら、水平に戻す
				DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
				DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

				float angle = 0.0f;

				//回転させる角度を計算
				float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
				angle = acosf(dotProduct);

				//角度を度数で計算
				float an = XMConvertToDegrees(angle);

				if (an > 0.1f)
				{
					//回転させる軸を計算
					XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);

					if (an > CPlayerAttack::Get()->GetEnemyMaxRotation())
					{
						angle = XMConvertToRadians(CPlayerAttack::Get()->GetEnemyMaxRotation());
					}

					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

					//回転行列作成
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//現在の回転行列にかけ合わせる
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}

				if (StateAttackUpdate())
				{
					EndChaseAndAttack = true;
					Times = GetTickCount64();
				}
			}

			//敵のスピード設定
			SetAddMoveSpeed((-(float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed / 2.0f));
			if (MoveSpeed <= 0.1f)
				MoveSpeed = 0.1f;
		}
	}
	else
	{
		if (GetTickCount64() < Times + CPlayerAttack::Get()->GetEnemyParameter().Attack_CoolTime)
			StateMoveForwardUpdate(2.0f);
		else
			EndChaseAndAttack = false;

		//if (GetTickCount64() >= Times + FOLLOW_CCOLTIME)
		//	AAA = false;
	}

	if (EndChaseAndAttack)
	{
		//敵のスピード設定
		SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
		if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
		{
			MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;
		}
	}
	else
	{
		if (PlayerDistance >= 200)
		{
			//敵のスピード設定
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
			if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
			{
				MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;
			}
		}
		else
		{
			//敵のスピード設定
			SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed);
			if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed)
			{
				MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed;
			}
		}
	}

	//移動処理
	CGameObject::MoveUpdate();

	//当たり判定設定
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//敵の当たり判定作成
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateEntourageEnemy()
{
	//ボスが倒された場合、FOLLOW_ENEMYモードに変更
	if (CPlayerAttack::Get()->GetBossEnemy() == nullptr || !CPlayerAttack::Get()->GetBossEnemy()->GetActive())
	{
		Tag = FOLLOW_ENEMY;
		return;

		if (!EndChaseAndAttack)
		{
			EndChaseAndAttack = true;
			CPlayerAttack::Get()->SetEntourageAddNum(-1);
		}
	}

	//一定時間後に、位置を変更する
	if (!IsInitChengeBoidPos || (((GetTickCount64() >= Times + CPlayerAttack::Get()->GetEnemyParameter().Entourage_ChengePos_Time) && EndChaseAndAttack)))
	{
		switch (EnemyState)
		{
		case CEnemy::ENTOURAGE_FOLLOW:
		{
			break;
		}
		case CEnemy::ENTOURAGE_FRIGHT:

			break;
		case CEnemy::ENTOURAGE_DEFENCE:
		{
			break;
		}
		case CEnemy::ENTOURAGE_ATTACK:
			//移動する場所をランダムな数字で取得(3～-3)
			GetRandomNum(-2.0f, 2.0f, EntouragePos.x);
			GetRandomNum(-2.0f, 2.0f, EntouragePos.y);
			GetRandomNum(10.0f, 20.0f, EntouragePos.z);
			break;
		default:
			break;
		}

		IsInitChengeBoidPos = true;
		Times = GetTickCount64();
	}

	//プレイヤー攻撃状態のみ、常にプレイヤーの位置を取得する
	if (EnemyState == ENTOURAGE_ATTACK)
	{
		FollowPos = CGame::Get()->GetPlayer()->Position;

		//敵のスピード設定
		SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed);
		if (MoveSpeed >= CGame::Get()->GetPlayer()->GetMoveSpeed() * 0.8f)
		{
			MoveSpeed = CGame::Get()->GetPlayer()->GetMoveSpeed() * 0.8f;
		}
	}
	else if (EnemyState == ENTOURAGE_FRIGHT)
	{
		//ボスを中心とした移動ポイント取得
		for (int i = 0; i < 8; i++)
		{
			MovePos[i] = CPlayerAttack::Get()->GetBossEnemy()->GetEntourageMovePos(i);
		}

		//敵のスピード設定
		SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed);
		if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed)
		{
			MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed;
		}
	}
	else
	{
		FollowPos = CPlayerAttack::Get()->GetBossEnemy()->Position;
	}

	//移動ポイント設定
	TargetPosition = { FollowPos.x - (Forward.x * EntouragePos.z) + (RIGHT.x * EntouragePos.x) + (UP.x * EntouragePos.y),
					   FollowPos.y - (Forward.y * EntouragePos.z) + (RIGHT.y * EntouragePos.x) + (UP.y * EntouragePos.y),
					   FollowPos.z - (Forward.z * EntouragePos.z) + (RIGHT.z * EntouragePos.x) + (UP.z * EntouragePos.y) };

	//更新処理
	switch (EnemyState)
	{
	case CEnemy::ENTOURAGE_DEFENCE: StateFollowEntourageUpdate((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed); break;
	case CEnemy::ENTOURAGE_FRIGHT: StateMoveUpdate(CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxRotate); break;
	case CEnemy::ENTOURAGE_FOLLOW: StateFollowEntourageUpdate((float)CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed); break;
	case CEnemy::ENTOURAGE_ATTACK: StateAttackEntourageUpdate(); break;
	default:
		break;
	}

	//移動処理
	CGameObject::MoveUpdate();

	//当たり判定設定
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//敵の当たり判定作成
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateBossEnemy()
{
	//ボスの周囲の座標に移動ポイントを設定
	float PointNum = CPlayerAttack::Get()->GetEnemyParameter().Entourage_Move_Pos_Dis;
	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * PointNum) + (Forward.z * PointNum) }, 0);		//右上前

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * PointNum) + (Forward.z * PointNum) }, 1);	//左上前

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * PointNum) + (Forward.z * -PointNum) }, 2);	//左上後ろ

	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * PointNum) + (Forward.z * -PointNum) }, 3);	//右上後ろ

	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * -PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * -PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * -PointNum) + (Forward.z * PointNum) }, 4);	//右下前

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * -PointNum) + (Forward.x * PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * -PointNum) + (Forward.y * PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * -PointNum) + (Forward.z * PointNum) }, 5);	//左下前

	SetEntourageMovePos({ Position.x + (RIGHT.x * -PointNum) + (UP.x * -PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * -PointNum) + (UP.y * -PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * -PointNum) + (UP.z * -PointNum) + (Forward.z * -PointNum) }, 6);	//左下後ろ

	SetEntourageMovePos({ Position.x + (RIGHT.x * PointNum) + (UP.x * -PointNum) + (Forward.x * -PointNum),
						  Position.y + (RIGHT.y * PointNum) + (UP.y * -PointNum) + (Forward.y * -PointNum),
						  Position.z + (RIGHT.z * PointNum) + (UP.z * -PointNum) + (Forward.z * -PointNum) }, 7);	//右下後ろ

	if (InsertEntourage)
	{
		StateMoveUpdate(CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxRotate);
	}
	else
	{
		if (GetTickCount64() >= Times)
		{
			//取り巻きを召喚
			for (int i = 0; i < CPlayerAttack::Get()->GetEnemyParameter().InsertEntourageNum; i++)
			{
				// ランダムな整数を取得
				float randomValueX/* = dist(rng)*/;
				GetRandomNum(-6.0f,6.0f, randomValueX);
				float randomValueY/* = dist(rng)*/;
				GetRandomNum(-6.0f, 6.0f, randomValueY);
				float randomValueZ/* = dist2(rng)*/;
				GetRandomNum(2.0f, 4.0f, randomValueZ);

				DirectX::XMFLOAT3 InsertPos = { Position.x - (Forward.x * randomValueZ) + (RIGHT.x * randomValueX) + (UP.x * randomValueY)  ,
												Position.y - (Forward.y * randomValueZ) + (RIGHT.y * randomValueX) + (UP.y * randomValueY) ,
												Position.z - (Forward.z * randomValueZ) + (RIGHT.z * randomValueX) + (UP.z * randomValueY)  };

				//取り巻きを召喚
				CGame::Get()->InsertEnemy(CGameObject::TAG::ENTOURAGE_ENEMY, InsertPos, Rotation, { 1.5f,1.5f,1.5f });
			}

			//機関銃敵を召喚
			for (int j = 0; j < 5; j++)
			{
				CGame::Get()->InsertEnemy(CGameObject::TAG::GUN_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 0.5f, 0.5f, 0.5f });
			}

			InsertEntourage = true;
			//Times = 0;
			CPlayerAttack::Get()->SetTrueBreak();
		}
	}

	//敵のスピード設定
	SetAddMoveSpeed((float)CPlayerAttack::Get()->GetEnemyParameter().Boss_AddSpeed);
	if (MoveSpeed >= CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxSpeed)
	{
		MoveSpeed = CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxSpeed;
	}

	//取り巻きに攻撃支持を行う
	//全ての敵の攻撃支持が終了しているかつ
	//攻撃支持のクールタイムを経過したら
	if (CPlayerAttack::Get()->GetEntourageAttackNum() == 0 && DebugIsAttack)
	{
		//攻撃支持が出されていないとき
		if (!IsAttackEntourage)
		{
			//攻撃のためのクールタイム開始
			ShotTime = GetTickCount64();
			IsAttackEntourage = true;
		}

		if (GetTickCount64() >= ShotTime + CPlayerAttack::Get()->GetEnemyParameter().Entourage_Attack_CoolTime)
		{
			CPlayerAttack::Get()->SetAttackEntourageEnemy();
			//ShotTime = GetTickCount64();
			IsAttackEntourage = false;
		}
	}

	//取り巻きの防衛指示設定
	if (PlayerDistance < 30 && IsTarget)
	{
		//プレイヤーとの距離が近いかつターゲットされている
		CPlayerAttack::Get()->SetDefenceEntourageEnemy(true);
	}
	else
	{
		//プレイヤーとの距離が遠いまたはターゲットされていない
		CPlayerAttack::Get()->SetDefenceEntourageEnemy(false);
	}

	//プレイヤーとの距離が近いときかつターゲットされているとき
	//取り巻きの一部をプレイヤーの後ろに配置する

	OldPosition = Position;

	//移動処理
	CGameObject::MoveUpdate();

	//移動ベクトル計算
	// deltaTime: フレーム間の経過時間
	float deltaTime = 0.016f; // 例: 60FPS

	// 現在位置と前フレームの位置を取得
	XMVECTOR currentTargetPosition = XMLoadFloat3(&Position);
	XMVECTOR previousTargetPosition = XMLoadFloat3(&OldPosition);

	// 移動ベクトルを計算
	XMVECTOR movementVector = XMVectorSubtract(currentTargetPosition, previousTargetPosition);

	// 速度ベクトルを計算 (移動距離 ÷ 時間)
	XMVECTOR velocity = XMVectorScale(movementVector, 1.0f / deltaTime);

	// 速度ベクトルを保存しておく
	DirectX::XMStoreFloat3(&MoveVelocity, velocity);

	//当たり判定設定
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//敵の当たり判定作成
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::UpdateGunEnemy()
{
	//移動方向ベクトル
	XMFLOAT3 MoveVector = { 0.0f,0.0f,0.0f };

	//中身が空かどうか
	MoveSpeed = 0.5f;

	//通過座標を通過していなかったら
	if (!NowFlag.IsPassPos)
	{
		//移動先のベクトルを計算
		SetDirPos(NowFlag.MovePos);

		//XMFLOAT3に変換
		MoveVector.x = XMVectorGetX(TargetDir);
		MoveVector.y = XMVectorGetY(TargetDir);
		MoveVector.z = XMVectorGetZ(TargetDir);

		//移動先に到着したか
		if (CheckMovePos(NowFlag.MovePos, 1.0f))
		{
			MoveSpeed = 0.0f;
			NowFlag.EndForwardRotate = true;

			if (CPlayerAttack::Get()->GetGunEnemysPass())
			{
				NowFlag.IsPassPos = true;
				NowFlag.PassTime = GetTickCount64();
			}
		}

		//設定した座標の方向を向いていない場合
		if (!NowFlag.EndRotate)
		{
			//ターゲットの方向取得
			SetDirPos(NowFlag.MovePos);

			float Initangle = 0.0f;

			//回転させる角度を計算
			//現在の正面と移動先のベクトル計算
			float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
			if (dotProduct > 1.0f)
				Initangle = 0.0f;
			else
				Initangle = acosf(dotProduct);

			//角度を度数で計算
			float an = XMConvertToDegrees(Initangle);

			XMVECTOR axis;

			//ターゲットまでの回転角度が最大回転角度を超えている場合
			if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
			{
				//回転させる軸を計算
				axis = XMVector3Cross(newForward, TargetDir);
				if (XMVector3Equal(axis, XMVectorZero()))
				{
					axis.m128_f32[1] = 1.0f;
				}

				an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;

				an = XMConvertToRadians(an);
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

				//回転行列作成
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//現在の回転行列にかけ合わせる
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);
			}
			else
			{
				//回転させる軸を計算
				axis = XMVector3Cross(newForward, TargetDir);
				if (XMVector3Equal(axis, XMVectorZero()))
				{
					axis.m128_f32[1] = 1.0f;
				}

				an = XMConvertToRadians(an);
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

				//回転行列作成
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//現在の回転行列にかけ合わせる
				RotateMatrix *= QMT;

				Rotation = ExtractRotationFromMatrix(RotateMatrix);

				NowFlag.EndRotate = true;
			}
		}
	}
	else
	{
		//配列の中身が空の場合、移動終了
		if (qMoveArray.empty())
		{
			MoveSpeed = 0.0f;
		}
		else
		{
			//停止時間を超えたら
			if (GetTickCount64() >= NowFlag.PassTime + NowFlag.WaitTime)
			{
				//移動先配列の先頭要素を取得
				NowFlag = qMoveArray.front();
				qMoveArray.push_back(qMoveArray.front());
				qMoveArray.pop_front();
				NowFlag.IsPassPos = false;
				XMFLOAT3 ResFloat;
				ResFloat = CPlayerAttack::Get()->GetBossEnemy()->Position;
				ResFloat = { ResFloat.x + (CPlayerAttack::Get()->GetBossEnemy()->GetForward().x * NowFlag.MovePos.z) + (CPlayerAttack::Get()->GetBossEnemy()->GetRight().x * NowFlag.MovePos.x) + (CPlayerAttack::Get()->GetBossEnemy()->GetUp().x * NowFlag.MovePos.y),
						ResFloat.y + (CPlayerAttack::Get()->GetBossEnemy()->GetForward().y * NowFlag.MovePos.z) + (CPlayerAttack::Get()->GetBossEnemy()->GetRight().y * NowFlag.MovePos.x) + (CPlayerAttack::Get()->GetBossEnemy()->GetUp().y * NowFlag.MovePos.y),
						ResFloat.z + (CPlayerAttack::Get()->GetBossEnemy()->GetForward().z * NowFlag.MovePos.z) + (CPlayerAttack::Get()->GetBossEnemy()->GetRight().z * NowFlag.MovePos.x) + (CPlayerAttack::Get()->GetBossEnemy()->GetUp().z * NowFlag.MovePos.y) };
				NowFlag.MovePos = ResFloat;
			}
			else
			{
				//ターゲットの方向取得
				SetDirPos(CGame::Get()->GetPlayer()->Position);

				float Initangle = 0.0f;

				//回転させる角度を計算
				//現在の正面と移動先のベクトル計算
				float dotProduct = XMVectorGetX(XMVector3Dot(newForward, TargetDir));
				if (dotProduct > 1.0f)
					Initangle = 0.0f;
				else
					Initangle = acosf(dotProduct);

				//角度を度数で計算
				float an = XMConvertToDegrees(Initangle);

				XMVECTOR axis;

				//ターゲットまでの回転角度が最大回転角度を超えている場合
				if (an > CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f)
				{
					//回転させる軸を計算
					axis = XMVector3Cross(newForward, TargetDir);
					if (XMVector3Equal(axis, XMVectorZero()))
					{
						axis.m128_f32[1] = 1.0f;
					}

					an = CPlayerAttack::Get()->GetEnemyMaxRotation() + 1.5f;

					an = XMConvertToRadians(an);
					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

					//回転行列作成
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//現在の回転行列にかけ合わせる
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}
				else
				{
					//回転させる軸を計算
					axis = XMVector3Cross(newForward, TargetDir);
					if (XMVector3Equal(axis, XMVectorZero()))
					{
						axis.m128_f32[1] = 1.0f;
					}

					an = XMConvertToRadians(an);
					XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, an);

					//回転行列作成
					XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

					//現在の回転行列にかけ合わせる
					RotateMatrix *= QMT;

					Rotation = ExtractRotationFromMatrix(RotateMatrix);
				}

				//弾を発射
				if (an <= 30 && CPlayerAttack::Get()->GetBossEnemy()->GetDebugIsAttack())
				{
					if (GetTickCount64() >= GunShotTime + 1000)
					{
						//弾を撃つ処理
						CEnemyAttack::Get()->AttackTarget(CEnemyAttack::ATTACK_ID::GUN, Position, Rotation);

						GunShotTime = GetTickCount64();
					}
				}
			}
		}
	}

	//移動処理
	CGameObject::MoveVectorUpdate(MoveVector);

	//当たり判定設定
	ColliderObject->Position = Position;

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	//敵の当たり判定作成
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

void CEnemy::Draw()
{
	if (Tag == INITENEMY)
	{
		return;
	}

	if (Hp <= 0)
	{
		isActive = false;
		return;
	}

	if (IsRefMainCamera)
		CGameObject::Draw();


}

void CEnemy::SetDirPos(XMFLOAT3 _targetpos)
{
	// オブジェクト間の位置ベクトルを計算
	//方向はターゲット-プレイヤーの位置で求められる
	XMFLOAT3 directionVector;
	directionVector.x = _targetpos.x - Position.x;
	directionVector.y = _targetpos.y - Position.y;
	directionVector.z = _targetpos.z - Position.z;

	// 位置ベクトルを正規化
	TargetDir = XMVector3Normalize(XMLoadFloat3(&directionVector));
}

bool CEnemy::CheckMovePos(DirectX::XMFLOAT3 _targetpos, float _allowdis)
{
	// 2点間の距離を計算
	float distance = sqrt((_targetpos.x - Position.x) * (_targetpos.x - Position.x) +
		(_targetpos.y - Position.y) * (_targetpos.y - Position.y) +
		(_targetpos.z - Position.z) * (_targetpos.z - Position.z));

	return distance <= _allowdis;

	//return distance <= THRESHOLD;
}

void CEnemy::SetTargetMiniMapObjectModel(bool _istarget)
{
	if (Tag == INITENEMY)
		return;

	//ターゲットされたとき
	if (_istarget)
	{
		NottargetMiniMapModel = pMinimapObject->GetModel();
		pMinimapObject->SetModel(TargetMiniMapModel);
	}
	else
	{
		//ターゲットから外れた
		pMinimapObject->SetModel(NottargetMiniMapModel);
	}
}

void CEnemy::SetNewMovePosition()
{	
	int randomValue = 0;

	while (true)
	{
		// 乱数の範囲を指定
		if (Tag != ENTOURAGE_ENEMY)
		{
			// 乱数生成器の初期化
			std::random_device rd;  // ハードウェアの乱数生成器を使用する
			std::mt19937 rng(rd()); // メルセンヌ・ツイスタ法に基づく擬似乱数生成器

			std::uniform_int_distribution<int> dist(0, 3); // 0から3の範囲の整数を生成

			// ランダムな整数を取得
			randomValue = dist(rng);

			if (OldRndNum != randomValue)
			{
				OldRndNum = randomValue;
				break;
			}
		}
		else
		{
			// 乱数生成器の初期化
			std::random_device rd;  // ハードウェアの乱数生成器を使用する
			std::mt19937 rng(rd()); // メルセンヌ・ツイスタ法に基づく擬似乱数生成器

			std::uniform_int_distribution<int> dist(0, 7); // 0から3の範囲の整数を生成

			// ランダムな整数を取得
			randomValue = dist(rng);

			if (OldRndNum != randomValue)
			{
				OldRndNum = randomValue;
				break;
			}
		}


	}

}

void CEnemy::SetEnemyState(ENEMY_STATE _newstate)
{
	if (EnemyState != _newstate)
	{
		SetNewEntouragePos();
	}

	EnemyState = _newstate;
}

float CEnemy::GetToPlayerRadius()
{
	//メインカメラからこの敵への方向ベクトルを求める
	XMFLOAT3 directionVector;
	XMFLOAT3 CameraPos = CGame::Get()->GetCamera3D()->GetCameraPos();
	directionVector.x = Position.x - CameraPos.x;
	directionVector.y = Position.y - CameraPos.y;
	directionVector.z = Position.z - CameraPos.z;

	// 位置ベクトルを正規化
	DirectX::XMVECTOR vDirection = XMVector3Normalize(XMLoadFloat3(&directionVector));

	//メインカメラから注視座標へのベクトルを求める
	XMFLOAT3 directionVector2;
	XMFLOAT3 CameraFocusPos = CGame::Get()->GetCamera3D()->GetCameraFocus();
	directionVector2.x = CameraFocusPos.x - CameraPos.x;
	directionVector2.y = CameraFocusPos.y - CameraPos.y;
	directionVector2.z = CameraFocusPos.z - CameraPos.z;

	// 位置ベクトルを正規化
	DirectX::XMVECTOR vDirection2 = XMVector3Normalize(XMLoadFloat3(&directionVector2));


	//角度を度数で取得
	float dotProduct = XMVectorGetX(XMVector3Dot(vDirection2, vDirection));
	dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);
	ToPlayerRadius = XMConvertToDegrees(acosf(dotProduct));

	return ToPlayerRadius;
}

void CEnemy::ChengeFollowPos(DirectX::XMFLOAT3& _addpos)
{
	GetRandomNum(0.0f, _addpos.x, EntouragePos.x);
	GetRandomNum(0.0f, _addpos.y, EntouragePos.y);
	GetRandomNum(0.0f, _addpos.z, EntouragePos.z);

	if (EntouragePos.x < _addpos.x / 2.0f)
	{
		EntouragePos.x -= _addpos.x;
	}
	if (EntouragePos.y < _addpos.y / 2.0f)
	{
		EntouragePos.y -= _addpos.y;
	}
}

void CEnemy::SetGunEnemyNewPos(CPlayerAttack::GUNENEMYSTARTPOS _startpos)
{
	//値だけを配列に設定し、それを取得して計算した結果を配列に入れなおす

	//機関銃の敵は、ボスを中心に移動する
	if (GunENemyStartPos == CPlayerAttack::GUNENEMYSTARTPOS::NoneVerTex)
		GunENemyStartPos = _startpos;

	GUNENEMY_MOVEFLAG ResPos;
	XMFLOAT3 MovePos;

	bool res = true;
	//機関銃の敵の移動先を設定(ボスを中心)
	switch (GunENemyStartPos)
	{
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex1:
		ResPos.MovePos = { 10,0,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex2:
		ResPos.MovePos = { 0,10,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex3:
		ResPos.MovePos = { -10,0,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex4:
		ResPos.MovePos = { -5,-10,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::VerTex5:
		ResPos.MovePos = { 5,-10,20 };
		qMoveArray.push_back(ResPos);
		break;
	case CPlayerAttack::GUNENEMYSTARTPOS::MAXVerTex:
		break;
	default:
		break;
	}

	GunENemyStartPos = static_cast<CPlayerAttack::GUNENEMYSTARTPOS>(static_cast<int>(GunENemyStartPos) + 1);

	while (GunENemyStartPos != _startpos)
	{
		//機関銃の敵の移動先を設定(ボスを中心)
		switch (GunENemyStartPos)
		{
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex1:
			ResPos.MovePos = { 10,0,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex2:
			ResPos.MovePos = { 0,10,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex3:
			ResPos.MovePos = { -10,0,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex4:
			ResPos.MovePos = { -5,-10,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::VerTex5:
			ResPos.MovePos = { 5,-10,20 };
			qMoveArray.push_back(ResPos);
			break;
		case CPlayerAttack::GUNENEMYSTARTPOS::MAXVerTex:
			break;
		default:
			break;
		}

		//一個ずらす
		GunENemyStartPos = static_cast<CPlayerAttack::GUNENEMYSTARTPOS>(static_cast<int>(GunENemyStartPos) + 1);
		if (GunENemyStartPos == MAX)
		{
			//MAXの場合、最初に戻す
			GunENemyStartPos = CPlayerAttack::GUNENEMYSTARTPOS::VerTex1;
		}
	}

	//移動先配列の先頭要素を取得
	NowFlag = qMoveArray.front();

	return;
}
