#include "CGameObject.h"
#include "direct3d.h"
#include"CGame.h"
#include "CPlayer.h"
#include "CPlayerAttack.h"
#include "CEnemy.h"
#include "CCamera.h"
#include"Input.h"
#include "CControllerInput.h"
#include "CEffect.h"
#include "CScene.h"

#include <DirectXMath.h>
#include <iostream>
#include "DebugUI.h"

using namespace DirectX;

//ミサイルの装填時間
#define MISILE_TIME (6000)

std::unordered_map<int, std::list<DirectX::BoundingBox>> CGameObject::mColliderBoxsAABB;

DirectX::XMMATRIX CGameObject::GetUvMatrix()
{
	return XMMatrixIdentity();
}

CGameObject::CGameObject()
{
	RotateMatrix = GetIsMatrix();
	Hp = 10;
}

CGameObject::~CGameObject()
{
	isActive = false;

	if (pMinimapObject != nullptr)
	{
		delete pMinimapObject;
		pMinimapObject = nullptr;
	}

	if (ColliderObject != nullptr)
	{
		delete ColliderObject;
		ColliderObject = nullptr;
	}

	//if (!mColliderBoxsAABB.empty())
	//{
	//	mColliderBoxsAABB.clear();
	//}
}

void CGameObject::Update()
{
	if (Tag == UI || Tag == HITMISILE_UI)
		return;
	else if (Tag == BACK_SKY_CIRCLE)
	{
		//Position = CScene::Get()->GetPlayer()->Position;
		return;
	}
	else if (Tag == MISILE_UI)
	{
		if (materialDiffuse.w < 1.0f)
		{
			//時間を計り、指定時間たったらアクティブになる(これにより、ミサイル攻撃が可能になる)
			if (GetTickCount64() >= MisileShotTime + MISILE_TIME)
			{
				materialDiffuse.w = 1.0f;
			}
		}
		else
		{
			return;
		}
	}
	else if (Tag == CENTER_UI)
	{
		//Position = CGame::Get()->GetPlayer()->Position;
		//Position = { CGame::Get()->GetPlayer()->Position.x + CGame::Get()->GetPlayer()->GetForward().x * 10.0f,
		//			 CGame::Get()->GetPlayer()->Position.y + CGame::Get()->GetPlayer()->GetForward().y * 10.0f,
		//			 CGame::Get()->GetPlayer()->Position.z + CGame::Get()->GetPlayer()->GetForward().z * 10.0f, };
		RotateMatrix = CGame::Get()->GetPlayer()->GetRotateMatrix();
	}
	else if (Tag == PRESS_UI || Tag == CARSOL)
	{
		if (IsFadeOut)
		{
			materialDiffuse.w -= 0.02f;

			if (materialDiffuse.w <= 0.0f)
			{
				materialDiffuse.w = 0.0f;
				IsFadeOut = false;
			}
		}
		else
		{
			materialDiffuse.w += 0.02f;

			if (materialDiffuse.w >= 1.0f)
			{
				materialDiffuse.w = 1.0f;
				IsFadeOut = true;
			}
		}

		return;
	}
	else if (Tag == ATTACKINF_UI)
	{
		switch (CPlayerAttack::Get()->GetAttackId())
		{
		case CPlayerAttack::ATTACK_ID::MISILE:
			SetModel(CGame::Get()->GetModel(L"asset/UI/Stage_MSL.png"));
			Position = { 1.8f,-0.72f,-0.1f };
			Scaling = { 0.4f, 0.18f, 0.1f };
			break;
		case CPlayerAttack::ATTACK_ID::ONETARGET_FOURMISILE:
			SetModel(CGame::Get()->GetModel(L"asset/UI/Stage_4M1A.png"));
			Position = { 1.8f,-0.7f,-0.1f };
			Scaling = { 0.4f, 0.2f, 0.1f };
			break;
		case CPlayerAttack::ATTACK_ID::FOURTARGET_FOURMISILE:
			SetModel(CGame::Get()->GetModel(L"asset/UI/Stage_4M4A.png"));
			Position = { 1.8f,-0.7f,-0.1f };
			Scaling = { 0.45f, 0.2f, 0.1f };
			break;
		}
	}

	//この中では移動処理と正面および上方向の取得処理のみ行う
	SetVector();

	//移動処理
	MoveUpdate();


	if (IsUpdateMiniMap)
		MiniMapObjectUpdate();
}

