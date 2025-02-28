#pragma once
#include<vector>
#include"direct3d.h"

class CEffect;

/**
 * これはCEffectManagerクラスの説明です。
 * このクラスはエフェクト制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CEffectManager
{
private: //メンバー変数

	//エフェクトの可変長配列
	std::vector<CEffect*> vEffect;
	std::vector<CEffect*> vEffect2D;

	//描画するべきモデルデータ
	D3D::Model model;	

	CEffectManager() {}
	~CEffectManager();

public:
	static CEffectManager* Get();

	/**
	*CEffectの更新処理を行う関数です。
	*/
	void Update();

	/**
	*CEffectの描画処理行う関数です。
	*/
	void Draw();

	/**
	* UI用のDraw関数
	*/
	void Draw2D();

	/**
	*エフェクトをセットする関数です。
	* 
	* @param セットするエフェクト
	*/
	void SetvEffect(CEffect* _effect, DirectX::XMFLOAT3 _position, int _animpattern, float _animspeed);

	void Set3DvEffect(CEffect* _effect, int _animpattern, float _animspeed);

	/**
	*エフェクトをセットする関数です。
	*
	* @param セットするエフェクト
	*/
	void SetvEffectMisileUI(CEffect* _effect, int _animpattern, float _animspeed);

	/**
	* エフェクトの中身をクリアする関数です
	*/
	void ClearEffect();
};

