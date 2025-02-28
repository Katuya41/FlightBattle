#pragma once
#include "direct3d.h"
#include <DirectXCollision.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <deque>
#include <memory>

//クラス型の前方宣言
//お互いにインクルードしあう状況をなくすのに使う
//そのクラス型のポインタ変数を宣言したい場合に使う
class CGameObject;
class CPlayer;
class CCamera; 
class CEffect;
class CEnemy;
class CVector;

/**
 * これはCGameクラスの説明です。
 * このクラスはゲーム制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CGame
{
private:
	/**
	* メニュー時の状態を示す列挙型です
	*/
	enum MAINMENU
	{
		MMENU_NONE,
		MMENU_GAMESTART,
		MMENU_QUITGAME,
		MMENU_MAX,
	};
	MAINMENU MainMenu = MMENU_GAMESTART;

	/**
	* メインメニューでゲームスタートを選択した時の列挙型です。
	*/
	enum GAMESTAGE
	{
		GMENU_NONE,
		GMENU_STAGE1,
		GMENU_STAGE2,
		GMENU_STAGE3,
		GMENU_STAGE4,
		GMENU_MAX,
	};
	GAMESTAGE GameStage = GMENU_NONE;

	//カメラ用古いプレイヤー座標
	//配列にプレイヤーの正面方向を格納していく
	//指定のフレーム経った後に、順番に配列から抜いていく
	//DirectX::XMFLOAT3 CameraOldPos[3];
	std::deque<DirectX::XMFLOAT3> NewCameraPos;
	bool InsertNewCamFocus = false;

	//ゲームが終了するかの処理
	bool IsGameEnd = false;

	//ゲームが開始するかの変数
	bool IsStartGame = false;

	//ステージカウントダウン
	bool IsStartCountDown = false;

	//ゲーム終了を選択したか
	bool IsCheckGamequit = false;

	//メインメニューで選択したか
	bool IsCheckMainMenu = false;

	//ゲームステージメニューで選択したか
	bool IsCheckGameStage = false;

	//プレイヤースピード測定用
	int MoveSpeedNum = 0;

	//プレイヤー高度測定用
	int AltitudeNum = 0;

	//ステージからメニューに戻る変数
	bool BackStage = false;

	//雲を出現させるための一時変数
	ULONGLONG CloudOldTime = 0;

	//開始カウントダウンのSE用変数
	ULONGLONG CountDowntime = 0;
	int Cnt = 0;

	//プロジェクト開始時刻
	ULONGLONG StartProjTime = 0;
	//プロジェクト経過時間
	int NowMoveTime = 0;

	//歪みの強さ
	float Dist = 0.01f;

	DirectX::XMFLOAT3 SunPosition = { 0.0f,60.0f,0.0f };

public:
	float& GetDist() { return Dist; }

	bool GetIsGAmeEnd() { return IsGameEnd; }

	void SetSunPosition(DirectX::XMFLOAT3 _sunpos) { SunPosition = _sunpos; }

	DirectX::XMFLOAT3& GetSunPosition() { return SunPosition; }

public:
	// カメラの初期位置
	const DirectX::XMFLOAT3 iniCameraPos = { 0,1,-2 };
	// カメラ位置を持つ変数
	DirectX::XMFLOAT3 cameraPos = { 0.0f,0.0f,0.0f };
	// カメラの回転角度
	DirectX::XMFLOAT3 cameraRotate = { 0,0,0 };

	DirectX::XMFLOAT3 MiniMapPos = { 0,0,0 };

	// カメラ位置を持つ変数
	DirectX::XMFLOAT3 DebugCameraPos = { 0.0f,3.0f,0.0f };
	// カメラ位置を持つ変数
	DirectX::XMFLOAT3 DebugCameraUp = { 0.0f,1.0f,0.0f };
	// カメラの回転角度
	DirectX::XMFLOAT3 DebugCameraFocus = { 0.0f,3.0f,10.0f };

	//ゲームカメラの設定
	enum CAMERA_ID
	{
		Third,
		First,
	};
	CAMERA_ID CameraId = First;

