#include "CPlayerAttack.h"
#include "CPlayer.h"
#include <DirectXMath.h>
#include "CCamera.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CScene.h"
#include "CMisile.h"
#include "CShottoBall.h"
#include "CEffect.h"
#include "xa2.h"
#include "Input.h"
#include "CMisileUI.h"

#include <cassert>
#include "DebugUI.h"
#include <random>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <list>

using namespace DirectX;
using namespace std;

//�ʂ�������܂ł̎���
#define DeleteBallTime (1000)

//�ʂ̃N�[���^�C��
#define ShottoBallCoolTime (100)

//�ʂ̃X�s�[�h
#define GUN_SPEED (0.6f)

//�^�[�Q�b�g�\����
#define MAX_TARGET_DISTANCE (150)

//�v���C���[�̊e�����̔��ˉ\����
#define MAX_PLAYER_MISILETARGET_DISTANCE (150)
#define MAX_PLAYER_FOURMISILE_DISTANCE (150)
#define MAX_PLAYER_BULLETTARGET_DISTANCE (30)

//�^�[�Q�b�g��������
#define TARGET_TIME (500)

CPlayerAttack::GUNENEMYSTARTPOS CPlayerAttack::GetGunENemyStartPos()
{
	return GunENemyStartPos;
}

void CPlayerAttack::AddGunEnemyStartPos()
{
	GunENemyStartPos = static_cast<GUNENEMYSTARTPOS>(static_cast<int>(GunENemyStartPos) + 1);
}

