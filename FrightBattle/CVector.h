#pragma once
#include "CGameObject.h"
class CEnemy;

/**
 * これはCVectorクラスの説明です。
 * このクラスは敵の方向を指す矢印制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CVector :
    public CGameObject
{
public:
    //ターゲットの座標
    DirectX::XMFLOAT3 TargetPos;

    CEnemy* Target;

    //ベクトルの方向
    DirectX::XMVECTOR vDirect;
    DirectX::XMFLOAT3 fDirect;

private:

public:
    CVector();
    ~CVector(){}

    /**
    * 更新処理を行う関数です。
    */
    void Update();

    /**
    * 描画処理を行う関数です。
    */
    void Draw();

    /**
    * ターゲットへの方向を計算する関数です。
    * 
    * @return 計算したターゲットの方向
    */
    DirectX::XMFLOAT3 SetDirect();

    /**
    * ターゲットの座標を設定する関数です。
    * 
    * @param  ターゲットオブジェクトの座標
    */
    void SetTargetPos(DirectX::XMFLOAT3 _targetpos) { TargetPos = _targetpos; }

    /**
    * ターゲットを設定する関数です。
    * 
    * @param  ターゲットオブジェクト
    */
    void SetTarget(CEnemy* _target);
};

