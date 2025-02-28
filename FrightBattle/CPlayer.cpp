#include "CPlayer.h"
#include "CScene.h"
#include "Input.h"
#include "CPlayerAttack.h"
#include "CControllerInput.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include "CEnemy.h"
#include "CGame.h"
#include "CCamera.h"
#include "CCheckCollision.h"
#include "DebugUI.h"
#include "xa2.h"

#include <random>
#include <cmath>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

//����
//�v���C���[�W�F�b�g�G�t�F�N�g
//LOW	:�n�_.1.0,1.0,2.0 �I�_.2.0,2.0,4.0
//NORMAl:�n�_.1.5,1.5,2.0 �I�_.2.5,2.5,7.0
//HIGH	:�n�_.2.5,2.5,2.0 �I�_.4.0,4.0,7.0

using namespace DirectX;

using namespace std;

void CPlayer::InsertParameter()
{
	//�t�@�C������p�����[�^��ǂݍ���
	//��s�ڂ͐����Ȃ̂Ŗ���
	//�����p�̕ϐ���ݒ肵�Ă���(�\���̂�)

	// �ǂݍ��ރt�@�C���̖��O

	std::ifstream file("Parameter/Player.txt");

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

			switch (ParamTag)
			{
			case CPlayer::MAXINITROTATE:Parameter.MaxInitRotate = std::stof(line); break;
			case CPlayer::MAXSLIPZ:Parameter.Max_SlipZ = std::stof(line); break;
			case CPlayer::MAXSLIPX:Parameter.Max_SlipX = std::stof(line); break;
			case CPlayer::PARTICLEDELETETIME:Parameter.Particle_DeleteTime = std::stof(line); break;
			case CPlayer::PARTICLECOOLTIMELOW:Parameter.Particle_CoolTime_Low = std::stof(line); break;
			case CPlayer::PARTICLECOOLTIMENORMAL:Parameter.Particle_CoolTime_Normal = std::stof(line); break;
			case CPlayer::PARTICLECOOLTIMEHIGH:Parameter.Particle_CoolTime_High = std::stof(line); break;
			case CPlayer::ADDSPEED:Parameter.Move_Add_Speed = std::stof(line); break;
			case CPlayer::NOMOVEADDSPEED:Parameter.NoMove_AddSpeed = std::stof(line); break;
			case CPlayer::RIGHTMOVESPEED:Parameter.Right_Move_Speed = std::stof(line); break;
			case CPlayer::MAXRIGHTMOVESPEED:Parameter.Max_Right_Move_Speed = std::stof(line); break;
			case CPlayer::MAXSPEED:Parameter.Max_MoveSpeed = std::stof(line); break;
			case CPlayer::MINSPEED:Parameter.Min_MoveSpeed = std::stof(line); break;
			case CPlayer::ROTATESTICKSPEED:Parameter.RotateX_StickSpeed = std::stof(line); break;
			case CPlayer::ROTATEZSTICKSPEED:Parameter.RotateZ_StickSpeed = std::stof(line); break;
			case CPlayer::YOWROTATESPEED:Parameter.Yow_Rotate_Speed = std::stof(line); break;
			}

			ParamTag = static_cast<PARAM_TAG>(static_cast<int>(ParamTag) + 1);
		}
	}

	file.close();
}

void CPlayer::PaeticleUpdate()
{
	//�p�[�e�B�N������ёւ���(�J��������ʒu���������̂��ɕ`��)
	std::sort(qParticles.begin(), qParticles.end(), [](const PARTICLE& a, const PARTICLE& b) {
		//if (a.Object != nullptr && b.Object != nullptr)
			return a.Object->CheckDistance(nullptr, CGame::Get()->GetCameraPos()) > b.Object->CheckDistance(nullptr, CGame::Get()->GetCameraPos());
		});

	for (auto it = qParticles.begin(); it != qParticles.end(); it++)
	{
		(*it).Object->materialDiffuse.w -= 0.04f;
		(*it).Object->SetMoveSpeed(MoveSpeed);

		if ((*it).Object->materialDiffuse.w <= 0.0f)
		{
			delete it->Object;
			it->Object = nullptr;
			qParticles.erase(it);
			break;
		}
		else
		{
			(*it).Object->MoveUpdate();
			(*it).Object->Position = { (*it).Object->Position.x - (0.1f * GetForward().x),
										(*it).Object->Position.y - (0.1f * GetForward().y),
										(*it).Object->Position.z - (0.1f * GetForward().z), };
		}
	}
}

void CPlayer::InsertParticle()
{

	// �o���ʒu�����߂闐��
	std::random_device rd;
	std::mt19937 rng(rd());

	// �����͈̔͂��w��
	std::uniform_real_distribution<float> dist(0.0f, 0.4f);

	//DirectX::XMFLOAT2 AddPos = { dist(rng) - 0.1f ,dist(rng) - 0.1f };
	DirectX::XMFLOAT2 AddPos = { dist(rng) - 0.2f ,0.1f };

	//�p�[�e�B�N���ǉ�
	PARTICLE Particle;
	Particle.Object = new CGameObject();

	// �s�񐶐�
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	rotationMatrix.r[0] = newRIGHT;       // X���i�E�����j
	rotationMatrix.r[1] = newUP;          // Y���i������j
	rotationMatrix.r[2] = newForward;    // Z���i�������j
	rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // ���s�ړ��͂Ȃ�

	//�I�C���[�p�x�v�Z
	DirectX::XMFLOAT3 ResRotation = ExtractRotationFromMatrix(rotationMatrix);

	// �p�[�e�B�N���̉�]�ɓK�p
	Particle.Object->Rotation = ResRotation;
	Particle.Object->SetInitRotateMatrix();
	Particle.Object->SetVector();

	//Particle.Object->Position = { Position.x - (0.7f * GetForward().x) + (AddPos.x * GetRight().x) + (AddPos.y * GetUp().x),
	//							  Position.y - (0.7f * GetForward().y) + (AddPos.x * GetRight().y) + (AddPos.y * GetUp().y),
	//							  Position.z - (0.7f * GetForward().z) + (AddPos.x * GetRight().z) + (AddPos.y * GetUp().z) };

	Particle.Object->Position = { Position.x + (1.0f * GetForward().x) + (AddPos.x * GetRight().x) + (AddPos.y * GetUp().x),
								  Position.y + (1.0f * GetForward().y) + (AddPos.x * GetRight().y) + (AddPos.y * GetUp().y),
								  Position.z + (1.0f * GetForward().z) + (AddPos.x * GetRight().z) + (AddPos.y * GetUp().z) };

	Particle.Object->SetMoveSpeed(MoveSpeed);

	Particle.Object->Scaling = { 0.3f, 0.3f, 0.1f };
	Particle.Object->materialDiffuse.w = 1.0f;
	Particle.Object->SetActive(true);
	Particle.Object->SetLighting(false);
	Particle.Object->SetBillboard(true);
	Particle.Object->SetModel(ParticleModel);
	Particle.StartTime = GetTickCount64();
	qParticles.push_back(Particle);
}