void CPlayerAttack::InsertEnemyParameter()
{
	//�t�@�C������p�����[�^��ǂݍ���
	//��s�ڂ͐����Ȃ̂Ŗ���
	//�����p�̕ϐ���ݒ肵�Ă���(�\���̂�)

	// �ǂݍ��ރt�@�C���̖��O

	std::ifstream file("Parameter/Enemy.txt");

	//�t�@�C���I�[�v��
	if (!file.is_open()) {
		// �ǂݍ��݂Ɏ��s�����ꍇ
		MessageBoxA(NULL, "Miss Open File", "�G���[",
			MB_OK | MB_ICONERROR);
	}

	bool IsInsert = false;

	//�t�@�C�����I���܂œǂݍ���
	//�t�@�C���̖������ǂ�������
	while (!file.eof())
	{
		string bufferstream;

		//�t�@�C���̑Ώۂ̍s�̒��g��bufferstream�ɓ����
		file >> bufferstream;
		//getline�֐����g�����߂ɕϊ�
		istringstream stream(bufferstream);

		string line;

		IsInsert = false;

		//���g���K���}�ŋ�؂��Ă���
		while (getline(stream, line, ','))
		{
			// �����񒷂��v�Z
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);

			// ���C�h������p�̃o�b�t�@���m��
			wstring wstr(bufferSize, L'\0');  // �k�������ŏ�����

			switch (EnemyParamTag)
			{
			case CPlayerAttack::ENEMY_PARAMS_TAG::HOLD:EnemyParameter.ThresHold = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENEMYMAXSPEED:EnemyParameter.Fright_MaxSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FRIGHT_MAX_SPEED:EnemyParameter.Follow_MaxSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FOLLOW_MAX_SPEED:EnemyParameter.Entourage_MaxSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGE_MAX_SPEED:EnemyParameter.Entourage_MaxSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::BOSS_MAX_SPEED:EnemyParameter.Boss_MaxSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ADDSPEED:EnemyParameter.AddSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FRIGHTADDSPEED:EnemyParameter.Fright_AddSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FOLLOWADDSPEED:EnemyParameter.Follow_AddSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGEADDSPEED:EnemyParameter.Entourage_AddSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::BOSSADDSPEED:EnemyParameter.Boss_AddSpeed = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FRIGHTMAXROTATE:EnemyParameter.Fright_MaxRotate = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FOLLOWMAXROTATE:EnemyParameter.Follow_MaxRotate = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGEMAXROTATE:EnemyParameter.Entourage_MaxRotate = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::BOSSMAXROTATE:EnemyParameter.Boss_MaxRotate = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::FRIGHTENEMYHP:EnemyParameter.FrightEnemy_HP = std::stoi(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::GROUNDENEMYHP:EnemyParameter.GroundEnemy_HP = std::stoi(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGEENEMYHO:EnemyParameter.EntourageEnemy_HP = std::stoi(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::BOSSENEMYHP:EnemyParameter.BossEnemy_HP = std::stoi(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::GROUNDENEMY_ATTACKTIME:EnemyParameter.GroundEnemy_AttackTime = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::MAXATTACKDISTANCE:EnemyParameter.Max_Attack_Distance = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::TARGETTIME:EnemyParameter.Target_Time = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ATTACKCOOLTIME:EnemyParameter.Attack_CoolTime = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::INSERTENtoURAGENUM:EnemyParameter.InsertEntourageNum = std::stoi(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGECHANGEPOSTIME:EnemyParameter.Entourage_ChengePos_Time = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGEATTACKCOOLTIME:EnemyParameter.Entourage_Attack_CoolTime = std::stof(line); break;
			case CPlayerAttack::ENEMY_PARAMS_TAG::ENTOURAGEMOVEPOSDIS:EnemyParameter.Entourage_Move_Pos_Dis = std::stof(line); break;
			}
			EnemyParamTag = static_cast<ENEMY_PARAMS_TAG>(static_cast<int>(EnemyParamTag) + 1);
		}
		EnemyParamTag = HOLD;
	}

	file.close();
}

void CPlayerAttack::InsertMisileParameter()
{
	//�t�@�C������p�����[�^��ǂݍ���
//��s�ڂ͐����Ȃ̂Ŗ���
//�����p�̕ϐ���ݒ肵�Ă���(�\���̂�)

// �ǂݍ��ރt�@�C���̖��O

	std::ifstream file("Parameter/Misile.txt");

	//�t�@�C���I�[�v��
	if (!file.is_open()) {
		// �ǂݍ��݂Ɏ��s�����ꍇ
		MessageBoxA(NULL, "Miss Open File", "�G���[",
			MB_OK | MB_ICONERROR);
	}

	bool IsInsert = false;

	//�t�@�C�����I���܂œǂݍ���
	//�t�@�C���̖������ǂ�������
	while (!file.eof())
	{
		string bufferstream;

		//�t�@�C���̑Ώۂ̍s�̒��g��bufferstream�ɓ����
		file >> bufferstream;
		//getline�֐����g�����߂ɕϊ�
		istringstream stream(bufferstream);

		string line;

		IsInsert = false;

		//���g���K���}�ŋ�؂��Ă���
		while (getline(stream, line, ','))
		{
			// �����񒷂��v�Z
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);

			// ���C�h������p�̃o�b�t�@���m��
			wstring wstr(bufferSize, L'\0');  // �k�������ŏ�����

			switch (MisileParamTag)
			{
			case CPlayerAttack::MISILE_PARAMS_TAG::MAXROTATE:MisileParameter.MaxRotate = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::MINROTATE:MisileParameter.MinRotate = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::ENEMYMINROTATE:MisileParameter.Enemy_Min_Rotate = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::ENEMYMAXROTATE:MisileParameter.Enemy_Max_Rotate = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::MINSPEED:MisileParameter.Min_Speed = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::MISILEMAXSPEED:MisileParameter.Max_Speed = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::MINADDSPEED:MisileParameter.Min_Add_Speed = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::MAXADDSPEED:MisileParameter.Max_Add_Speed = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::ENEMYMINADDSPEED:MisileParameter.Enemy_Min_Add_Speed = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::ENEMYMAXADDSPEED:MisileParameter.Enemy_Max_Add_Speed = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::DELETEMISILETIME:MisileParameter.Delete_Misile_Time = std::stoi(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::PARTICLECOOLTIME:MisileParameter.Particle_Cooltime = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::PARTICLEOUTTIME:MisileParameter.Particle_OutTime = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::PARTICLEADDSCALE:MisileParameter.Particle_AddScale = std::stof(line); break;
			case CPlayerAttack::MISILE_PARAMS_TAG::MISILECHARGESPEED:MisileParameter.Misile_Charge_Speed = std::stof(line); break;
			}
			MisileParamTag = static_cast<MISILE_PARAMS_TAG>(static_cast<int>(MisileParamTag) + 1);
		}
	}

	file.close();
}

CPlayerAttack::CPlayerAttack()
{
	if (!CreateEnemyFrame)
	{
		// �f�o�b�O�\���֐��o�^
		EnemyFrame = DebugUI::RedistDebugFunction([&]() {
			DrawTargetLine();
			});
		CreateEnemyFrame = true;
	}

	//�G�ƃ~�T�C���̃p�����[�^���t�@�C������擾����
	InsertEnemyParameter();
	InsertMisileParameter();

	GunENemyStartPos = VerTex1;
}

CPlayerAttack::~CPlayerAttack()
{
	vuMisile.clear();

	//delete pBossEnemy;
	delete pInitEnemy;

	lTargetEnemys.clear();

	for (auto& Enemy : vEnemys) {
		if (Enemy != nullptr) {
			if (Enemy->Enemy != nullptr) {
				delete Enemy->Enemy;  // RESENEMY �̓��������������
				Enemy->Enemy = nullptr;
			}
			delete Enemy;  // RESENEMY ���̂����
			Enemy = nullptr;
		}
	}
	vEnemys.clear();
}

CPlayerAttack* CPlayerAttack::Get()
{
	static CPlayerAttack instance;
	return &instance;
}

void CPlayerAttack::DrawTargetLine()
{
	if (CScene::Get()->GetNowScene() == CScene::SCENE::STAGE || CScene::Get()->GetNowScene() == CScene::SCENE::TEST_STAGE)
	{
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			if ((*it)->Enemy->GetActive() && (*it)->Enemy->GetIsCameraRef())
			{
				// ���[���h���W���x�N�g���`���ɕϊ�
				XMVECTOR worldPosition = XMLoadFloat3(&(*it)->Enemy->Position);

				// �r���[�s��ƃv���W�F�N�V�����s����|�����킹��
				XMMATRIX viewProj = XMMatrixMultiply(CGame::Get()->GetCamera3D()->GetViewMatrix(), CGame::Get()->GetCamera3D()->GetProjectionMatrix());

				// ���[���h���W���N���b�v��Ԃɕϊ�
				XMVECTOR clipSpacePos = XMVector3TransformCoord(worldPosition, viewProj);

				// �N���b�v��ԍ��W�𐳋K���f�o�C�X���W�ɕϊ� (-1 �` 1 �͈̔�)
				float clipX = XMVectorGetX(clipSpacePos);
				float clipY = XMVectorGetY(clipSpacePos);
				float clipZ = XMVectorGetZ(clipSpacePos); // �����̎擾

				// ���K���f�o�C�X���W���X�N���[�����W�ɕϊ�
				XMFLOAT2 screenPos;
				screenPos.x = (clipX + 1.0f) * 0.5f * 1264.0f;
				screenPos.y = (1.0f - clipY) * 0.5f * 681.0f; // Y���͋t����

				// UI��`�� (��: ImGui ���g�p)
				float rectSize = 30.0f; // �g�̃T�C�Y
				rectSize = rectSize * (1.0f / clipZ); // �X�P�[�����O��̘g�T�C�Y
				rectSize = std::clamp(rectSize, 30.0f, 100.0f); // �ŏ�10, �ő�100�̐���
				DirectX::XMFLOAT2 EnemyFrameLeft = { screenPos.x - rectSize * 0.5f,screenPos.y - rectSize * 0.5f };
				DirectX::XMFLOAT2 EnemyFrameRight = { screenPos.x + rectSize * 0.5f, screenPos.y + rectSize * 0.5f };

				if ((*it)->Enemy->GetIsTarget())
				{
					if ((*it)->Enemy->GetCanAttack())
					{
						ImVec2 Vertex[4];
						Vertex[0] = ImVec2(screenPos.x, screenPos.y - rectSize / 2.0f); // �㒸�_
						Vertex[1] = ImVec2(screenPos.x + rectSize / 2.0f, screenPos.y); // �E���_
						Vertex[2] = ImVec2(screenPos.x, screenPos.y + rectSize / 2.0f); // �����_
						Vertex[3] = ImVec2(screenPos.x - rectSize / 2.0f, screenPos.y); // �����_
						ImGui::GetForegroundDrawList()->AddPolyline(Vertex, 4, IM_COL32(255, 0, 0, 255), true, 1.0f);

						ImGui::GetForegroundDrawList()->AddRect(ImVec2(EnemyFrameLeft.x, EnemyFrameLeft.y),
							ImVec2(EnemyFrameRight.x, EnemyFrameRight.y), IM_COL32(255, 0, 0, 255));
					}
					else
					{
						ImGui::GetForegroundDrawList()->AddRect(ImVec2(EnemyFrameLeft.x, EnemyFrameLeft.y),
							ImVec2(EnemyFrameRight.x, EnemyFrameRight.y), IM_COL32(0, 0, 0, 255));
					}
				}
				else
				{
					(*it)->Enemy->SetCanAttack(false);
					ImGui::GetForegroundDrawList()->AddRect(ImVec2(EnemyFrameLeft.x, EnemyFrameLeft.y),
						ImVec2(EnemyFrameRight.x, EnemyFrameRight.y), IM_COL32(0, 255, 0, 255));
				}
			}
			else
			{
				(*it)->Enemy->SetIsTarget(false);
				(*it)->Enemy->SetCanAttack(false);
			}
		}
	}
}

void CPlayerAttack::Update()
{
	//�����̓G���^�[�Q�b�g����ꍇ�A��ɗD��x�`�F�b�N������
	if (!IsOneTarget)
	{
		SetTarget(false);
	}

	//�^�[�Q�b�g�X�V����
	TargetUpdate();

	//�~�T�C���X�V����
	MisileUpdate();
}

void CPlayerAttack::Draw()
{
	MisileDraw();

	MisileParticleDraw();
}

void CPlayerAttack::MisileUpdate()
{
	//�~�T�C���̍X�V����
	for (auto i = vuMisile.begin(); i != vuMisile.end(); i++)
	{
		if ((*i)->GetMisileEnd())
		{
			vuMisile.erase(i);
			break;
		}
		else
		{
			(*i)->Update();
		}
	}
}

void CPlayerAttack::TargetUpdate()
{
	for (auto it = lTargetEnemys.begin(); it != lTargetEnemys.end(); it++)
	{
		if ((*it)->GetHp() > 0)
		{
			(*it)->SetIsTarget(true);
		}
		else
		{
			SetTarget(true);
			break;
		}
	}

	TargetNum = int(lTargetEnemys.size());
}

void CPlayerAttack::EnemyUpdates()
{
	IsBreak = false;
	bool IsRockOn = false;
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		if ((*it)->Enemy != nullptr && (*it) != nullptr)
		{
			if ((*it)->Enemy->GetActive() || (*it)->Enemy->GetTag() == CGameObject::TAG::INITENEMY)
			{
				(*it)->Enemy->Update();

				if (IsBreak)
				{
					break;
				}

				if ((*it)->Enemy->GetCanAttack())
				{
					IsRockOn = true;
				}
			}
			else
			{
				if (IsOneTarget && (*lTargetEnemys.begin()) == (*it)->Enemy)
					SetTarget(true);
				else if (!IsOneTarget)
				{
					for (auto itTarget = lTargetEnemys.begin(); itTarget != lTargetEnemys.end(); itTarget++)
					{
						if ((*itTarget) == (*it)->Enemy)
						{
							lTargetEnemys.erase(itTarget);
							break;
						}
					}

					SetTarget(true);
				}

				(*it)->Enemy->SetActive(false);
				CGame::Get()->GetPlayer()->DeleteMoveTargetList((*it)->Enemy);

				if ((*it) != nullptr) {
					if ((*it)->Enemy != nullptr) {
						delete (*it)->Enemy;  // RESENEMY �̓��������������
						(*it)->Enemy = nullptr;
					}
					delete (*it);  // RESENEMY ���̂����
					(*it) = nullptr;
				}
				vEnemys.erase(it);
				break;
			}
		}
	}

	if (IsRockOn)
	{
		if (!IsPlayRockOnSE)
		{
			XA_Play(SE_BUZZER, CScene::Get()->GetSeVolume(), 2.0f);
			IsPlayRockOnSE = true;
		}
	}
	else
	{
		XA_Stop(SE_BUZZER);
		IsPlayRockOnSE = false;
	}

	//�@�֏e�G��p�X�V
	CheckGunEnemysPass();

}

void CPlayerAttack::MisileDraw()
{
	for (auto it = vuMisile.begin(); it != vuMisile.end(); it++)
	{
		(*it)->Draw();

		if (CScene::Get()->GetIsDrawCollider())
		{
			(*it)->DrawColliderObject();
		}
	}
}

void CPlayerAttack::MisileParticleDraw()
{
	for (auto it = vuMisile.begin(); it != vuMisile.end(); it++)
	{
		(*it)->DrawParticle();
	}
}

void CPlayerAttack::EnemysDraw()
{
	//�G��`��
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		(*it)->Enemy->Draw();
	}

	//�G�̃f�o�b�O�p�����蔻��I�u�W�F�N�g��`��
	if (CScene::Get()->GetIsDrawCollider())
	{
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			(*it)->Enemy->DrawColliderObject();
		}
	}
}

