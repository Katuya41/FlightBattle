#pragma once
#include "CGameObject.h"
#include <list>
#include <deque>
#include <vector>

class CEnemy;
class CEffect;

/**
 * これはCMisileクラスの説明です。
* このクラスはミサイル制御で使用されます。
 *
* @author 吉村括哉
* @since 2024-7-10
*/
class CMisile :
	public CGameObject
{
public:
	//ミサイルの状態
	enum MISILE_STATE
	{
		SHOTTO,		//発射中
		READY,		//発射可能
		WAIT,		//発射時間待機中
	};

private:
	//発射された時間
	ULONGLONG ShotTime;

	//現在のミサイルの状態
	MISILE_STATE MisileState = READY;

	//ターゲットのオブジェクトの座標
	DirectX::XMFLOAT3 TargetPosition = { 0,0,0 };

	//ターゲットの方向
	DirectX::XMFLOAT3 fDirection;
	DirectX::XMVECTOR vDirection;

	//ターゲット情報
	//これには敵またはプレイヤーのアドレスを入れるため、
	//CPlayer又はCplayerAttackで開放を行う
	CGameObject* Target;

	//もっているエフェクト情報
	CEffect* pEffect;

	//敵の当たり判定
	DirectX::BoundingOrientedBox EnemyColOBB;
	DirectX::BoundingBox EnemyColAABB;

	//発射可能かどうか
	bool IsShotto = true;

	//敵を追いかけるか
	bool IsMoveEnemy = false;

	//撃ったオブジェクトがプレイヤーか
	bool IsShotPlayer = false;

	//パーティクルの構造体です
	struct PARTICLE
	{
		std::unique_ptr<CGameObject> Object;	//ユニークポインタ使用
		float CoolTime;				//クールタイム
		ULONGLONG DeleteTime;			//消去時間
		ULONGLONG StartTime;			//開始時間
	};

	//パーティクルのオブジェクトを格納する変数
	//vectorはeraseを行う時にメモリ移動を行う
	//移動を行わないdequeを使用
	std::deque<PARTICLE> Particles;

	//パーティクルの判定時間
	LONGLONG OldParticleTime;

	//パーティクルのモデル
	D3D::Model ParticleModel;

	//敵に向かっていく処理
	void MoveUpdate();

	//消去判定
	bool IsEnd = false;

	//ターゲットに当たった時のUIを表示するか
	bool IsDrawHitUI = false;

	//ターゲットとの距離
	float TargetDistance = 0.0f;

	//回転角度
	float RotateAngle = 0.0f;

	//加速度
	float AddSpeed = 0.0f;

	/**
	* パーティクルを生成する関数です。
	*/
	void InsertParticle();

public:
	//コンストラクタ
	CMisile();
	~CMisile();

	/**
	*更新処理を行う関数です。
	*/
	void Update() override;

	/**
	*描画処理行う関数です。
	*/
	void Draw() override;

	/**
	* パーティクルの更新処理
	*/
	void ParticleUpdate();

	/**
	* パーティクルの描画処理を行う関数です。
	*/
	void DrawParticle();

	/**
	*ターゲット設定を行う関数です。
	*
	* @param ターゲットしたいオブジェクト
	*/
	void SetTarget(CGameObject* _target) { Target = _target; }

	/**
	*ターゲットへの方向の計算を行う関数です。
	*
	* @return このオブジェクトから見たターゲットオブジェクトへの方向
	*/
	DirectX::XMFLOAT3 SetDirect();

	/**
	*ミサイルが出現する座標の設定を行う関数です。
	*
	* @return オブジェクトが出現する座標
	*/
	void SetSpornPos(DirectX::XMFLOAT3 _spornpos) { Position = _spornpos; }

	//ミサイルの初期角度をセットする
	void SetSpornRotate(DirectX::XMFLOAT3 _spornrotate) { Rotation = _spornrotate; }

	//ミサイルのヒットテクスチャ設定
	void SetHitMisileEffect(D3D::Model _model);

	void SetUv_Size(DirectX::XMFLOAT2 _uv);

	//ミサイルの状態を返す
	MISILE_STATE GetMisileState() { return MisileState; }

	//ミサイルの状態を設定する
	void SetMisileState(MISILE_STATE _state) { MisileState = _state; }

	//ミサイルのパーティクル時間設定
	void SetMisileParticleTime(LONGLONG _time) { OldParticleTime = _time; }

	/**
	* 撃ったオブジェクトがプレイヤーかを設定する関数です。
	*/
	void SetIsShotPlayer() { IsShotPlayer = true; }

	/**
	* 消去するかを返す
	* 
	* @return ミサイルとしての処理を終えたか
	*/
	bool GetMisileEnd() { return IsEnd; }

	/**
	* パーティクルのモデルを設定する関数です。
	* 
	* @param 設定するパーティクルのモデル
	*/
	void SetParticleModel(D3D::Model _model) { ParticleModel = _model; }

	/**
	* パーティクル配列の中身をクリアする
	*/
	void ClearPareticles() { Particles.clear(); }

	/**
	* 敵を追いかけるかの判定を設定する関数です。
	* 
	* @param 　敵を追いかけるか
	*/
	void SetIsMoveEnemy(bool _ismoveenemy) { IsMoveEnemy = _ismoveenemy; }

	/**
	* 敵を追いかけるかの判定を取得する関数です。
	* 
	* @return  敵を追いかけるかの判定
	*/
	bool GetIsMoveTarget() { return IsMoveEnemy; }

	/**
	* ミサイルが発射された時間を設定する
	*/
	void SetShotTime(ULONGLONG _time) { ShotTime = _time; }

	/**
	* ミサイルが当たった時にUIを表示するか
	*/
	void SetIsDrawHitUI(bool _isdrawui) { IsDrawHitUI = _isdrawui; }

	/**
	* ターゲットとの距離を返す
	* 
	* @return ターゲットとの距離
	*/
	float GetTargetDistance() { return TargetDistance; }

	/**
	* 加速度を返す
	*/
	float GetAddSpeed() { return AddSpeed; }
};