// ピクセルシェーダー

// ピクセルの情報の構造体（受け取り用）
struct PS_IN
{
    float4 pos : SV_POSITION0; // ピクセルの画面上の座標
    float2 tex : TEXCOORD0;  // ピクセルに対応するテクスチャ座標
    float4 normal : NORMAL0; // ピクセル面の法線ベクトル
};

// グローバル変数の宣言
// ※シェーダーのグローバル変数は、C言語プログラムから渡された
// 　データを受け取るために使う。
Texture2D myTexture : register(t0); //テクスチャー
SamplerState mySampler : register(s0); //サンプラー

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
    
    float3 LightVector;
    //int dummy2, dummy3, dummy4;
}
 

// ピクセルシェーダーのエントリポイント
float4 ps_main(PS_IN input) : SV_Target
{
    //float4 FinalColor;
    //FinalColor = myTexture.Sample(mySampler, input.tex);
    //FinalColor *= materialDiffuse;
    //return FinalColor;
    
    //色を指定している
    if (!isLighting)
    {
        // Sample関数→テクスチャから指定したUVを元にピクセル色を取って来る
        float4 color = myTexture.Sample(mySampler, input.tex);
        // テクスチャ色とマテリアル色を混ぜる
        return color * materialDiffuse;
    }
    
    // 光の向きを表すベクトル
    float3 lightVector = normalize(-LightVector.xyz); // ライトの方向を正規化
    //float3 lightVector = float3(-1, -1, 0);
    // ピクセル面の法線ベクトル
    float3 pixelNormal = input.normal.xyz;
    
    // 環境光（アンビエント）
    float4 ambientLight = float4(0.1f, 0.1f, 0.1f, 0.0f);
    
    // ベクトルを正規化する＝単位ベクトルにする
    //lightVector = normalize(lightVector);
    pixelNormal = normalize(pixelNormal);
    
    // 光ベクトルと法線ベクトルを内積する
    float diffusePower = dot(lightVector, pixelNormal);
    
    // diffusePowerを0.0f～1.0fの範囲に丸める
    diffusePower = saturate(diffusePower);
    
    // 光の当たる強さが一定以下にならないようにする
    diffusePower = max(diffusePower, 0.2f);
    
    // テクスチャ色を取得する
    float4 textureColor = myTexture.Sample(mySampler, input.tex);
    
    // 光の当たる強さとテクスチャ色を掛ける
    float4 pixelColor = textureColor * diffusePower;
    // 変化してしまったアルファ値を戻す
    pixelColor.a = textureColor.a;
    
    // 環境光を適用する
    pixelColor = pixelColor + ambientLight;
    
    return pixelColor;
}