struct VS_IN
{
    float3 pos : POSITION0;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
};
struct VS_OUT
{
    float4 pos : SV_POSITION0;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
	// スクリーン座標とは別に、ワールド座標のデータを用意
	// ピクセルシェーダーに渡ると、ピクセルごとのワールド座標のデータになる
    float4 worldPos : POSITION0;
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

VS_OUT w_vs_main(VS_IN input)
{
    VS_OUT vout;
    vout.pos = float4(input.pos, 1.0f);
    vout.pos = mul(vout.pos, matrixWorld);
	// ピクセルシェーダーに渡すために、途中で変数にコピー
    vout.worldPos = vout.pos;

    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);
    vout.tex = input.tex;
    vout.normal = mul(input.normal, (float3x3) world);
    return vout;
}