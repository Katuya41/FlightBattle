#pragma once
#include <list>
#include <unordered_map>
#include <iostream>
#include <DirectXMath.h>
#include "CGameObject.h"
#include <string>

class CCamera;
class CPlayer;
class CMisileUI;

/**
 * これはCSceneクラスの説明です。
 * このクラスはシーン制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CScene
{
public:

	/**
	* シーンの状態を表す列挙型です。
	*/
	enum SCENE
	{
		TITLE,			//タイトル
		MENU,			//メニュー
		STAGE,			//テストステージ
		STAGE_01,		//ステージ1
		STAGE_02,		//ステージ2
		STAGE_03,		//ステージ3
		STAGE_04,		//ステージ4
		TEST_STAGE,		//テストシーン
		RESULT,			//リザルト
		GAMEOVER,		//ゲームオーバー
		GAMECLEAR,		//ゲームクリア
		CREATE_STAGE,	//ステージ作成
	};
	SCENE Scene;

private:

	enum ENUM_GET_TEXT
	{
		TAG,
		BILLBORD,
		LIGHTING,
		POSX,
		POSY,
		POSZ,
		ROTX,
		ROTY,
		ROTZ,
		SCAX,
		SCAY,
		SCAZ,
		MATERIAL,
		ISUI,
		END,
	};
	ENUM_GET_TEXT EGetText = TAG;

	/**
	* テキストから取得した文字列や数字を格納する構造体です
	*/
	struct STR_GET_TEXT
	{
		int TagId;
		int IsBillbord;
		int IsLighting;
		float PositionX;
		float PositionY;
		float PositionZ;
		float RotationX;
		float RotationY;
		float RotationZ;
		float ScalingX;
		float ScalingY;
		float ScalingZ;
		float MaterialDiffuseW;
		int Isui;
	};
	STR_GET_TEXT SGetText;

	std::list<CMisileUI*> plMisileUI;

	//CGameObject型の変数を入れる配列
	std::list<CGameObject*> lGameObject;

	//UIを入れる配列
	std::list<CGameObject*> lGameUI;

	//プレイヤー
	std::list<CPlayer*>lPlayer;

	//速度数字を入れる配列
	std::unordered_map<int, CGameObject*> lGameSpeedNums;

	//高度数字を入れる配列
	std::unordered_map<int, CGameObject*> lGameAltNums;

	//ボスのHPバーを入れる配列
	std::list<CGameObject*> lBossHPUI;

	//プレイヤーを表示するか
	bool IsDrawPlayer = true;

	//当たり判定を描画させるか
	bool IsDrawCollider = false;

	//ステージデバッグウィンドウを作成したか
	bool IsCreateStageDebugWindoe = false;

	//ステージ作成デバッグウィンドウを作成したか
	bool IsCreateCreateStageWindow = false;

	//BGMの音量
	float BGM_Volume = 0.005f;
	//SEの音量
	float SE_Volume = 0.02f;

	//新しく追加するオブジェクト用変数
	DirectX::XMFLOAT3 NewObjectPos = { 0.0f,-40.0f,0.0f };
	DirectX::XMFLOAT3 NewObjectRot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewObjectSca = { 10.0f,20.0f,10.0f };

	//新しく追加する地面敵用変数
	DirectX::XMFLOAT3 NewGroundEnemyPos = { 0.0f,-58.0f,0.0f };
	DirectX::XMFLOAT3 NewGroundEnemyRot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewGroundEnemySca = { 0.3f,0.3f,0.3f };

	//新しく追加する空中敵用変数
	DirectX::XMFLOAT3 NewFrightEnemyPos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewFrightEnemyRot = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 NewFrightEnemySca = { 1.0f,1.0f,1.0f };

	//新しく追加するプレイヤー用変数
	DirectX::XMFLOAT3 NewPlayerPos;
	DirectX::XMFLOAT3 NewPlayerRot;

	//テキストから取得した文字列テスト用
	std::string TestText;

	CScene();
	~CScene();

	/**
	* 作成したゲームオブジェクトを削除する
	*/
	void ClearGameObject();

	/**
	* タイトルシーンセットする関数です。
	*/
	void SetTitleScene();

	/**
	* メニューシーンセットする関数です。
	*/
	void SetMenuScene();

	/**
	* ステージシーンを設定する関数です。
	*/
	void SetStageScene(std::string _stagename);

	/**
	* テストステージシーンを設定する関数です。
	*/
	void SetTestStageScene();

	/**
	* ゲームクリア画面をセットする関数です。
	*/
	void SetGameClearScene();

	/**
	* ゲームオーバー画面をセットする関数です。
	*/
	void SetGameOverScene();

	/**
	* ステージ作成画面をセットする関数です。
	*/
	void SetCreateStageScene();


	/**
	* 速度数字オブジェクトUIを作成して中の情報を設定して配列に格納する関数です。
	*
	* @param _moedlname モデルの名前
	* @param _isbillboar _islighting ビルボート,ライティングするか
	* @param _position _rotation _scaling _materialw 座標,角度,大きさ,アルファ値
	* @param _tag タグ
	* @param _gameobject 設定するオブジェクトの変数
	*/
	void SetGameObjectSppedNumsInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, CGameObject* _gameobject, int _num);

	/**
	* 高度数字オブジェクトUIを作成して中の情報を設定して配列に格納する関数です。
	*
	* @param _moedlname モデルの名前
	* @param _isbillboar _islighting ビルボート,ライティングするか
	* @param _position _rotation _scaling _materialw 座標,角度,大きさ,アルファ値
	* @param _tag タグ
	* @param _gameobject 設定するオブジェクトの変数
	*/
	void SetGameObjectAltNumsInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, CGameObject* _gameobject, int _num);