void CGameObject::MoveUpdate()
{
	// 前進する処理
	// XMVECTOR→XMFLOAT3変換
	Forward.x = XMVectorGetX(newForward);
	Forward.y = XMVectorGetY(newForward);
	Forward.z = XMVectorGetZ(newForward);

	// XMVECTOR→XMFLOAT3変換
	UP.x = XMVectorGetX(newUP);
	UP.y = XMVectorGetY(newUP);
	UP.z = XMVectorGetZ(newUP);

	// XMVECTOR→XMFLOAT3変換
	RIGHT.x = XMVectorGetX(newRIGHT);
	RIGHT.y = XMVectorGetY(newRIGHT);
	RIGHT.z = XMVectorGetZ(newRIGHT);

	XMFLOAT3 forward = GetForward();
	Position.x += Forward.x * MoveSpeed;
	Position.y += Forward.y * MoveSpeed;
	Position.z += Forward.z * MoveSpeed;
}

void CGameObject::MoveVectorUpdate(DirectX::XMFLOAT3 _movevector)
{
	//// XMVECTOR→XMFLOAT3変換
	////受け取る引数がVECTOR型の場合
	//XMFLOAT3 MoveVector;
	//MoveVector.x = XMVectorGetX(_movevector);
	//MoveVector.y = XMVectorGetY(_movevector);
	//MoveVector.z = XMVectorGetZ(_movevector);

	Position.x += _movevector.x * MoveSpeed;
	Position.y += _movevector.y * MoveSpeed;
	Position.z += _movevector.z * MoveSpeed;
}

void CGameObject::MiniMapObjectUpdate()
{
	if (pMinimapObject == nullptr)
		return;

	if (Tag == GROUND)
	{
		pMinimapObject->Position = CGame::Get()->GetPlayer()->Position;
		pMinimapObject->Position.y -= 1.0f;
	}
	else if (Tag == MINIMAP_UI)
	{
		pMinimapObject->Position = CGame::Get()->GetMiniMapCameraPos();
		pMinimapObject->Position.y = pMinimapObject->Position.y - 2.0f;
	}
	else
	{
		pMinimapObject->Position = Position;
		pMinimapObject->Position.y = CGame::Get()->GetPlayer()->Position.y;
	}

	if (Tag != MINIMAP_UI)
	{
		//もし機体のZ軸角度が傾いていたら、水平に戻す
		DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
		DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));
		DirectX::XMMATRIX ResRotetMatrix = RotateMatrix;
		DirectX::XMFLOAT3 ResRotate = pMinimapObject->Rotation;

		float angle = 0.0f;

		//回転させる角度を計算
		float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
		//float dotProduct = XMVectorGetX(XMVector3Dot(pMinimapObject->GetNewUp(), vTargetDir));
		angle = acosf(dotProduct);

		//角度を度数で計算
		float an = XMConvertToDegrees(angle);

		if (an > 1.0f)
		{
			//回転させる軸を計算
			XMVECTOR axisY = XMVector3Cross(newUP, vTargetDir);
			//XMVECTOR axisY = XMVector3Cross(newUP, horizontalTargetDir);
			if (!XMVector3Equal(axisY, XMVectorZero()))
			{
				XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axisY, angle);

				//回転行列作成
				XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

				//現在の回転行列にかけ合わせる
				ResRotetMatrix *= QMT;

				ResRotate = ExtractRotationFromMatrix(ResRotetMatrix);
			}
		}

		pMinimapObject->RotateMatrix = ResRotetMatrix;
		pMinimapObject->SetVector();
	}
	else
	{
		pMinimapObject->Rotation.x = 90.0f;
		pMinimapObject->SetInitRotateMatrix();
	}


	//pMinimapObject->Rotation = ResRotate;
	//pMinimapObject->Rotation = Rotation;

	//pMinimapObject->SetInitRotateMatrix();


	switch (CGame::Get()->GetMiniMapNum())
	{
	case 0:
		if (Tag == GROUND)
			pMinimapObject->Scaling = { 100.0f, 0.1f, 100.0f };
		else if (Tag == MISILE)
			pMinimapObject->Scaling = { 1.3f, 1.7f, 1.7f };
		else if (Tag == GROUND_ENEMY)
			pMinimapObject->Scaling = { 2.3f, 1.3f, 1.3f };
		else if(Tag == OBJECT)
			pMinimapObject->Scaling = { 10.0f, 10.0f, 10.0f };
		else if (Tag == MINIMAP_UI)
			pMinimapObject->Scaling = { 1.5f, 1.0f, 1.0f };
		else
			pMinimapObject->Scaling = { 2.0f, 2.0f, 2.0f };
		break;
	case 1:
		if (Tag == GROUND)
			pMinimapObject->Scaling = { 100.0f, 0.1f, 100.0f };
		else if (Tag == MISILE)
			pMinimapObject->Scaling = { 1.5f, 2.0f, 2.0f };
		else if (Tag == GROUND_ENEMY)
			pMinimapObject->Scaling = { 2.3f, 1.3f, 1.3f };
		else if (Tag == OBJECT)
			pMinimapObject->Scaling = { 10.0f, 10.0f, 10.0f };
		else if (Tag == MINIMAP_UI)
			pMinimapObject->Scaling = { 1.0f, 0.5f, 1.0f };
		else
			pMinimapObject->Scaling = { 3.0f, 3.0f, 3.0f };
		break;
	case 2:
		if (Tag == GROUND)
			pMinimapObject->Scaling = { 100.0f, 0.1f, 100.0f };
		else if (Tag == MISILE)
			pMinimapObject->Scaling = { 2.5f, 2.5f, 2.5f };
		else if (Tag == GROUND_ENEMY)
			pMinimapObject->Scaling = { 3.0f, 3.0f, 3.0f };
		else if (Tag == OBJECT)
			pMinimapObject->Scaling = { 10.0f, 10.0f, 10.0f };
		else if (Tag == MINIMAP_UI)
			pMinimapObject->Scaling = { 1.0f, 0.5f, 1.0f };
		else
			pMinimapObject->Scaling = { 4.0f, 4.0f, 4.0f };
		break;
	default:
		break;
	}
}

