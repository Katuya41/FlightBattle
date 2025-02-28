#pragma once

#include "direct3d.h"

/**
 * �����Animation�N���X�̐����ł��B
 * ���̃N���X�̓A�j���[�V��������Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class Animation
{

private:
	/**
	*�A�j���[�V�����̃p�^�[��������񋓌^�ł��B
	*
	* @param �A�j���[�V�������Ȃ�
	* @param �G�t�F�N�g�A�j���[�V�����p
	*/
	enum PATTERN
	{
		NO_ANIMATION,
		FX_0_11 = 5  // �G�t�F�N�g�p�O�`�P�P�Đ�
	};

	// �����o�[�ϐ�
	//�A�j���[�V������UV�ړ��ʂ�ۑ�
	DirectX::XMFLOAT2 uv = { 0,0 };
	DirectX::XMFLOAT2 GetUvSize = { 0,0 };

	//�A�j���e�[�u���̃J�E���^�[
	float animCounter = 0.0f;

	//�Đ�����p�^�[��ID�����ϐ�
	int animPattern = 0;

	//�Đ����x��ݒ肷��
	float animSpeed = 0.1f;

	//�A�j���[�V�������Đ�����
	bool isPlaying = false;

public: 	// �����o�[�֐�//
	/**
	 *�A�j���[�V�����̃R���X�g���N�^�ł��B
	 */
	Animation();

	/**
	 *�A�j���[�V�����̃f�X�g���N�^�ł��B
	 */
	~Animation();

	/**
	 *�A�j���[�V�����̍X�V�������s���֐��ł��B
	 */
	void Update();

	/**
	 *UV�ړ��ʂ��擾����֐��ł��B
	 * @return UV�ړ��ʂ��擾���܂�
	 */
	DirectX::XMFLOAT2 GetUV();

	/**
	 *UV�ړ��ʂ��Z�b�g����֐��ł��B
	 * @param �Z�b�g������uv�ړ���
	 */
	void SetUv(DirectX::XMFLOAT2& _uv);

	/**
	 *�A�j���[�V�����X�s�[�h���Z�b�g����֐��ł��B
	 * @param �Z�b�g�������A�j���[�V�����X�s�[�h
	 */
	void SetAnimSpeed(float& _speed);

	/**
	 *�Đ�����p�^�[����ύX����֐��ł��B
	 * @return �ύX��̃p�^�[��
	 */
	void SetPattern(int pattern);

	/**
	 *�G�t�F�N�g���Đ��������擾����֐��ł��B
	 * @return �G�t�F�N�g���Đ�������Ԃ��܂��B
	 */
	bool GetIsPlaying() { return isPlaying; }

	/**
	 *�G�t�F�N�g�̏�Ԃ�ݒ肷��֐��ł�
	 * @return �G�t�F�N�g�̏�Ԃ�ݒ肵�܂�
	 */
	void SetIsPlaying(bool _isplay) { isPlaying = _isplay; }

	/**
	* �A�j���[�V�����̏�Ԃ����Z�b�g����
	*/
	void ResetAnimation();

};