void CPlayer::DrawParticle()
{

	for (auto it = qParticles.begin(); it != qParticles.end(); it++)
	{
		if ((*it).Object != nullptr)
			(*it).Object->Draw();
	}
}

CPlayer::CPlayer()
{
	Hp = 60;

	ParamTag = MAXINITROTATE;

	pEffect = new CEffect();
	pEffect->SetBillboard(true);
	pEffect->SetLighting(false);
	pEffect->Scaling = { 3.0f,3.0f,3.0f };
	//CShottoManager::Get()->CheckShotPos(Position, Target);

	eJetEffect = new CEffect();
	eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_low.png"));
	eJetEffect->SetBillboard(false);
	eJetEffect->SetLighting(false);
	eJetEffect->SetAnimUVSize(10, 1);

	eJetEffect->Position = { Position.x + GetForward().x * -0.8f + GetUp().x * -0.35f + GetRight().x * 0.1f,
							 Position.y + GetForward().y * -0.8f + GetUp().y * -0.35f + GetRight().y * 0.1f,
							 Position.z + GetForward().z * -0.8f + GetUp().z * -0.35f + GetRight().z * 0.1f };
	eJetEffect->Rotation = Rotation;
	eJetEffect->SetInitRotateMatrix();
	eJetEffect->Scaling = { 0.5f,1.0f,1.0f };
	eJetEffect->materialDiffuse.w = 1.0f;
	eJetEffect->SetTag(BACKFIRE);

	CEffectManager::Get()->Set3DvEffect(eJetEffect, 6, 1.0f);

	pEnemyRock = make_unique<CGameObject>();
	pRockModel = make_unique<CGameObject>();
	pRockOnModel = make_unique<CGameObject>();

	//���W��傫���ݒ�
	pEnemyRock->SetModel(CGame::Get()->GetModel(L"asset/UI/EnemyRock_UI.png"));
	pEnemyRock->SetBillboard(false);
	pEnemyRock->SetLighting(false);
	pEnemyRock->Position = { 0.0f,0.3f,0.0f };
	pEnemyRock->Rotation = { 0.0f,0.0f,0.0f };
	pEnemyRock->Scaling = { 0.5f, 0.2f, 0.1f };

	pRockModel->Position = { 0.0f,0.0f,0.0f };
	pRockModel->Rotation = { 0.0f,0.0f,0.0f };
	pRockModel->Scaling = { 4.8f, 2.7f, 0.1f };

	pRockOnModel->Position = { 0.0f,0.0f,0.0f };
	pRockOnModel->Rotation = { 0.0f,0.0f,0.0f };
	pRockOnModel->Scaling = { 4.8f, 2.7f, 0.1f };

	ColliderObject = new CGameObject();

	ColliderObject->Position = { Position.x + 0.3f * GetForward().x,
								Position.y + 0.3f * GetForward().y,
								Position.z + 0.3f * GetForward().z };

	ColliderObject->Scaling = { 0.6f, 0.2f, 1.0f };
	ColliderObject->materialDiffuse.w = 0.7f;
	ColliderObject->SetLighting(false);

	pCaveatObject = make_unique<CGameObject>();
	pCaveatObject->Position = { Position.x + 3.0f * GetForward().x,
								Position.y + 3.0f * GetForward().y,
								Position.z + 6.0f * GetForward().z };
	pCaveatObject->Scaling = { 0.6f, 0.2f, 4.0f };
	pCaveatObject->materialDiffuse.w = 0.7f;
	pCaveatObject->SetLighting(false);
	pCaveatObject->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));

	ClearMoveTargetList();

	//�p�����[�^�擾
	InsertParameter();

	OldInsertParticleTime = GetTickCount64();

	// �f�o�b�O�\���֐��o�^
	TargetUIId = DebugUI::RedistDebugFunction([&]() {
		DrawTargetLine();
		});
}

CPlayer::~CPlayer()
{
	if (pEffect != nullptr)
	{
		delete pEffect;
		pEffect = nullptr;
	}

	//if (eJetEffect != nullptr)
	//{
	//	delete eJetEffect;
	//	eJetEffect = nullptr;
	//}

	if (!plMoveTargetUI.empty())
	{
		for (auto MoveTarget : plMoveTargetUI)
		{
			(MoveTarget).pTargetObject->SetCanAttack(false);
			(MoveTarget).pTargetObject->SetIsTarget(false);
			MoveTarget.IsDraw = false;
			MoveTarget.Position = { 0.0f,0.0f,0.0f };
		}
		plMoveTargetUI.clear();
	}

	for (auto Particle : qParticles)
	{
		delete Particle.Object;
		Particle.Object = nullptr;
	}
	qParticles.clear();

	DebugUI::UnregisterDebugFunction(TargetUIId);
}

