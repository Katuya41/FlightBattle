#pragma once // �d���C���N���[�h�������h�~����

#include <d3d11.h>  // DirectX11���g�����߂̃w�b�_�[�t�@�C��
#include <DirectXMath.h> // DirextX�̐��w�֘A�̃w�b�_�[�t�@�C��
#include <DirectXCollision.h>

// Direct3D����̊ȗ����}�N��
#define SAFE_RELEASE(p)      { if( NULL != p ) { p->Release(); p = NULL; } }

// �����̌^����ʖ��ł��g����悤�ɂ���
typedef ID3D11ShaderResourceView* D3DTEXTURE;
typedef DirectX::XMFLOAT2 FLOAT_XY;

// Direct3D�̋@�\���J�v�Z���������N���X
class D3D
{

public:
	// �\���̂̒�`
	// �F��\��
	struct Color
	{
		float r, g, b, a;
	};
	// ���f����\��
	struct Model
	{
		//ComPtr���g�p���邱�Ƃň��S�ɉ���ł���
		//ComPtr�̓X�}�[�g�|�C���^�̈��
		//�Q�ƃJ�E���g���s�����߁A�Q�Ƃ����ׂĂȂ��Ȃ��Ă��������s��
		ID3D11Buffer* vertexBuffer = nullptr; // ���_�o�b�t�@�����_�f�[�^������
		D3DTEXTURE texture = nullptr;		  // �e�N�X�`��
		ID3D11Buffer* indexBuffer = nullptr; // �C���f�b�N�X�o�b�t�@
		size_t numIndex = 0;				 // �C���f�b�N�X��
	};
	// �萔�o�b�t�@�p�\����
	struct ConstBuffer
	{
		// ���[���h�s��
		DirectX::XMMATRIX world;

		// �r���[�s��
		DirectX::XMMATRIX view;

		// �v���W�F�N�V�����s��
		DirectX::XMMATRIX proj;

		// ���[���h�ϊ��s��
		DirectX::XMMATRIX matrixWorld;

		// �@���x�N�g������]������s��
		DirectX::XMMATRIX matrixWorldNormal;

		// UV�A�j���[�V�����s��
		DirectX::XMMATRIX matrixUV;

		// �}�e���A���F
		DirectX::XMFLOAT4 materialDiffuse;

		unsigned int isLighting;
		DirectX::XMFLOAT3 CameraPos;

		float time;
		float Dist;

		DirectX::XMFLOAT3 LightVector;
	};

	DirectX::XMFLOAT3 Vertices[8];

	// Direct3D�̏��������������s����
	HRESULT Create(HWND hwnd);
	~D3D();

	// ��ʓh��Ԃ��Ɛݒ���s���֐�
	void ClearScreen();
	// ��ʂ��X�V����֐�
	void UpdateScreen();

	// �l�p�`�|���S�����쐬���A���_�o�b�t�@�������s��
	static Model CreateSquare(FLOAT_XY center, FLOAT_XY size, FLOAT_XY uv);
	// Obj�t�@�C����ǂݍ���
	// �����F(1)�t�@�C����
	static Model LoadObjModel(const wchar_t* fileName);
	// �e�N�X�`����ǂݍ���
	// �����F(1)�t�@�C����
	static D3DTEXTURE LoadTexture(const wchar_t* fileName);
	// ���̃N���X�̗B��̃C���X�^���X��Ԃ�
	static D3D* Get();

	static DirectX::XMFLOAT3 RotateVector3(DirectX::XMFLOAT3 srcVector, DirectX::XMFLOAT3 rotation);

	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pImmediateContext; }
	ID3D11Buffer* GetConstantBuffer() { return m_pConstantBuffer; }

	//�[�x�o�b�t�@�𖳌�
	void SetDepthStencilState(int _num);

	// ���_�f�[�^�P������̃o�C�g�T�C�Y��Ԃ�
	UINT GetVertexStride();

	//�r���[�{�[�h��ݒ肷��
	void SetViewBord(int _num);

	//���ʂ��e�N�X�`����\������悤�ɐݒ�
	void SetCulling(bool _cullmode);

	//���j�A�⊮��ݒ肷��֐�
	void SetLinear();

	/**
	* �V�F�[�_�[��ύX����֐��ł��B
	* 
	* @param  _id
	* 0:�ʏ�V�F�[�_�[
	* 1:�v���C���[�V�F�[�_�[
	* 2:
	* 3:�o�b�N�t�@�C�A�V�F�[�_�[
	* 4:�A���t�@�f�B�U�V�F�[�_�[
	*/
	void ChengeShader(const int& _id);

private:
	D3D() {};

	// ��ID3D11�Ŏn�܂�|�C���^�^�̕ϐ��́A�������K�v������
	ID3D11Device* m_pDevice; // �f�o�C�X��DirectX�̊e��@�\�����
	// �R���e�L�X�g���`��֘A���i��@�\
	ID3D11DeviceContext* m_pImmediateContext;
	// �X���b�v�`�F�C�����_�u���o�b�t�@�@�\
	IDXGISwapChain* m_pSwapChain;
	// �����_�[�^�[�Q�b�g���`����\���@�\
	ID3D11RenderTargetView* m_pRenderTargetView;
	// �[�x�o�b�t�@�p�e�N�X�`��
	ID3D11Texture2D* m_pDepthStencilTexture;
	// �[�x�o�b�t�@
	ID3D11DepthStencilView* m_pDepthStencilView;

	// �C���v�b�g���C�A�E�g
	ID3D11InputLayout* m_pInputLayout;
	// ���_�V�F�[�_�[�I�u�W�F�N�g
	ID3D11VertexShader* m_pVertexShader;
	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ID3D11PixelShader* m_pPixelShader;
	// �v���C���[�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ID3D11PixelShader* m_pPlayerPixelShader;
	// �r���[�|�[�g
	D3D11_VIEWPORT          m_Viewport[2];

	// �u�����h�X�e�[�g�p�ϐ��i�A���t�@�u�����f�B���O�j
	ID3D11BlendState* m_pBlendStateAlpha;
	// �u�����h�X�e�[�g�p�ϐ��i���Z�����j
	ID3D11BlendState* m_pBlendStateAdditive;

	// �T���v���[�p�ϐ�
	ID3D11SamplerState* m_pSampler;

	// �萔�o�b�t�@�p�ϐ�
	ID3D11Buffer* m_pConstantBuffer;

};