private:
	enum FADE
	{
		NO_FADE,
		FADE_IN,
		FADE_OUT,
	};
	FADE Fade = NO_FADE;

	//フェードパネル
	CGameObject* pFadePanel = nullptr;
	CGameObject* pWaitPanel = nullptr;

	//std::unique_ptr<CGameObject> pFadePanel;
	//std::unique_ptr<CGameObject> pWaitPanel;

	//UI
	CGameObject* pShotPosUI = nullptr;		//銃弾照射予定
	//スタートカウントダウン
	CEffect* pCountDownUI = nullptr;
	//スタートルール
	CGameObject* RuleUI = nullptr;

	ULONGLONG CameraShakeStartTime = 0;	//カメラが揺れ始める開始時間
	bool IsDamegeShakeCamera = false;		//カメラが揺れるかの判定
	bool IsDamegeShakeright = true;			//カメラが揺れるときの次の揺れる方向

	bool IsSpeedShakeCamera = false;		//カメラが揺れるかの判定
	bool IsSpeedShakeright = true;		//カメラが揺れるときの次の揺れる方向

	//カメラ変数
	CCamera* camera = nullptr;
	CCamera* camera3D = nullptr;		//一般の3Dオブジェクトとして描画に使う
	CCamera* camera2D = nullptr;		//2D的な表示をしたいオブジェクト(UI等に使う)
	CCamera* MiniMapCam = nullptr;	    //ミニマップカメラ
	CCamera* DebugCamera = nullptr;		//デバッグシーン用カメラ

	//ミニマップ用変数
	DirectX::XMFLOAT3 MinimapDir = { 0.0f,0.0f,0.0f };
	DirectX::XMVECTOR minimapdir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&MinimapDir));;

	typedef std::unordered_map<std::wstring, D3D::Model> ModelHash;
	ModelHash* modelManager;

	D3D::Model model;

	bool WaitFadeSecne = false;
	double Length = 0.0f;

	//ミニマップ切り替えよう変数
	int MiniMapNum = 0;

	//一時停止用変数
	bool IsStop = false;

	//デバッグ表示用
	bool IsDebug = false;

	CGame() {}
	~CGame();

	/**
	 *オブジェクトのリリース処理をする関数です。
	 */
	void ReleseObject();

