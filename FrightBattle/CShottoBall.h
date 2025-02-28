#pragma once
#include "CGameObject.h"

class CEnemy;
class CEffect;

/**
 * これはCShottoBallクラスの説明です。
 * このクラスは弾を撃つ制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CShottoBall :
    public CGameObject
{
public:
      //発射された時間
    ULONGLONG ShotTime;

private:
	//ターゲットのオブジェクトの座標
	DirectX::XMFLOAT3 TargetPosition = { 0,0,0 };

	//ターゲットの方向
	DirectX::XMFLOAT3 fDirection;
	DirectX::XMVECTOR vDirection;

	//ターゲット情報
	CEnemy* Target;

	//もっているエフェクト情報
	CEffect* pEffect;

	//敵の当たり判定
	DirectX::BoundingOrientedBox EnemyCol;

	//玉が発射された時間
	LONGLONG ShottoTime = 0;

	float ShotSpeed = 0.0f;

public:
	CShottoBall();
	~CShottoBall();

	/**
	* 更新処理を行う関数です
	*/
	void Update() override;

	/**
	* 描画処理を行う関数です
	*/
	void Draw() override;

	/**
	* 弾の進行方向を設定する関数です。
	*/
	void SetDiviatDir();

	/**
	* ターゲットを設定する関数です
	* 
	* @param  設定したいターゲット
	*/
	void SetTarget(CEnemy* _target) { Target = _target; }

	/**
	* ターゲットへの方向を計算する関数です。
	* 
	* @return 計算したターゲットの方向
	*/
	DirectX::XMFLOAT3 SetDirect();

	/**
	* 弾のモデルを設定する関数です。
	* 
	* @param  設定したい弾のモデル
	*/
	void SetModel(D3D::Model _model);

	/**
	* 飛ぶ速度を設定する関数です。
	* 
	* @param  設定したい移動速度
	*/
	void SetSpeed(float _speed) { MoveSpeed = _speed; }

	/**
	* ターゲットの座標を設定する関数です。
	* 
	* @param  設定したいターゲットの座標
	*/
	void TargetPos(DirectX::XMFLOAT3 _targetpos) { TargetPosition = _targetpos; }

	/**
	* 弾が出現する座標を設定する関数です。
	* 
	* @param  弾を出現させたい座標
	*/
	void SetSpornPos(DirectX::XMFLOAT3 _spornpos) { Position = _spornpos; }

	/**
	* ミサイルの初期角度を設定する関数です。
	* 
	* @param  弾を出現させたときの初期角度
	*/
	void SetSpornRotate(DirectX::XMFLOAT3 _spornrotate) { Rotation = _spornrotate; }

	/**
	* 初期回転行列を設定する関数です。
	*/
	void SetRotateMatrix();

	/**
	* 弾のヒットエフェクトモデルを設定する関数です。
	* 
	* @param  設定したい弾のヒットエフェクトモデル
	*/
	void SetHitBallColBoxEffect(D3D::Model _model);

	/**
	* 弾が発射された時間を設定する関数です。
	* 
	* @param  弾が発射された時間
	*/
	void SetShottoTime(LONGLONG _time) { ShottoTime = _time; }

	/**
	* アニメーションのUVサイズを設定する関数です。
	* 
	* @param  _u	設定したいUVのU
	* @param  _v	設定したいUVのV
	*/
	void SetAnimUVSize(float _u,float _v);

};

