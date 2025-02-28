#pragma once
#include "direct3d.h"
#include<list>
#include <vector>
#include <memory>

class CEffect;
class CGameObject;
class CPlayer;
class CEnemy;
class CShottoBall;
class CMisile;

/**
 * これはCEnemyAttackクラスの説明です。
 * このクラスは敵が攻撃の際にするターゲットと弾の生成を行います。
 *
 * @author 吉村括哉
 * @since 2024-10-28
 */
class CEnemyAttack
{
public:
	/**
	* 攻撃の状態を示している列挙型です。
	* @param MISILE		ミサイル
	* @param GUN		機関銃
	* @param MAX		最大数
	*/
	enum ATTACK_ID
	{
		MISILE,
		GUN,
		MAX,
	};

private:
	//メンバー変数
	D3D::Model MisileModel;		//ミサイルのモデル
	D3D::Model MisileHitModel;	//ミサイルが当たった時のエフェクトモデル

	D3D::Model GunModel;		//弾のモデル
	D3D::Model GunHitModel;		//弾が当たった時のエフェクトモデル

	D3D::Model MisileParticleModel;	//ミサイルのパーティクルモデル

	D3D::Model MiniMapMisileModel;	//ミニマップのミサイルモデル

	//弾とミサイルのエフェクトのUVサイズ
	DirectX::XMFLOAT2 GunHitUVSize = { 0,0 };
	DirectX::XMFLOAT2 MisileHitUVSize = { 0,0 };

	//現在の攻撃兵器
	ATTACK_ID AttackId = MISILE;

	//弾を格納する配列
	//std::deque<CShottoBall*> vShottoBall;
	std::vector<CShottoBall*> vShottoBall;

	//ミサイルを格納する配列
	//std::deque<CMisile*> vMisile;
	//std::vector<CMisile*> Misiles;
	std::vector<std::unique_ptr<CMisile>> Misiles;

	//プレイヤーのロックオンUIを表示するか
	bool IsDrawPlayerRock = false;

	//プレイヤーとの距離が指定距離以内か
	bool IsClosePlayerDis = false;

	CEnemyAttack();
	~CEnemyAttack();

	/**
	* ミサイルの更新処理を行う関数です。
	*/
	void MisileUpdate();

	/**
	* 機関銃の更新処理を行う関数です。
	*/
	void GunUpdate();

	/**
	* ターゲットの更新処理を行う関数です。
	*/
	void TargetUpdate();

	/**
	* ミサイルの描画処理
	*/
	void MisileDraw();

	/**
	* ミサイルのパーティクル描画
	*/
	void MisileParticleDraw();

	/**
	* 弾の描画処理
	*/
	void GunDraw();

	/**
	*ミサイルの発射処理を行う関数です。
	*
	* @param _sppornobj		スポーンする際の基準のオブジェクト
	* @param _target		ターゲットのオブジェクト
	* @param _moveenemy		敵を追いかけるか
	*/
	CMisile* AttackMisile(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation ,CPlayer* _target, bool _moveenemy);

	/**
	* 機関銃の発射処理を行う処理
	* 
	* @param _sppornobj		スポーンする際の基準のオブジェクト
	*/
	void AttackShotBall(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation);

public:

	static CEnemyAttack* Get();

	/**
	* 更新処理を行う関数です。
	*/
	void Update();

	/**
	* 描画処理を行う関数です。
	*/
	void Draw();

	/**
	* ミニマップのミサイルを描画する関数です。
	*/
	void MiniMapMisileDraw();

	/**
	* プレイヤーの攻撃処理を行う関数です。
	*/
	CMisile* AttackTarget(ATTACK_ID _attackid , DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation);

	/**
	*ミサイルのモデルとUVサイズを設定する関数です。
	*
	* @param _misilemodel		設定したいミサイルのモデル
	* @param _misilhitemodel	設定したいミサイルのヒットエフェクトモデル
	* @param _minimapmisilemodel	設定したいミニマップモデル
	* @param _misilehituvsize	設定したいミサイルのエフェクトUVサイズ
	*/
	void SetMisileInf(D3D::Model _misilemodel, D3D::Model _misilhitemodel, D3D::Model _minimapmisilemodel, DirectX::XMFLOAT2 _misilehituvsize);

	/**
	*機関銃のモデルとUVサイズを設定する関数です。
	*
	* @param _misilemodel			設定したいミサイルのモデル
	* @param _misilhitemodel		設定したいミサイルのヒットエフェクトモデル
	*/
	void SetGunInf(D3D::Model _gunmodel, D3D::Model _gunhitmodel, DirectX::XMFLOAT2 _ballhituvsize);

	/**
	* ミサイルのパーティクルを設定する関数です。
	*
	* @param ミサイルのパーティクルモデル
	*/
	void SetMisileParticleModel(D3D::Model _model) { MisileParticleModel = _model; }

	/**
	* 弾の情報を取得する関数です。
	*/
	void GetShotBalls(std::list<CShottoBall*>& _shotballs);
};