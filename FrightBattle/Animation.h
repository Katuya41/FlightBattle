#pragma once

#include "direct3d.h"

/**
 * これはAnimationクラスの説明です。
 * このクラスはアニメーション制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class Animation
{

private:
	/**
	*アニメーションのパターンを入れる列挙型です。
	*
	* @param アニメーションがない
	* @param エフェクトアニメーション用
	*/
	enum PATTERN
	{
		NO_ANIMATION,
		FX_0_11 = 5  // エフェクト用０～１１再生
	};

	// メンバー変数
	//アニメーションのUV移動量を保存
	DirectX::XMFLOAT2 uv = { 0,0 };
	DirectX::XMFLOAT2 GetUvSize = { 0,0 };

	//アニメテーブルのカウンター
	float animCounter = 0.0f;

	//再生するパターンIDを持つ変数
	int animPattern = 0;

	//再生速度を設定する
	float animSpeed = 0.1f;

	//アニメーションが再生中か
	bool isPlaying = false;

public: 	// メンバー関数//
	/**
	 *アニメーションのコンストラクタです。
	 */
	Animation();

	/**
	 *アニメーションのデストラクタです。
	 */
	~Animation();

	/**
	 *アニメーションの更新処理を行う関数です。
	 */
	void Update();

	/**
	 *UV移動量を取得する関数です。
	 * @return UV移動量を取得します
	 */
	DirectX::XMFLOAT2 GetUV();

	/**
	 *UV移動量をセットする関数です。
	 * @param セットしたいuv移動量
	 */
	void SetUv(DirectX::XMFLOAT2& _uv);

	/**
	 *アニメーションスピードをセットする関数です。
	 * @param セットしたいアニメーションスピード
	 */
	void SetAnimSpeed(float& _speed);

	/**
	 *再生するパターンを変更する関数です。
	 * @return 変更後のパターン
	 */
	void SetPattern(int pattern);

	/**
	 *エフェクトが再生中かを取得する関数です。
	 * @return エフェクトが再生中かを返します。
	 */
	bool GetIsPlaying() { return isPlaying; }

	/**
	 *エフェクトの状態を設定する関数です
	 * @return エフェクトの状態を設定します
	 */
	void SetIsPlaying(bool _isplay) { isPlaying = _isplay; }

	/**
	* アニメーションの状態をリセットする
	*/
	void ResetAnimation();

};

