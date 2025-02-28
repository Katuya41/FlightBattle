#pragma once
#include <DirectXMath.h> // DirextXの数学関連のヘッダーファイル
#include <DirectXCollision.h>
#include <d3d11.h>  // DirectX11を使うためのヘッダーファイル
class DrawLine
{
public:
	DirectX::XMFLOAT3 Vertices[8];

private:
	DrawLine() {}
	~DrawLine() {}

public :
	static DrawLine* Get();

	//更新処理
	void Update();

	//描画処理
	void Draw(const DirectX::BoundingBox& bbox);

	void GetBoundingBoxVertices(const DirectX::BoundingBox& bbox);
};

