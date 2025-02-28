#include "CCamera.h"
using namespace DirectX;

#define ADD_RADIUS_NUM (1)

//初期視野角
#define INITFOVRAD (45)

void CCamera::SetVector()
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

    // XMVECTOR→XMFLOAT3変換
    Forward.x = XMVectorGetX(newForward);
    Forward.y = XMVectorGetY(newForward);
    Forward.z = XMVectorGetZ(newForward);

    //// XMVECTOR→XMFLOAT3変換
    //Up.x = XMVectorGetX(newUP);
    //Up.y = XMVectorGetY(newUP);
    //Up.z = XMVectorGetZ(newUP);

    // XMVECTOR→XMFLOAT3変換
    Right.x = XMVectorGetX(newRIGHT);
    Right.y = XMVectorGetY(newRIGHT);
    Right.z = XMVectorGetZ(newRIGHT);

}

CCamera::CCamera()
{
    XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);            //カメラの座標
    XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);    //カメラの映す方向
    XMVECTOR vUp = XMVectorSet(up.x, up.y, up.z, 0);                //カメラの上方向

    // View変換行列作成の関数を呼び出す
    matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);
}

void CCamera::InitMatrixRotate()
{
    //現在の角度をもとに行列を作成
    XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(Rotate.y));
    XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(Rotate.x));
    XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(Rotate.z));
    RotateMatrix = matrixRotateX * matrixRotateY * matrixRotateZ;
}

void CCamera::MoveForwardDebugCamera(bool _plus)
{
    // 前進する処理
    // XMVECTOR→XMFLOAT3変換
    Forward.x = XMVectorGetX(newForward);
    Forward.y = XMVectorGetY(newForward);
    Forward.z = XMVectorGetZ(newForward);

    if (_plus)
    {
        eye.x += Forward.x * MoveSpeed;
        eye.y += Forward.y * MoveSpeed;
        eye.z += Forward.z * MoveSpeed;
    }
    else
    {
        eye.x -= Forward.x * MoveSpeed;
        eye.y -= Forward.y * MoveSpeed;
        eye.z -= Forward.z * MoveSpeed;
    }
}

void CCamera::MoveRightDebugCamera(bool _plus)
{
    // 前進する処理
    // XMVECTOR→XMFLOAT3変換
    Right.x = XMVectorGetX(newRIGHT);
    Right.y = XMVectorGetY(newRIGHT);
    Right.z = XMVectorGetZ(newRIGHT);

    if (_plus)
    {
        eye.x += Right.x * MoveSpeed;
        eye.y += Right.y * MoveSpeed;
        eye.z += Right.z * MoveSpeed;
    }
    else
    {
        eye.x -= Right.x * MoveSpeed;
        eye.y -= Right.y * MoveSpeed;
        eye.z -= Right.z * MoveSpeed;
    }
}

void CCamera::MoveUpDebugCamera(bool _plus)
{
    //// 前進する処理
    //// XMVECTOR→XMFLOAT3変換
    //Up.x = XMVectorGetX(newUP);
    //Up.y = XMVectorGetY(newUP);
    //Up.z = XMVectorGetZ(newUP);

    //if (_plus)
    //{
    //    eye.x += Up.x * MoveSpeed;
    //    eye.y += Up.y * MoveSpeed;
    //    eye.z += Up.z * MoveSpeed;
    //}
    //else
    //{
    //    eye.x -= Up.x * MoveSpeed;
    //    eye.y -= Up.y * MoveSpeed;
    //    eye.z -= Up.z * MoveSpeed;
    //}
}

void CCamera::RotateDebugCamera(bool _right)
{
    if (_right)
    {
        //任意軸で回転させる
        //クォータニオンでの回転
        XMVECTOR QrX = XMQuaternionRotationAxis(newRIGHT, XMConvertToRadians(0.0f));
        XMVECTOR QrY = XMQuaternionRotationAxis(newUP, XMConvertToRadians(RotateSpeed));
        XMVECTOR QrZ = XMQuaternionRotationAxis(newForward, XMConvertToRadians(0.0f));

        XMMATRIX QmtX = XMMatrixRotationQuaternion(QrX);
        XMMATRIX QmtY = XMMatrixRotationQuaternion(QrY);
        XMMATRIX QmtZ = XMMatrixRotationQuaternion(QrZ);

        //XMMATRIX QMT = QmtX * QmtY * QmtZ;
        XMMATRIX QMT = QmtY * QmtX * QmtZ;
        RotateMatrix *= QMT;
    }
    else
    {
        //任意軸で回転させる
        //クォータニオンでの回転
        XMVECTOR QrX = XMQuaternionRotationAxis(newRIGHT, XMConvertToRadians(0.0f));
        XMVECTOR QrY = XMQuaternionRotationAxis(newUP, XMConvertToRadians(-RotateSpeed));
        XMVECTOR QrZ = XMQuaternionRotationAxis(newForward, XMConvertToRadians(0.0f));

        XMMATRIX QmtX = XMMatrixRotationQuaternion(QrX);
        XMMATRIX QmtY = XMMatrixRotationQuaternion(QrY);
        XMMATRIX QmtZ = XMMatrixRotationQuaternion(QrZ);

        //XMMATRIX QMT = QmtX * QmtY * QmtZ;
        XMMATRIX QMT = QmtY * QmtX * QmtZ;
        RotateMatrix *= QMT;
    }
}

/**
*カメラが存在する座標を設定する関数です。
* @param カメラの座標をセットします。
*/
void CCamera::SetEye(DirectX::XMFLOAT3 _eyeposition)
{
	eye = _eyeposition;
}