void CPlayerAttack::MiniMapEnemyDraw()
{
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		(*it)->Enemy->DrawMiniMapObject();
	}
}

void CPlayerAttack::MiniMapMisileDraw()
{
	for (auto it = vuMisile.begin(); it != vuMisile.end(); it++)
	{
		(*it)->DrawMiniMapObject();
	}
}

void CPlayerAttack::AttackTarget(CGameObject* _spornobj)
{
	switch (AttackId)
	{
	case CPlayerAttack::MISILE:
		//�U���\�ł���A�U���͈͓���
		if ((*lTargetEnemys.begin())->GetCanAttack() && MAX_PLAYER_MISILETARGET_DISTANCE > (*lTargetEnemys.begin())->GetPlayerDistance())
			AttackMisile(_spornobj, (*lTargetEnemys.begin()), true);
		else
			AttackMisile(_spornobj, (*lTargetEnemys.begin()), false);
		break;
	case CPlayerAttack::ONETARGET_FOURMISILE:
		//�U���\�ł���A�U���͈͓���
		if ((*lTargetEnemys.begin())->GetCanAttack() && MAX_PLAYER_MISILETARGET_DISTANCE > (*lTargetEnemys.begin())->GetPlayerDistance())
		{
			if (CScene::Get()->CheckFourMisileActive() == 4)
			{
				for (int i = 1; i < 5; i++)
					AttackFourMisile(_spornobj, (*lTargetEnemys.begin()), i, true,false);

				//�~�T�C��UI�̏�Ԃ�ύX
				CScene::Get()->SetMisileUICharge(4);
			}

		}
		else
		{
			if (CScene::Get()->CheckFourMisileActive() == 4)
			{
				for (int i = 1; i < 5; i++)
					AttackFourMisile(_spornobj, (*lTargetEnemys.begin()), i, false,false);

				//�~�T�C��UI�̏�Ԃ�ύX
				CScene::Get()->SetMisileUICharge(4);
			}
		}
		break;
	case CPlayerAttack::FOURTARGET_FOURMISILE:
		int Num = 0;
		for (auto It = lTargetEnemys.begin(); It != lTargetEnemys.end(); It++)
		{
			//�U���\�ł���A�U���͈͓���
			if ((*It)->GetCanAttack() && MAX_PLAYER_MISILETARGET_DISTANCE > (*It)->GetPlayerDistance())
			{
				AttackFourMisile(_spornobj, (*It), Num, true,true);
			}
			else
			{
				AttackFourMisile(_spornobj, (*It), Num, false,true);
			}
			Num++;
		}
		break;
	}
}

void CPlayerAttack::AttackMisile(CGameObject* _spornobj, CEnemy* _target, bool _moveenemy)
{
	//�擾�����~�T�C��UI��nullptr�Ȃ�łĂȂ�
	CMisileUI* MisileUI = CScene::Get()->GetActiveMisileUITest(false);
	if (MisileUI == nullptr)
	{
		return;
	}

	//CMisileUI* MisileUI = CScene::Get()->GetActiveMisileUI();

	unique_ptr<CMisile> resunique;
	resunique = make_unique<CMisile>();
	resunique->SetModel(MisileModel);							//���f��
	resunique->SetMiniMapModel(MiniMapMisileModel);			//�~�j�}�b�v�̃��f��
	resunique->SetParticleModel(MisileParticleModel);			//�~�T�C���̃p�[�e�B�N���̃��f��
	resunique->Scaling = { 0.2f, 0.2f, 0.2f };				//�傫���ݒ�
	resunique->SetTag(CGameObject::MISILE);					//�^�O�̐ݒ�
	resunique->SetMisileState(CMisile::MISILE_STATE::READY);
	resunique->SetUv_Size(MisileHitUVSize);
	resunique->SetIsMoveEnemy(_moveenemy);

	resunique->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -0.5f,
					   _spornobj->Position.y + _spornobj->GetUp().y * -0.5f,
					   _spornobj->Position.z + _spornobj->GetUp().z * -0.5f });				//�X�|�[��������W

	resunique->SetSpornRotate(_spornobj->Rotation);			//�p�x�ݒ�
	resunique->SetInitRotateMatrix();							//�����s��ݒ�
	resunique->SetVector();									//���݂�vector�v�Z
	resunique->SetMoveSpeed(_spornobj->GetMoveSpeed());		//�������x
	resunique->SetTarget(_target);							//�^�[�Q�b�g���擾
	resunique->SetShotTime(GetTickCount64());					//���Ԏ����̐ݒ�
	resunique->SetMisileParticleTime(GetTickCount64());
	resunique->SetIsShotPlayer();								//�������I�u�W�F�N�g���v���C���[��
	resunique->SetHitMisileEffect(MisileHitModel);			//�~�T�C���̓����������̃G�t�F�N�g�e�N�X�`��
	XA_Play(SE_MISILESHOTTO, CScene::Get()->GetSeVolume(), 1.0f);

	////�~�T�C��UI�̔�A�N�e�B�u��
	//MisileUI->materialDiffuse.w = 0.5f;
	//MisileUI->SetMisileShotTime(GetTickCount64());

	MisileUI->SetMisileUIStateCharge();

	vuMisile.push_back(move(resunique));

}

