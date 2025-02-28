#pragma once
#include"direct3d.h"
#include <DirectXCollision.h>
#include <xinput.h>
#include <unordered_map>
#include <list>
#include <deque>
#include "string"
#include <memory>
#pragma comment (lib, "xinput.lib")
class CCamera;
class CEffect;
class CEnemy;

/**
 * これはCGameObjectクラスの説明です。
 * このクラスはゲームオブジェクト制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CGameObject
{
private:


protected:
	//ミニマップ用オブジェクト
	CGameObject* pMinimapObject;

	//ミニマップ更新を行うか
	bool IsUpdateMiniMap = false;

	//このオブジェクトのミニマップモデルを描画するか
	bool IsDrawMiniMapModel = false;

	//ミサイルUI用変数
	ULONGLONG MisileShotTime = 0;

	//当たり判定のオブジェクト
	CGameObject* ColliderObject;

	//オブジェクト当たり判定
	DirectX::BoundingBox ColliderBoxAABB;
	DirectX::BoundingOrientedBox ColliderBoxOBB;

	//当たり判定の種類がAABBか
	bool IsColAABB = true;

	//当たり判定をいれた配列
	static std::unordered_map<int, std::list<DirectX::BoundingBox>> mColliderBoxsAABB;

	//メンバー変数
	D3D::Model model;	//描画するべきモデルデータ
	std::wstring ModelName;	//描画するモデルデータの名前

	D3D::Model NoiseModel;

	//前向きベクトル = キャラが向いている方向を表す矢印
	DirectX::XMFLOAT3 Forward = { 0,0,1 };

	//上向きベクトル = キャラの上方向を表す矢印
	DirectX::XMFLOAT3 UP = { 0,1,0 };

	//上向きベクトル = キャラの上方向を表す矢印
	DirectX::XMFLOAT3 RIGHT = { 1,0,0 };

	//プレイヤーの各方向ベクトル作成
	DirectX::XMVECTOR newForward = { 0,0,1,0 };
	DirectX::XMVECTOR newUP = { 0,1,0,0 };
	DirectX::XMVECTOR newRIGHT = { 1,0,0,0 };

	//合成用行列
	DirectX::XMMATRIX RotateMatrix;

	//オブジェクトの体力
	int Hp = 0;

	//移動速度を持つ変数
	float MoveSpeed = 0.0f;

	//右移動スピード
	float RightMoveSpeed = 0.0f;

	//ミニマップで描画するか
	bool IsDrawMiniMap = false;

	// ビルボードかどうかを持つ変数
	bool isBillboard = false;

	//描画可能かを表す変数
	bool isActive = true;

	//ライティングを行うかを表す変数
	bool isLighting = true;

	//影を描画するかを表す変数
	bool IsShadow = false;

	//プレイヤーが入力したか
	bool IsInput = false;

	//常に地面と平行になるようにするかの判定
	bool IsAutoPilot = false;

	//急旋回モードかどうか
	//急旋回モードについて
	//急旋回中では、空側へのX軸回転は速度が落ち、
	//地面側へのX軸回転は速度が上がる
	//それと回転角度がこの間は2倍になる
	bool IsVeryRotate = false;

	//加速したか
	bool IsFastSpeed = false;

	//オブジェクトがカメラに映っているか
	bool IsRefMainCamera = false;

	//オブジェクトが中心カメラに写っているか
	bool IsRefCenterCamera = false;

	//UIか
	bool IsUi = false;

	//攻撃されているか
	bool IsAttacked = false;

	//プレスUI用Fade変数
	bool IsFadeOut = false;

	//ミサイルヒットUI用セット時間
	ULONGLONG SetTime = 0;

	//UV座標を設定する関数
	virtual DirectX::XMMATRIX GetUvMatrix();

	DWORD DwResult = ERROR_DEVICE_NOT_CONNECTED;	//コントローラーの接続確認に使う
	XINPUT_STATE State;			//現在のコントローラーの状態管理

	//コントローラーの初期化
	//十字ボタン
	int iPad_left = 0, iPad_right = 0, iPad_up = 0, iPad_down = 0;
	//LRトリガーボタン
	int iPad_leftshoulder = 0, iPad_rightshoulder = 0;
	//LRスティック押し込みボタン
	int iPad_leftthumb = 0, iPad_rightthumb = 0;
	//A,B,X,Yボタン
	int iPad_A = 0, iPad_B = 0, iPad_X = 0, iPad_Y = 0;
	//メニューボタン,ビューボタン
	int iPad_Start = 0, iPad_Back = 0;
	//左スティック
	int iPad_LStickX = 0, iPad_LStickY = 0;
	int iPad_RStickX = 0, iPad_RStickY = 0;

	//コントローラートリガー用変数
	int OldAKey = 0;		//A
	int OldBKey = 0;		//B
	int OldXKey = 0;		//X
	int OldYKey = 0;		//Y
	int OldRightKey = 0;	//RIGHT
	int OldLeftKey = 0;		//LEFT
	int OldUpKey = 0;		//UP
	int OldDownKey = 0;		//DOWN
	int OldStartKey = 0;	//START
	int OldBackKey = 0;	//BACK
	int OldRightThumb = 0;
	int OldLeftThumb = 0;
	int LeftThumbLX = 0;	//
	int LeftThumbLY = 0;	//

	//視錐台構造体
	struct Plane {
		DirectX::XMVECTOR normal;
		float distance;
	};

	///**
	//* プレイヤー用ターゲットラインを引く
	//*/
	//void DrawPlayerTargetLine();

