#pragma once
#include <vector>
#include <list>
#include <deque>
#include <unordered_map>
#include <memory>
#include "direct3d.h"

class CEffect;
class CGameObject;
class CPlayer;
class CEnemy;
class CShottoBall;
class CMisile;
class CMisileUI;

/**
 * これはCPlayerAttackクラスの説明です。
 * このクラスはプレイヤーが攻撃の際にするターゲットと弾の生成を行います。
 *
 * @author 吉村括哉
 * @since 2024-10-23
 */
class CPlayerAttack
{
public:
	/**
	* 攻撃の状態を示している列挙型です。
	* @param MISILE		ミサイル
	* @param GUN		機関銃
	* @param FOURMISILE	4連ミサイル
	* @param MAX		最大数
	*/
	enum ATTACK_ID
	{
		MISILE,
		ONETARGET_FOURMISILE,
		FOURTARGET_FOURMISILE,
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

	//初期的用変数
	CEnemy* pInitEnemy;

	//弾とミサイルのエフェクトのUVサイズ
	DirectX::XMFLOAT2 GunHitUVSize = { 0,0 };
	DirectX::XMFLOAT2 MisileHitUVSize = { 0,0 };

	//現在の攻撃兵器
	ATTACK_ID AttackId = MISILE;

	//射撃ポイント
	DirectX::XMFLOAT3 ShotPos = { 0.0f,0.0f,0.0f };

	//射撃ポイントを表示するか
	bool IsShotPos = false;

	//攻撃キーが押されたままの状態か
	bool IsPressAttackKey = false;

	//ターゲット方法の切り替え(true:通常のターゲット false:画面に映っている敵全てをターゲットする)
	bool IsOneTarget = true;

	//敵変数用ブレイクするか
	bool IsBreak = false;

	//初期化用ターゲットを設定したか
	bool IsSetTarget = false;

	//作成したターゲットUIのID
	int EnemyFrame = 0;

	bool CreateEnemyFrame = false;

	//ロックオンSEを鳴らしているか
	bool IsPlayRockOnSE = false;

	//ミサイルを格納する配列
	std::vector<std::unique_ptr<CMisile>> vuMisile;

	//優先度について
	// ターゲット機能は、優先度が高いものから候補に入れていく
	// 0:優先度無し(初期値)
	// 1:カメラに映っていなく、ターゲット可能距離以上
	// 2:カメラに映っていないが、ターゲット距離距離以内
	// 3:カメラに映っているかつ、ターゲット距離以上
	// 4:カメラに映っているかつ、ターゲット距離以内
	// 5:中心カメラに写っているかつターゲット距離以内
	// 5:プレイヤーとの距離が基準距離より近い
	//
	/**
	* 敵候補を入れる際に使用する構造体
	*
	* @param Enemy		敵
	* @param FastPoint	優先度(0:なし,1 < 2 < 3 < 4 < 5 : 5が一番優先度が高い)
	*/
	struct RESENEMY
	{
		CEnemy* Enemy = nullptr;
		int FastPoint = 0;
	};
	//敵を格納する変数
	//std::vector<std::unique_ptr<RESENEMY>> vEnemys;
	std::vector<RESENEMY*> vEnemys;

	//敵の最高スピード
	float EnemyMaxSpeed = 0.4f;
	float EnemyTargetMaxSpeed = 0.2f;

	//敵の最大回転可能角度
	float EnemysMaxRotation = 1.5f;

	/**
	* ターゲットの情報をいれた構造体
	*/
	std::vector<CEnemy*> lTargetEnemys;

	//ターゲット数
	int TargetNum = 0;

	//ボス取り巻き敵用守護敵数
	int EntourageDefenceNum = 0;

	//取り巻き敵の攻撃中敵数
	int EntourageAttackNum = 0;

	//機関銃敵の全敵移動ポイント到達判定
	bool IsGunEnemysPass = false;

	//機関銃敵の移動先保存配列
	std::deque<DirectX::XMFLOAT3> ResArray;