void CPlayerAttack::AttackFourMisile(CGameObject* _spornobj, CEnemy* _target, int _misilenum, bool _moveenemy, bool _checkmisileui)
{
	//�~�T�C���̍��W�p�ϐ�
	float ResNum = 0.0f;
	for (int i = 0; i < _misilenum; i++)
		ResNum += 0.5f;

	if (_checkmisileui)
	{
		//�擾�����~�T�C��UI��nullptr�Ȃ�łĂȂ�
		CMisileUI* MisileUI = CScene::Get()->GetActiveMisileUITest(true);
		if (MisileUI == nullptr)
		{
			return;
		}
		else
		{
			MisileUI->SetMisileUIStateCharge();
		}
	}


	//CMisile* res = new CMisile();
	unique_ptr<CMisile> res;
	res = make_unique<CMisile>();
	res->SetModel(MisileModel);							//���f��
	res->SetMiniMapModel(MiniMapMisileModel);			//�~�j�}�b�v�̃��f��
	res->SetParticleModel(MisileParticleModel);			//�~�T�C���̃p�[�e�B�N���̃��f��
	res->Scaling = { 0.2f, 0.2f, 0.2f };				//�傫���ݒ�
	res->SetTag(CGameObject::MISILE);					//�^�O�̐ݒ�
	res->SetMisileState(CMisile::MISILE_STATE::READY);
	res->SetUv_Size(MisileHitUVSize);
	res->SetIsMoveEnemy(_moveenemy);

	//�~�T�C���̏o���ʒu�����炷
	if (_misilenum < 3)
	{
		res->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -0.5f + (_spornobj->GetRight().x * 0.5f),
						   _spornobj->Position.y + _spornobj->GetUp().y * -0.5f + (_spornobj->GetRight().y * 0.5f),
						   _spornobj->Position.z + _spornobj->GetUp().z * -0.5f + (_spornobj->GetRight().z * 0.5f) });				//�X�|�[��������W
	}
	else
	{
		res->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -0.5f + (_spornobj->GetRight().x * -0.5f),
						   _spornobj->Position.y + _spornobj->GetUp().y * -0.5f + (_spornobj->GetRight().y * -0.5f),
						   _spornobj->Position.z + _spornobj->GetUp().z * -0.5f + (_spornobj->GetRight().z * -0.5f) });				//�X�|�[��������W
	}

	//�~�T�C���̏����p�x��ύX
	DirectX::XMFLOAT3 ResRotate = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 ResRight = _spornobj->GetRight();
	DirectX::XMFLOAT3 ResUp = _spornobj->GetUp();
	XMMATRIX ResMatrix = _spornobj->GetRotateMatrix();
	XMVECTOR Qr;
	XMMATRIX Qmt;
	float AngleX = 0.0f;
	float AngleY = 0.0f;

	if (AttackId == ONETARGET_FOURMISILE)
	{
		if (_misilenum % 2 == 0)
		{
			AngleX = 25.0f;
			//�N�H�[�^�j�I���ł̉�]
			Qr = XMQuaternionRotationAxis(XMLoadFloat3(&ResRight), XMConvertToRadians(AngleX));
			Qmt = XMMatrixRotationQuaternion(Qr);
		}
		else
		{
			if (_misilenum == 1)
				AngleY = 25.0f;
			else if (_misilenum == 3)
				AngleY = -25.0f;
			//�N�H�[�^�j�I���ł̉�]
			Qr = XMQuaternionRotationAxis(XMLoadFloat3(&ResUp), XMConvertToRadians(AngleY));
			Qmt = XMMatrixRotationQuaternion(Qr);
		}

		ResMatrix *= Qmt;
		ResRotate = _spornobj->ExtractRotationFromMatrix(ResMatrix);
		res->SetSpornRotate(ResRotate);		//�p�x�ݒ�
	}
	else if(AttackId == FOURTARGET_FOURMISILE)
	{
		res->SetSpornRotate(_spornobj->Rotation);		//�p�x�ݒ�
	}

	res->SetInitRotateMatrix();							//�����s��ݒ�
	res->SetVector();									//���݂�vector�v�Z
	res->SetMoveSpeed(_spornobj->GetMoveSpeed());		//�������x
	res->SetTarget(_target);							//�^�[�Q�b�g���擾
	res->SetShotTime(GetTickCount64());					//���Ԏ����̐ݒ�
	res->SetIsShotPlayer();								//�������I�u�W�F�N�g���v���C���[��
	res->SetMisileParticleTime(GetTickCount64());
	res->SetHitMisileEffect(MisileHitModel);			//�~�T�C���̓����������̃G�t�F�N�g�e�N�X�`��
	XA_Play(SE_MISILESHOTTO, CScene::Get()->GetSeVolume(), 1.0f);

	vuMisile.push_back(move(res));
}

void CPlayerAttack::SetMisileInf(D3D::Model _misilemodel, D3D::Model _misilhitemodel, D3D::Model _minimapmisilemodel, DirectX::XMFLOAT2 _misilehituvsize)
{
	MisileModel = _misilemodel;
	MisileHitModel = _misilhitemodel;
	MisileHitUVSize = _misilehituvsize;
	MiniMapMisileModel = _minimapmisilemodel;
}

void CPlayerAttack::SetBallInf(D3D::Model _ballmodel, D3D::Model _ballhitemodel, DirectX::XMFLOAT2 _ballhituvsize)
{
	GunModel = _ballmodel;
	GunHitModel = _ballhitemodel;
	GunHitUVSize = _ballhituvsize;
}

void CPlayerAttack::InsertEnemys(CEnemy* _enemy)
{
	//unique_ptr<RESENEMY> ResEnemy = make_unique<RESENEMY>();
	RESENEMY* ResEnemy = new RESENEMY();
	ResEnemy->Enemy = _enemy;
	ResEnemy->FastPoint = 0;
	vEnemys.push_back(move(ResEnemy));
	if (lTargetEnemys.empty())
	{
		lTargetEnemys.push_back(_enemy);
		pInitEnemy = _enemy;
	}

	//if (_enemy->GetTag() == CGameObject::TAG::BOSS_ENEMY)
	//{
	//	pBossEnemy = _enemy;	
	//}

	if (!CreateEnemyFrame)
	{
		// �f�o�b�O�\���֐��o�^
		EnemyFrame = DebugUI::RedistDebugFunction([&]() {
			DrawTargetLine();
			});
		CreateEnemyFrame = true;
	}

	return;
}

void CPlayerAttack::ClearEnemys()
{
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		(*it)->Enemy->SetActive(false);
		delete (*it)->Enemy;
		(*it)->Enemy = nullptr;
	}
	vEnemys.clear();
}