XMMATRIX CGameObject::GetIsMatrix()
{
	//現在の角度をもとに行列を作成
	XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(Rotation.y));
	XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(Rotation.x));
	XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(Rotation.z));
	XMMATRIX matrixRotate = matrixRotateX * matrixRotateY * matrixRotateZ;
	return matrixRotate;
}

void CGameObject::SetInitRotateMatrix()
{
	RotateMatrix = GetIsMatrix();
}

void CGameObject::SetVector()
{
	//行列取得
	XMMATRIX matrixRotate = RotateMatrix;

	// 初期前向きベクトル
	XMVECTOR iniForward = XMVectorSet(0, 0, 1, 0);

	// 初期上向きベクトル
	XMVECTOR iniUp = XMVectorSet(0, 1, 0, 0);

	// 初期右向きベクトル
	XMVECTOR iniRight = XMVectorSet(1, 0, 0, 0);

	//プレイヤーの各方向ベクトル作成
	// 初期前向きベクトルと回転行列を掛ける
	newForward
		= XMVector3TransformCoord(iniForward, matrixRotate);

	// 初期上向きベクトルと回転行列を掛ける
	newUP
		= XMVector3TransformCoord(iniUp, matrixRotate);

	// 初期右向きベクトルと回転行列を掛ける
	newRIGHT
		= XMVector3TransformCoord(iniRight, matrixRotate);

	// 前進する処理
	// XMVECTOR→XMFLOAT3変換
	Forward.x = XMVectorGetX(newForward);
	Forward.y = XMVectorGetY(newForward);
	Forward.z = XMVectorGetZ(newForward);

	// XMVECTOR→XMFLOAT3変換
	UP.x = XMVectorGetX(newUP);
	UP.y = XMVectorGetY(newUP);
	UP.z = XMVectorGetZ(newUP);

	// XMVECTOR→XMFLOAT3変換
	RIGHT.x = XMVectorGetX(newRIGHT);
	RIGHT.y = XMVectorGetY(newRIGHT);
	RIGHT.z = XMVectorGetZ(newRIGHT);
}

