#pragma once
#include <DirectXMath.h> // DirextX�̐��w�֘A�̃w�b�_�[�t�@�C��
#include <DirectXCollision.h>
#include <d3d11.h>  // DirectX11���g�����߂̃w�b�_�[�t�@�C��
class DrawLine
{
public:
	DirectX::XMFLOAT3 Vertices[8];

private:
	DrawLine() {}
	~DrawLine() {}

public :
	static DrawLine* Get();

	//�X�V����
	void Update();

	//�`�揈��
	void Draw(const DirectX::BoundingBox& bbox);

	void GetBoundingBoxVertices(const DirectX::BoundingBox& bbox);
};