void CPlayerAttack::SetTarget(bool _resettime)
{
	//�z��쐬
	//���̂Ȃ��ɗD��x�̓������G�̏�������
	std::vector<RESENEMY*> vResEnemys;
	//std::vector<std::unique_ptr<RESENEMY>> vResEnemys;
	InsertResEnemys(vResEnemys);

	//

	//�D��x�ō~���ɂ���(����ŗD��x���������̂��ŏ��̂ق��ɂł���)
	std::sort(vResEnemys.begin(), vResEnemys.end(), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) { return _enemy1->FastPoint > _enemy2->FastPoint; });

	//std::sort(vResEnemys.begin(), vResEnemys.end(), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) 
	//	{ 
	//		if(_enemy1->FastPoint != _enemy2->FastPoint)
	//			return _enemy1->FastPoint > _enemy2->FastPoint; 
	//	
	//		return _enemy1->Enemy->GetToPlayerRadius() < _enemy1->Enemy->GetToPlayerRadius();
	//	});

	//�ŏ��ƍŌ�̃C�e���[�^���擾
	auto ResStartIt = vResEnemys.begin();
	auto ResEndIt = vResEnemys.end();

	for (auto it = vResEnemys.begin(); it != vResEnemys.end(); it++)
	{
		if ((*it)->FastPoint == 4)
		{
			ResEndIt = it;
		}
		else
		{
			break;
		}
	}

	//�D��x��5,4�̕����J��������̃x�N�g���Ń\�[�g
	if (ResEndIt != vResEnemys.end())
		std::sort(ResStartIt, std::next(ResEndIt), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) { return _enemy1->Enemy->GetToPlayerRadius() < _enemy2->Enemy->GetToPlayerRadius(); });
	else
		std::sort(ResStartIt, vResEnemys.end(), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) { return _enemy1->Enemy->GetToPlayerRadius() < _enemy2->Enemy->GetToPlayerRadius(); });

	//4�~�T�C���U���̎��A�^�[�Q�b�g�̗D��x��4�����̎��A�^�[�Q�b�g���O��
	if (!IsOneTarget)
	{
		//���ׂẴ^�[�Q�b�g��Ԃ̓G�̂����A�D��x���Ⴂ���̂��^�[�Q�b�g��Ԃ���O��
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			if ((*it)->Enemy->GetIsTarget())
			{
				if ((*it)->FastPoint < 4)
				{
					(*it)->Enemy->SetIsTarget(false);
					(*it)->Enemy->SetCanAttack(false);
					(*it)->Enemy->SetTargetMiniMapObjectModel(false);
				}
			}
		}

		//���ׂẴ^�[�Q�b�g��Ԃ̓G�̂����A�D��x���Ⴂ���̂��^�[�Q�b�g��Ԃ���O��
		for (auto it = lTargetEnemys.begin(); it != lTargetEnemys.end();)
		{
			if (!(*it)->GetIsTarget())
			{
				it = lTargetEnemys.erase(it);
				continue;
			}

			++it;
		}
	}

	//���݂̂��ׂĂ̓G���^�[�Q�b�g����Ă��邩�ƍU���\�����m�F
	//�^�[�Q�b�g�z��̓G�ƓG�z��̓G���ׁA
	//�^�[�Q�b�g����Ă���̂Ƀ^�[�Q�b�g�z��̒��g�ɂȂ��ꍇ�ɂȂ���
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		bool End = false;

		if ((*it)->Enemy->GetIsTarget() || (*it)->Enemy->GetCanAttack())
		{
			for (auto itTarget = lTargetEnemys.begin(); itTarget != lTargetEnemys.end(); itTarget++)
			{
				//���̒��Ń^�[�Q�b�g�z��Ɠ����Ȃ甲����
				if ((*it)->Enemy == (*itTarget))
				{
					End = true;
					break;
				}
			}

			if (End)
				break;

			//�����ɗ�����A�^�[�Q�b�g��Ԗ��͍U���\��Ԃ����ǁA
			//�^�[�Q�b�g�z��ɂȂ�����
			(*it)->Enemy->SetIsTarget(false);
			(*it)->Enemy->SetCanAttack(false);
			(*it)->Enemy->SetTargetMiniMapObjectModel(false);
		}
	}

	//�^�[�Q�b�g�����Z�b�g���邩
	if (_resettime)
	{
		//�^�[�Q�b�g�����G�̃^�[�Q�b�g���Ԃ����Z�b�g
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			//�^�[�Q�b�g�̏ꍇ
			if ((*it)->Enemy->GetIsTarget())
			{
				if (IsOneTarget)
				{
					(*it)->Enemy->SetIsTarget(false);
					(*it)->Enemy->SetCanAttack(false);
					(*it)->Enemy->OldTargetTime = GetTickCount64();
					(*it)->Enemy->TargetTime = 0;
					break;
				}
				else
				{
					(*it)->Enemy->SetIsTarget(false);
					(*it)->Enemy->SetCanAttack(false);
					(*it)->Enemy->OldTargetTime = GetTickCount64();
					(*it)->Enemy->SetTargetMiniMapObjectModel(false);
					(*it)->Enemy->TargetTime = 0;
				}
			}
		}

		if (!IsOneTarget)
		{
			lTargetEnemys.clear();
		}
	}

	//���݂̃^�[�Q�b�g����ݒ�
	TargetNum = int(lTargetEnemys.size());

	//�^�[�Q�b�g�\�����P�̂�������
	if (IsOneTarget)
	{
		//�^�[�Q�b�g�z��ɗv�f�������Ă��邩
		if (TargetNum > 0)
		{
			//���łɃ^�[�Q�b�g���Ă�����
			auto It = lTargetEnemys.begin();

			//�D��x���������̂���^�[�Q�b�g����Ă��邩�𒲂ׂ�
			for (auto it = vResEnemys.begin(); it != vResEnemys.end(); it++)
			{
				//�^�[�Q�b�g����Ă��邩
				if (!(*it)->Enemy->GetIsTarget())
				{
					//���^�[�Q�b�g���^�[�Q�b�g����O��
					(*It)->SetTargetMiniMapObjectModel(false);
					(*It)->SetIsTarget(false);
					(*It)->SetCanAttack(false);

					//�V�^�[�Q�b�g���^�[�Q�b�g�z��ɐݒ�
					(*It) = (*it)->Enemy;

					//�V�^�[�Q�b�g���^�[�Q�b�g�Ƃ��Đݒ�
					(*It)->SetIsTarget(true);
					(*It)->SetTargetMiniMapObjectModel(true);
					TargetNum = int(lTargetEnemys.size());
					break;
				}
			}
		}
		else
		{
			//�V�^�[�Q�b�g���^�[�Q�b�g�z��ɐݒ�
			(*vResEnemys.begin())->Enemy->SetIsTarget(true);
			(*vResEnemys.begin())->Enemy->SetTargetMiniMapObjectModel(true);
			lTargetEnemys.push_back((*vResEnemys.begin())->Enemy);
			TargetNum = int(lTargetEnemys.size());
		}

		//�^�[�Q�b�g���C���쐬
		CGame::Get()->GetPlayer()->InsertMovetargetUI((*lTargetEnemys.begin()));
	}
	else
	{
		for (auto it = vResEnemys.begin(); it != vResEnemys.end(); it++)
		{
			//�D��x��4�ȏ�̂��̂̂ݑΏ�
			if ((*it)->FastPoint >= 4)
			{
				if (!(*it)->Enemy->GetIsTarget())
				{
					if (TargetNum >= 4)
						break;

					(*it)->Enemy->SetIsTarget(true);
					(*it)->Enemy->OldTargetTime = GetTickCount64();
					(*it)->Enemy->TargetTime = GetTickCount64();
					(*it)->Enemy->SetTargetMiniMapObjectModel(true);
					lTargetEnemys.push_back((*it)->Enemy);
					TargetNum = int(lTargetEnemys.size());

					CGame::Get()->GetPlayer()->InsertMovetargetUI((*it)->Enemy);
				}
			}
			else
			{
				break;
			}
		}

		//�����G�����Ȃ��ꍇ�AInitEnemy���^�[�Q�b�g�ɓ����
		if (TargetNum == 0)
		{
			pInitEnemy->OldTargetTime = GetTickCount64();
			pInitEnemy->TargetTime = GetTickCount64();
			pInitEnemy->SetTargetMiniMapObjectModel(true);
			lTargetEnemys.push_back(pInitEnemy);
			TargetNum = int(lTargetEnemys.size());
		}
	}
}