void CPlayer::Update()
{
	if (!isActive)
		return;

	//�X�e�[�W�V�[����
	if (CScene::Get()->GetNowScene() != CScene::SCENE::TITLE)
	{
		//�v���C���[�̃_���[�W��Ԃ�\��
		if (Hp >= 50)
		{
			CScene::Get()->Get2DGameObject(PLAYER_UI)->SetModel(CGame::Get()->GetModel(L"asset/UI/FightJet_UI_01.png"));
		}
		else if (Hp < 50 && Hp >= 30)
		{
			CScene::Get()->Get2DGameObject(PLAYER_UI)->SetModel(CGame::Get()->GetModel(L"asset/UI/FightJet_UI_02.png"));
		}
		else if (Hp < 30)
		{
			CScene::Get()->Get2DGameObject(PLAYER_UI)->SetModel(CGame::Get()->GetModel(L"asset/UI/FightJet_UI_03.png"));
		}
		if (Hp <= 0)
		{
			isActive = false;
			CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
			CPlayerAttack::Get()->ClearEnemys();
			return;
		}

		//�X�e�[�W�ŊJ�n������
		if (CGame::Get()->GetIsStartStage())
		{
			//���݂̎����v�Z
			CGameObject::SetVector();

			//�R���g���[���[�ڑ��m�F
			GetControllerState();

			IsInput = false;
			//�R���g���[���[�̓��͔���
			DoControllerEvent();
			//�p�x�ɍX�V�p�x��ݒ�
			CheckInput(Angle);

			//�R���g���[���[�̐U��
			BivUpdate();

			//�������c��������
			if (IsAutoPilot)
			{
				//���s��]�p�x�Ɉڍs
				SetInitRotate();
			}

			//�������Ă���Ƃ�
			if (IsFastSpeed)
			{
				//��O�̏�Ԃ��������Ă��Ȃ��Ƃ�
				if (!OldFastSpeed)
				{
					//����Ԃƌ��ݎ��Ԃ��擾
					OldSpeedUpTime = GetTickCount64();
					SpeedUpTime = 0;
					OldFastSpeed = true;
					eJetEffect->SetActive(true);

					switch (SpeedUpMode)
					{
					case CPlayer::LOW:		eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_low.png")); break;
					case CPlayer::NORMAL:	eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_normal.png")); break;
					case CPlayer::HIGH:		eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_high.png")); break;
					}
				}
				else
				{
					//��O�̏�Ԃ��������Ă���Ƃ�
					//��O�̎��Ԃƌ��݂̎��Ԃ���������Ă��鎞�Ԃ��v�Z
					SpeedUpTime = GetTickCount64();
					SpeedUpTime = SpeedUpTime - OldSpeedUpTime;
				}

				if (SpeedUpTime > 3000)
				{
					if (SpeedUpMode == LOW)
					{
						eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_normal.png"));
						SpeedUpMode = NORMAL;
						//SetBivTime(GetTickCount64() + 3000);
					}
					else if (SpeedUpMode == NORMAL)
					{
						eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_high.png"));
						SpeedUpMode = HIGH;
					}
					else if (SpeedUpMode == HIGH)
					{
						eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_high.png"));
						SpeedUpMode = HIGH;
					}

					OldSpeedUpTime = GetTickCount64();
					SpeedUpTime = GetTickCount64();
				}
			}
			else
			{
				//�������Ă��Ȃ��Ƃ�

				//��O�̏�Ԃ��������Ă���Ƃ�
				if (OldFastSpeed)
				{
					//����Ԃƌ��ݎ��Ԃ��擾
					OldSpeedUpTime = GetTickCount64();
					SpeedUpTime = 0;
					OldFastSpeed = false;
				}
				else
				{
					//��O�̏�Ԃ��������Ă���Ƃ�
					//��O�̎��Ԃƌ��݂̎��Ԃ���������Ă��鎞�Ԃ��v�Z
					SpeedUpTime = GetTickCount64();
					SpeedUpTime = SpeedUpTime - OldSpeedUpTime;
				}

				if (SpeedUpTime > 3000)
				{
					if (SpeedUpMode == HIGH)
					{
						eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_normal.png"));
						SpeedUpMode = NORMAL;
					}
					else if (SpeedUpMode == NORMAL)
					{
						eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_low.png"));
						SpeedUpMode = LOW;
						//eJetEffect->SetActive(false);
					}
					else if (SpeedUpMode == LOW)
					{
						eJetEffect->SetModel(CGame::Get()->GetModel(L"asset/JetEffect_low.png"));
						SpeedUpMode = LOW;
						//eJetEffect->SetActive(false);
					}

					OldSpeedUpTime = GetTickCount64();
					SpeedUpTime = GetTickCount64();
				}
			}

			//��]�X�V����
			RotateUpdate();

			//�����ޕύX
			if (Input::Get()->GetKeyTrigger(DIK_X) ||
				CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_X))
			{
				CPlayerAttack::Get()->ChengeAttackweapon();
			}

			//�U���J�n
			if ((Input::Get()->GetKeyTrigger(DIK_SPACE) ||
				CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_B)))
			{
				CPlayerAttack::Get()->AttackTarget(this);
			}

			//�^�[�Q�b�g�ύX
			if (Input::Get()->GetKeyTrigger(DIK_C) || CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_Y))
			{
				CPlayerAttack::Get()->SetTarget(false);
			}

			//�ړ�����
			MoveUpdate();

			eJetEffect->Position = { Position.x + GetForward().x * -0.8f + GetUp().x * -0.35f + GetRight().x * 0.01f,
									 Position.y + GetForward().y * -0.8f + GetUp().y * -0.35f + GetRight().y * 0.01f,
									 Position.z + GetForward().z * -0.8f + GetUp().z * -0.35f + GetRight().z * 0.01f };
			eJetEffect->Rotation = Rotation;
			eJetEffect->SetInitRotateMatrix();

			MiniMapObjectUpdate();
		}
		else
		{
			eJetEffect->Position = { Position.x + GetForward().x * -0.7f + GetUp().x * -0.35f + GetRight().x * 0.02f,
									 Position.y + GetForward().y * -0.7f + GetUp().y * -0.35f + GetRight().x * 0.02f,
									 Position.z + GetForward().z * -0.7f + GetUp().z * -0.35f + GetRight().x * 0.02f };
			eJetEffect->Rotation = Rotation;
			eJetEffect->SetInitRotateMatrix();

			//�ړ�����
			MoveUpdate();
		}
	}
	else
	{
		eJetEffect->Position = { Position.x + GetForward().x * -0.8f + GetUp().x * -0.35f + GetRight().x * 0.01f,
								 Position.y + GetForward().y * -0.8f + GetUp().y * -0.35f + GetRight().y * 0.01f,
								 Position.z + GetForward().z * -0.8f + GetUp().z * -0.35f + GetRight().z * 0.01f };
		eJetEffect->Rotation = Rotation;
		eJetEffect->SetInitRotateMatrix();
		eJetEffect->Scaling = { 0.5f,1.0f,1.0f };
		eJetEffect->materialDiffuse.w = 1.0f;
		eJetEffect->SetActive(true);
	}

	//�����蔻��̍��W�p�x�ݒ�//

	ColliderObject->Rotation = Rotation;
	ColliderObject->SetIsMatrix(RotateMatrix);

	ColliderObject->Position = Position;
	pCaveatObject->Position = { Position.x + 3.0f * GetForward().x,
								Position.y + 3.0f * GetForward().y,
								Position.z + 3.0f * GetForward().z };

	pCaveatObject->Rotation = Rotation;
	pCaveatObject->SetIsMatrix(RotateMatrix);
	pCaveatObject->SetColliderBoxOBB(pCaveatObject->Position, pCaveatObject->Scaling);
	DirectX::BoundingOrientedBox CaveatBox = pCaveatObject->GetColliderObjectOBB();
	IsDrawCaveat = false;

	//�����蔻��ݒ�
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

	//�n�ʂƌx������̓����蔻��
	if (CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND) != nullptr)
	{
		DirectX::BoundingBox GroundCol = CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->GetColliderObjectAABB();
		if (CCheckCollision::Get()->CheckColBoxOBBAABB(CaveatBox, GroundCol))
		{
			IsDrawCaveat = true;
		}
	}

	//�����ƌx������̓����蔻��
	for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
	{
		if (CCheckCollision::Get()->CheckColBoxOBBAABB(CaveatBox, Collider))
		{
			IsDrawCaveat = true;
		}
	}

	//�n�ʂƂ̓����蔻��
	if (CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND) != nullptr)
	{
		DirectX::BoundingBox GroundCol = CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->GetColliderObjectAABB();
		if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, GroundCol))
		{
			isActive = false;
			CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
			CPlayerAttack::Get()->ClearEnemys();
			XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(),1.0f);
			return;
		}
	}

	////�n�ʂƃv���C���[�̓����蔻��
	//for (auto Collider : mColliderBoxsAABB[TAG::GROUND])
	//{
	//	if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
	//	{
	//		isActive = false;
	//		CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
	//		CPlayerAttack::Get()->ClearEnemys();
	//		//XA_Play(SE_MISILEHIT);
	//		return;
	//	}
	//}

	//�����ƃv���C���[�̓����蔻��
	for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
	{
		if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
		{
			isActive = false;
			CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
			CPlayerAttack::Get()->ClearEnemys();
			XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
			return;
		}
	}

	////�p�[�e�B�N���쐬
	//ULONGLONG ResTime = 0;
	//if (IsFastSpeed)
	//{
	//	switch (SpeedUpMode)
	//	{
	//	case CPlayer::LOW:ResTime = (ULONGLONG)Parameter.Particle_CoolTime_Low; break;
	//	case CPlayer::NORMAL:ResTime = (ULONGLONG)Parameter.Particle_CoolTime_Normal; break;
	//	case CPlayer::HIGH:ResTime = (ULONGLONG)Parameter.Particle_CoolTime_High; break;
	//	}
	//}

	//if (GetTickCount64() >= OldInsertParticleTime + ResTime && MoveSpeed >= 0.3f)
	//{
	//	//�p�[�e�B�N����ǉ�
	//	InsertParticle();
	//	OldInsertParticleTime = GetTickCount64();
	//}

	////�p�[�e�B�N���X�V����
	//PaeticleUpdate();

	if (MoveSpeed >= 0.7f)
	{
		CGame::Get()->ShakeCamera(2);
	}
}

