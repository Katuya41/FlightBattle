#pragma once
#include <vector>
class CGameObject;
class CEnemy;
class CPlayer;

/**
 * これはCTargetManagerクラスの説明です。
 * このクラスは敵をターゲットする制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CTargetManager
{
	//敵を格納する変数
	std::vector<CEnemy*> vEnemy;

	int TargetNum = 0;
	int MaxNum;

	CTargetManager() {}
	~CTargetManager() {}

public:
	static CTargetManager* Get();

	//更新処理
	void Update();

	//敵をセットする
	void SetEnemy();

	//ターゲット更新
	void SetTargetNum();

	//ターゲットできるか
	bool CheckIsTarget();

	CEnemy* GetTargetObj();

	size_t GetEnemyNum() { return vEnemy.size(); }
};