void CPlayerAttack::InsertResEnemys()
{
	////���g���N���A
	//for (int i = 0; i < 6; i++)
	//{
	//	vEnemy3[i].clear();
	//}

	////�G��D��x���Ƃɔz��Ɋi�[
	//for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	//{
	//	//HP��0�ȏ�
	//	if ((*it)->Enemy->GetHp() > 0 && (*it)->Enemy->GetTag() != CGameObject::TAG::INITENEMY)
	//	{
	//		//�����̎�����ɉf���Ă���
	//		if ((*it)->Enemy->GetIsCenterCameraRef() && (*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
	//		{
	//			(*it)->FastPoint = 5;
	//			(*it)->Enemy->Fastpoint = 5;
	//			vEnemy3[5].push_back((*it));
	//		}
	//		else if ((*it)->Enemy->GetIsCameraRef())
	//		{
	//			//�J�����ɉf���Ă���
	//			//�v���C���[�Ƃ̋������^�[�Q�b�g�\�͈͓��Ȃ�
	//			if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
	//			{
	//				(*it)->FastPoint = 4;
	//				(*it)->Enemy->Fastpoint = 4;
	//				vEnemy3[4].push_back((*it));
	//			}
	//			else
	//			{
	//				if (!IsOneTarget)
	//				{
	//					if ((*it)->Enemy->GetIsTarget())
	//					{
	//						(*it)->Enemy->SetIsTarget(false);
	//						(*it)->Enemy->SetTargetMiniMapObjectModel(false);
	//					}
	//				}

	//				(*it)->Enemy->Fastpoint = 3;
	//				(*it)->FastPoint = 3;
	//				vEnemy3[3].push_back((*it));
	//			}
	//		}
	//		else
	//		{
	//			//�v���C���[�Ƃ̋������^�[�Q�b�g�\�͈͓��Ȃ�
	//			if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
	//			{
	//				if (!IsOneTarget)
	//				{
	//					if ((*it)->Enemy->GetIsTarget())
	//					{
	//						(*it)->Enemy->SetIsTarget(false);
	//						(*it)->Enemy->SetTargetMiniMapObjectModel(false);
	//					}
	//				}

	//				(*it)->Enemy->Fastpoint = 2;
	//				(*it)->FastPoint = 2;
	//				vEnemy3[2].push_back((*it));
	//			}
	//			else
	//			{
	//				if (!IsOneTarget)
	//				{
	//					if ((*it)->Enemy->GetIsTarget())
	//					{
	//						(*it)->Enemy->SetIsTarget(false);
	//						(*it)->Enemy->SetTargetMiniMapObjectModel(false);
	//					}
	//				}

	//				(*it)->Enemy->Fastpoint = 1;
	//				(*it)->FastPoint = 1;
	//				vEnemy3[1].push_back((*it));
	//			}
	//		}
	//	}
	//}

	//return;
}

void CPlayerAttack::InsertResEnemys(std::vector<RESENEMY*>& _enemys)
{
	//���g���N���A
	_enemys.clear();

	//�G��D��x���Ƃɔz��Ɋi�[
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		//HP��0�ȏ�
		if ((*it)->Enemy->GetHp() > 0 && (*it)->Enemy->GetTag() != CGameObject::TAG::INITENEMY)
		{
			////�����̎�����ɉf���Ă���
			//if ((*it)->Enemy->GetIsCenterCameraRef() && (*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
			//{
			//	(*it)->FastPoint = 5;
			//	(*it)->Enemy->Fastpoint = 5;
			//	_enemys.push_back((*it));
			//}

			if ((*it)->Enemy->GetIsCameraRef())
			{
				//�J�����ɉf���Ă���
				//�v���C���[�Ƃ̋������^�[�Q�b�g�\�͈͓��Ȃ�
				if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
				{
					//�v���C���[�Ƃ̋�����20�ȉ�
					//if ((*it)->Enemy->GetPlayerDistance() <= 20)
					//{
					//	(*it)->FastPoint = 5;
					//	(*it)->Enemy->Fastpoint = 5;
					//	_enemys.push_back((*it));
					//	//continue;
					//}
					//else
					//{
					//	(*it)->FastPoint = 4;
					//	(*it)->Enemy->Fastpoint = 4;
					//	_enemys.push_back((*it));
					//}

					(*it)->FastPoint = 4;
					(*it)->Enemy->Fastpoint = 4;
					_enemys.push_back((*it));
				}
				else
				{
					if (!IsOneTarget)
					{
						if ((*it)->Enemy->GetIsTarget())
						{
							(*it)->Enemy->SetIsTarget(false);
							(*it)->Enemy->SetTargetMiniMapObjectModel(false);
						}
					}

					(*it)->Enemy->Fastpoint = 3;
					(*it)->FastPoint = 3;
					_enemys.push_back((*it));
				}
			}
			else
			{
				//�v���C���[�Ƃ̋������^�[�Q�b�g�\�͈͓��Ȃ�
				if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
				{
					if (!IsOneTarget)
					{
						if ((*it)->Enemy->GetIsTarget())
						{
							(*it)->Enemy->SetIsTarget(false);
							(*it)->Enemy->SetTargetMiniMapObjectModel(false);
						}
					}

					(*it)->Enemy->Fastpoint = 2;
					(*it)->FastPoint = 2;
					_enemys.push_back((*it));
				}
				else
				{
					if (!IsOneTarget)
					{
						if ((*it)->Enemy->GetIsTarget())
						{
							(*it)->Enemy->SetIsTarget(false);
							(*it)->Enemy->SetTargetMiniMapObjectModel(false);
						}
					}

					(*it)->Enemy->Fastpoint = 1;
					(*it)->FastPoint = 1;
					_enemys.push_back((*it));
				}
			}
		}
	}

	return;
}

