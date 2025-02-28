#include "Animation.h"

/**
 *�A�j���[�V�����̃R���X�g���N�^�ł��B
 */
Animation::Animation()
{
}

/**
 *�A�j���[�V�����̃f�X�g���N�^�ł��B
 */
Animation::~Animation()
{
}

/**
*�A�j���[�V�����̍X�V�������s���֐��ł��B
*/
void Animation::Update()
{
    if (!isPlaying)
    {
        return;
    }

    // �e�[�u���A�j���[�V�����c���߂�ꂽ���ԂōĐ������邽�߂̃f�[�^
    // ���e�[�u���c�����̖ړI�̂��߂ɍ��ꂽ�f�[�^�̏W�܂�
    int animTable[][32] =
    {
        // �G�t�F�N�g�p�O�`�P�P�܂ōĐ�
        {0,-1},
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -2 },
        {0,1,2,3,4,5,6,7,8,9,-2},
        {0,1,2,3,4,5,6,7,8,9,-2},
        {0,1,2,3,4,5,6,7,8,-2},
        {0,1,2,3,-2},
        {0,1,2,3,4,5,6,7,8,9,-1},
        {1,-1},
    };

    // �\��������R�}ID���擾
    int animID = animTable[animPattern][(int)animCounter];

    //// �\��������R�}��UV���v�Z
    //uv.x = animID % 10 * 0.1f;
    //uv.y = animID / 10 * 1.0f;

    // �\��������R�}��UV���v�Z
    uv.x = animID % int(GetUvSize.x) * (1 / GetUvSize.x);
    uv.y = animID / int(GetUvSize.x) * (1 / GetUvSize.y);

    // �A�j���[�V�����̃J�E���^�[��i�߂�
    animCounter += animSpeed;

    if (animTable[animPattern][(int)animCounter] == -1)
    {
        //�ŏ��ɖ߂�
        animCounter = 0.0f;
    }
    else if (animTable[animPattern][(int)animCounter] == -2)
    {
        //�A�j���[�V�����I��
        animCounter = 0.0f;
        //animSpeed = 0.0f;
        isPlaying = false;
    }
}

/**
 *UV�ړ��ʂ��擾����֐��ł��B
 * @return UV�ړ��ʂ��擾���܂�
 */
FLOAT_XY Animation::GetUV()
{
    return uv;
}

/**
 *UV�ړ��ʂ��Z�b�g����֐��ł��B
 * @param �Z�b�g������uv�ړ���
 */
void Animation::SetUv(DirectX::XMFLOAT2& _uv)
{
    GetUvSize = _uv;
}

/**
 *�A�j���[�V�����X�s�[�h���Z�b�g����֐��ł��B
 * @param �Z�b�g�������A�j���[�V�����X�s�[�h
 */
void Animation::SetAnimSpeed(float& _speed)
{
    animSpeed = _speed;
}

/**
 *�Đ�����p�^�[����ύX����֐��ł��B
 * @return �ύX��̃p�^�[��
 */
void Animation::SetPattern(int pattern)
{
    animPattern = pattern;
}

void Animation::ResetAnimation()
{
    //�A�j���[�V�����I��
    animCounter = 0.0f;
    //animSpeed = 0.0f;
    isPlaying = false;
}