void CPlayer::MoveUpdate()
{
	if (RightMoveSpeed > 0.0f)
	{
		RightMoveSpeed -= 0.001f;
		if (RightMoveSpeed <= 0.0f)
			RightMoveSpeed = 0.0f;
	}
	else if (RightMoveSpeed < 0.0f)
	{
		RightMoveSpeed += 0.001f;
		if (RightMoveSpeed >= 0.0f)
			RightMoveSpeed = 0.0f;
	}

	//�E�ړ�
	XMFLOAT3 Right = GetRight();
	Position.x += Right.x * RightMoveSpeed;
	Position.y += Right.y * RightMoveSpeed;
	Position.z += Right.z * RightMoveSpeed;


	CGameObject::MoveUpdate();
}

void CPlayer::RotateUpdate()
{
	////���͂��Ȃ��ꍇ
	//if (!IsInput)
	//{
	//	//���͂��Ȃ��ꍇ�A�@�̂̌����Ă��鐳�ʊp�x�ɂ���āA�p�x��ύX����
	//	//1:�n�ʑ��������Ă���ꍇ�A���x�������Ȃ�
	//	//2:Z�����X���Ă���Ƃ��A����ɑΉ����ď��X��Y����]���s��
	//	//3:�@�̂̏㉺���t(�@�̂̏�������n��)�̏ꍇ�A�@�̂̐��ʂ͋}���ɒn�ʂ̕�������
	//	SetNotInputRotate();
	//}

	//�p�x�����X��0�ɋ߂Â���
	if (Angle.x > 0.0f)
	{
		Angle.x -= Parameter.Max_SlipX;
		if (Angle.x <= 0.0f)
			Angle.x = 0.0f;
	}
	else if (Angle.x < 0.0f)
	{
		Angle.x += Parameter.Max_SlipX;
		if (Angle.x >= 0.0f)
			Angle.x = 0.0f;
	}

	if (Angle.y > 0.0f)
	{
		Angle.y -= Parameter.Max_SlipX;
		if (Angle.y <= 0.0f)
			Angle.y = 0.0f;
	}
	else if (Angle.y < 0.0f)
	{
		Angle.y += Parameter.Max_SlipX;
		if (Angle.y >= 0.0f)
			Angle.y = 0.0f;
	}

	if (Angle.z > 0.0f)
	{
		Angle.z -= Parameter.Max_SlipZ;
		if (Angle.z <= 0.0f)
			Angle.z = 0.0f;
	}
	else if (Angle.z < 0.0f)
	{
		Angle.z += Parameter.Max_SlipZ;
		if (Angle.z >= 0.0f)
			Angle.z = 0.0f;
	}

	//�C�ӎ��ŉ�]������
	//�N�H�[�^�j�I���ł̉�]
	XMVECTOR QrX = XMQuaternionRotationAxis(newRIGHT, XMConvertToRadians(Angle.x));
	XMVECTOR QrY = XMQuaternionRotationAxis(newUP, XMConvertToRadians(Angle.y));
	XMVECTOR QrZ = XMQuaternionRotationAxis(newForward, XMConvertToRadians(Angle.z));

	XMMATRIX QmtX = XMMatrixRotationQuaternion(QrX);
	XMMATRIX QmtY = XMMatrixRotationQuaternion(QrY);
	XMMATRIX QmtZ = XMMatrixRotationQuaternion(QrZ);

	//XMMATRIX QMT = QmtX * QmtY * QmtZ;
	XMMATRIX QMT = QmtY * QmtX * QmtZ;
	RotateMatrix *= QMT;
	Rotation = ExtractRotationFromMatrix(RotateMatrix);

	//���݂̎����v�Z
	CGameObject::SetVector();

}