	/**
	* 敵パラメータを入れる構造体です。
	*/
	struct ENEMY_PARAMETER
	{
		float ThresHold;            //到達する許容範囲
		float MaxSpeed;				//最高速度
		float Fright_MaxSpeed;		//飛行敵の最高速度
		float Follow_MaxSpeed;		//追従敵の最高速度
		float Entourage_MaxSpeed;	//取り巻き敵の最高速度
		float Boss_MaxSpeed;		//ボス敵の最高速度
		float AddSpeed;             //加速速度
		float Fright_AddSpeed;		//飛行敵の加速速度
		float Follow_AddSpeed;		//追従敵の加速速度
		float Entourage_AddSpeed;   //取り巻き敵の加速速度
		float Boss_AddSpeed;        //ボス敵の加速速度
		float Fright_MaxRotate;		//飛行敵の最高回転角度
		float Follow_MaxRotate;		//飛行敵の最高回転角度
		float Entourage_MaxRotate;	//飛行敵の最高回転角度
		float Boss_MaxRotate;		//飛行敵の最高回転角度
		int FrightEnemy_HP;         //飛ぶ敵のHP
		int GroundEnemy_HP;         //地面敵のHP
		int EntourageEnemy_HP;		//取り巻き敵のHP
		int BossEnemy_HP;			//ボス的のHP
		float GroundEnemy_AttackTime;   //地面敵の攻撃間隔
		float Max_Attack_Distance;  //最大攻撃可能距離
		float Target_Time;          //ターゲット完了時間
		float Attack_CoolTime;      //攻撃間隔
		int InsertEntourageNum;     //取り巻きの召喚数
		float Entourage_ChengePos_Time;    //取り巻きの座標変更間隔
		float Entourage_Attack_CoolTime;   //取り巻きの攻撃間隔
		float Entourage_Move_Pos_Dis;	   //取り巻きの移動間隔
	};
	ENEMY_PARAMETER EnemyParameter;

	enum ENEMY_PARAMS_TAG
	{
		HOLD,
		ENEMYMAXSPEED,
		FRIGHT_MAX_SPEED,
		FOLLOW_MAX_SPEED,
		ENTOURAGE_MAX_SPEED,
		BOSS_MAX_SPEED,
		ADDSPEED,
		FRIGHTADDSPEED,
		FOLLOWADDSPEED,
		ENTOURAGEADDSPEED,
		BOSSADDSPEED,
		FRIGHTMAXROTATE,
		FOLLOWMAXROTATE,
		ENTOURAGEMAXROTATE,
		BOSSMAXROTATE,
		FRIGHTENEMYHP,
		GROUNDENEMYHP,
		ENTOURAGEENEMYHO,
		BOSSENEMYHP,
		GROUNDENEMY_ATTACKTIME,
		MAXATTACKDISTANCE,
		TARGETTIME,
		ATTACKCOOLTIME,
		INSERTENtoURAGENUM,
		ENTOURAGECHANGEPOSTIME,
		ENTOURAGEATTACKCOOLTIME,
		ENTOURAGEMOVEPOSDIS,
		ENEMYMAX,
	};
	ENEMY_PARAMS_TAG EnemyParamTag = HOLD;

	/**
	* ミサイルパラメータを入れる構造体です。
	*/
	struct MISILE_PARAMETER
	{
		float MaxRotate;			//プレイヤーミサイルの最低角度
		float MinRotate;			//プレイヤーミサイルの最高角度
		float Enemy_Min_Rotate;		//敵ミサイルの最低角度
		float Enemy_Max_Rotate;		//敵ミサイルの最高角度
		float Min_Speed;			//ミサイルの低基準速度
		float Max_Speed;			//最高速度
		float Min_Add_Speed;		//基準値以下速度時の加速度
		float Max_Add_Speed;		//基準値以上速度時の加速度
		float Enemy_Min_Add_Speed;	//敵基準値以下速度時の加速度
		float Enemy_Max_Add_Speed;	//敵基準値以上速度時の加速度
		int	  Delete_Misile_Time;	//ミサイルの生存時間
		float Particle_Cooltime;	//パーティクルの出現間隔
		float Particle_OutTime;		//パーティクルの消える速度
		float Particle_AddScale;	//パーティクルの大きくなる速度
		float Misile_Charge_Speed;	//ミサイルのチャージ速度
	};
	MISILE_PARAMETER MisileParameter;

	enum MISILE_PARAMS_TAG
	{
		MAXROTATE,
		MINROTATE,
		ENEMYMINROTATE,
		ENEMYMAXROTATE,
		MINSPEED,
		MISILEMAXSPEED,
		MINADDSPEED,
		MAXADDSPEED,
		ENEMYMINADDSPEED,
		ENEMYMAXADDSPEED,
		DELETEMISILETIME,
		PARTICLECOOLTIME,
		PARTICLEOUTTIME,
		PARTICLEADDSCALE,
		MISILECHARGESPEED,
		MISILEMAX,
	};
	MISILE_PARAMS_TAG MisileParamTag = MAXROTATE;