public:
	static CScene* Get();

	/**
	* 更新処理
	*/
	void Update();

	/**
	* 3D画面の描画
	*/
	void Draw3DCamera();

	/**
	* 2D画面での描画
	*/
	void Draw2DCamera();

	/**
	* シーンをセットする関数です。
	*
	* @param  設定したいシーン
	*/
	void SetScene(SCENE _scene);

	/**
	* 現在のシーンを取得する関数です。
	*
	* @return  現在のシーンを返します。
	*/
	SCENE GetNowScene() { return Scene; }

	/**
	* 3Dオブジェクトの配列の中身を取得する
	*
	* @param  取得するオブジェクトのタグ
	* @return 引数のタグに応じたオブジェクト
	*/
	CGameObject* Get3DGameObject(CGameObject::TAG _objecttag);

	/**
	* 2Dオブジェクトの配列の中身を取得する
	*
	* @param  取得するオブジェクトのタグ
	* @return 引数のタグに応じたオブジェクト
	*/
	CGameObject* Get2DGameObject(CGameObject::TAG _objecttag);

	/**
	* ステージシーンでのプレイヤーを取得する
	*
	* @return プレイヤーを返す
	*/
	CPlayer* GetPlayer();

	/**
	* 雲オブジェクトを取得する関数です。
	*
	* @param  雲オブジェクトを格納する動的配列
	*/
	void GetCloudObject(std::list<CGameObject*>& _cloudlist);

	/**
	* 雲オブジェクトを追加する関数です。
	*/
	void InsertCloudObject();

	/**
	* ステージシーンでの速度数字UIを取得する
	*
	* @param  取得したい番号のID(0～9)
	* @return 引数に応じた数字オブジェクトを返す
	*/
	CGameObject* GetStageSpeedNums(int _numsid);

	/**
	* ステージシーンでの高度数字UIを取得する
	*
	* @param  取得したい番号のID(0～9)
	* @return 引数に応じた数字オブジェクトを返す
	*/
	CGameObject* GetStageAltNums(int _numsid);

	/**
	* アクティブなミサイルUIを取得する
	*
	* @return  アクティブなミサイルUIを返す
	*/
	CGameObject* GetActiveMisileUI();

	/**
	* アクティブなミサイルUIを取得する
	*
	* @return  アクティブなミサイルUIを返す
	*/
	CMisileUI* GetActiveMisileUITest(bool _isfour);

	/**
	* FourMisile用確認
	*/
	int CheckFourMisileActive();

	/**
	* 指定した数のミサイルUIの状態を変更
	*/
	void SetMisileUICharge(const int _num);

	/**
	* ミサイルUIをリセットする
	*/
	void ResetMisileUI();

	/**
	* 当たり判定を描画させるかを返す
	*/
	bool GetIsDrawCollider() { return IsDrawCollider; }

	/**
	* 当たり判定を描画させるかを設定する
	*/
	void SetIsDrawCollider() { IsDrawCollider = !IsDrawCollider; }

	/**
	* プレイヤーを配列で返す(デバッグ用)
	*
	* @param  b3Dオブジェクトを入れる配列
	*/
	void Get3DObject(std::list<CGameObject*>& _3dobject);

	/**
	* オブジェクトを作成して中の情報を設定して配列に格納する関数です。
	*
	* @param _moedlname モデルの名前
	* @param _isbillboar _islighting ビルボート,ライティングするか
	* @param _position _rotation _scaling _materialw 座標,角度,大きさ,アルファ値
	* @param _tag タグ
	* @param _gameobject 設定するオブジェクトの変数
	* @param _isui		 UIかどうか
	*/
	void SetGameObjectInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, CGameObject* _gameobject, bool _isui);
	void SetGameObjectInfTest(bool _isbillboar, bool _islighting,
		DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw,
		CGameObject::TAG _tag, bool _isui);

	/**
	* 新しいオブジェクト作成用座標を取得する
	*/
	DirectX::XMFLOAT3& GetNewObjectPos() { return NewObjectPos; }
	DirectX::XMFLOAT3& GetNewObjectRot() { return NewObjectRot; }
	DirectX::XMFLOAT3& GetNewObjectSca() { return NewObjectSca; }

	/**
	* 新しい地上敵オブジェクト作成用座標を取得する
	*/
	DirectX::XMFLOAT3& GetNewGroundEnemyPos() { return NewGroundEnemyPos; }
	DirectX::XMFLOAT3& GetNewGroundEnemyRot() { return NewGroundEnemyRot; }
	DirectX::XMFLOAT3& GetNewGroundEnemySca() { return NewGroundEnemySca; }

	/**
	* 新しい空中敵オブジェクト作成用座標を取得する
	*/
	DirectX::XMFLOAT3& GetNewFrightEnemyPos() { return NewFrightEnemyPos; }
	DirectX::XMFLOAT3& GetNewFrightEnemyRot() { return NewFrightEnemyRot; }
	DirectX::XMFLOAT3& GetNewFrightEnemySca() { return NewFrightEnemySca; }

	/**
	* 新しいプレイヤー作成用変数を取得する
	*/
	DirectX::XMFLOAT3& GetNewPlayerPos() { return NewPlayerPos; }
	DirectX::XMFLOAT3& GetNewPlayerRot() { return NewPlayerRot; }


	/**
	* プレイヤーを表示するかを設定する関数
	*/
	void SetDrawPlayer(bool _isdraw) { IsDrawPlayer = _isdraw; }

	/**
	* プレイヤーを配列に格納する関数です(デバッグ用)
	*/
	void InsertPlayer(CPlayer* _player) { lPlayer.push_back(_player); }

	/**
	* プレイヤー配列をクリアする関数です。
	*/
	void ClearLPlayer() { lPlayer.clear(); }

	/**
	* テキストから値を取得する関数です。
	*/
	void GetTextValue(std::string _stagename);

	/**
	* テキストに値を設定する関数です。
	*/
	void SetTextValue(std::string _stagename);

	/**
	* テキストにパラメーターをセットする
	*
	* @param  セットしたいファイル名
	* @param  セットしたいパラメータのオブジェクト番号
	* 1:Player 2:Enemy 3:Misile
	*/
	void SetTextFileParameter(std::string _stagename, int _num);

	/**
	* BGMの音量を返す
	*/
	float& GetBGMVolume() { return BGM_Volume; }

	/**
	* SEの音量を返す
	*/
	float& GetSeVolume() { return SE_Volume; }
};

