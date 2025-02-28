#pragma once // 重複インクルードを自動防止する

#include <d3d11.h>  // DirectX11を使うためのヘッダーファイル
#include <DirectXMath.h> // DirextXの数学関連のヘッダーファイル
#include <DirectXCollision.h>

// Direct3D解放の簡略化マクロ
#define SAFE_RELEASE(p)      { if( NULL != p ) { p->Release(); p = NULL; } }

// 既存の型名を別名でも使えるようにする
typedef ID3D11ShaderResourceView* D3DTEXTURE;
typedef DirectX::XMFLOAT2 FLOAT_XY;

// Direct3Dの機能をカプセル化したクラス
class D3D
{

public:
	// 構造体の定義
	// 色を表す
	struct Color
	{
		float r, g, b, a;
	};
	// モデルを表す
	struct Model
	{
		//ComPtrを使用することで安全に解放できる
		//ComPtrはスマートポインタの一種
		//参照カウントを行うため、参照がすべてなくなってから解放を行う
		ID3D11Buffer* vertexBuffer = nullptr; // 頂点バッファ＝頂点データを持つ
		D3DTEXTURE texture = nullptr;		  // テクスチャ
		ID3D11Buffer* indexBuffer = nullptr; // インデックスバッファ
		size_t numIndex = 0;				 // インデックス数
	};
	// 定数バッファ用構造体
	struct ConstBuffer
	{
		// ワールド行列
		DirectX::XMMATRIX world;

		// ビュー行列
		DirectX::XMMATRIX view;

		// プロジェクション行列
		DirectX::XMMATRIX proj;

		// ワールド変換行列
		DirectX::XMMATRIX matrixWorld;

		// 法線ベクトルを回転させる行列
		DirectX::XMMATRIX matrixWorldNormal;

		// UVアニメーション行列
		DirectX::XMMATRIX matrixUV;

		// マテリアル色
		DirectX::XMFLOAT4 materialDiffuse;

		unsigned int isLighting;
		DirectX::XMFLOAT3 CameraPos;

		float time;
		float Dist;

		DirectX::XMFLOAT3 LightVector;
	};

	DirectX::XMFLOAT3 Vertices[8];

	// Direct3Dの初期化処理を実行する
	HRESULT Create(HWND hwnd);
	~D3D();

	// 画面塗りつぶしと設定を行う関数
	void ClearScreen();
	// 画面を更新する関数
	void UpdateScreen();

	// 四角形ポリゴンを作成し、頂点バッファ生成を行う
	static Model CreateSquare(FLOAT_XY center, FLOAT_XY size, FLOAT_XY uv);
	// Objファイルを読み込む
	// 引数：(1)ファイル名
	static Model LoadObjModel(const wchar_t* fileName);
	// テクスチャを読み込む
	// 引数：(1)ファイル名
	static D3DTEXTURE LoadTexture(const wchar_t* fileName);
	// このクラスの唯一のインスタンスを返す
	static D3D* Get();

	static DirectX::XMFLOAT3 RotateVector3(DirectX::XMFLOAT3 srcVector, DirectX::XMFLOAT3 rotation);

	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pImmediateContext; }
	ID3D11Buffer* GetConstantBuffer() { return m_pConstantBuffer; }

	//深度バッファを無効
	void SetDepthStencilState(int _num);

	// 頂点データ１つあたりのバイトサイズを返す
	UINT GetVertexStride();

	//ビューボードを設定する
	void SetViewBord(int _num);

	//裏面もテクスチャを表示するように設定
	void SetCulling(bool _cullmode);

	//リニア補完を設定する関数
	void SetLinear();

	/**
	* シェーダーを変更する関数です。
	* 
	* @param  _id
	* 0:通常シェーダー
	* 1:プレイヤーシェーダー
	* 2:
	* 3:バックファイアシェーダー
	* 4:アルファディザシェーダー
	*/
	void ChengeShader(const int& _id);

private:
	D3D() {};

	// ※ID3D11で始まるポインタ型の変数は、解放する必要がある
	ID3D11Device* m_pDevice; // デバイス＝DirectXの各種機能を作る
	// コンテキスト＝描画関連を司る機能
	ID3D11DeviceContext* m_pImmediateContext;
	// スワップチェイン＝ダブルバッファ機能
	IDXGISwapChain* m_pSwapChain;
	// レンダーターゲット＝描画先を表す機能
	ID3D11RenderTargetView* m_pRenderTargetView;
	// 深度バッファ用テクスチャ
	ID3D11Texture2D* m_pDepthStencilTexture;
	// 深度バッファ
	ID3D11DepthStencilView* m_pDepthStencilView;

	// インプットレイアウト
	ID3D11InputLayout* m_pInputLayout;
	// 頂点シェーダーオブジェクト
	ID3D11VertexShader* m_pVertexShader;
	// ピクセルシェーダーオブジェクト
	ID3D11PixelShader* m_pPixelShader;
	// プレイヤーピクセルシェーダーオブジェクト
	ID3D11PixelShader* m_pPlayerPixelShader;
	// ビューポート
	D3D11_VIEWPORT          m_Viewport[2];

	// ブレンドステート用変数（アルファブレンディング）
	ID3D11BlendState* m_pBlendStateAlpha;
	// ブレンドステート用変数（加算合成）
	ID3D11BlendState* m_pBlendStateAdditive;

	// サンプラー用変数
	ID3D11SamplerState* m_pSampler;

	// 定数バッファ用変数
	ID3D11Buffer* m_pConstantBuffer;

};

