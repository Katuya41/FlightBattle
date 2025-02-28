// �s�N�Z���V�F�[�_�[

// �s�N�Z���̏��̍\���́i�󂯎��p�j
struct PS_IN
{
    float4 pos : SV_POSITION0; // �s�N�Z���̉�ʏ�̍��W
    float2 tex : TEXCOORD0; // �s�N�Z���ɑΉ�����e�N�X�`�����W
    float4 normal : NORMAL0; // �s�N�Z���ʂ̖@���x�N�g��
    float3 wpos : POSITION0;  
};

// �O���[�o���ϐ��̐錾
// ���V�F�[�_�[�̃O���[�o���ϐ��́AC����v���O��������n���ꂽ
// �@�f�[�^���󂯎�邽�߂Ɏg���B
Texture2D myTexture : register(t0); //�e�N�X�`���[
Texture2D MaskTexture : register(t1); //�e�N�X�`���[
SamplerState mySampler : register(s0); //�T���v���[

float2 distortionStrength = float2(0.05, 0.05); // �c�݂̋���

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
    float time;
    float Dist;
    int dummy2, dummy3;
}

float4 bf_ps_main(PS_IN input) : SV_Target
{
    // ����UV���W
    float2 uv = input.tex;

    // ���I�ȃG�t�F�N�g�̂��߂̎��ԃx�[�X�̘c�ݒǉ� (�I�v�V����)
    float2 animatedOffset = float2(
        sin(time + uv.y * 10.0) * 0.005,
        cos(time + uv.x * 10.0) * 0.005
    );

    // �c�݃e�N�X�`������I�t�Z�b�g���擾
    float3 distortion = MaskTexture.Sample(mySampler, uv).rgb;

    //// UV���W�ɘc�݂�K�p (RGB�`�����l������I�t�Z�b�g���擾)
    //uv.x += (distortion.r - 0.5f * time) * Dist + animatedOffset.x;
    //uv.y += (distortion.r - 0.5f * time) * Dist + animatedOffset.y;
    
    // UV���W���I�t�Z�b�g (RGB�ňقȂ�c�݂��g�p�\)
    //uv.x += (distortion.r - 0.5f * time) * Dist;
    //uv.y += (distortion.g - 0.5f * time) * Dist;
    
    uv.x += (distortion.r - 0.5f) * Dist;
    uv.y += (distortion.g - 0.5f) * Dist;

    // �w�i�e�N�X�`�����T���v�����O
    float4 color = myTexture.Sample(mySampler, uv);

    return color;
}