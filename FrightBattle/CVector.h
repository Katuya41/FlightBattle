#pragma once
#include "CGameObject.h"
class CEnemy;

/**
 * �����CVector�N���X�̐����ł��B
 * ���̃N���X�͓G�̕������w����󐧌�Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CVector :
    public CGameObject
{
public:
    //�^�[�Q�b�g�̍��W
    DirectX::XMFLOAT3 TargetPos;

    CEnemy* Target;

    //�x�N�g���̕���
    DirectX::XMVECTOR vDirect;
    DirectX::XMFLOAT3 fDirect;

private:

public:
    CVector();
    ~CVector(){}

    /**
    * �X�V�������s���֐��ł��B
    */
    void Update();

    /**
    * �`�揈�����s���֐��ł��B
    */
    void Draw();

    /**
    * �^�[�Q�b�g�ւ̕������v�Z����֐��ł��B
    * 
    * @return �v�Z�����^�[�Q�b�g�̕���
    */
    DirectX::XMFLOAT3 SetDirect();

    /**
    * �^�[�Q�b�g�̍��W��ݒ肷��֐��ł��B
    * 
    * @param  �^�[�Q�b�g�I�u�W�F�N�g�̍��W
    */
    void SetTargetPos(DirectX::XMFLOAT3 _targetpos) { TargetPos = _targetpos; }

    /**
    * �^�[�Q�b�g��ݒ肷��֐��ł��B
    * 
    * @param  �^�[�Q�b�g�I�u�W�F�N�g
    */
    void SetTarget(CEnemy* _target);
};

