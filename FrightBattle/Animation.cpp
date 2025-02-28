#include "Animation.h"

/**
 *アニメーションのコンストラクタです。
 */
Animation::Animation()
{
}

/**
 *アニメーションのデストラクタです。
 */
Animation::~Animation()
{
}

/**
*アニメーションの更新処理を行う関数です。
*/
void Animation::Update()
{
    if (!isPlaying)
    {
        return;
    }

    // テーブルアニメーション…決められた順番で再生させるためのデータ
    // ※テーブル…何かの目的のために作られたデータの集まり
    int animTable[][32] =
    {
        // エフェクト用０～１１まで再生
        {0,-1},
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -2 },
        {0,1,2,3,4,5,6,7,8,9,-2},
        {0,1,2,3,4,5,6,7,8,9,-2},
        {0,1,2,3,4,5,6,7,8,-2},
        {0,1,2,3,-2},
        {0,1,2,3,4,5,6,7,8,9,-1},
        {1,-1},
    };

    // 表示させるコマIDを取得
    int animID = animTable[animPattern][(int)animCounter];

    //// 表示させるコマのUVを計算
    //uv.x = animID % 10 * 0.1f;
    //uv.y = animID / 10 * 1.0f;

    // 表示させるコマのUVを計算
    uv.x = animID % int(GetUvSize.x) * (1 / GetUvSize.x);
    uv.y = animID / int(GetUvSize.x) * (1 / GetUvSize.y);

    // アニメーションのカウンターを進める
    animCounter += animSpeed;

    if (animTable[animPattern][(int)animCounter] == -1)
    {
        //最初に戻る
        animCounter = 0.0f;
    }
    else if (animTable[animPattern][(int)animCounter] == -2)
    {
        //アニメーション終了
        animCounter = 0.0f;
        //animSpeed = 0.0f;
        isPlaying = false;
    }
}

/**
 *UV移動量を取得する関数です。
 * @return UV移動量を取得します
 */
FLOAT_XY Animation::GetUV()
{
    return uv;
}

/**
 *UV移動量をセットする関数です。
 * @param セットしたいuv移動量
 */
void Animation::SetUv(DirectX::XMFLOAT2& _uv)
{
    GetUvSize = _uv;
}

/**
 *アニメーションスピードをセットする関数です。
 * @param セットしたいアニメーションスピード
 */
void Animation::SetAnimSpeed(float& _speed)
{
    animSpeed = _speed;
}

/**
 *再生するパターンを変更する関数です。
 * @return 変更後のパターン
 */
void Animation::SetPattern(int pattern)
{
    animPattern = pattern;
}

void Animation::ResetAnimation()
{
    //アニメーション終了
    animCounter = 0.0f;
    //animSpeed = 0.0f;
    isPlaying = false;
}