public:
	enum TAG
	{
		NONE,				//何もない
		BACK_SKY_FLAT,		//タイトルリザルトの背景の空
		BACK_SKY_CIRCLE,	//ゲーム背景の空
		GROUND,				//地面
		PLAYER,				//プレイヤー
		VECTOR,				//矢印
		INITENEMY,			//初期敵
		FRIGHT_ENEMY,		//飛行敵
		GROUND_ENEMY,		//地面敵
		GUN_ENEMY,			//機関銃敵
		FOLLOW_ENEMY,		//追従敵
		ENTOURAGE_ENEMY,	//取り巻き敵
		BOSS_ENEMY,			//ボス敵
		MISILE,				//ミサイル
		BACKFIRE,			//バックファイア
		MISILE_SMOKE,		//ミサイルの煙
		GUN,				//機関銃
		OBJECT,				//障害物
		CLOUD,				//雲
		SUN,				//太陽
		COLLIDER,			//当たり判定
		CARSOL,				//カーソル
		UI,					//UI
		PRESS_UI,			//プレスUI
		TARGET_UI,			//敵をターゲットするときに移動するUI
		CENTER_UI,			//画面中央UI
		CAVEAT_UI,			//警告UI
		ATTACKINF_UI,		//攻撃情報
		MISILE_UI,			//ミサイルの弾を表すUI
		HITMISILE_UI,		//ミサイルヒットを表すUI
		PLAYER_UI,			//プレイヤーの状態を表すUI
		BOSSNAME_UI,		//ボスの名前をだすUI
		BOSSHP_UI,			//ボスのHPを出すUI
		MINIMAP_UI,			//ミニマップ上のUIを表す
		SPEED_NUM,			//スピードを表す数字
		ALT_NUM,			//高度を表す数字
		WAIT_UI,			//一旦停止UI
		MAX,				//最大数
	};

	enum BOTTUN_NAME
	{
		NO_Key,
		CONTROLLER_A,
		CONTROLLER_B,
		CONTROLLER_X,
		CONTROLLER_Y,
		CONTROLLER_RIGHT,
		CONTROLLER_LEFT,
		CONTROLLER_UP,
		CONTROLLER_DOWN,
		CONTROLLER_RB,
		CONTROLLER_LB,
		CONTROLLER_RT,
		CONTROLLER_LT,
		CONTROLLER_MENU,
		CONTROLLER_BACK,
		CONTROLLER_RIGHT_THUMB,
		CONTROLLER_LEFT_THUMB,
	};


	protected:
		BOTTUN_NAME BottunName = NO_Key;
		TAG Tag = NONE;

	public:

	CGameObject();
	~CGameObject();

	//機能
	//更新関数
	virtual void Update();

	//移動処理
	virtual void MoveUpdate();

	//入力されたベクトルの方向に移動する
	virtual void MoveVectorUpdate(DirectX::XMFLOAT3 _movevector);

	///**
	//* ターゲットしたときに移動するターゲットUIの更新処理
	//*/
	//void MoveTargetUpdate();

	/**
	* ミニマップでのサイズ更新処理
	*/
	void MiniMapObjectUpdate();

	//現在の角方向ベクトルを取得する
	DirectX::XMMATRIX GetIsMatrix();

	//回転行列設定
	void SetIsMatrix(DirectX::XMMATRIX _matriox) { RotateMatrix = _matriox; }

	//初期の回転行列設定
	void SetInitRotateMatrix();

	/**
	* 現在の回転行列を取得
	* 
	* @return オブジェクトの回転行列
	*/
	DirectX::XMMATRIX GetRotateMatrix() { return RotateMatrix; }

	//現在のオブジェクトの角度から見た軸を計算
	void SetVector();

	//描画関数
	virtual void Draw();

	/**
	* ミニマップのオブジェクトを描画
	*/
	void DrawMiniMapObject();

	/**
	* 当たり判定のオブジェクトを描画
	*/
	void DrawColliderObject();

	/**
	* ミニマップオブジェクトの設定を行う関数です。
	*/
	void SetMiniMapModel(D3D::Model _model);

	/**
	* HPを減らす関数です
	* @param 現在のHPから減らしたい値
	*/
	void SetDamege(int _num) { Hp -= _num; }

	/**
	* 現在のHPを返す関数です
	* @return 現在のHPを返します。
	*/
	int GetHp() { return Hp; }

	/**
	* 加速したかを返します。
	* 
	* @return 加速したか
	*/
	bool GetIsFastSpeed() { return IsFastSpeed; }

	/**
	* ミニマップのオブジェクトとして描画するように変更する関数です。
	*/
	void SetIsMiniMap() { IsDrawMiniMap = true; }

	//タグの設定
	void SetTag(TAG _tag) { Tag = _tag; }

	//タグ取得
	TAG GetTag() { return Tag; }

	//モデルにセットする関数
	void SetModel(D3D::Model& _model);

	//モデルを取得
	D3D::Model& GetModel() { return model; }

	/**
	* モデルの名前を設定
	* 
	* @param  設定したいモデルの名前
	*/
	void SetModelName(std::wstring _modelname) { ModelName = _modelname; }

	/**
	* モデルの名前を取得する
	* 
	* @return 取得したいモデルの名前
	*/
	std::wstring GetModelName() { return ModelName; }

	//当たり判定を取得(AABBとOBBの二つがある)
	DirectX::BoundingBox GetColliderObjectAABB(){ return ColliderBoxAABB; }
	DirectX::BoundingOrientedBox GetColliderObjectOBB() { return ColliderBoxOBB; }

	/**
	* 当たり判定を作成する関数です。(AABBとOBBの二つ)
	*/
	void SetColliderBoxAABB();
	void SetColliderBoxOBB();
	void SetColliderBoxAABB(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scaling);
	void SetColliderBoxOBB(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scaling);

	/**
	* オブジェクトとの距離を計算する関数
	* 
	* @param  距離を測りたいオブジェクト
	* @return 引数に設定したオブジェクトとの距離
	*/
	float CheckDistance(const CGameObject* _obj,const DirectX::XMFLOAT3& _pos) const;

	//このGameObjectの仮想世界での位置(ワールド座標)
	DirectX::XMFLOAT3 Position = {};

	//1フレーム前の座標
	DirectX::XMFLOAT3 OldPosition = {};

	//オイラー角
	DirectX::XMFLOAT3 Rotation = {};

	//クォータニオン角
	DirectX::XMFLOAT3 QuaternionRotation = {};

	//移動ベクトル
	DirectX::XMFLOAT3 MoveVelocity = {};

	//拡大縮小率
	DirectX::XMFLOAT3 Scaling = { 1,1,1 };

		//このキャラクターのマテリアル色
	DirectX::XMFLOAT4 materialDiffuse = { 1,1,1,1 };

	//移動スピードを設定する関数
	void SetMoveSpeed(float _speed) { MoveSpeed = _speed; }

	//移動スピードを加速させる
	void SetAddMoveSpeed(float _speed) { MoveSpeed += _speed; }

	//移動スピードを取得する
	float GetMoveSpeed() { return MoveSpeed; }

	//前向きベクトルを返す
	DirectX::XMFLOAT3 GetForward() { return Forward; }

	//上向きベクトルを返す
	DirectX::XMFLOAT3 GetUp() { return UP; }

	//右向きベクトルを返す
	DirectX::XMFLOAT3 GetRight() { return RIGHT; }

	//オブジェクトの各向きベクトルを返す
	DirectX::XMVECTOR& GetNewForward() { return newForward; }
	DirectX::XMVECTOR& GetNewUp() { return newUP; }
	DirectX::XMVECTOR& GetNewRight() { return newRIGHT; }

	// ビルボードかどうかを設定する関数
	void SetBillboard(bool isBillboard);
	/**
	* ビルボードを取得する
	*/
	bool GetIsbillBord() { return isBillboard; }

	/**
	* UIかを設定する
	*/
	void SetIsUI(bool _isui) { IsUi = _isui; }

	/**
	* UIかを取得する
	*/
	bool GetIsUI() { return IsUi; }

	void SetActive(bool _Active) { isActive = _Active; }
	bool GetActive() { return isActive; }


	void SetLighting(bool bLighting) { isLighting = bLighting; }

	/**
	* プレイヤーに攻撃されているか(ミサイルに追われているか)
	*
	* @param  プレイヤーの攻撃に追われているか
	*/
	void SetIsAttacked(bool _isatacked) { IsAttacked = _isatacked; }

	/**
	* 攻撃されているか(ミサイルに追われているか)の変数を取得する関数です。
	* 
	* @param  ミサイルにターゲットされているかを返します。
	*/
	bool GetIsAttacked() { return IsAttacked; }

	/**
	* ライティングするかを取得する
	*/
	bool GetIsLighting() { return isLighting; }

	void SetShadow(bool _isshadow);

	//void GetCameraTexture(CCamera* _camera);

	////入力判定
	//DirectX::XMFLOAT3 CheckInput(DirectX::XMFLOAT3 _angle);

	//入力押し込み判定
	bool PadInputTrigget(int _num);

	//回転行列からオイラー角度を取得
	DirectX::XMFLOAT3 ExtractRotationFromMatrix(const DirectX::XMMATRIX& matrix);

	//コントローラーの入力
	void DoControllerEvent();

	//コントローターのトリガーチェック
	bool CheckControllerTrigger(BOTTUN_NAME _buttonname);

	/**
	* コントローラーのボタンが押されているか確認する関数です。
	* 
	* @param  確認したいボタンの名前
	* @return ボタンが押されているか
	*/
	bool CheckControllerPress(BOTTUN_NAME _buttonname);

	//コントローラーが接続されているか
	void GetControllerState();

	//オブジェクトを映しているカメラを取得
	bool GetIsCameraRef();

	//オブジェクトを映している中心カメラを取得
	bool GetIsCenterCameraRef();

	//カメラに映っているか判定
	void ChackRefCamera();

	//正規化
	void NormalizePlane(Plane& plane);

	//当たり判定オブジェクトを設定
	void SetColliderObject(CGameObject* _colliderobject);

	/**
	* 当たり判定のモデルを設定
	* 
	* @param 設定したい当たり判定のモデル
	*/
	void SetColliderModel(D3D::Model _model) { ColliderObject->SetModel(_model); }

	//カリングを無効にする
	void SetCullingMode(bool _cullmode);

	/**
	* リニア補完を有効にする
	*/
	void SetLinear();

	/**
	* シェーダーを変更する
	*/
	void ChengeShader(const int& _id);

	/**
	* 当たり判定の共有変数をクリアする
	*/
	void ClearColliderAABBBox();

	/**
	* ミニマップ描画用のオブジェクトを取得する関数です。
	* 
	* @return  ミニマップ描画用オブジェクト
	*/
	CGameObject* GetMiniMapObject() { return pMinimapObject; }

	/**
	* 当たり判定がAABB化を取得する関数です。
	*/
	bool GetIsColAABB() { return IsColAABB; }

	/**
	* ミニマップ更新を行うかの設定
	* 
	* @param  ミニマップ更新を行うか
	*/
	void SetIsMiniMapUpdate(bool _isminimapupdate) { IsUpdateMiniMap = _isminimapupdate; }

	/**
	* ミサイルヒット用の時間設定
	*/
	void SetMisileHitSetTime(ULONGLONG _settime) { SetTime  = _settime; }

	/**
	* ミサイルを発射した時間を設定する関数です。
	*/
	void SetMisileShotTime(ULONGLONG _shottime) { MisileShotTime = _shottime; }
};