//void CPlayerAttack::InsertResEnemys(std::vector<std::unique_ptr<RESENEMY>>& _enemys)
//{
//	//���g���N���A
//	_enemys.clear();
//
//	//�G��D��x���Ƃɔz��Ɋi�[
//	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
//	{
//		//HP��0�ȏ�
//		if ((*it)->Enemy->GetHp() > 0 && (*it)->Enemy->GetTag() != CGameObject::TAG::INITENEMY)
//		{
//			//�����̎�����ɉf���Ă���
//			if ((*it)->Enemy->GetIsCenterCameraRef() && (*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
//			{
//				(*it)->FastPoint = 5;
//				(*it)->Enemy->Fastpoint = 5;
//				_enemys.push_back((*it));
//			}
//			else if ((*it)->Enemy->GetIsCameraRef())
//			{
//				//�J�����ɉf���Ă���
//				//�v���C���[�Ƃ̋������^�[�Q�b�g�\�͈͓��Ȃ�
//				if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
//				{
//					(*it)->FastPoint = 4;
//					(*it)->Enemy->Fastpoint = 4;
//					_enemys.push_back((*it));
//				}
//				else
//				{
//					if (!IsOneTarget)
//					{
//						if ((*it)->Enemy->GetIsTarget())
//						{
//							(*it)->Enemy->SetIsTarget(false);
//							(*it)->Enemy->SetTargetMiniMapObjectModel(false);
//						}
//					}
//
//					(*it)->Enemy->Fastpoint = 3;
//					(*it)->FastPoint = 3;
//					_enemys.push_back((*it));
//				}
//			}
//			else
//			{
//				//�v���C���[�Ƃ̋������^�[�Q�b�g�\�͈͓��Ȃ�
//				if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
//				{
//					if (!IsOneTarget)
//					{
//						if ((*it)->Enemy->GetIsTarget())
//						{
//							(*it)->Enemy->SetIsTarget(false);
//							(*it)->Enemy->SetTargetMiniMapObjectModel(false);
//						}
//					}
//
//					(*it)->Enemy->Fastpoint = 2;
//					(*it)->FastPoint = 2;
//					_enemys.push_back((*it));
//				}
//				else
//				{
//					if (!IsOneTarget)
//					{
//						if ((*it)->Enemy->GetIsTarget())
//						{
//							(*it)->Enemy->SetIsTarget(false);
//							(*it)->Enemy->SetTargetMiniMapObjectModel(false);
//						}
//					}
//
//					(*it)->Enemy->Fastpoint = 1;
//					(*it)->FastPoint = 1;
//					_enemys.push_back((*it));
//				}
//			}
//		}
//	}
//
//	return;
//}

void CPlayerAttack::ResetPlayerAttack()
{
	IsSetTarget = false;

	vuMisile.clear();

	for (auto& Enemy : vEnemys) {
		//Enemy->Enemy->SetActive(false);

		if (Enemy != nullptr) {
			if (Enemy->Enemy != nullptr) {
				delete Enemy->Enemy;  // RESENEMY �̓��������������
				Enemy->Enemy = nullptr;
			}
			delete Enemy;  // RESENEMY ���̂����
			Enemy = nullptr;
		}
	}
	vEnemys.clear();

	lTargetEnemys.clear();

	DebugUI::UnregisterDebugFunction(EnemyFrame);
	CreateEnemyFrame = false;

	AttackId = MISILE;

	IsOneTarget = true;
}

void CPlayerAttack::ChengeAttackweapon()
{
	//�������킪�ő�l����Ȃ�������
	AttackId = static_cast<ATTACK_ID>(static_cast<int>(AttackId) + 1);

	//�v���C���[���̃^�[�Q�b�gUI���Z�b�g
	CGame::Get()->GetPlayer()->ClearMoveTargetList();

	if (AttackId == MAX)
		AttackId = MISILE;

	//�ύX������̍U��ID
	if (AttackId == FOURTARGET_FOURMISILE)
	{
		if (lTargetEnemys.size() != 0)
		{
			(*lTargetEnemys.begin())->SetCanAttack(false);
			(*lTargetEnemys.begin())->SetIsTarget(false);
		}

		lTargetEnemys.clear();
		IsOneTarget = false;
		SetTarget(true);
	}
	else
	{
		IsOneTarget = true;

		for (auto it = lTargetEnemys.begin(); it != lTargetEnemys.end(); it++)
		{
			(*it)->TargetTime = 0;
			(*it)->OldTargetTime = GetTickCount64();
			(*it)->SetCanAttack(false);
			(*it)->SetIsTarget(false);
			(*it)->SetTargetMiniMapObjectModel(false);
			if ((*it)->GetTag() != CGameObject::TAG::INITENEMY)
				CGame::Get()->GetPlayer()->InsertMovetargetUI((*lTargetEnemys.begin()));
		}
	}




	if (IsOneTarget && lTargetEnemys.size() > 1)
	{
		CEnemy* ResEnemy = (*lTargetEnemys.begin());
		lTargetEnemys.clear();
		lTargetEnemys.push_back(ResEnemy);
	}

	CScene::Get()->ResetMisileUI();
}

void CPlayerAttack::ClearMisileParticles()
{
	for (auto it = vuMisile.begin(); it != vuMisile.end(); it++)
	{
		(*it)->ClearPareticles();
	}
}

float CPlayerAttack::GetWeaponMaxDistance()
{
	switch (AttackId)
	{
	case CPlayerAttack::MISILE:
		IsOneTarget = true;
		return float(MAX_PLAYER_MISILETARGET_DISTANCE);
		break;
	case CPlayerAttack::ONETARGET_FOURMISILE:
		IsOneTarget = true;
		return float(MAX_PLAYER_MISILETARGET_DISTANCE);
		break;
	case CPlayerAttack::FOURTARGET_FOURMISILE:
		IsOneTarget = false;
		return float(MAX_PLAYER_FOURMISILE_DISTANCE);
		break;
	case CPlayerAttack::MAX:
		break;
	default:
		break;
	}

	return 100.0f;
}

CEnemy* CPlayerAttack::GetBossEnemy()
{
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		if ((*it)->Enemy->GetTag() == CGameObject::TAG::BOSS_ENEMY)
			return (*it)->Enemy;
	}

	return nullptr;
}

void CPlayerAttack::SetAttackEntourageEnemy()
{
	int Num = 0;

	//�����ŁA�U���x�����s���Ă���G���m�F����
	Num = GetEntourageAttackNum();
	if (Num < 0)
		Num = 0;

	if (Num >= 2)
		return;

	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		if (Num >= 2)
			return;

		//�^�[�Q�b�g����Ă��Ȃ����U���w�����o�Ă��Ȃ��G����
		//�h�q�Ǐ]��芪������Ȃ��Ƃ��Ɏw��
		if (!(*it)->Enemy->GetIsTarget() && (*it)->Enemy->GetIsEndChaseAttack() &&
			(*it)->Enemy->GetEnemyState() != CEnemy::ENEMY_STATE::ENTOURAGE_FOLLOW
			&& (*it)->Enemy->GetEnemyState() != CEnemy::ENEMY_STATE::ENTOURAGE_DEFENCE)
		{
			//��ԕύX
			(*it)->Enemy->SetEnemyState(CEnemy::ENEMY_STATE::ENTOURAGE_ATTACK);
			//�V������芪���̎w����W�ݒ�
			(*it)->Enemy->SetNewEntouragePos();
			//Enemy->Enemy->SetIsEndChaseAttack();
			Num++;
			SetEntourageAddNum(1);
		}
	}
}

