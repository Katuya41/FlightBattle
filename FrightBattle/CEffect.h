#pragma once
#include "CGameObject.h"

class Animation;

/**
 * これはCEffectクラスの説明です。
 * このクラスはエフェクト制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CEffect :
    public CGameObject
{
private:
    Animation* anim;

    DirectX::XMFLOAT2 UV_Size;

protected:
    /**
    *UVアニメーションの行列を返す関数です。
    * 
    * @return UVアニメーションの行列
    */
    DirectX::XMMATRIX GetUvMatrix() override;

public:
    /**
    *CEffectのコンストラクタです。
    */
    CEffect();

    /**
    *CEffectのデストラクタです。
    */
    ~CEffect();

    /**
    *CEffectの更新処理を行う関数です。
    */
    void Update();

    /**
    *アニメーションの状態を取得する関数です。
    */
    void Play();

    /**
    *アニメーションの状態を取得する関数です。
    */
    bool GetAnimPlay();

    /**
    * アニメーションの状態をリセットする
    */
    void ResetAnimation();

    /**
    *アニメーションパターンを設定する関数です。
    * 
    * @param アニメーションパターン番号
    */
    void SetAnimPattern(int _num);

    /**
    *アニメーションのUVサイズを設定する関数です。
    * 
    * @param _u UVサイズのU
    * @param _v UVサイズのV
    */
    void SetAnimUVSize(float _u, float _v);

    /**
    * アニメーションのスピードを設定する関数です。
    * 
    * @param  設定したいアニメーションスピード
    */
    void SetAnimSpeed(float _animspeed);

    /**
    *アニメーションのUVサイズを取得する関数です。
    * 
    * @return UVサイズを返します。
    */
    DirectX::XMFLOAT2 GetAnimUV() { return UV_Size; }
};