void CGameObject::Draw()
{
	if (!isActive)
	{
		return;
	}

	if (Tag == PLAYER)
	{
		if (CGame::Get()->GetIsFirst())
		{
			return;
		}
	}
	else if (Tag == HITMISILE_UI)
	{
		if (SetTime <= GetTickCount64())
			return;
	}

	ID3D11DeviceContext* d3dContext;
	D3D::ConstBuffer cb;

	//ワールド行列とビュー行列とプロジェクション行列行列
	DirectX::XMMATRIX matrixWorld, matrixView, matrixProj;

	if (!IsDrawMiniMap)
	{
		D3D::Get()->SetDepthStencilState(0);

		D3D::Get()->SetViewBord(0);
		d3dContext = D3D::Get()->GetContext();

		// Gameクラスが持つカメラから計算済みビュー変換行列を取得
		matrixView = CGame::Get()->GetCamera()->GetViewMatrix();

		matrixProj = CGame::Get()->GetCamera()->GetProjectionMatrix();
	}
	else
	{
		D3D::Get()->SetDepthStencilState(1);

		D3D::Get()->SetViewBord(1);
		d3dContext = D3D::Get()->GetContext();

		// Gameクラスが持つカメラから計算済みビュー変換行列を取得
		matrixView = CGame::Get()->GetMiniCamera()->GetViewMatrix();

		matrixProj = CGame::Get()->GetMiniCamera()->GetProjectionMatrix();
	}


	// ワールド変換行列の作成
	// 移動行列
	XMMATRIX matrixMove = XMMatrixTranslation(
		Position.x, Position.y, Position.z);
	// 拡大縮小行列
	XMMATRIX matrixScale = XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);
	// 回転行列
	XMMATRIX matrixRotate;
	if (isBillboard) // ビルボードなら
	{
		// ビュー変換行列の逆行列を作る
		XMMATRIX invView = matrixView;
		// 行列の移動成分を消し去る
		invView.r[3].m128_f32[0] = 0.0f;
		invView.r[3].m128_f32[1] = 0.0f;
		invView.r[3].m128_f32[2] = 0.0f;
		invView = XMMatrixInverse(nullptr, invView);
		// できた逆行列を回転行列として使う
		matrixRotate = invView;
	}
	else
	{
		matrixRotate = RotateMatrix;
	}

	// 行列は掛け算で合成できる （注）掛ける順番が影響する
	matrixWorld =
		matrixScale *   // 拡大縮小行列
		matrixRotate *  // 回転行列
		matrixMove;  // 移動行列

	//ワールド、ビュー、プロジェクション行列を格納
	cb.world = matrixWorld;
	cb.view = CGame::Get()->GetCamera3D()->GetViewMatrix();
	cb.proj = CGame::Get()->GetCamera3D()->GetProjectionMatrix();

	// 法線ベクトル回転用行列
	cb.matrixWorldNormal = XMMatrixTranspose(matrixRotate);

	cb.matrixWorld = matrixWorld * matrixView * matrixProj;
	// 合成した行列の転置行列を作成する ※シェーダーとC++でメモリの並びが異なるため
	cb.matrixWorld = XMMatrixTranspose(cb.matrixWorld);

	// UVアニメーション行列
	cb.matrixUV = GetUvMatrix();
	cb.matrixUV = XMMatrixTranspose(cb.matrixUV);

	cb.isLighting = isLighting;
	cb.materialDiffuse = { 1,1,1,1 };
	cb.CameraPos = CGame::Get()->GetCamera3D()->GetCameraPos();

	//cb.materialDiffuse = materialDiffuse;

	if (!isLighting)
	{
		//マテリアル色を定数バッファデータに代入
		cb.materialDiffuse = materialDiffuse;
	}
	else
	{
		cb.materialDiffuse = { 1,1,1,1 };
	}

	cb.time = ((float)CGame::Get()->GetNowMoveTime() / 1000);
	cb.Dist = CGame::Get()->GetDist();

	XMFLOAT3 LightVec = { CGame::Get()->GetSunPosition().x - Position.x,
						  CGame::Get()->GetSunPosition().y - Position.y ,
						  CGame::Get()->GetSunPosition().z - Position.z };

	cb.LightVector = LightVec;

	NoiseModel = CGame::Get()->GetModel(L"asset/DistNoise.png");

	if (Tag == PLAYER || Tag == FRIGHT_ENEMY || Tag == BOSS_ENEMY || Tag == FOLLOW_ENEMY)
	{
		ChengeShader(1);
	}
	else if (Tag == BACKFIRE)
	{
		ChengeShader(2);
	}
	else
	{
		ChengeShader(0);
	}

	// 行列をシェーダーに渡す
	d3dContext->UpdateSubresource(D3D::Get()->GetConstantBuffer(), 0, NULL,
		&cb, 0, 0);

	D3D::Model& mdl = model;
	D3D::Model& NoseModel = NoiseModel;

	// 今からDrawする頂点バッファ（モデル）を指定する
	UINT strides = D3D::Get()->GetVertexStride();
	UINT offsets = 0;
	d3dContext->IASetVertexBuffers(0, 1, &mdl.vertexBuffer,
		&strides, &offsets);

	// 描画に使うインデックスバッファを指定する
	d3dContext->IASetIndexBuffer(mdl.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// ピクセルシェーダーにテクスチャを渡す
	d3dContext->PSSetShaderResources(0, 1, &mdl.texture);
	// ピクセルシェーダーにテクスチャを渡す
	d3dContext->PSSetShaderResources(1, 1, &NoseModel.texture);

	// 第１引数　→　描画する頂点数
	d3dContext->DrawIndexed(UINT(mdl.numIndex), 0, 0); // 描画命令
}