public:
	static CGame* Get();

	/**
	*ゲームオブジェクト作成とアセット読み込みをする関数です。
	*/
	void SetGameObjectInit();

	/**
	 *ゲームオブジェクトの初期化処理をする関数です。
	 */
	void Initialize();

	/**
	 *ゲームの更新処理をする関数です。
	 */
	void Update();

	/**
	 *ゲームの描画処理をする関数です。
	 */
	void Draw();

	/**
	 *描画設定されているカメラを取得する関数です。
	 * 
	 * @return 描画に設定されているカメラを返します。
	 */
	CCamera* GetCamera() { return camera; }

	/**
	 *描画設定のカメラを設定する関数です。
	 *
	 * @param 描画設定にしたいカメラ
	 */
	void SetCamera(CCamera* cam) { camera = cam; }

	/**
	 *3dのカメラを取得する関数です。
	 *
	 * @return 3dのカメラを返します。
	 */
	CCamera* GetCamera3D() { return camera3D; }

	/**
	* デバッグカメラを取得する
	* 
	* @return デバッグカメラ
	*/
	CCamera* GetDebugCamera() { return DebugCamera; }

	////3Dのカメラの座標取得
	//DirectX::XMFLOAT3 GetCameraPos() { return camera3D->GetCameraPos(); }

	/**
	 *2dのカメラを取得する関数です。
	 *
	 * @return 2dのカメラを返します。
	 */
	CCamera* GetCamera2D() { return camera2D; }

	/**
	 *ミニマップカメラを取得する関数です。
	 *
	 * @return ミニマップカメラを返します。
	 */
	CCamera* GetMiniCamera() { return MiniMapCam; }

	/**
	 *プレイヤーオブジェクトを取得する関数です。
	 *
	 * @return プレイヤーオブジェクトを返します。
	 */
	CPlayer* GetPlayer();

	/**
	 *メインカメラの正面方向を取得する関数です。
	 *
	 * @param  基準となるプレイヤー位置
	 * @return メインカメラの正面方向を返します。
	 */
	DirectX::XMFLOAT3 GetMainCameraForward(CGameObject* _player);

	////三人称カメラの設定
	//DirectX::XMFLOAT3 SetMainCamera(CPlayer* _player);

	/**
	 *オブジェクトのReleseとDeleteを行う関数です。
	 *
	 * @param  解放したいオブジェクト
	 */
	void EndObject(CGameObject* _obj);

	/**
	 *フェードの設定を行う関数です。
	 *
	 * @param  変更したいフェードの状態
	 */
	void SetFade(FADE _fade) { Fade = _fade; }

	/**
	* ゲームが開始するかの変数を設定する関数です。
	*/
	void SetIsGameStageStart(bool _isstart) { IsStartGame = _isstart; IsStartCountDown = _isstart; }

	/**
	* ステージが開始したかの変数を取得する関数です。
	*/
	bool GetIsStartStage() { return IsStartGame; }

	/**
	 *ミサイルがヒットした時のUI制御を行う関数です。
	 */
	void MisileHitUIActive();

	/**
	* プレイヤーにミサイルが当たった時にカメラを揺らす関数です。
	* 
	* @param id
	* 1:ダメージ振動
	* 2:スピードアップ振動
	* 3:
	* 4:
	* 
	*/
	void ShakeCamera(const int& _id);

	/**
	 *カメラの視点の設定を行う関数です。
	 * 
	 * @param 変更したいカメラの視点
	 */
	void SetCameraID(CAMERA_ID _id) { CameraId  = _id; }

	/**
	 *カメラ座標の設定を行う関数です。
	 */
	void SetCameraPos();

	/**
	 *カメラ座標の取得を行う関数です。
	 * 
	 * @return カメラ座標を返します。
	 */
	DirectX::XMFLOAT3 GetCameraPos() { return cameraPos; }

	/**
	* ミニマップ座標を返す
	*/
	DirectX::XMFLOAT3& GetMiniMapCameraPos() { return MiniMapPos; }

	/**
	 *カメラが一人称視点かを返す関数です。
	 *
	 * @return カメラが一人称視点かを返します。
	 */
	bool GetIsFirst();

	/**
	* 敵を出現させる関数です。
	*
	* @param _tag		  敵のタグID
	* @param _newposition 出現させるオブジェクトのポジション
	* @param _newrotate   出現させるオブジェクトの回転角度
	* @param _newscaling  出現させるオブジェクトのスケール
	*/
	void InsertEnemy(int _tag ,DirectX::XMFLOAT3 _newposition, DirectX::XMFLOAT3 _newrotate, DirectX::XMFLOAT3 _newscaling);

	/**
	* ミニマップ切り替え変数を返す
	* 
	* @return ミニマップ切り替え変数
	*/
	int GetMiniMapNum() { return MiniMapNum; }

	/**
	* 一時停止用変数を変更する関数です。
	*/
	void SetIsGameStop() { IsStop = !IsStop; }

	/**
	* 一時停止変数を取得する関数です。
	* 
	* @return  一時停止変数
	*/
	bool GetIsStop() { return IsStop; }

	/**
	* 経過時間を設定する関数です。
	*/
	void SetNowMoveTime();

	/**
	* 経過時間を返す関数です。
	*/
	int GetNowMoveTime();

	int GetStartTime() { return (int)StartProjTime; }

	/**
	* モデルを入れた関数を取得する
	* 
	* @param  モデルテクスチャの名前
	* @return テクスチャの名前をもとに取得したモデルを返す
	*/
	D3D::Model& GetModel(const std::wstring _moedlname);

	/**
	* デバッグモード化を取得
	*/
	bool GetIsDebug() { return IsDebug; }
};

