#pragma once
#include"direct3d.h"

/**
 * これはCCameraクラスの説明です。
 * このクラスはカメラ制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CCamera
{
public:
	/**
	*カメラの状態を入れる構造体です。
	*
	* @param NORMAL  通常カメラ
	* @param MINIMAP ミニマップカメラ
	*/
	enum CAMERA_MODE
	{
		NORMAL,
		MINIMAP,
	};

	/**
	*カメラが存在する座標を設定する関数です。
	* @param カメラの座標をセットします。
	*/
	enum PROJECTION_MODE
	{
		_3D, _2D
	};

private:
	// メンバー変数
	// 計算済みビュー変換行列を保存する変数
	DirectX::XMMATRIX matrixView = {};

	// Eyeを保存する変数
	DirectX::XMFLOAT3 eye = { 0,0,-1 };
	// Focusを保存する変数
	DirectX::XMFLOAT3 focus = { 0,0,0 };
	// Upを保存する変数
	DirectX::XMFLOAT3 up = { 0,1,0 };
	// Rightを保存する変数
	DirectX::XMFLOAT3 right = { 1.0f,0.0f,0.0f };

	//デバッグ用//

	//前向きベクトル = カメラが向いている方向を表す矢印
	DirectX::XMFLOAT3 Forward = { 0,0,1 };

	//上向きベクトル = カメラの上方向を表す矢印
	DirectX::XMFLOAT3 Right = { 1,0,0 };

	//カメラの各方向ベクトル作成
	DirectX::XMVECTOR newForward = { 0,0,1,0 };
	DirectX::XMVECTOR newUP = { 0,1,0,0 };
	DirectX::XMVECTOR newRIGHT = { 1,0,0,0 };

	//合成用行列
	DirectX::XMMATRIX RotateMatrix = {};

	//角度(デバッグ)
	DirectX::XMFLOAT3 Rotate = { 0.0f,0.0f,0.0f };

	//カメラの各頂点を入れる構造体です。
	struct Frustum
	{
		DirectX::XMFLOAT4 planes[6];
	};
	Frustum frus = {};

	//カメラのモード
	CAMERA_MODE CameraMode = NORMAL;
	
	// デフォルトは3D → 透視投影
	PROJECTION_MODE projectionMode = _3D;

	//カメラのFOV角度
	float FovRadius = 45.0f;
	//float FovRadius = 60.0f;

	//デバッグ用カメラか
	bool IsDebug = false;

	//デバッグカメラの移動スピード
	float MoveSpeed = 0.2f;

	//デバッグカメラの回転速度
	float RotateSpeed = 2.0f;

	//カメラのオフセット位置
	DirectX::XMFLOAT3 OffsetCameraPos = { 0.0f,0.7f,2.5f };

	//カメラのアルファ値
	float FocusAlpha = 0.2f;
	float UpAlpha = 0.1f;
	float PosAlpha = 0.0f;

	/**
	* カメラの方向計算
	*/
	void SetVector();

public:

	CCamera();
	~CCamera(){}

	/**
	 *カメラが存在する座標を設定する関数です。
	 * @param カメラの座標をセットします。
	 */
	void SetEye(DirectX::XMFLOAT3 eyePosition);

	/**
	 *カメラが狙う座標を設定する関数です。
	 * @param カメラが狙う座標をセットします。
	 */
	void SetFocus(DirectX::XMFLOAT3 focusPosition);

	/**
	 *カメラの上方向を設定する関数です。
	 * @param カメラの上方向をセットします。
	 */
	void SetUp(DirectX::XMFLOAT3 upPosition);

	/**
	 *カメラの上方向を増加する関数です。
	 * @param カメラの上方向に足したい座標をセットします。
	 */
	void AddUp(DirectX::XMFLOAT3 upPosition);

	/**
	 *カメラの座標を取得する関数です。
	 * @return カメラの座標を返します。
	 */
	DirectX::XMFLOAT3 GetCameraPos() { return eye; }

	/**
	* カメラのフォーカスを取得する関数です
	* 
	* @return カメラのフォーカス座標
	*/
	DirectX::XMFLOAT3 GetCameraFocus() { return focus; }

	/**
	* カメラの上方向を取得する関数です。
	*/
	DirectX::XMFLOAT3 GetCameraUp() { return up; }

	/**
	 *ビュー変換行列を計算する関数(更新処理)です。
	 */
	void Update();

	/**
	 *計算済みビュー変換行列を取得する関数です。
	 * @return 計算済みビュー変換行列を取得します。
	 */
	DirectX::XMMATRIX GetViewMatrix();
	
	 /**
	 *オブジェクトの投影方法を取得する関数です。
	 * @return オブジェクトの投影方法を取得します。
	 */
	DirectX::XMMATRIX GetProjectionMatrix();

	/**
	 *オブジェクトの投影方法を設定する関数です。
	 * @param 設定したいオブジェクトの投影方法です。
	 */
	void SetProjectionMode(PROJECTION_MODE mode) { projectionMode = mode; }

	/**
	 *オブジェクトの投影方法を設定する関数です。
	 * @param 設定したいオブジェクトの投影方法です。
	 */
	void SetCameraMode(CAMERA_MODE _cameramode) { CameraMode = _cameramode; }

	/**
	 *カメラの各頂点を計算する関数です。
	 * @param カメラの各頂点を格納する場所です。
	 * @param 現在のプロジェクション行列です
	 */
	void ExtractFrustum(Frustum& frustum, DirectX::XMMATRIX viewProj);

	/**
	* カメラのFOV角度をセットする関数です。
	* 
	* @param 設定したいFOV角度
	*/
	void SetFovRadius(float _fovradius) { FovRadius = _fovradius; }

	/**
	* カメラのFOV角度をプラスする関数です。
	* 
	* @param 足したい値
	*/
	void AddFovRadius(float _addrad) { FovRadius += _addrad; }

	/**
	* 徐々に初期のFOV角度に戻す
	*/
	void MoveInitFovRadius(float _moverad);

	/**
	* 初期の視野角に設定する
	*/
	void InitFovRadius() { FovRadius = 45.0f; }

	/**
	* 現在の視野角を取得する
	*/
	float GetFovRadius() { return FovRadius; }

	/**
	* カメラのオフセット位置を返す関数です。
	*/ 
	DirectX::XMFLOAT3& GetOffsetCameraPos() { return OffsetCameraPos; }

	/**
	* カメラのフォーカス補完を返す
	* カメラのアップ補完を返す
	*/
	float& GetFocusAlpha() { return FocusAlpha; }
	float& GetUpAlpha() { return UpAlpha; }

	/**
	* デバッグ用カメラに変更
	*/
	void SetIsDebug() { IsDebug = true; }

	/**
	* 初期回転行列作成(デバッグ用)
	*/
	void InitMatrixRotate();

	/**
	* カメラ座標前後移動(デバッグ用)
	*/
	void MoveForwardDebugCamera(bool _plus);

	/**
	* カメラ座標左右移動(デバッグ用)
	*/
	void MoveRightDebugCamera(bool _plus);

	/**
	* カメラ座標上下移動(デバッグ用)
	*/
	void MoveUpDebugCamera(bool _plus);

	/**
	* デバッグカメラ回転
	*/
	void RotateDebugCamera(bool _right);

	/**
	* デバッグカメラの移動スピードを取得する関数です。
	*/
	float& GetDebugMoveSpeed() { return MoveSpeed; }

	/**
	* デバッグカメラの回転速度を取得する関数です。
	*/
	float& GetDebugRotateSpeed() { return RotateSpeed; }
};