void CPlayer::CheckInput(XMFLOAT3& _angle)
{
	bool ChengeSpeed = false;
	IsFastSpeed = false;
	IsAutoPilot = false;
	IsVeryRotate = false;

	//UP��DOWA�̓��������̏ꍇ�A�}���ȋ@�̂̉�]���ł���
	//�������A�㏸�ɂ͑��x���K�v�ɂȂ�A���~�ł͑��x�������Ȃ�
	//���x���Ȃ����ɏ㏸���悤�Ƃ����Ƃ��A�@�̂̌����͋}���ɒn�ʂ������悤�ɂȂ�
	//�@�̂̑��x�ݒ�
	if (Input::Get()->GetKeyPress(DIK_UP) && Input::Get()->GetKeyPress(DIK_DOWN))
	{
		IsVeryRotate = true;
	}
	else if (Input::Get()->GetKeyPress(DIK_UP))
	{
		//���x�̑���
		ChengeSpeed = true;
		IsFastSpeed = true;
		SetAddMoveSpeed((float)Parameter.Move_Add_Speed);
		if (MoveSpeed >= (float)Parameter.Max_MoveSpeed)
		{
			SetMoveSpeed((float)Parameter.Max_MoveSpeed);
		}
	}
	else if (Input::Get()->GetKeyPress(DIK_DOWN))
	{
		//���x�̒ቺ
		ChengeSpeed = true;
		SetAddMoveSpeed(-(float)Parameter.Move_Add_Speed);
		//if (MoveSpeed < 0.0f)
		//{
		//	SetMoveSpeed(0.0f);
		//}

		if (MoveSpeed < Parameter.Min_MoveSpeed)
		{
			SetMoveSpeed(Parameter.Min_MoveSpeed);
		}
	}

	//�R���g���[���[���ڑ�����Ă���
	if (DwResult == ERROR_SUCCESS)
	{
		//�O�i
		if (State.Gamepad.bRightTrigger && State.Gamepad.bLeftTrigger)
		{
			//�}���񃂁[�h
			IsVeryRotate = true;

			//���x�̒ቺ
			ChengeSpeed = true;
			SetAddMoveSpeed(-(float)Parameter.Move_Add_Speed);
			if (MoveSpeed < 0.0f)
			{
				SetMoveSpeed(0.0f);
			}
		}
		else if (State.Gamepad.bRightTrigger)
		{
			//���x�̑���
			ChengeSpeed = true;
			IsFastSpeed = true;
			SetAddMoveSpeed((float)Parameter.Move_Add_Speed);
			if (MoveSpeed >= (float)Parameter.Max_MoveSpeed)
			{
				SetMoveSpeed((float)Parameter.Max_MoveSpeed);
			}
		}
		else if (State.Gamepad.bLeftTrigger)
		{
			//���x�̒ቺ
			ChengeSpeed = true;
			SetAddMoveSpeed(-(float)Parameter.Move_Add_Speed);
			if (MoveSpeed < 0.0f)
			{
				SetMoveSpeed(0.0f);
			}
		}

		//RB��LB�̓�������
		if (iPad_leftshoulder && iPad_rightshoulder)
		{
			IsAutoPilot = true;
			return;
		}

		//���[��]
		if (iPad_leftshoulder)
		{
			RightMoveSpeed -= Parameter.Right_Move_Speed;
			if (RightMoveSpeed <= -Parameter.Max_Right_Move_Speed)
				RightMoveSpeed = -Parameter.Max_Right_Move_Speed;

			_angle.y = -(float)Parameter.Yow_Rotate_Speed;
			IsInput = true;
		}
		else if (iPad_rightshoulder)	//LB�{�^��(�E�ړ�)
		{
			RightMoveSpeed += Parameter.Right_Move_Speed;
			if (RightMoveSpeed >= Parameter.Max_Right_Move_Speed)
				RightMoveSpeed = Parameter.Max_Right_Move_Speed;

			_angle.y = (float)Parameter.Yow_Rotate_Speed;
			IsInput = true;
		}

		//�E�X�e�B�b�N
		//�J�������񂹂�
		if (iPad_RStickX == 1)
		{

		}
		else if (iPad_RStickX == -1)
		{

		}

		if (iPad_RStickY == 1)
		{

		}
		else if (iPad_RStickY == -1)
		{

		}

		float StickSpeedX = 0.5f;
		float StickSpeedY = 0.5f;

		if (iPad_LStickX > 0)
		{
			if (iPad_LStickX < 10000)
				StickSpeedX = StickSpeedX / 4;
			else if (iPad_LStickX > 9999 && iPad_LStickX < 20000)
				StickSpeedX = StickSpeedX / 2;
		}
		else if (iPad_LStickX < 0)
		{
			if (iPad_LStickX > -10000)
				StickSpeedX = StickSpeedX / 4;
			else if (iPad_LStickX > -20000)
				StickSpeedX = StickSpeedX / 2;
		}

		if (iPad_LStickY > 0)
		{
			if (iPad_LStickY < 10000)
				StickSpeedY = StickSpeedY / 4;
			else if (iPad_LStickY < 20000)
				StickSpeedY = StickSpeedY / 2;
		}
		else if (iPad_LStickY < 0)
		{
			if (iPad_LStickY > -10000)
				StickSpeedY = StickSpeedY / 4;
			else if (iPad_LStickY > -20000)
				StickSpeedY = StickSpeedY / 2;
		}

		//���X�e�B�b�N�̏㉺���͍��E�����΂̒l���傫��������A
		//�t�̃X�e�B�b�N�̉�]�X�s�[�h������������
		if (iPad_LStickX > 0 && iPad_LStickY > 0)
		{
			if (iPad_LStickX > iPad_LStickY)
			{
				StickSpeedY = StickSpeedY / 4;
			}
			else if (iPad_LStickX < iPad_LStickY)
			{
				StickSpeedX = StickSpeedY / 4;
			}
		}
		else if (iPad_LStickX < 0 && iPad_LStickY < 0)
		{
			if (iPad_LStickX < iPad_LStickY)
			{
				StickSpeedY = StickSpeedY / 4;
			}
			else if (iPad_LStickX > iPad_LStickY)
			{
				StickSpeedX = StickSpeedY / 4;
			}
		}

		//�����E�X�e�B�b�N
		if (iPad_LStickX > 0)
		{
			if (IsVeryRotate)
			{
				_angle.z += -StickSpeedX * 1.5f;
				if (_angle.z <= -Parameter.RotateZ_StickSpeed * 1.5f)
					_angle.z = -Parameter.RotateZ_StickSpeed * 1.5f;
			}
			else
			{
				_angle.z += -StickSpeedX;
				if (_angle.z <= -Parameter.RotateZ_StickSpeed)
					_angle.z = -Parameter.RotateZ_StickSpeed;
			}

			IsInput = true;
		}
		else if (iPad_LStickX < 0)
		{
			if (IsVeryRotate)
			{
				_angle.z += StickSpeedX * 1.5f;
				if (_angle.z >= Parameter.RotateZ_StickSpeed * 1.5f)
					_angle.z = Parameter.RotateZ_StickSpeed * 1.5f;
			}
			else
			{
				_angle.z += StickSpeedX;
				if (_angle.z >= Parameter.RotateZ_StickSpeed)
					_angle.z = Parameter.RotateZ_StickSpeed;
			}

			IsInput = true;
		}

		//���㉺�X�e�B�b�N
		if (iPad_LStickY > 0)
		{
			if (IsVeryRotate)
			{
				_angle.x += -StickSpeedY * 1.5f;
				if (_angle.x <= -Parameter.RotateX_StickSpeed * 1.5f)
					_angle.x = -Parameter.RotateX_StickSpeed * 1.5f;
			}
			else
			{
				_angle.x += -StickSpeedY;
				if (_angle.x <= -Parameter.RotateX_StickSpeed)
					_angle.x = -Parameter.RotateX_StickSpeed;
			}

			IsInput = true;
		}
		else if (iPad_LStickY < 0)
		{
			if (IsVeryRotate)
			{
				_angle.x += StickSpeedY * 1.5f;
				if (_angle.x >= Parameter.RotateX_StickSpeed * 1.5f)
					_angle.x = Parameter.RotateX_StickSpeed * 1.5f;
			}
			else
			{
				_angle.x += StickSpeedY;
				if (_angle.x >= Parameter.RotateX_StickSpeed)
					_angle.x = Parameter.RotateX_StickSpeed;
			}
			IsInput = true;
		}

		//���\���L�[�̍��E
		if (iPad_right)
		{
			_angle.z -= 1.0f;
			if (_angle.z <= -Parameter.RotateZ_StickSpeed)
				_angle.z = -Parameter.RotateZ_StickSpeed;
			IsInput = true;
		}
		else if (iPad_left)
		{
			_angle.z += 1.0f;
			if (_angle.z >= Parameter.RotateZ_StickSpeed)
				_angle.z = Parameter.RotateZ_StickSpeed;
			IsInput = true;
		}

		//���\���L�[�̏㉺
		if (iPad_up)
		{
			_angle.x -= 0.5f;
			if (_angle.x <= -Parameter.RotateX_StickSpeed)
				_angle.x = -Parameter.RotateX_StickSpeed;
			IsInput = true;
		}
		else if (iPad_down)
		{
			_angle.x += 0.5f;
			if (_angle.x >= Parameter.RotateX_StickSpeed)
				_angle.x = Parameter.RotateX_StickSpeed;
			IsInput = true;
		}
	}

	//�v���C���[����
	if (Input::Get()->GetKeyPress(DIK_A))
	{
		//�}���񃂁[�h�̎�
		if (IsVeryRotate)
		{
			//�v���C���[�̏�x�N�g�����󑤂������Ă���Ȃ瑬�x���������Ă���
			//���̕��A��]���x�������Ȃ�(��̓I�ɂ͒ʏ��2�{���炢
			_angle.z += 2.0f;
			if (_angle.z >= Parameter.RotateZ_StickSpeed * 1.5f)
				_angle.z = Parameter.RotateZ_StickSpeed * 1.5f;
			IsInput = true;
		}
		else
		{
			//_angle.z += speed;
			//_angle.z += ROTATE_STICKSPEED;
			_angle.z += 1.0f;
			if (_angle.z >= Parameter.RotateZ_StickSpeed)
				_angle.z = Parameter.RotateZ_StickSpeed;
			IsInput = true;
		}
	}
	else if (Input::Get()->GetKeyPress(DIK_D))
	{
		if (IsVeryRotate)
		{
			//�v���C���[�̉��x�N�g�����󑤂������Ă���Ȃ瑬�x���������Ă���
			//���̕��A��]���x�������Ȃ�(��̓I�ɂ͒ʏ��2�{���炢
			//�t�ɉ��x�N�g�����n�ʑ��Ȃ�A���x�����X�ɏオ���Ă���
			//��]���x��2�{���炢��
			_angle.z -= 2.0f;
			if (_angle.z <= -Parameter.RotateZ_StickSpeed * 1.5f)
				_angle.z = -Parameter.RotateZ_StickSpeed * 1.5f;
			IsInput = true;
		}
		else
		{
			//_angle.z += -speed;
			//_angle.z += -ROTATE_STICKSPEED;
			_angle.z -= 1.0f;
			if (_angle.z <= -Parameter.RotateZ_StickSpeed)
				_angle.z = -Parameter.RotateZ_StickSpeed;
			IsInput = true;
		}
	}

	if (Input::Get()->GetKeyPress(DIK_W))
	{
		if (IsVeryRotate)
		{
			//_angle.x += -speed;
			//_angle.x += -ROTATE_STICKSPEED;
			_angle.x -= 1.0f;
			if (_angle.x <= -Parameter.RotateX_StickSpeed * 1.5f)
				_angle.x = -Parameter.RotateX_StickSpeed * 1.5f;
			IsInput = true;
		}
		else
		{
			//_angle.x += -speed;
			//_angle.x += -Parameter.Rotate_StickSpeed;
			_angle.x -= 0.5f;
			if (_angle.x <= -Parameter.RotateX_StickSpeed)
				_angle.x = -Parameter.RotateX_StickSpeed;
			IsInput = true;
		}
	}
	else if (Input::Get()->GetKeyPress(DIK_S))
	{
		if (IsVeryRotate)
		{
			_angle.x += 1.0f;
			if (_angle.x >= Parameter.RotateX_StickSpeed * 1.5f)
				_angle.x = Parameter.RotateX_StickSpeed * 1.5f;
			IsInput = true;
		}
		else
		{
			//_angle.x += speed;
			//_angle.x += ROTATE_STICKSPEED;
			_angle.x += 0.5f;
			if (_angle.x >= Parameter.RotateX_StickSpeed)
				_angle.x = Parameter.RotateX_StickSpeed;
			IsInput = true;
		}
	}

	if (Input::Get()->GetKeyPress(DIK_LEFTARROW))
	{
		RightMoveSpeed -= Parameter.Right_Move_Speed;
		if (RightMoveSpeed <= -Parameter.Max_Right_Move_Speed)
			RightMoveSpeed = -Parameter.Max_Right_Move_Speed;

		//_angle.y += -Yspeed;
		_angle.y = -(float)Parameter.Yow_Rotate_Speed;
		IsInput = true;
	}
	else if (Input::Get()->GetKeyPress(DIK_RIGHTARROW))
	{
		RightMoveSpeed += Parameter.Right_Move_Speed;
		if (RightMoveSpeed >= Parameter.Max_Right_Move_Speed)
			RightMoveSpeed = Parameter.Max_Right_Move_Speed;

		_angle.y = (float)Parameter.Yow_Rotate_Speed;
		IsInput = true;
	}

	//�X�s�[�h���삪�Ȃ��ꍇ�A�X�s�[�h�����ɂȂ鏈��
	if (!ChengeSpeed)
	{
		if (MoveSpeed < (float)Parameter.Min_MoveSpeed + Parameter.NoMove_AddSpeed && MoveSpeed >(float)Parameter.Min_MoveSpeed - Parameter.NoMove_AddSpeed)
			SetMoveSpeed((float)Parameter.Min_MoveSpeed);
		else if(MoveSpeed > (float)Parameter.Min_MoveSpeed)
			SetAddMoveSpeed(-(float)Parameter.Move_Add_Speed / 2);
		else if (MoveSpeed < (float)Parameter.Min_MoveSpeed)
			SetAddMoveSpeed((float)Parameter.Move_Add_Speed / 2);
	}
}

