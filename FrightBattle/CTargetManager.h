#pragma once
#include <vector>
class CGameObject;
class CEnemy;
class CPlayer;

/**
 * �����CTargetManager�N���X�̐����ł��B
 * ���̃N���X�͓G���^�[�Q�b�g���鐧��Ŏg�p����܂��B
 *
 * @author �g������
 * @since 2024-7-10
 */
class CTargetManager
{
	//�G���i�[����ϐ�
	std::vector<CEnemy*> vEnemy;

	int TargetNum = 0;
	int MaxNum;

	CTargetManager() {}
	~CTargetManager() {}

public:
	static CTargetManager* Get();

	//�X�V����
	void Update();

	//�G���Z�b�g����
	void SetEnemy();

	//�^�[�Q�b�g�X�V
	void SetTargetNum();

	//�^�[�Q�b�g�ł��邩
	bool CheckIsTarget();

	CEnemy* GetTargetObj();

	size_t GetEnemyNum() { return vEnemy.size(); }
};

