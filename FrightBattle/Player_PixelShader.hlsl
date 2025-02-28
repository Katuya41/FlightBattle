// �s�N�Z���V�F�[�_�[

// �s�N�Z���̏��̍\���́i�󂯎��p�j
struct PS_IN
{
    float4 pos : POSITION0; // �s�N�Z���̉�ʏ�̍��W
    float2 tex : TEXCOORD0; // �s�N�Z���ɑΉ�����e�N�X�`�����W
    float4 normal : NORMAL0; // �s�N�Z���ʂ̖@���x�N�g��
};

// �O���[�o���ϐ��̐錾
// ���V�F�[�_�[�̃O���[�o���ϐ��́AC����v���O��������n���ꂽ
// �@�f�[�^���󂯎�邽�߂Ɏg���B
Texture2D myTexture : register(t0); //�e�N�X�`���[
SamplerState mySampler : register(s0); //�T���v���[

// �萔�o�b�t�@�󂯎��p
cbuffer ConstBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    
    // ���[���h�ϊ��s��
    matrix matrixWorld;
    // �@���x�N�g������]������s��
    matrix matrixWorldNormal;
    // UV�A�j���[�V�����p�s��
    matrix matrixUV;
    // �}�e���A���F
    float4 materialDiffuse;
    
    unsigned int isLighting;
    
    float3 CameraPos;
    //int dummy2, dummy3, dummy4;
}

float4 p_ps_main(PS_IN input) : SV_Target
{
     //�F���w�肵�Ă���
    if (!isLighting)
    {
        // Sample�֐����e�N�X�`������w�肵��UV�����Ƀs�N�Z���F������ė���
        float4 color = myTexture.Sample(mySampler, input.tex);
        // �e�N�X�`���F�ƃ}�e���A���F��������
        return color * materialDiffuse;
    }
    
    float4 color;

	// �e�N�X�`������F���擾
    color = myTexture.Sample(mySampler, input.tex);
    // ���̌�����\���x�N�g��
    float3 lightVector = float3(-1, -1, 0);
    float4 lightDiffuse = float4(0.8, 0.7, 0.6, 1.0); // �g�F�n�̏_�炩����
    float4 lightAmbient = float4(0.5, 0.5, 0.5, 1.0); // ���������߂�

    float3 N = normalize(input.normal); // �@���x�N�g���𐳋K��
    float3 L = normalize(lightVector.xyz); // ���C�g�̕����𐳋K��
    float diffuse = saturate(dot(N, L)); // �g�U���ˌ��̌v�Z(�@���x�N�g���ƃ��C�g�̕����̓���)

    color *= diffuse * lightDiffuse + lightAmbient; //�J���[�Ɋg�U���ˌ��Ɗ�����K�p
	// �����x�N�g���Ɣ��˃x�N�g���̌v�Z
    float3 V = normalize(CameraPos.xyz - float3(input.pos.xyz));
    float3 R = normalize(reflect(-L, N));

	// ���ʌ��̌v�Z
    float specular = saturate(dot(V, R));

	// �J���[�ɋ��ʌ������Z
    color += pow(specular, 5.0f);

	// �A���t�@�l��1.0�ɐݒ�
    color.a = 1.0f;

    return color;
}