void CPlayer::DrawTargetUI()
{
	if (IsEnemyRock)
	{
		pEnemyRock->Draw();

		//����I�Ɍx��SE��炷
		if (GetTickCount64() >= EnemyRockTime + 1000)
		{
			EnemyRockTime = GetTickCount64();
			XA_Play(SE_ENEMYROCK, CScene::Get()->GetSeVolume(),1.0f);
		}
	}

	//�^�[�Q�b�g����Ă���
	if (IsDrawRockUI)
	{
		//���b�N�I������Ă��邩
		if (IsRockOn)
		{
			pRockOnModel->Draw();
		}
		else
		{
			pRockModel->Draw();
		}
	}
}

void CPlayer::SetHitMisileEffect(D3D::Model _model)
{
	pEffect->SetModel(_model);
}

void CPlayer::SetInitRotate()
{
	//���͂��Ȃ��ꍇ�A���[����0�ɕύX����

	DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
	DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

	float angle = 0.0f;

	//��]������p�x���v�Z
	float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
	angle = acosf(dotProduct);

	//�p�x��x���Ōv�Z
	float an = XMConvertToDegrees(angle);
	//angle = XMConvertToRadians(angle);

	if (an > 0.0f)
	{
		//��]�����鎲���v�Z
		XMVECTOR axis = XMVector3Cross(newUP, vTargetDir);

		if (an > Parameter.MaxInitRotate)
		{
			angle = XMConvertToRadians(Parameter.MaxInitRotate);
		}

		XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

		//��]�s��쐬
		XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

		//���݂̉�]�s��ɂ������킹��
		RotateMatrix *= QMT;

		Rotation = ExtractRotationFromMatrix(RotateMatrix);
	}
}

