// ピクセルシェーダー

// ピクセルの情報の構造体（受け取り用）
struct PS_IN
{
    float4 pos : POSITION0; // ピクセルの画面上の座標
    float2 tex : TEXCOORD0; // ピクセルに対応するテクスチャ座標
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
    //int dummy2, dummy3, dummy4;
}

float4 p_ps_main(PS_IN input) : SV_Target
{
     //色を指定している
    if (!isLighting)
    {
        // Sample関数→テクスチャから指定したUVを元にピクセル色を取って来る
        float4 color = myTexture.Sample(mySampler, input.tex);
        // テクスチャ色とマテリアル色を混ぜる
        return color * materialDiffuse;
    }
    
    float4 color;

	// テクスチャから色を取得
    color = myTexture.Sample(mySampler, input.tex);
    // 光の向きを表すベクトル
    float3 lightVector = float3(-1, -1, 0);
    float4 lightDiffuse = float4(0.8, 0.7, 0.6, 1.0); // 暖色系の柔らかい光
    float4 lightAmbient = float4(0.5, 0.5, 0.5, 1.0); // 環境光を強める

    float3 N = normalize(input.normal); // 法線ベクトルを正規化
    float3 L = normalize(lightVector.xyz); // ライトの方向を正規化
    float diffuse = saturate(dot(N, L)); // 拡散反射光の計算(法線ベクトルとライトの方向の内積)

    color *= diffuse * lightDiffuse + lightAmbient; //カラーに拡散反射光と環境光を適用
	// 視線ベクトルと反射ベクトルの計算
    float3 V = normalize(CameraPos.xyz - float3(input.pos.xyz));
    float3 R = normalize(reflect(-L, N));

	// 鏡面光の計算
    float specular = saturate(dot(V, R));

	// カラーに鏡面光を加算
    color += pow(specular, 5.0f);

	// アルファ値を1.0に設定
    color.a = 1.0f;

    return color;
}