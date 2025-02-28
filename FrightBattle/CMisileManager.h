#pragma once
#include<vector>
#include"direct3d.h"

class CMisile;
class CGameObject;
class CEnemy;
class CEffect;

/**
 * これはCMisileManagerクラスの説明です。
 * このクラスはミサイル制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CMisileManager
{
	//ミサイルの配列
	std::vector<CMisile*> vMisile;

	//メンバー変数
	D3D::Model model;	//描画するべきモデルデータ
	D3D::Model MisileHitModel;

	DirectX::XMFLOAT2 UVSize = { 0,0 };

	CMisileManager();
	~CMisileManager() {}

	struct MISILE_UI
	{
		ULONGLONG OldTime = 0;
		bool Active = true;
	};

	MISILE_UI MisileUi[2];

	//ミサイルの最大ロックオン数
	int MaxRockonNuum = 1;

public:
	static CMisileManager* Get();
	
	/**
	*更新処理を行う関数です。
	*/
	void Update();

	/**
	*描画処理を行う関数です。
	*/
	void Draw();

	/**
	*モデルを設定する関数です。
	* 
	* @param 設定したいミサイルのモデル
	*/
	void SetModel(D3D::Model _model) { model = _model; }
	
	/**
	*ミサイルが当たった時のUIのモデル設定を行う関数です。
	* 
	* @param 設定したいミサイルが当たった時のUIモデル
	*/
	void SetMisileHitModel(D3D::Model _model) { MisileHitModel = _model; }

	/**
	*エフェクトのUVサイズの設定を行う関数です。
	* 
	* @param 設定したいUVサイズ
	*/
	void SetUV_Size(DirectX::XMFLOAT2 _uvsize); 

	/**
	*ミサイルの設定を行う関数です。
	* 
	* @param _sppornobj		スポーンする際の基準のオブジェクト
	* @param _target		ターゲットのオブジェクト
	* @param _misileuinum	指定するミサイルのUI
	*/
	void SetMisileInf(CGameObject* _spornobj, CEnemy* _target, int _misileuinum);
};

