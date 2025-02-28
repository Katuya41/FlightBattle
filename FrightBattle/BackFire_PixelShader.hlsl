// ピクセルシェーダー

// ピクセルの情報の構造体（受け取り用）
struct PS_IN
{
    float4 pos : SV_POSITION0; // ピクセルの画面上の座標
    float2 tex : TEXCOORD0; // ピクセルに対応するテクスチャ座標
    float4 normal : NORMAL0; // ピクセル面の法線ベクトル
    float3 wpos : POSITION0;  
};

// グローバル変数の宣言
// ※シェーダーのグローバル変数は、C言語プログラムから渡された
// 　データを受け取るために使う。
Texture2D myTexture : register(t0); //テクスチャー
Texture2D MaskTexture : register(t1); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

float2 distortionStrength = float2(0.05, 0.05); // 歪みの強さ

// 定数バッファ受け取り用
cbuffer ConstBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    
    // ワールド変換行列
    matrix matrixWorld;
    // 法線ベクトルを回転させる行列
    matrix matrixWorldNormal;
    // UVアニメーション用行列
    matrix matrixUV;
    // マテリアル色
    float4 materialDiffuse;
    
    unsigned int isLighting;
    float3 CameraPos;
    float time;
    float Dist;
    int dummy2, dummy3;
}

float4 bf_ps_main(PS_IN input) : SV_Target
{
    // 元のUV座標
    float2 uv = input.tex;

    // 動的なエフェクトのための時間ベースの歪み追加 (オプション)
    float2 animatedOffset = float2(
        sin(time + uv.y * 10.0) * 0.005,
        cos(time + uv.x * 10.0) * 0.005
    );

    // 歪みテクスチャからオフセットを取得
    float3 distortion = MaskTexture.Sample(mySampler, uv).rgb;

    //// UV座標に歪みを適用 (RGBチャンネルからオフセットを取得)
    //uv.x += (distortion.r - 0.5f * time) * Dist + animatedOffset.x;
    //uv.y += (distortion.r - 0.5f * time) * Dist + animatedOffset.y;
    
    // UV座標をオフセット (RGBで異なる歪みを使用可能)
    //uv.x += (distortion.r - 0.5f * time) * Dist;
    //uv.y += (distortion.g - 0.5f * time) * Dist;
    
    uv.x += (distortion.r - 0.5f) * Dist;
    uv.y += (distortion.g - 0.5f) * Dist;

    // 背景テクスチャをサンプリング
    float4 color = myTexture.Sample(mySampler, uv);

    return color;
}