void CGameObject::DrawMiniMapObject()
{
	//if (Tag != INITENEMY && Hp > 0)
	//	pMinimapObject->Draw();

	if (Tag == MINIMAP_UI)
	{
		pMinimapObject->Position = CGame::Get()->GetMiniMapCameraPos();
		pMinimapObject->Position.y = pMinimapObject->Position.y - 2.0f;
		//pMinimapObject->Rotation = CGame::Get()->GetPlayer()->GetMiniMapObject()->Rotation;
		pMinimapObject->Rotation.x = 90.0f;
		//pMinimapObject->Rotation.y = CGame::Get()->GetPlayer()->GetMiniMapObject()->Rotation.y;
		pMinimapObject->SetInitRotateMatrix();
	}

	if (pMinimapObject != nullptr && IsDrawMiniMapModel)
	{
		pMinimapObject->Draw();
	}
}

void CGameObject::DrawColliderObject()
{
	//if (CScene::Get()->GetIsDrawCollider())

	if (ColliderObject != nullptr)
		ColliderObject->Draw();
}

void CGameObject::SetMiniMapModel(D3D::Model _model)
{
	pMinimapObject = new CGameObject();
	pMinimapObject->SetIsMiniMap();
	IsDrawMiniMapModel = true;
	pMinimapObject->SetLighting(false);
	pMinimapObject->model = _model;
}

void CGameObject::SetModel(D3D::Model& _model)
{
	model = _model;
}

/**
* 当たり判定AABBを作成する関数です。
*/
void CGameObject::SetColliderBoxAABB()
{
	//当たり判定作成
	ColliderBoxAABB.Center = Position;

	if (Tag == GROUND)
	{
		ColliderBoxAABB.Extents.x = Scaling.x * 5.0f;
		ColliderBoxAABB.Extents.y = Scaling.y;
		ColliderBoxAABB.Extents.z = Scaling.z * 5.0f;
	}
	else
		ColliderBoxAABB.Extents = Scaling;

	if(Tag != GROUND_ENEMY)
	mColliderBoxsAABB[Tag].push_back(ColliderBoxAABB);

	IsColAABB = true;

	if (ColliderObject == nullptr)
	{
		ColliderObject = new CGameObject();
	}
	ColliderObject->Position = Position;
	ColliderObject->Rotation = { 0.0f,0.0f,0.0f };

	if (Tag == GROUND)
	{
		ColliderObject->Position.y -= 5.0f;
		ColliderObject->Scaling.x = Scaling.x * 5.0f;
		ColliderObject->Scaling.y = Scaling.y;
		ColliderObject->Scaling.z = Scaling.z * 5.0f;
	}
	else
	{
		ColliderObject->Scaling = Scaling;
	}

}

/**
* 当たり判定OBBを作成する関数です。
*/
void CGameObject::SetColliderBoxOBB()
{
	//当たり判定作成
	ColliderBoxOBB.Center = Position;
	ColliderBoxOBB.Extents = Scaling;
	XMVECTOR scaling;
	XMVECTOR rotationQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scaling, &rotationQuat, &translation, RotateMatrix);
	XMStoreFloat4(&ColliderBoxOBB.Orientation, rotationQuat);

	IsColAABB = false;
}