	/**
	* 敵パラメーターをファイルから取得する関数です。
	*/
	void InsertEnemyParameter();

	/**
	* ミサイルパラメーターをファイルから取得する関数です。
	*/
	void InsertMisileParameter();

	//CEnemy* TargetEnemy;

	CPlayerAttack();
	~CPlayerAttack();

	/**
	* 敵のラインを引く関数です。
	*/
	void DrawTargetLine();

	/**
	* ミサイルの更新処理を行う関数です。
	*/
	void MisileUpdate();

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
	*ミサイルの発射処理を行う関数です。
	*
	* @param _sppornobj		スポーンする際の基準のオブジェクト
	* @param _target		ターゲットのオブジェクト
	* @param _moveenemy		敵を追いかけるか
	*/
	void AttackMisile(CGameObject* _spornobj, CEnemy* _target,bool _moveenemy);

	/**
	*4つのミサイルの発射処理を行う関数です。
	*
	* @param _sppornobj		スポーンする際の基準のオブジェクト
	* @param _target		ターゲットのオブジェクト
	* @param _misilenum		ミサイルの順番
	* @param _moveenemy		敵を追いかけるか
	*/
	void AttackFourMisile(CGameObject* _spornobj, CEnemy* _target, int _misilenum, bool _moveenemy,bool _checkmisileui);

public:
	static CPlayerAttack* Get();

	//パラメータ//
	enum GUNENEMYSTARTPOS
	{
		VerTex1,
		VerTex2,
		VerTex3,
		VerTex4,
		VerTex5,
		MAXVerTex,
		NoneVerTex,
	};
	GUNENEMYSTARTPOS GunENemyStartPos = VerTex1;

	GUNENEMYSTARTPOS GetGunENemyStartPos();
	void AddGunEnemyStartPos();

	/**
	* 更新処理を行う関数です。
	*/
	void Update();

	/**
	* 描画処理を行う関数です。
	*/
	void Draw();

	/**
	* 敵の更新処理を行う関数です。
	*/
	void EnemyUpdates();

	/**
	* 敵の描画処理を行う関数です。
	*/
	void EnemysDraw();

	/**
	* ミニマップの敵を描画する関数です
	*/
	void MiniMapEnemyDraw();

	/**
	* ミニマップのミサイルを描画する関数です。
	*/
	void MiniMapMisileDraw();

	/**
	* プレイヤーの攻撃処理を行う関数です。
	*/
	void AttackTarget(CGameObject* _spornobj);

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
	* ミサイルのパーティクルを設定する関数です。
	* 
	* @param ミサイルのパーティクルモデル
	*/
	void SetMisileParticleModel(D3D::Model _model) { MisileParticleModel = _model; }

	/**
	*機関銃の玉のモデルとUVサイズを設定する関数です。
	*
	* @param _misilemodel		設定したいミサイルのモデル
	* @param _misilhitemodel	設定したいミサイルのヒットエフェクトモデル
	* @param _misilehituvsize	設定したいミサイルのエフェクトUVサイズ
	*/
	void SetBallInf(D3D::Model _ballmodel, D3D::Model _ballhitemodel, DirectX::XMFLOAT2 _ballhituvsize);

	/**
	* 敵の情報をCGameクラスから取得する関数です。
	*/
	void InsertEnemys(CEnemy* _enemy);

	/**
	* 敵が格納されている配列をクリアする
	*/
	void ClearEnemys();

	/**
	* ターゲット情報を更新する
	* 
	* @param ターゲット時間をリセットするか
	*/
	void SetTarget(bool _resettime);

	/**
	* 条件に合う敵をターゲット候補に格納する関数です
	*/
	void InsertResEnemys();
	void InsertResEnemys(std::vector<RESENEMY*>& _enemys);
	//void InsertResEnemys(std::vector<std::unique_ptr<RESENEMY>>& _enemys);

	/**
	* ターゲットを返します。
	* 
	* @return ターゲットオブジェクト
	*/
	CEnemy* GetTargetObj() {
		if (lTargetEnemys.empty())
		{
			return pInitEnemy;
		}
		else
		{
			return (*lTargetEnemys.begin());
		}

		//if (lTargetEnemys.empty()) { return pInitEnemy; }
		//else { return lTargetEnemys.begin()->Enemy;}
	}