/**
 *カメラが狙う座標を設定する関数です。
 * @param カメラが狙う座標をセットします。
 */
void CCamera::SetFocus(DirectX::XMFLOAT3 _focusposition)
{
	focus = _focusposition;
}

/**
 *カメラの上方向を設定する関数です。
 * @param カメラの上方向をセットします。
 */
void CCamera::SetUp(DirectX::XMFLOAT3 upPosition)
{
	up = upPosition;
}

/**
 *カメラの上方向を増加する関数です。
 * @param カメラの上方向に足したい座標をセットします。
 */
void CCamera::AddUp(DirectX::XMFLOAT3 upPosition)
{
	up.x += upPosition.x;
	up.y += upPosition.y;
	up.z += upPosition.z;
}

/**
 *ビュー変換行列を計算する関数(更新処理)です。
 */
void CCamera::Update()
{
	//XMFLOAT3からXMVECTORに型変換する
    if (!IsDebug)
    {
        XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);            //カメラの座標
        XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);    //カメラの映す方向
        XMVECTOR vUp = XMVectorSet(up.x, up.y, up.z, 0);                //カメラの上方向

        // View変換行列作成の関数を呼び出す
        matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);

        ExtractFrustum(frus, matrixView);
    }
    else
    {
        //方向計算
        SetVector();

        focus.x = eye.x + Forward.x * 10.0f;
        focus.y = eye.y + Forward.y * 10.0f;
        focus.z = eye.z + Forward.z * 10.0f;

        XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);            //カメラの座標
        XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);    //カメラの映す方向
        XMVECTOR vUp = newUP;                                           //カメラの上方向

        // View変換行列作成の関数を呼び出す
        matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);

        ExtractFrustum(frus, matrixView);
    }

}

/**
 *計算済みビュー変換行列を取得する関数です。
 * @return 計算済みビュー変換行列を取得します。
 */
DirectX::XMMATRIX CCamera::GetViewMatrix()
{
	return matrixView;
}

/**
*オブジェクトの投影方法を取得する関数です。
* @return オブジェクトの投影方法を取得します。
*/
DirectX::XMMATRIX CCamera::GetProjectionMatrix()
{
	FXMMATRIX matrixProj3D = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(FovRadius), 16.0f / 9.0f, 1.0f, 3000.0f);

	// 平行投影の行列作成
	// 引数①②：映し出す面の横縦の長さ
	// 引数③④：映し出す空間の奥行（手前と最奥）
	FXMMATRIX matrixProj2D = XMMatrixOrthographicLH(
		16.0f * 0.3f, 9.0f * 0.3f, 0.0f, 3000.0f);

	if (projectionMode == _3D)
	{
		return matrixProj3D;
	}
	else
	{
		return matrixProj2D;
	}
}

/**
 *カメラの各頂点を計算する関数です。
 * @param カメラの各頂点を格納する場所です。
 * @param 現在のプロジェクション行列です
 */
void CCamera::ExtractFrustum(Frustum& frustum, XMMATRIX viewProj)
{
    // Transpose the view-projection matrix
    XMMATRIX mat = XMMatrixTranspose(viewProj);

    // Left plane
    frustum.planes[0] = XMFLOAT4(
        mat.r[3].m128_f32[0] + mat.r[0].m128_f32[0],
        mat.r[3].m128_f32[1] + mat.r[0].m128_f32[1],
        mat.r[3].m128_f32[2] + mat.r[0].m128_f32[2],
        mat.r[3].m128_f32[3] + mat.r[0].m128_f32[3]);

    // Right plane
    frustum.planes[1] = XMFLOAT4(
        mat.r[3].m128_f32[0] - mat.r[0].m128_f32[0],
        mat.r[3].m128_f32[1] - mat.r[0].m128_f32[1],
        mat.r[3].m128_f32[2] - mat.r[0].m128_f32[2],
        mat.r[3].m128_f32[3] - mat.r[0].m128_f32[3]);

    // Bottom plane
    frustum.planes[2] = XMFLOAT4(
        mat.r[3].m128_f32[0] + mat.r[1].m128_f32[0],
        mat.r[3].m128_f32[1] + mat.r[1].m128_f32[1],
        mat.r[3].m128_f32[2] + mat.r[1].m128_f32[2],
        mat.r[3].m128_f32[3] + mat.r[1].m128_f32[3]);

    // Top plane
    frustum.planes[3] = XMFLOAT4(
        mat.r[3].m128_f32[0] - mat.r[1].m128_f32[0],
        mat.r[3].m128_f32[1] - mat.r[1].m128_f32[1],
        mat.r[3].m128_f32[2] - mat.r[1].m128_f32[2],
        mat.r[3].m128_f32[3] - mat.r[1].m128_f32[3]);

    // Near plane
    frustum.planes[4] = XMFLOAT4(
        mat.r[2].m128_f32[0],
        mat.r[2].m128_f32[1],
        mat.r[2].m128_f32[2],
        mat.r[2].m128_f32[3]);

    // Far plane
    frustum.planes[5] = XMFLOAT4(
        mat.r[3].m128_f32[0] - mat.r[2].m128_f32[0],
        mat.r[3].m128_f32[1] - mat.r[2].m128_f32[1],
        mat.r[3].m128_f32[2] - mat.r[2].m128_f32[2],
        mat.r[3].m128_f32[3] - mat.r[2].m128_f32[3]);
}

void CCamera::MoveInitFovRadius(float _moverad)
{
    if (FovRadius > INITFOVRAD)
        FovRadius -= _moverad;
    else if (FovRadius < INITFOVRAD)
        FovRadius += _moverad;
    else
        FovRadius = INITFOVRAD;
}
