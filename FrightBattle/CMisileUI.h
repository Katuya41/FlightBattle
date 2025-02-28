#pragma once
#include "CGameObject.h"

/**
 * これはCMisileUIクラスの説明です。
* このクラスはミサイルUI制御で使用されます。
 *
* @author 吉村括哉
* @since 2024-7-10
*/
class CMisileUI :
	public CGameObject
{
private:

	//もっているエフェクト情報
	CEffect* pEffect;

	/**
	* 現在のミサイルUIの状態を表す構造体です
	*/
	enum MISILEUI_STATE
	{
		READY,		//発射準備完了
		CHARGE,		//リロード中
	};
	MISILEUI_STATE MisileUIState = READY;

	//ミサイルを発射した時間
	ULONGLONG MisileShotTime = 0;

	//リロード中にミサイルの中身を隠すパネル
	CGameObject* MisileUIPanel;

	//アニメーションのスピード
	float AnimSpeed = 0.0f;

public:
	//コンストラクタ
	CMisileUI();
	~CMisileUI();

	/**
	*更新処理を行う関数です。
	*/
	void Update() override;

	/**
	* 現在のミサイルUIの状態を返します
	* 
	* @return ミサイルUIの状態を返します。
	*/
	MISILEUI_STATE& GetMisileUIState() { return MisileUIState; }

	/**
	* 現在のミサイルUIの状態を設定します。
	* 
	* @param  設定したいミサイルUIの状態
	*/
	void SetMisileUIStateCharge();

	/**
	* ミサイルUIの情報をセットします。
	*/
	void SetMisileUIInf(D3D::Model _model,DirectX::XMFLOAT2 _effectsize);

	/**
	* エフェクトの座標を設定する関数です。
	*/
	void SetEffectPos(const DirectX::XMFLOAT3& _pos);

	/**
	* エフェクトのサイズを設定する関数です。
	*/
	void SetEffectScale(const DirectX::XMFLOAT3& _scale);

	/**
	* ミサイルの状態をリセットする
	*/
	void ResetMisileUI();

	/**
	* アニメーションのスピードを設定する関数です。
	* 
	* @param  設定したいアニメーションのスピード
	*/
	void SetAnimSped(const float& _speed) { AnimSpeed = _speed; }

	/**
	* アニメーションのスピードを取得する関数です。
	* 
	* @return アニメーションのスピード
	*/
	const float& GetAnimSpeed() { return AnimSpeed; }

};