void CGameObject::SetColliderBoxAABB(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scaling)
{
	//デバッグ用当たり判定オブジェクト作成
	if (ColliderObject == nullptr)
	{
		ColliderObject = new CGameObject();
	}
	ColliderObject->Position = Position;

	if (Tag == GROUND)
	{
		ColliderObject->Position.y -= 5.0f;
		ColliderObject->Rotation = Rotation;
		ColliderObject->SetInitRotateMatrix();
		ColliderObject->Scaling = Scaling;
		ColliderObject->Scaling.x = Scaling.x * 0.5f;
		ColliderObject->Scaling.y = Scaling.y * 0.5f;
		ColliderObject->Scaling.z = Scaling.z;
	}
	else
	{
		ColliderObject->Rotation = { 0.0f,0.0f,0.0f };
		ColliderObject->Scaling = Scaling;
	}

	//当たり判定作成

	if (Tag == GROUND)
	{
		ColliderBoxAABB.Center = Position;
		ColliderBoxAABB.Extents.x = Scaling.x * 0.5f;
		ColliderBoxAABB.Extents.y = Scaling.z;
		ColliderBoxAABB.Extents.z = Scaling.y * 0.5f;
	}
	else
	{
		ColliderBoxAABB.Center = Position;
		ColliderBoxAABB.Extents = Scaling;
	}


	if (Tag != GROUND_ENEMY)
		mColliderBoxsAABB[Tag].push_back(ColliderBoxAABB);

	IsColAABB = true;
}

void CGameObject::SetColliderBoxOBB(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scaling)
{
	//当たり判定作成
	ColliderBoxOBB.Center = _position;
	ColliderBoxOBB.Extents = _scaling;
	XMVECTOR scaling;
	XMVECTOR rotationQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scaling, &rotationQuat, &translation, RotateMatrix);
	XMStoreFloat4(&ColliderBoxOBB.Orientation, rotationQuat);

	IsColAABB = false;
}

float CGameObject::CheckDistance(const CGameObject* _obj, const DirectX::XMFLOAT3& _pos) const
{
	if (this == nullptr)
	{
		return 1000.0f;
	}

	XMFLOAT3 ppos = Position;
	XMFLOAT3 epos;
	XMFLOAT3 res;

	if (_obj != nullptr)
	{
		epos = _obj->Position;
	}
	else
	{
		epos = _pos;
	}

	res = { epos.x - ppos.x,epos.y - ppos.y ,epos.z - ppos.z };
	return float(std::sqrt((pow(res.x, 2) + pow(res.y, 2) + pow(res.z, 2))));

}

void CGameObject::SetBillboard(bool isBillboard)
{
	this->isBillboard = isBillboard;
}

void CGameObject::SetShadow(bool _isshadow)
{
	IsShadow = _isshadow;
}

bool CGameObject::PadInputTrigget(int _num)
{

	switch (_num)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	default:
		break;
	}


	return false;
}

XMFLOAT3 CGameObject::ExtractRotationFromMatrix(const XMMATRIX& matrix)
{
	XMFLOAT3 rotation;
	XMVECTOR scaling;
	XMVECTOR rotationQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scaling, &rotationQuat, &translation, matrix);
	XMStoreFloat3(&rotation, rotationQuat);

	XMFLOAT3 euler;

	// クォータニオンの各要素を取得
	float x = XMVectorGetX(rotationQuat);
	float y = XMVectorGetY(rotationQuat);
	float z = XMVectorGetZ(rotationQuat);
	float w = XMVectorGetW(rotationQuat);

	// roll (x 軸周りの回転)
	float sinr_cosp = 2 * (w * x + y * z);
	float cosr_cosp = 1 - 2 * (x * x + y * y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y 軸周りの回転)
	float sinp = 2 * (w * y - z * x);
	if (std::abs(sinp) >= 1)
		euler.y = std::copysign(3.14f / 2, sinp); // 垂直方向の回転が 90 度の場合
	else
		euler.y = std::asin(sinp);

	// yaw (z 軸周りの回転)
	float siny_cosp = 2 * (w * z + x * y);
	float cosy_cosp = 1 - 2 * (y * y + z * z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	euler.x = XMConvertToDegrees(euler.x);
	euler.y = XMConvertToDegrees(euler.y);
	euler.z = XMConvertToDegrees(euler.z);

	return euler;
}