	/**
	* すべての配列を初期化する関数です
	*/
	void ResetPlayerAttack();

	/**
	* 格納されている敵の数を取得する関数です。
	* 
	* @return 格納されている敵の数
	*/
	size_t GetEnemyNum() { return vEnemys.size(); }

	/**
	* 攻撃兵器の種類変更
	*/
	void ChengeAttackweapon();

	/**
	* 攻撃キーが押されたままの状態かを取得する関数です。
	* 
	* @param 攻撃キーが押されたままかの判定
	*/
	void SetIsPressAttackKey(bool _ispressattackkey) { IsPressAttackKey = _ispressattackkey; }

	/**
	* ミサイルパーティクル配列の中身をクリアにする
	*/
	void ClearMisileParticles();

	/**
	* 現在の武装の攻撃可能距離を返す
	* 
	* @return 現在の武装の攻撃可能距離
	*/
	float GetWeaponMaxDistance();

	/**
	* 現在の武装の種類を返す
	* 
	* @return  現在の武装の種類
	*/
	ATTACK_ID GetAttackId() { return AttackId; }

	/**
	* ターゲット可能数が一体だけかを返す
	*/
	bool GetIsOnetarget() { return IsOneTarget; }

	/**
	* ボスオブジェクトを返す関数です。
	*/
	CEnemy* GetBossEnemy();

	/**
	* 取り巻き敵の攻撃中の数設定
	*/
	void SetEntourageAddNum(int _addnum) { EntourageAttackNum += _addnum; }

	/**
	* 取り巻きオブジェクトの攻撃指示が出されている敵を確認
	*/
	int GetEntourageAttackNum() { return EntourageAttackNum; }

	/**
	* 取り巻き敵の防衛の数
	*/
	void SetEntourageDefAddNum(int _addnum) { EntourageDefenceNum += _addnum; }

	/**
	* 取り巻き敵の防衛状態の数を返す
	* 
	* @return 防衛状態の取り巻きの数
	*/
	int GetEntourageDefNum() { return EntourageDefenceNum; }

	/**
	* 取り巻きオブジェクトの攻撃指示
	*/
	void SetAttackEntourageEnemy();

	/**
	* 取り巻きオブジェクトの防衛指示設定関数です。
	*/
	void SetDefenceEntourageEnemy(bool _isdefence);

	/**
	* 敵変数でブレイクする
	*/
	void SetTrueBreak() { IsBreak = true; }

	/**
	* 敵の最大回転角度を取得する関数です。
	*
	*  @return 敵の最大回転角度
	*/
	float& GetEnemyMaxRotation() { return EnemysMaxRotation; }

	/**
	* 機関銃敵の構造体を返す関数です。
	*/
	void GetGunEnemyFlag(std::deque<DirectX::XMFLOAT3>& _qgunflagarray);

	/**
	* 機関銃敵の構造体の中身をずらす
	*/
	void ShiftGunEnemyArray();

	/**
	* 機関銃敵全てが移動ポイントに到達したかを返す
	*/
	bool GetGunEnemysPass() { return IsGunEnemysPass; }

	/**
	* 機関銃の敵が全て移動完了しているか
	*/
	void CheckGunEnemysPass();

	/**
	* ミサイルのパラメータの構造体を渡す関数です。
	* 
	* @return ミサイルのパラメーター構造体
	*/
	MISILE_PARAMETER& GetMisileParameter() { return MisileParameter; }

	/**
	* 敵のパラメータの構造体を渡す関数です。
	*
	* @return 敵のパラメーター構造体
	*/
	ENEMY_PARAMETER& GetEnemyParameter() { return EnemyParameter; }

	/**
	* 敵のすべての移動先を変更する(デバッグ用)
	*/
	void ChengeEnemysMovePos();

	/**
	* 敵の情報を配列に入れて返す関数(デバッグ用)
	* 
	* @param  敵の情報を入れる配列
	*/
	void GetEnemys(std::list<CEnemy*>& _enemys);

	/**
	* ターゲット情報を入れて返す関数(デバッグ用)
	* 
	* @param  ターゲットの情報を入れる配列
	*/
	void GetTargets(std::list<CEnemy*>& _targets);

	/**
	* パラメータをセーブする関数です。
	*/
	void SaveEnemyParameter();

	/**
	* パラメータをセーブする関数です。
	*/
	void SaveMisileParameter();
};

