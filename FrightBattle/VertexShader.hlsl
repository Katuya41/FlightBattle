// 頂点シェーダー

// 頂点のデータを表す構造体（受け取り用）
struct VS_IN
{
    float4 pos : POSITION; // 位置座標が入る
    float2 tex : TEX;       // UV座標が入る
    float4 normal : NORMAL; // 頂点の法線ベクトルが入る
};

// 頂点のデータを表す構造体（送信用） 
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL; // 頂点の法線ベクトルが入る
};

// グローバル変数の宣言
// 定数バッファ受け取り用
cbuffer ConstBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    
   // ワールド変換行列×ビュー変換行列×投影行列
    matrix matrixWorld;
    // 法線ベクトルを回転させる行列
    matrix matrixWorldNormal;
    // UVアニメーション用行列
    matrix matrixUV;
    // マテリアル色
    float4 materialDiffuse;
}
 

// 頂点シェーダーのエントリポイント 
VS_OUT vs_main( VS_IN input )
{
    VS_OUT output;
 
    // ワールド変換行列を頂点に掛ける
    output.pos = mul(input.pos, matrixWorld);
        
    // UVアニメーション
    float4 tex4;
    tex4.xy = input.tex;
    tex4.z = 0;
    tex4.w = 1;
    output.tex = mul(tex4, matrixUV).xy;
    
    // 頂点の法線ベクトルに回転行列を適用する
    output.normal = mul(input.normal, matrixWorldNormal);
        
    return output;
}