void CPlayer::SetAnimUvSize(DirectX::XMFLOAT2 _uv)
{
	pEffect->SetAnimUVSize(_uv.x, _uv.y);
}

void CPlayer::BivUpdate()
{
	XINPUT_VIBRATION vibration;

	//�U�����邩
	if (GetTickCount64() < BivTime || !isActive)
	{
		//�U����
		//�Q�[���p�b�h��A�܂���B�����������̏����F�Q�[���p�b�h�̐U��
		vibration.wLeftMotorSpeed = 65535;
		XInputSetState(0, &vibration);
	}
	else  
	{
		//�U���X�g�b�v
		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;
		XInputSetState(0, &vibration);
	}
}

void CPlayer::BivStop()
{
	//�U���X�g�b�v
	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);
}

void CPlayer::SetBivTime(ULONGLONG _stoptime)
{
	BivTime = _stoptime;
	ULONGLONG nowtime = GetTickCount64();
}

void CPlayer::SetNotInputRotate()
{
	//�@�̂�Z����]�ƒn�ʂƂ̕��s���𒲂ׂ�
}

void CPlayer::SetRockUI(D3D::Model& _rockmodel, D3D::Model& _rockonmodel)
{
	pRockModel->SetModel(_rockmodel);
	pRockModel->SetBillboard(true);
	pRockModel->SetLighting(false);

	pRockOnModel->SetModel(_rockonmodel);
	pRockOnModel->SetBillboard(true);
	pRockOnModel->SetLighting(false);
}

void CPlayer::InsertMovetargetUI(CEnemy* _targetobject)
{
	MOVETARGET_UI MovetargetUI;
	MovetargetUI.Position = { Position.x + Forward.x * (float)_targetobject->GetPlayerDistance(),
							Position.y + Forward.y * (float)_targetobject->GetPlayerDistance(),
							Position.z + Forward.z * (float)_targetobject->GetPlayerDistance(), };

	MovetargetUI.pTargetObject = _targetobject;

	//�^�[�Q�b�g�ΏۊO�̂��̂��폜
	if (CPlayerAttack::Get()->GetAttackId() == CPlayerAttack::ATTACK_ID::FOURTARGET_FOURMISILE)
	{
		for (auto It = plMoveTargetUI.begin(); It != plMoveTargetUI.end(); It++)
		{
			if ((It)->pTargetObject == _targetobject)
			{
				(It)->pTargetObject->SetCanAttack(false);
				(It)->pTargetObject->SetIsTarget(false);
				It->IsDraw = false;
				It->Position = { 0.0f,0.0f,0.0f };

				plMoveTargetUI.erase(It);
				break;
			}
		}
	}
	else
	{
		for (auto Target : plMoveTargetUI)
		{
			(Target).pTargetObject->SetCanAttack(false);
			(Target).pTargetObject->SetIsTarget(false);
			Target.IsDraw = false;
			Target.Position = { 0.0f,0.0f,0.0f };
		}
		plMoveTargetUI.clear();
	}

	//�z��Ɋi�[
	plMoveTargetUI.push_back(MovetargetUI);
}

void CPlayer::TargetLineUpdate()
{
	//�ŏ��ɏo���ʒu��ݒ�
	//���̌�A�o���ʒu����^�[�Q�b�g�ʒu�܂ł̕����x�N�g�����擾
	//�����x�N�g���ɂ����Ĉړ�������
	for (auto It = plMoveTargetUI.begin(); It != plMoveTargetUI.end(); It++)
	{
		//if (!It->pTargetObject->GetIsCameraRef())
		//{
		//	It->IsDraw = false;
		//	It->Position = { 0.0f,0.0f,0.0f };
		//	plMoveTargetUI.erase(It);
		//	break;
		//}

		//�����^�[�Q�b�g������ꍇ
		if (!CPlayerAttack::Get()->GetIsOnetarget())
		{
			//�^�[�Q�b�g���Ă���G���^�[�Q�b�g��Ԃ�
			if (!(It)->pTargetObject->GetIsTarget())
			{
				(It)->pTargetObject->SetCanAttack(false);
				(It)->pTargetObject->SetIsTarget(false);
				It->IsDraw = false;
				It->Position = { 0.0f,0.0f,0.0f };
				//�^�[�Q�b�g��Ԃł͂Ȃ�
				plMoveTargetUI.erase(It);
				break;
			}
		}

		if (It->pTargetObject->GetActive() == false)
		{
			(It)->pTargetObject->SetCanAttack(false);
			(It)->pTargetObject->SetIsTarget(false);
			It->IsDraw = false;
			It->Position = { 0.0f,0.0f,0.0f };
			plMoveTargetUI.erase(It);
			break;
		}

		//�J�����ɉf���Ă��Ȃ�������`�悵�Ȃ�
		if (!It->pTargetObject->GetIsCameraRef())
		{
			It->IsDraw = false;
		}

		//�^�[�Q�b�g���J�����ɉf���Ă����Ԃ���ы�����150�ȓ��̂Ƃ��Ɉړ�
		if ((It)->pTargetObject->GetIsCameraRef() && (It)->pTargetObject->GetPlayerDistance() <= 150)
		{
			(It)->IsDraw = true;

			//�ړ����x
			const float constantSpeed = 60.0f;

			// ���Ԍo�߂��l������i��FdeltaTime ��n�����Ɖ���j
			float deltaTime = 0.016f; // 1�t���[���̎��ԁi��F60FPS�̏ꍇ�j

			// �^�[�Q�b�g�̌��݈ʒu�Ƒ��x���擾
			XMVECTOR vPosition = XMLoadFloat3(&(It)->Position);
			XMVECTOR vTargetPosition = XMLoadFloat3(&(It)->pTargetObject->Position);

			// �^�[�Q�b�g�܂ł̕����x�N�g�����v�Z
			XMVECTOR direction = XMVectorSubtract(vTargetPosition, vPosition);
			direction = XMVector3Normalize(direction);

			// �^�[�Q�b�g�܂ł̎c�苗�����v�Z
			XMVECTOR totalDistance = XMVectorSubtract(vTargetPosition, vPosition);
			float distance = XMVectorGetX(XMVector3Length(totalDistance));

			// �^�[�Q�b�g���߂Â��������ꍇ�̕␳
			if (distance <= constantSpeed * deltaTime) {
				// �^�[�Q�b�g�ɒ��ړ��B
				XMStoreFloat3(&(It)->Position, vTargetPosition);
				(It)->IsDraw = false;
				(It)->pTargetObject->SetCanAttack(true);
			}
			else {
				// ��葬�x�ňړ�
				XMVECTOR velocity = XMVectorScale(direction, constantSpeed);
				XMVECTOR newPosition = XMVectorAdd(vPosition, XMVectorScale(velocity, deltaTime));
				XMStoreFloat3(&(It)->Position, newPosition);
				(It)->pTargetObject->SetCanAttack(false);
			}
		}
		else if (!(It)->pTargetObject->GetIsCameraRef())
		{
			(It)->pTargetObject->SetCanAttack(false);
			(It)->pTargetObject->SetIsTarget(false);

			(It)->Position = { Position.x + Forward.x * (float)(It)->pTargetObject->GetPlayerDistance(),
							   Position.y + Forward.y * (float)(It)->pTargetObject->GetPlayerDistance(),
							   Position.z + Forward.z * (float)(It)->pTargetObject->GetPlayerDistance(), };
		}
		else
		{
			(It)->IsDraw = false;
			(It)->Position = { Position.x + Forward.x * (float)(It)->pTargetObject->GetPlayerDistance(),
								Position.y + Forward.y * (float)(It)->pTargetObject->GetPlayerDistance(),
								Position.z + Forward.z * (float)(It)->pTargetObject->GetPlayerDistance(), };

			(It)->pTargetObject->SetCanAttack(false);

		}
	}
}

