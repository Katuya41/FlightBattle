#pragma once
#include "CGameObject.h"

class Animation;

/**
 * �����CEffect�N���X�̐����ł��B
 * ���̃N���X�̓G�t�F�N�g����Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CEffect :
    public CGameObject
{
private:
    Animation* anim;

    DirectX::XMFLOAT2 UV_Size;

protected:
    /**
    *UV�A�j���[�V�����̍s���Ԃ��֐��ł��B
    * 
    * @return UV�A�j���[�V�����̍s��
    */
    DirectX::XMMATRIX GetUvMatrix() override;

public:
    /**
    *CEffect�̃R���X�g���N�^�ł��B
    */
    CEffect();

    /**
    *CEffect�̃f�X�g���N�^�ł��B
    */
    ~CEffect();

    /**
    *CEffect�̍X�V�������s���֐��ł��B
    */
    void Update();

    /**
    *�A�j���[�V�����̏�Ԃ��擾����֐��ł��B
    */
    void Play();

    /**
    *�A�j���[�V�����̏�Ԃ��擾����֐��ł��B
    */
    bool GetAnimPlay();

    /**
    * �A�j���[�V�����̏�Ԃ����Z�b�g����
    */
    void ResetAnimation();

    /**
    *�A�j���[�V�����p�^�[����ݒ肷��֐��ł��B
    * 
    * @param �A�j���[�V�����p�^�[���ԍ�
    */
    void SetAnimPattern(int _num);

    /**
    *�A�j���[�V������UV�T�C�Y��ݒ肷��֐��ł��B
    * 
    * @param _u UV�T�C�Y��U
    * @param _v UV�T�C�Y��V
    */
    void SetAnimUVSize(float _u, float _v);

    /**
    * �A�j���[�V�����̃X�s�[�h��ݒ肷��֐��ł��B
    * 
    * @param  �ݒ肵�����A�j���[�V�����X�s�[�h
    */
    void SetAnimSpeed(float _animspeed);

    /**
    *�A�j���[�V������UV�T�C�Y���擾����֐��ł��B
    * 
    * @return UV�T�C�Y��Ԃ��܂��B
    */
    DirectX::XMFLOAT2 GetAnimUV() { return UV_Size; }
};

