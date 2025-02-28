#pragma once
#include<vector>
#include"direct3d.h"

class CShottoBall;
class CGameObject;
class CEnemy;

/**
 * これはCShottoManagerクラスの説明です。
 * このクラスは弾を撃つ制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CShottoManager
{
private:
	//ミサイルの可変長配列
	std::vector<CShottoBall*> vShottoBall;

	//射撃ポイント
	DirectX::XMFLOAT3 ShotPos = { 0.0f,0.0f,0.0f };

	//射撃ポイントを表示するか
	bool IsShotPos = false;

	//メンバー変数
	D3D::Model model;	//描画するべきモデルデータ
	D3D::Model BallHitModel;

	DirectX::XMFLOAT2 UVSize = { 0,0 };

	//玉を撃った時間
	ULONGLONG BallShottoTime = 0;

	CShottoManager() {}
	~CShottoManager() {}

public:
	static CShottoManager* Get();

	//更新処理
	void Update();

	//描画処理
	void Draw();

	//射撃ポイントチェック
	void CheckShotPos(DirectX::XMFLOAT3 _spornpos, CEnemy* _target);

	//モデルをセットする
	void SetModel(D3D::Model _model) { model = _model; }

	//弾が当たった時のエフェクト
	void SetBallHitModel(D3D::Model _model) { BallHitModel = _model; }

	//エフェクトのuvサイズ設定
	void SetUV_Size(DirectX::XMFLOAT2 _uvsize) { UVSize = _uvsize; }

	//射撃座標を返す
	DirectX::XMFLOAT3 GetShotPoint() { return ShotPos; }

	//射撃ポイントを表示するか
	bool GetIsActiveShotpos() { return IsShotPos; }

	//機関銃をセットする
	//第一引数:スピード
	//第二引数:ミサイルのスポーンする座標
	//第三引数:ミサイルの初期角度
	//第四引数:ターゲットの座標
	void SetBallInf(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _spornrotate, CEnemy* _target);
};