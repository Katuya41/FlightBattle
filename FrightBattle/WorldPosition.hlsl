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
	// �X�N���[�����W�Ƃ͕ʂɁA���[���h���W�̃f�[�^��p��
	// �s�N�Z���V�F�[�_�[�ɓn��ƁA�s�N�Z�����Ƃ̃��[���h���W�̃f�[�^�ɂȂ�
    float4 worldPos : POSITION0;
};

// �O���[�o���ϐ��̐錾
// �萔�o�b�t�@�󂯎��p
cbuffer ConstBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    
   // ���[���h�ϊ��s��~�r���[�ϊ��s��~���e�s��
    matrix matrixWorld;
    // �@���x�N�g������]������s��
    matrix matrixWorldNormal;
    // UV�A�j���[�V�����p�s��
    matrix matrixUV;
    // �}�e���A���F
    float4 materialDiffuse;
}

VS_OUT w_vs_main(VS_IN input)
{
    VS_OUT vout;
    vout.pos = float4(input.pos, 1.0f);
    vout.pos = mul(vout.pos, matrixWorld);
	// �s�N�Z���V�F�[�_�[�ɓn�����߂ɁA�r���ŕϐ��ɃR�s�[
    vout.worldPos = vout.pos;

    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);
    vout.tex = input.tex;
    vout.normal = mul(input.normal, (float3x3) world);
    return vout;
}