void CGameObject::DoControllerEvent()
{
	//コントローラーの初期化
	iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
	iPad_leftshoulder = 0, iPad_rightshoulder = 0;
	iPad_leftthumb = 0, iPad_rightthumb = 0,
	iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
	iPad_Start = 0, iPad_Back = 0;
	iPad_LStickX = 0, iPad_LStickY = 0;
	iPad_RStickX = 0, iPad_RStickY = 0;

	//コントローラーのアップデート
	//対応する操作がされたらされた操作のフラグが1になる
	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		iPad_left = 1;//ゲームパッド十字キー左
	else OldLeftKey = false;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
		iPad_right = 1;//ゲームパッド十字キー右
	else OldRightKey = false;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		iPad_up = 1;//ゲームパッド十字キー上
	else OldUpKey = false;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		iPad_down = 1;//ゲームパッド十字キー下
	else OldDownKey = false;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
		iPad_leftshoulder = 1;//ゲームパッドL
	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		iPad_rightshoulder = 1;//ゲームパッドR

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
		iPad_leftthumb = 1;
	else
		OldLeftThumb = 0;
	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
		iPad_rightthumb = 1;
	else
		OldRightThumb = 0;
	

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		iPad_A = 1;//ゲームパッドA
		//OldAKey = 1;
	}
	else
		OldAKey = 0;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		iPad_B = 1;//ゲームパッドB
		//OldBKey = 1;
	}
	else
		OldBKey = 0;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		iPad_X = 1;//ゲームパッドX
		//OldXKey = 1;
		
	}
	else
		OldXKey = 0;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		iPad_Y = 1;//ゲームパッドY
		//OldYKey = 1;
	}
	else
		OldYKey = 0;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_START)
		iPad_Start = 1;//ゲームパッドスタート
	else
		OldStartKey = 0;

	if (State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
		iPad_Back = 1;//ゲームパッドスタート
	else
		OldBackKey = 0;

	

	//ゲームパッドアナログスティックのデッドゾーン処理
	if ((State.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && State.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(State.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && State.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		State.Gamepad.sThumbLX = 0;
		State.Gamepad.sThumbLY = 0;
	}

	if (State.Gamepad.sThumbLX != 0)
	{
		iPad_LStickX = State.Gamepad.sThumbLX;
	}

	//if (State.Gamepad.sThumbLX > 0)//ゲームパッドアナログスティック入力時処理
	//{
	//	iPad_LStickX = 1;
	//}
	//else if (State.Gamepad.sThumbLX < 0)
	//{
	//	iPad_LStickX = -1;
	//}

	if (State.Gamepad.sThumbLY != 0)
	{
		iPad_LStickY = State.Gamepad.sThumbLY;
	}

	//if (State.Gamepad.sThumbLY > 0)//ゲームパッドアナログスティック入力時処理
	//{
	//	iPad_LStickY = 1;
	//}
	//else if (State.Gamepad.sThumbLY < 0)
	//{
	//	iPad_LStickY = -1;
	//}

	if (State.Gamepad.sThumbRY > 0)//ゲームパッドアナログスティック入力時処理
	{
		iPad_RStickY = 1;
	}
	else if (State.Gamepad.sThumbRY < 0)
	{
		iPad_RStickY = -1;
	}
}

bool CGameObject::CheckControllerTrigger(BOTTUN_NAME _buttonname)
{
	//コントローラー接続確認
	GetControllerState();

	//コントローラーの入力判定
	DoControllerEvent();

	switch (_buttonname)
	{
	case CGameObject::NO_Key:
		break;
	case CGameObject::CONTROLLER_A:
		if (!OldAKey)
		{
			if (iPad_A)
			{
				OldAKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_B:
		if (!OldBKey)
		{
			if (iPad_B)
			{
				OldBKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_X:
		if (!OldXKey)
		{
			if (iPad_X)
			{
				OldXKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_Y:
		if (!OldYKey)
		{
			if (iPad_Y)
			{
				OldYKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_RIGHT:
		break;
	case CGameObject::CONTROLLER_LEFT:
		break;
	case CGameObject::CONTROLLER_UP:
		if (!OldUpKey)
		{
			if (iPad_up)
			{
				OldUpKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_DOWN:
		if (!OldDownKey)
		{
			if (iPad_down)
			{
				OldDownKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_RB:
		break;
	case CGameObject::CONTROLLER_LB:
		break;
	case CGameObject::CONTROLLER_RT:
		break;
	case CGameObject::CONTROLLER_LT:
		break;
	case CGameObject::CONTROLLER_RIGHT_THUMB:
		if (!OldRightThumb)
		{
			if (iPad_rightthumb)
			{
				OldRightThumb = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_LEFT_THUMB:
		if (!OldLeftThumb)
		{
			if (iPad_leftthumb)
			{
				OldLeftThumb = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_MENU:
		if (!OldStartKey)
		{
			if(iPad_Start)
			{
				OldStartKey = true;
				return true;
			}
		}
		break;
	case CGameObject::CONTROLLER_BACK:
		if (!OldBackKey)
		{
			if (iPad_Back)
			{
				OldBackKey = true;
				return true;
			}
		}
		break;
	default:
		break;
	}

	return false;
}

bool CGameObject::CheckControllerPress(BOTTUN_NAME _buttonname)
{
	//コントローラー接続確認
	GetControllerState();

	//コントローラーの入力判定
	DoControllerEvent();

	switch (_buttonname)
	{
	case CGameObject::NO_Key:
		break;
	case CGameObject::CONTROLLER_A:
		if (iPad_A)
			return true;
		break;
	case CGameObject::CONTROLLER_B:
		if (iPad_B)
			return true;
		break;
	case CGameObject::CONTROLLER_X:
		if (iPad_X)
			return true;
		break;
	case CGameObject::CONTROLLER_Y:
		if (iPad_Y)
			return true;
		break;
	case CGameObject::CONTROLLER_RIGHT:
		if (iPad_right)
			return true;
		break;
	case CGameObject::CONTROLLER_LEFT:
		if (iPad_left)
			return true;
		break;
	case CGameObject::CONTROLLER_UP:
		if (iPad_up)
			return true;
		break;
	case CGameObject::CONTROLLER_DOWN:
		if (iPad_down)
			return true;
		break;
	case CGameObject::CONTROLLER_RB:
		break;
	case CGameObject::CONTROLLER_LB:
		break;
	case CGameObject::CONTROLLER_RT:
		break;
	case CGameObject::CONTROLLER_LT:
		break;
	case CGameObject::CONTROLLER_MENU:
		if (iPad_Start)
			return true;
		break;
	case CGameObject::CONTROLLER_RIGHT_THUMB:
		break;
	case CGameObject::CONTROLLER_LEFT_THUMB:
		break;
	default:
		break;
	}

	return false;
}

void CGameObject::GetControllerState()
{
	ZeroMemory(&State, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	DwResult = XInputGetState(0, &State);

	if (DwResult == ERROR_SUCCESS)
	{
		//接続に成功したときの処理

	}
	else
	{
		//接続に失敗したときの処理

	}
}

bool CGameObject::GetIsCameraRef()
{
	return IsRefMainCamera;
}

bool CGameObject::GetIsCenterCameraRef()
{
	return IsRefCenterCamera;
}

void CGameObject::ChackRefCamera()
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

	//// BoundingOrientedBox の生成
	//DirectX::BoundingOrientedBox obb = ColliderBoxOBB;

	//オブジェクトのバウンディングボックスの取得:
	XMVECTOR boxVertices[8];
	boxVertices[0] = XMVectorSet(Position.x - Scaling.x, Position.y - Scaling.y, Position.z - Scaling.z, 1.0f);
	boxVertices[1] = XMVectorSet(Position.x + Scaling.x, Position.y - Scaling.y, Position.z - Scaling.z, 1.0f);
	boxVertices[2] = XMVectorSet(Position.x - Scaling.x, Position.y + Scaling.y, Position.z - Scaling.z, 1.0f);
	boxVertices[3] = XMVectorSet(Position.x + Scaling.x, Position.y + Scaling.y, Position.z - Scaling.z, 1.0f);
	boxVertices[4] = XMVectorSet(Position.x - Scaling.x, Position.y - Scaling.y, Position.z + Scaling.z, 1.0f);
	boxVertices[5] = XMVectorSet(Position.x + Scaling.x, Position.y - Scaling.y, Position.z + Scaling.z, 1.0f);
	boxVertices[6] = XMVectorSet(Position.x - Scaling.x, Position.y + Scaling.y, Position.z + Scaling.z, 1.0f);
	boxVertices[7] = XMVectorSet(Position.x + Scaling.x, Position.y + Scaling.y, Position.z + Scaling.z, 1.0f);

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

void CGameObject::NormalizePlane(Plane& plane)
{
	float length = XMVectorGetX(XMVector3Length(plane.normal));
	plane.normal = XMVectorScale(plane.normal, 1.0f / length);
	plane.distance /= length;
}

void CGameObject::SetColliderObject(CGameObject* _colliderobject)
{
	ColliderObject = new CGameObject();
	ColliderObject = _colliderobject;
	ColliderObject->isActive = true;
}

void CGameObject::SetCullingMode(bool _cullmode)
{
	D3D::Get()->SetCulling(_cullmode);
}

void CGameObject::SetLinear()
{
	D3D::Get()->SetLinear();
}

void CGameObject::ChengeShader(const int& _id)
{
	D3D::Get()->ChengeShader(_id);
}

void CGameObject::ClearColliderAABBBox()
{
	mColliderBoxsAABB.clear();
}