void CPlayerAttack::SetDefenceEntourageEnemy(bool _isdefence)
{
	//EntourageDefenceNum = 0;
	if (_isdefence)
	{
		//�S�Ă̓G�̂����A��芪���ł���A�Ǐ]�G�݂̂�h�q�G�ɕύX����
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			if ((*it)->Enemy->GetTag() == CGameObject::TAG::ENTOURAGE_ENEMY &&
				(*it)->Enemy->GetEnemyState() == CEnemy::ENEMY_STATE::ENTOURAGE_FOLLOW)
			{
				//if (EntourageDefenceNum >= 5)
				//	break;
				//Enemy->Enemy->SetNewEntouragePos();
				XMFLOAT3 ResMovePos = { 12.0f ,12.0f ,2.0f };
				(*it)->Enemy->ChengeFollowPos(ResMovePos);
				(*it)->Enemy->SetEnemyState(CEnemy::ENEMY_STATE::ENTOURAGE_DEFENCE);
				EntourageDefenceNum++;
			}
		}
	}
	else
	{
		//�S�Ă̓G�̂����A��芪���ł���A�h�q�G�݂̂�Ǐ]�G�ɕύX����
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			if ((*it)->Enemy->GetTag() == CGameObject::TAG::ENTOURAGE_ENEMY &&
				(*it)->Enemy->GetEnemyState() == CEnemy::ENEMY_STATE::ENTOURAGE_DEFENCE)
			{
				//Enemy->Enemy->SetNewEntouragePos();
				XMFLOAT3 ResMovePos = { 6.0f ,6.0f ,4.0f };
				(*it)->Enemy->ChengeFollowPos(ResMovePos);
				(*it)->Enemy->SetEnemyState(CEnemy::ENEMY_STATE::ENTOURAGE_FOLLOW);
			}
		}
	}

}

void CPlayerAttack::GetGunEnemyFlag(std::deque<DirectX::XMFLOAT3>& _qgunflagarray)
{
	_qgunflagarray = ResArray;
	ShiftGunEnemyArray();
}

void CPlayerAttack::ShiftGunEnemyArray()
{
	//�擪�𖖔��Ɉړ�
	ResArray.push_back(ResArray.front());
	ResArray.pop_front();
}

void CPlayerAttack::CheckGunEnemysPass()
{
	//�S�Ă̓G����
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		//�@�֏e�G�̓�
		if ((*it)->Enemy->GetTag() == CGameObject::TAG::GUN_ENEMY)
		{
			//�ړ��������Ă��Ȃ��ꍇ�A
			if (!(*it)->Enemy->GetGunEnemyIsPass())
			{
				IsGunEnemysPass = false;
				return;
			}
		}
	}

	IsGunEnemysPass = true;
}

void CPlayerAttack::ChengeEnemysMovePos()
{
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		(*it)->Enemy->SetNewMovePosition();
	}
}

void CPlayerAttack::GetEnemys(std::list<CEnemy*>& _enemys)
{
	_enemys.clear();

	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		_enemys.push_back((*it)->Enemy);
	}
}

void CPlayerAttack::GetTargets(std::list<CEnemy*>& _targets)
{
	_targets.clear();

	for (auto Target : lTargetEnemys)
	{
		_targets.push_back(Target);
	}
}

void CPlayerAttack::SaveEnemyParameter()
{
	//�p�����[�^�̃t�@�C�����J��
	std::ofstream outFile("Parameter/Enemy.txt", std::ios::trunc);

	//�t�@�C���I�[�v��
	if (!outFile.is_open()) {
		// �ǂݍ��݂Ɏ��s�����ꍇ
		MessageBoxA(NULL, "Miss Open File", "�G���[",
			MB_OK | MB_ICONERROR);
	}

	// �������ރe�L�X�g//
	string ParameterText;

	ParameterText += to_string(EnemyParameter.ThresHold) + ",";
	ParameterText += to_string(EnemyParameter.MaxSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Fright_MaxSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Follow_MaxSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Entourage_MaxSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Boss_MaxSpeed) + ",";
	ParameterText += to_string(EnemyParameter.AddSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Fright_AddSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Follow_AddSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Entourage_AddSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Boss_AddSpeed) + ",";
	ParameterText += to_string(EnemyParameter.Fright_MaxRotate) + ",";
	ParameterText += to_string(EnemyParameter.Follow_MaxRotate) + ",";
	ParameterText += to_string(EnemyParameter.Entourage_MaxRotate) + ",";
	ParameterText += to_string(EnemyParameter.Boss_MaxRotate) + ",";
	ParameterText += to_string(EnemyParameter.FrightEnemy_HP) + ",";
	ParameterText += to_string(EnemyParameter.GroundEnemy_HP) + ",";
	ParameterText += to_string(EnemyParameter.GroundEnemy_AttackTime) + ",";
	ParameterText += to_string(EnemyParameter.Max_Attack_Distance) + ",";
	ParameterText += to_string(EnemyParameter.Target_Time) + ",";
	ParameterText += to_string(EnemyParameter.Attack_CoolTime) + ",";
	ParameterText += to_string(EnemyParameter.InsertEntourageNum) + ",";
	ParameterText += to_string(EnemyParameter.Entourage_ChengePos_Time) + ",";
	ParameterText += to_string(EnemyParameter.Entourage_Attack_CoolTime) + ",";
	ParameterText += to_string(EnemyParameter.Entourage_Move_Pos_Dis);

	// �t�@�C���Ƀe�L�X�g����������
	outFile << ParameterText;

	outFile.close();
}

void CPlayerAttack::SaveMisileParameter()
{
	//�p�����[�^�̃t�@�C�����J��
	std::ofstream outFile("Parameter/Misile.txt", std::ios::trunc);

	//�t�@�C���I�[�v��
	if (!outFile.is_open()) {
		// �ǂݍ��݂Ɏ��s�����ꍇ
		MessageBoxA(NULL, "Miss Open File", "�G���[",
			MB_OK | MB_ICONERROR);
	}

	// �������ރe�L�X�g//
	string ParameterText;

	ParameterText += to_string(MisileParameter.MaxRotate) + ",";
	ParameterText += to_string(MisileParameter.MinRotate) + ",";
	ParameterText += to_string(MisileParameter.Enemy_Min_Rotate) + ",";
	ParameterText += to_string(MisileParameter.Enemy_Max_Rotate) + ",";
	ParameterText += to_string(MisileParameter.Min_Speed) + ",";
	ParameterText += to_string(MisileParameter.Max_Speed) + ",";
	ParameterText += to_string(MisileParameter.Min_Add_Speed) + ",";
	ParameterText += to_string(MisileParameter.Max_Add_Speed) + ",";
	ParameterText += to_string(MisileParameter.Enemy_Min_Add_Speed) + ",";
	ParameterText += to_string(MisileParameter.Enemy_Max_Add_Speed) + ",";
	ParameterText += to_string(MisileParameter.Delete_Misile_Time) + ",";
	ParameterText += to_string(MisileParameter.Particle_Cooltime) + ",";
	ParameterText += to_string(MisileParameter.Particle_OutTime) + ",";
	ParameterText += to_string(MisileParameter.Particle_AddScale) + ",";
	ParameterText += to_string(MisileParameter.Misile_Charge_Speed);

	// �t�@�C���Ƀe�L�X�g����������
	outFile << ParameterText;

	outFile.close();
}