void CPlayer::DrawTargetLine()
{
	if (CScene::Get()->GetNowScene() == CScene::SCENE::STAGE || CScene::Get()->GetNowScene() == CScene::SCENE::TEST_STAGE)
	{
		if (!plMoveTargetUI.empty())
		{
			for (auto& TargetUI : plMoveTargetUI)
			{
				if (TargetUI.IsDraw)
				{
					// ���[���h���W���x�N�g���`���ɕϊ�
					XMVECTOR worldPosition = XMLoadFloat3(&TargetUI.Position);

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
					float rectSize = 50.0f; // �g�̃T�C�Y
					rectSize = rectSize * clipZ * 0.5f; // �X�P�[�����O��̘g�T�C�Y
					DirectX::XMFLOAT2 EnemyFrameLeft = { screenPos.x - rectSize * 0.5f,screenPos.y - rectSize * 0.5f };
					DirectX::XMFLOAT2 EnemyFrameRight = { screenPos.x + rectSize * 0.5f, screenPos.y + rectSize * 0.5f };

					ImVec2 Vertex[4];
					Vertex[0] = ImVec2(screenPos.x, screenPos.y - rectSize / 2.0f); // �㒸�_
					Vertex[1] = ImVec2(screenPos.x + rectSize / 2.0f, screenPos.y); // �E���_
					Vertex[2] = ImVec2(screenPos.x, screenPos.y + rectSize / 2.0f); // �����_
					Vertex[3] = ImVec2(screenPos.x - rectSize / 2.0f, screenPos.y); // �����_
					ImGui::GetForegroundDrawList()->AddPolyline(Vertex, 4, IM_COL32(255, 0, 0, 255), true, 1.0f);
				}
			}
		}
	}
	else
	{
		//plMoveTargetUI.clear();
	}
}

void CPlayer::ClearMoveTargetList()
{
	if (!plMoveTargetUI.empty())
	{
		for (auto MoveTarget : plMoveTargetUI)
		{
			(MoveTarget).pTargetObject->SetCanAttack(false);
			(MoveTarget).pTargetObject->SetIsTarget(false);
			MoveTarget.IsDraw = false;
			MoveTarget.Position = { Position.x + Forward.x * (float)MoveTarget.pTargetObject->GetPlayerDistance(),
									Position.y + Forward.y * (float)MoveTarget.pTargetObject->GetPlayerDistance(),
									Position.z + Forward.z * (float)MoveTarget.pTargetObject->GetPlayerDistance(), };
		}
		plMoveTargetUI.clear();
	}
}

void CPlayer::DeleteMoveTargetList(CEnemy* _enemy)
{
	for (auto It = plMoveTargetUI.begin(); It != plMoveTargetUI.end(); It++)
	{
		if (It->pTargetObject == _enemy)
		{
			(It)->pTargetObject->SetCanAttack(false);
			(It)->pTargetObject->SetIsTarget(false);
			It->IsDraw = false;
			It->Position = { 0.0f,0.0f,0.0f };
			plMoveTargetUI.erase(It);
			break;
		}
	}
}

void CPlayer::SaveParameter()
{
	//�p�����[�^�̃t�@�C�����J��
	std::ofstream outFile("Parameter/Player.txt", std::ios::trunc);

	//�t�@�C���I�[�v��
	if (!outFile.is_open()) {
		// �ǂݍ��݂Ɏ��s�����ꍇ
		MessageBoxA(NULL, "Miss Open File", "�G���[",
			MB_OK | MB_ICONERROR);
	}

	// �������ރe�L�X�g//
	string ParameterText;

	string MaxInitRotate = to_string(Parameter.MaxInitRotate) + ",";
	string Max_SlipZ = to_string(Parameter.Max_SlipZ) + ",";
	string Max_SlipX = to_string(Parameter.Max_SlipX) + ",";
	string Particle_DeleteTime = to_string(Parameter.Particle_DeleteTime) + ",";
	string Particle_CoolTime_Low = to_string(Parameter.Particle_CoolTime_Low) + ",";
	string Particle_CoolTime_Normal = to_string(Parameter.Particle_CoolTime_Normal) + ",";
	string Particle_CoolTime_High = to_string(Parameter.Particle_CoolTime_High) + ",";
	string Move_Add_Speed = to_string(Parameter.Move_Add_Speed) + ",";
	string NoMove_AddSpeed = to_string(Parameter.NoMove_AddSpeed) + ",";
	string Right_Move_Speed = to_string(Parameter.Right_Move_Speed) + ",";
	string Max_Right_Move_Speed = to_string(Parameter.Max_Right_Move_Speed) + ",";
	string Max_MoveSpeed = to_string(Parameter.Max_MoveSpeed) + ",";
	string Min_MoveSpeed = to_string(Parameter.Min_MoveSpeed) + ",";
	string RotateX_StickSpeed = to_string(Parameter.RotateX_StickSpeed) + ",";
	string RotateZ_StickSpeed = to_string(Parameter.RotateZ_StickSpeed) + ",";
	string Yow_Rotate_Speed = to_string(Parameter.Yow_Rotate_Speed);

	ParameterText = MaxInitRotate + Max_SlipZ + Max_SlipX + Particle_DeleteTime + Particle_CoolTime_Low + Particle_CoolTime_Normal + Particle_CoolTime_High;
	ParameterText = ParameterText + Move_Add_Speed + NoMove_AddSpeed + Right_Move_Speed + Max_Right_Move_Speed + Max_MoveSpeed + Min_MoveSpeed;
	ParameterText = ParameterText + RotateX_StickSpeed + RotateZ_StickSpeed + Yow_Rotate_Speed;

	// �t�@�C���Ƀe�L�X�g����������
	outFile << ParameterText;

	outFile.close();
}

