#include "CGame.h"
#include "CGameObject.h"
#include "Input.h"
#include "CCamera.h"
#include "CEffect.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CVector.h"
#include "CScene.h"
#include "CPlayerAttack.h"
#include "CEnemyAttack.h"
#include "CEffectManager.h"
#include "CControllerInput.h"
#include "DrawLine.h"
#include "xa2.h"
#include "DebugUI.h"
#include <random>
#include <iostream>

using namespace DirectX;

//�t�F�[�h���x
#define FadeSpeed (0.02)

//�n�ʂ�y���W
#define GROUND_YPOS (-60)

//�~�T�C���������������ɃJ�������h��鎞��
#define SHAKE_CAMERA_TIME (500)

CGame::~CGame()
{
	//�T�E���h�̉������
	XA_Release();

	//�I�u�W�F�N�g�������[�X����
	ReleseObject();
}

CGame* CGame::Get()
{
	static CGame instance;
	return &instance;
}

/**
*�Q�[���I�u�W�F�N�g�쐬�ƃA�Z�b�g�ǂݍ��݂�����֐��ł��B
*/
void CGame::SetGameObjectInit()
{
	pFadePanel = new CGameObject();
	pWaitPanel = new CGameObject();

	//pFadePanel = std::make_unique<CGameObject>();
	//pWaitPanel = std::make_unique<CGameObject>();

	pShotPosUI = new CGameObject();
	pCountDownUI = new CEffect();
	RuleUI = new CGameObject();

	camera3D = new CCamera();
	camera2D = new CCamera();
	MiniMapCam = new CCamera();
	DebugCamera = new CCamera();

	//�T�E���h�̏�����(�T�E���h�t�@�C���̓ǂݍ��݂�)
	HRESULT hr;
	hr = XA_Initialize();

	//�T�E���h�̏������̃G���[�`�F�b�N
	//����if���ɓ������玸�s
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "�T�E���h���������s", "�G���[", MB_ICONERROR | MB_OK);
	}

	// ���f���ǂݍ���
	const wchar_t* modelFileName[] =
	{
		L"asset/CHARACTER/Cube.obj",
		L"asset/CHARACTER/Bill.obj",
		L"asset/CHARACTER/Circle.obj",
		L"asset/CHARACTER/EngineParticle.obj",
		L"asset/CHARACTER/Enemy.obj",
		L"asset/CHARACTER/Ground.obj",
		L"asset/CHARACTER/MiniMapGround.obj",
		L"asset/CHARACTER/PlayerMiniMap.obj",
		L"asset/CHARACTER/EnemyMiniMap.obj",
		L"asset/CHARACTER/GunEnemy.obj",
		L"asset/CHARACTER/TargetEnemyMiniMap.obj",
		L"asset/CHARACTER/Misile.obj",
		L"asset/CHARACTER/MiniMapMisile.obj",
		L"asset/CHARACTER/Player.obj",
		L"asset/CHARACTER/Vectol.obj",
		L"asset/CHARACTER/cylinder.obj",

		L"asset/CHARACTER/F-2.obj",
		L"asset/CHARACTER/F-2Test.obj",
		L"asset/CHARACTER/F-2Test2.obj",
		L"asset/CHARACTER/SU_27.obj",

		L"asset/CHARACTER/Fighter_Jet.obj",
		L"asset/CHARACTER/Fighter_EnemyJet.obj",
		L"asset/CHARACTER/Circle_Collider.obj",
		L"asset/CHARACTER/BoxCollider.obj",
		L"asset/CHARACTER/BackCircle.obj",
		L"asset/CHARACTER/GroundEnemy1.obj",
		L"asset/CHARACTER/MiniMapGroundEnemy.obj",
		L"asset/CHARACTER/Clooud.obj",
		L"asset/CHARACTER/Sun.obj",
	};

	modelManager = new ModelHash();

	for (int i = 0; i < ARRAYSIZE(modelFileName); i++)
	{
		model = D3D::LoadObjModel(modelFileName[i]);
		modelManager->emplace(modelFileName[i], model);
	}

	//model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.33f, 0.25f });
	//model.texture = D3D::LoadTexture(L"asset/fx01.png");
	//modelManager->emplace(L"asset/fx01.png", model);

	//�~�T�C���q�b�g�G�t�F�N�g
	model = D3D::CreateSquare({ 0,0 }, { 3,3 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/Effect_01.png");
	modelManager->emplace(L"asset/Effect_01.png", model);

	//�W�F�b�g�G�t�F�N�g
	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/JetEffect_low.png");
	modelManager->emplace(L"asset/JetEffect_low.png", model);

	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/JetEffect_normal.png");
	modelManager->emplace(L"asset/JetEffect_normal.png", model);

	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/JetEffect_high.png");
	modelManager->emplace(L"asset/JetEffect_high.png", model);

	//�e�ۃq�b�g�G�t�F�N�g
	model = D3D::CreateSquare({ 0,0 }, { 2,2 }, { 0.0714f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/Effect_02.png");
	modelManager->emplace(L"asset/Effect_02.png", model);

	//�~�T�C��UI
	model = D3D::CreateSquare({ 0,0 }, { 0.15f,0.5f }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/UI/Misile_UI_Test.png");
	modelManager->emplace(L"asset/UI/Misile_UI_Test.png", model);

	//�J�n�̃J�E���g�_�E��
	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.25f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/UI/StartUI.png");
	modelManager->emplace(L"asset/UI/StartUI.png", model);

	// �P���|���S���{�e�N�X�`���̃��f������
	const wchar_t* textureFileName[] =
	{
		L"asset/None.png",

	   L"asset/UI/MoveTargetUI.png",
	   L"asset/UI/No_Rock.png",
	   L"asset/UI/Rock.png",
	   L"asset/UI/RockOn.png",
	   L"asset/UI/EnemyRock_UI.png",
	   L"asset/UI/NewRockOn.png",
	   L"asset/UI/FightJet_UI_01.png",
	   L"asset/UI/FightJet_UI_02.png",
	   L"asset/UI/FightJet_UI_03.png",
	   L"asset/UI/Misile_UI.png",
	   L"asset/UI/MiniMapFrame.png",
	   L"asset/UI/Center_UI.png",
	   L"asset/UI/MapFrame.png",

	   L"asset/TestBackFire.png",
	   L"asset/Misile2.png",

	   L"asset/SpeedUpPart.png",

	   L"asset/TestNoise.png",
	   L"asset/MaskNoise.png",
	   L"asset/DistNoise.png",
		L"asset/dissolve.png",
		L"asset/BayerMatrix.png",

	   L"asset/UI/Title_UI_01.png",
	   L"asset/UI/PressAButton.png",

	   L"asset/UI/GameClear_01.png",
	   L"asset/UI/GameOver.png",
	   L"asset/UI/TranceParent.png",
	   L"asset/UI/GunCircle.png",

	   L"asset/UI/Menu_Panel.png",
	   L"asset/UI/MenuCarsol.png",
	   L"asset/UI/GameEnd.png",
	   L"asset/UI/GameStart.png",
	   L"asset/UI/STAGE_01.png",
	   L"asset/UI/STAGE_02.png",
	   L"asset/UI/STAGE_03.png",
	   L"asset/UI/STAGE_04.png",
	   L"asset/UI/STAGE_05.png",

	   L"asset/UI/FirstUI.png",
	   L"asset/UI/MISILE_HIT_UI.png",
	   L"asset/UI/Warning.png",

	   L"asset/UI/RockPlayer.png",
	   L"asset/UI/RockOnPlayer.png",

	   L"asset/UI/Stage_4M1A.png",
	   L"asset/UI/Stage_4M4A.png",
	   L"asset/UI/Stage_ALT.png",
	   L"asset/UI/Stage_GUN.png",
	   L"asset/UI/Stage_MSL.png",
	   L"asset/UI/Stage_SCORE.png",
	   L"asset/UI/Stage_SPEED.png",
	   L"asset/UI/Stage_TIME.png",
	   L"asset/UI/STAGE_FRAME.png",

	   L"asset/UI/A_UI.png",
	   L"asset/UI/B_UI.png",
	   L"asset/UI/X_UI.png",
	   L"asset/UI/Y_UI.png",
	   L"asset/UI/LT_UI.png",
	   L"asset/UI/RT_UI.png",
	   L"asset/UI/Enter_UI.png",
	   L"asset/UI/Back_UI.png",
	   L"asset/UI/Attack_UI.png",
	   L"asset/UI/ChengeTarget_UI.png",
	   L"asset/UI/SpeedUP_UI.png",
	   L"asset/UI/SpeedDown_UI.png",
	   L"asset/UI/RuleUI.png",
	   L"asset/UI/BossUI.png",
	   L"asset/UI/BossHpUI.png",

	   L"asset/White_Circlr.png",

	   L"asset/Title.png",
	   L"asset/End.png",
	   L"asset/Result.png",
	   L"asset/FadePanel.png",
	   L"asset/WaitPanel.png",
	   L"asset/sky_01.png",
	   L"asset/Wave.png",
	   L"asset/Bill.png",
	   L"asset/Bill2.png",

	   L"asset/Smoke.png",
	   L"asset/SmokeTest.png",
	   L"asset/Red.png",
	   L"asset/White.png",
	   L"asset/Black.png",
	   L"asset/Gray.png",

	   L"asset/NUM/Zero.png",
	   L"asset/NUM/One.png",
	   L"asset/NUM/Two.png",
	   L"asset/NUM/Three.png",
	   L"asset/NUM/Four.png",
	   L"asset/NUM/Five.png",
	   L"asset/NUM/Six.png",
	   L"asset/NUM/Seven.png",
	   L"asset/NUM/Eight.png",
	   L"asset/NUM/Nine.png",

	   L"asset/NUM/Stage_0.png",
	   L"asset/NUM/Stage_1.png",
	   L"asset/NUM/Stage_2.png",
	   L"asset/NUM/Stage_3.png",
	   L"asset/NUM/Stage_4.png",
	   L"asset/NUM/Stage_5.png",
	   L"asset/NUM/Stage_6.png",
	   L"asset/NUM/Stage_7.png",
	   L"asset/NUM/Stage_8.png",
	   L"asset/NUM/Stage_9.png",
	};

	for (int i = 0; i < ARRAYSIZE(textureFileName); i++)
	{
		model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 1,1 });
		model.texture = D3D::LoadTexture(textureFileName[i]);
		modelManager->emplace(textureFileName[i], model);
	}

	Initialize();
}

/**
 *�Q�[���I�u�W�F�N�g�̏���������������֐��ł��B
 */
void CGame::Initialize()
{
	//SetGameObjectInit();

	//�������Ԑݒ�
	StartProjTime = GetTickCount64();
	SetNowMoveTime();

	//���f������

	//�~�T�C���̏����Z�b�g
	CPlayerAttack::Get()->SetMisileInf(modelManager->at(L"asset/CHARACTER/Misile.obj"), modelManager->at(L"asset/Effect_01.png"), modelManager->at(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });
	//�@�֏e�̒e�̏����Z�b�g
	CPlayerAttack::Get()->SetBallInf(modelManager->at(L"asset/CHARACTER/Cube.obj"), modelManager->at(L"asset/Effect_02.png"), { 12,1 });

	CEnemyAttack::Get()->SetMisileInf(modelManager->at(L"asset/CHARACTER/Misile.obj"), modelManager->at(L"asset/Effect_01.png"), modelManager->at(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });

	//�p�[�e�B�N���̃��f���ݒ�
	model = modelManager->at(L"asset/CHARACTER/Circle.obj");
	//CParticleManager::Get()->SetParticleModel(model, CParticleManager::MODEL_NAME::MISILE_PARTICLE);
	CPlayerAttack::Get()->SetMisileParticleModel(model);
	CEnemyAttack::Get()->SetMisileParticleModel(model);

	model = modelManager->at(L"asset/UI/GunCircle.png");
	pShotPosUI->SetModel(model);
	pShotPosUI->SetBillboard(true);
	pShotPosUI->SetLighting(false);

	//�t�F�[�h�p�l��
	model = modelManager->at(L"asset/FadePanel.png");
	pFadePanel->SetModel(model);
	pFadePanel->SetBillboard(true);
	pFadePanel->SetLighting(false);

	//���~�p�l��
	model = modelManager->at(L"asset/WaitPanel.png");
	//model = modelManager->at(L"asset/Misile2.png");
	//model = modelManager->at(L"asset/TestBackFire.png");
	pWaitPanel->SetModel(model);
	pWaitPanel->SetBillboard(true);
	pWaitPanel->SetLighting(false);

	//�X�^�[�g�J�E���g�_�E��UI
	model = modelManager->at(L"asset/UI/StartUI.png");
	pCountDownUI->SetModel(model);
	pCountDownUI->SetBillboard(true);
	pCountDownUI->SetLighting(false);
	pCountDownUI->SetAnimUVSize(4, 1);

	//���[���\��
	model = modelManager->at(L"asset/UI/RuleUI.png");
	RuleUI->SetModel(model);
	RuleUI->SetBillboard(true);
	RuleUI->SetLighting(false);

	//pWarning->Position = { 0.0f,0.3f,0.0f };
	//pWarning->Rotation = { 0.0f,0.0f,0.0f };
	//pWarning->Scaling = { 1.0f, 0.3f, 1.0f };
	//pWarning->materialDiffuse.w = 0.6f;

	pShotPosUI->Position = { 0.0f,0.0f,0.0f };
	pShotPosUI->Rotation = { 0.0f,0.0f,0.0f };
	pShotPosUI->Scaling = { 0.4f, 0.4f, 0.4f };
	////UI�ݒ�
	//pShotPosUI->SetActive(false);

	pFadePanel->Position = { 0.0f,0.0f,-0.5f };
	pFadePanel->Rotation = { 0.0f,0.0f,0.0f };
	pFadePanel->Scaling = { 13.5f, 7.5f, 1.0f };
	pFadePanel->materialDiffuse.w = 1.0f;

	pWaitPanel->Position = { 0.0f,0.0f,-0.4f };
	pWaitPanel->Rotation = { 0.0f,0.0f,0.0f };
	pWaitPanel->Scaling = { 13.5f, 7.5f, 1.0f };
	pWaitPanel->materialDiffuse.w = 0.5f;
	pWaitPanel->SetTag(CGameObject::TAG::WAIT_UI);

	pCountDownUI->Position = { 0.0f,0.5f,-0.3f };
	pCountDownUI->Rotation = { 0.0f,0.0f,0.0f };
	pCountDownUI->Scaling = { 1.0f,1.0f,1.0f };
	pCountDownUI->materialDiffuse.w = 1.0f;

	RuleUI->Position = { 0.0f,-0.5f,0.0f };
	RuleUI->Rotation = { 0.0f,0.0f,0.0f };
	RuleUI->Scaling = { 3.0f,0.6f,1.0f };
	RuleUI->materialDiffuse.w = 1.0f;

	//�^�O�̐ݒ�

	// �J�����𐶐�
	MiniMapCam->SetCameraMode(CCamera::MINIMAP);
	camera2D->SetProjectionMode(CCamera::_2D);
	camera2D->SetEye({ 0,0,-1 });
	camera2D->SetFocus({ 0,0,1 });
	camera2D->Update();

	DebugCamera->InitMatrixRotate();
	DebugCamera->SetIsDebug();
	DebugCamera->SetEye({ 0,0,0 });
	DebugCamera->SetFocus({ 0,0,10 });
	DebugCamera->Update();

	SetFade(FADE_OUT);

	SetCamera(camera3D);

	cameraPos = { 0.0f,1.0f,-4.0f };

	CameraId = Third;

	//�����X�e�[�W�쐬�V�[���Ȃ�


	//XA_Play(BGM_TITLE,CScene::Get()->GetBGMVolume(), 1.0f);
}

/**
 *�Q�[���̍X�V����������֐��ł��B
 */
void CGame::Update()
{
	SetNowMoveTime();

	XMFLOAT3 resFor = { 0,0,10 };
	XMFLOAT3 resup = { 0,1,0 };

	switch (Fade)
	{
	case CGame::NO_FADE:
		WaitFadeSecne = false;
		break;
	case CGame::FADE_IN:
		if (pFadePanel->materialDiffuse.w < 1)
		{
			pFadePanel->SetActive(true);
			//�t�F�[�h�p�l��
			pFadePanel->materialDiffuse.w += (float)FadeSpeed;
		}
		else if (pFadePanel->materialDiffuse.w >= 1)
		{
			//�t�F�[�h�p�l��
			pFadePanel->materialDiffuse.w = 1.0f;
			SetFade(NO_FADE);
		}
		break;
	case CGame::FADE_OUT:
		if (pFadePanel->materialDiffuse.w > 0)
		{
			//�t�F�[�h�p�l��
			pFadePanel->materialDiffuse.w -= (float)FadeSpeed;

		}
		else if (pFadePanel->materialDiffuse.w <= 0)
		{
			//�t�F�[�h�p�l����UI
			pFadePanel->materialDiffuse.w = 0.0f;
			pFadePanel->SetActive(false);

			SetFade(NO_FADE);
		}
		break;
	default:
		break;
	}

	switch (CScene::Get()->GetNowScene())
	{
	case CScene::TITLE:
	{
		//�_�𓮂���
		std::list<CGameObject*> CloudList;
		CScene::Get()->GetCloudObject(CloudList);
		if (CloudList.size() < 5)
		{
			if (GetTickCount64() >= CloudOldTime + 500)
			{
				CScene::Get()->InsertCloudObject();
				CloudOldTime = GetTickCount64();
			}
		}
		for (auto Cloud : CloudList)
		{
			Cloud->Position.z -= 0.1f;

			if (Cloud->Position.z <= -5.0f)
			{
				// ����������̏�����
				std::random_device rd;
				std::mt19937 rng(rd());

				// �����͈̔͂��w��
				std::uniform_real_distribution<float> dist(-5.0f, 5.0f);

				Cloud->Position.x = dist(rng);
				Cloud->Position.z = 10.0f;
			}
		}

		//�J�����̐ݒ�
		//�J�����ʒu��ݒ�
		camera3D->SetEye(cameraPos);
		//�J�����̌����Ă�������ݒ�
		camera3D->SetFocus({ 0.0f,0.0f,10.0f });
		//������x�N�g���ݒ�
		camera3D->SetUp({ 0.0f,1.0f,0.0f });
		//�X�V����
		camera3D->Update(); // �ݒ肵�����W�Ńr���[�ϊ��s����X�V

		//�V�[���̍X�V����
		CScene::Get()->Update();

		//�G�t�F�N�g�̍X�V����
		CEffectManager::Get()->Update();

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				CScene::Get()->SetScene(CScene::STAGE);
				CScene::Get()->SetScene(CScene::MENU);
				SetFade(FADE_OUT);
				WaitFadeSecne = false;
			}
		}

		if (Fade != FADE_OUT)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN) || pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_A))
			{
				if (!WaitFadeSecne)
				{
					XA_Play(SE_STARTGAME , CScene::Get()->GetSeVolume(), 1.0f);
					SetFade(FADE_IN);
					WaitFadeSecne = true;
				}
			}
		}

		break;
	}
	case CScene::MENU:
	{
		if (Input::Get()->GetKeyTrigger(DIK_DOWN) ||
			pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_DOWN))
		{
			if (!IsCheckMainMenu)
			{
				//�������킪�ő�l����Ȃ�������
				//MainMenu = static_cast<MAINMENU>(static_cast<int>(MainMenu) + 1);

				if (MainMenu != MMENU_QUITGAME)
				{
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position = { -0.95f,-0.5f,0.0f };
					//pMenuCarsol->Position = { -0.65f,-0.5f,0.0f };
				}

				MainMenu = MMENU_QUITGAME;

			}
			else
			{
				GameStage = static_cast<GAMESTAGE>(static_cast<int>(GameStage) + 1);

				if (GameStage != GMENU_MAX)
				{
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position.y -= 0.4f;
					//pMenuCarsol->Position = { 0.2f,0.5f,0.0f };
					//pMenuCarsol->Position.y -= 0.3f;
				}
				else
					GameStage = GMENU_STAGE4;
			}
		}
		else if (Input::Get()->GetKeyTrigger(DIK_UP) ||
			pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_UP))
		{
			if (!IsCheckMainMenu)
			{
				if (MainMenu != MMENU_NONE)
				{
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position = { -0.95f,0.0f,0.0f };
					//pMenuCarsol->Position = { -0.65f,0.0f,0.0f };
				}

				MainMenu = MMENU_GAMESTART;
			}
			else
			{
				GameStage = static_cast<GAMESTAGE>(static_cast<int>(GameStage) - 1);

				if (GameStage != GMENU_NONE)
				{
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position.y += 0.4f;

					//pMenuCarsol->Position = { 0.2f,0.5f,0.0f };
					//pMenuCarsol->Position.y += 0.3f;
				}
				else
					GameStage = GMENU_STAGE1;
			}
		}


		//���̃V�[���Ɉړ�
		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				SetFade(FADE_OUT);
				WaitFadeSecne = false;

				switch (MainMenu)
				{
				case CGame::MMENU_NONE:
					break;
				case CGame::MMENU_GAMESTART:
					switch (GameStage)
					{
					case CGame::GMENU_NONE:
						break;
					case CGame::GMENU_STAGE1:
						CScene::Get()->SetScene(CScene::STAGE_01);
						break;
					case CGame::GMENU_STAGE2:
						CScene::Get()->SetScene(CScene::STAGE_02);
						break;
					case CGame::GMENU_STAGE3:
						CScene::Get()->SetScene(CScene::STAGE_03);
						break;
					case CGame::GMENU_STAGE4:
						CScene::Get()->SetScene(CScene::STAGE_04);
						break;
					case CGame::GMENU_MAX:
						CScene::Get()->SetScene(CScene::STAGE_04);
						break;
					default:
						break;
					}
					break;
				case CGame::MMENU_QUITGAME:
					IsGameEnd = true;
					break;
				case CGame::MMENU_MAX:
					break;
				default:
					break;
				}

			}
		}

		if (Fade != FADE_OUT)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN) ||
				pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_A))
			{
				//�s���ȃ��j���[�ԍ�����Ȃ����
				//if ((MainMenu != MMENU_NONE && GameStage != GMENU_NONE) || MainMenu == MMENU_QUITGAME)
				//{
				//	SetFade(FADE_IN);
				//	WaitFadeSecne = true;
				//}

				if (IsCheckMainMenu && IsCheckGameStage)
				{
					//XA_Play(SE_MOVETITLE);
					//SetFade(FADE_IN);
					//WaitFadeSecne = true;
				}
				else if (!IsCheckMainMenu && !IsCheckGameStage)
				{
					if (MainMenu == MMENU_QUITGAME)
					{
						SetFade(FADE_IN);
						WaitFadeSecne = true;
						return;
					}
					//XA_Play(SE_MOVETITLE);
					//���C�����j���[����
					IsCheckMainMenu = true;
					GameStage = GMENU_STAGE1;
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position = { 0.1f,0.5f,0.0f };
					//pMenuCarsol->Position = { 0.2f,0.5f,0.0f };
				}
				else if (IsCheckMainMenu && !IsCheckGameStage)
				{
					XA_Play(SE_STARTGAME, CScene::Get()->GetSeVolume(), 1.0f);

					//�X�e�[�W����
					IsCheckGameStage = true;

					SetFade(FADE_IN);
					WaitFadeSecne = true;
				}

			}

			if (Input::Get()->GetKeyTrigger(DIK_ESCAPE) ||
				pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_B))
			{
				if (!IsCheckMainMenu && !IsCheckGameStage)
				{
					return;
				}
				else if (IsCheckMainMenu && !IsCheckGameStage)
				{
					IsCheckMainMenu = false;
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position = { -0.95f,0.0f,0.0f };
					//pMenuCarsol->Position = { -0.65f,0.0f,0.0f };

				}
			}
		}

		//�V�[���̍X�V����
		CScene::Get()->Update();

		break;
	}
	case CScene::STAGE:
	{
		if (Fade == NO_FADE && !IsStartGame)
		{
			if (!IsStartCountDown)
			{
				CEffectManager::Get()->SetvEffectMisileUI(pCountDownUI, 5, 0.03f);
				IsStartCountDown = true;
				XA_Play(SE_COUNTDOWN2, CScene::Get()->GetSeVolume(), 1.8f);
			}

			//�J�E���g�_�E���̃A�j���[�V�����I��
			if (pCountDownUI->GetAnimPlay() == false)
			{
				IsStartGame = true;
			}
		}

		//�V�[���ݒ�
		if (!WaitFadeSecne)
		{
			//�G�̐����w�萔�ȉ��܂��̓v���C���[�����Ȃ��Ȃ����Ȃ�t�F�[�h�ȍ~
			if (CPlayerAttack::Get()->GetEnemyNum() == 1 || !GetPlayer()->GetActive())
			{
				SetFade(FADE_IN);
				WaitFadeSecne = true;
			}
		}

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				if (BackStage)
				{
					//���j���[�Ɉڍs
					SetFade(FADE_OUT);
					WaitFadeSecne = false;
					BackStage = false;
					IsCheckMainMenu = false;
					IsCheckGameStage = false;
					GetPlayer()->BivStop();
					CScene::Get()->SetScene(CScene::MENU);
					//IsStop = false;
				}
				else
				{
					//���U���g��ʂɈڍs
					CameraId = Third;
					//�G�̐���1���̓v���C���[�̃A�N�e�B�u��false
					if (CPlayerAttack::Get()->GetEnemyNum() == 1)
					{
						SetFade(FADE_OUT);
						WaitFadeSecne = false;
						GetPlayer()->BivStop();
						CScene::Get()->SetScene(CScene::GAMECLEAR);
						return;
					}
					else if (!GetPlayer()->GetActive())
					{
						SetFade(FADE_OUT);
						WaitFadeSecne = false;
						GetPlayer()->BivStop();
						CScene::Get()->SetScene(CScene::GAMEOVER);
						return;
					}
				}
			}
		}

		//�ꎞ��~
		if (Input::Get()->GetKeyTrigger(DIK_ESCAPE))
		{
			SetIsGameStop();
		}
		if (IsStop)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN))
			{
				if (!WaitFadeSecne)
				{
					SetFade(FADE_IN);
					WaitFadeSecne = true;
					BackStage = true;
				}
			}
			return;
		}

		//���_�؂�ւ�
		if (pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_MENU) ||
			Input::Get()->GetKeyTrigger(DIK_Y))
		{
			if (CameraId == First)
				CameraId = Third;
			else if (CameraId == Third)
				CameraId = First;
		}

		//�G�t�F�N�g�̍X�V����
		CEffectManager::Get()->Update();

		if (IsStartGame)
		{
			//�G�̍X�V����
			CPlayerAttack::Get()->EnemyUpdates();
		}

		////���C���J�����̍��W��ݒ肷��
		//if (GetPlayer()->GetActive())
		//	SetCameraPos();

		//�V�[���̍X�V����
		CScene::Get()->Update();

		//���C���J�����̍��W��ݒ肷��
		if (GetPlayer()->GetActive())
			SetCameraPos();

		if (IsStartGame)
		{
			CPlayerAttack::Get()->Update();
			CEnemyAttack::Get()->Update();
		}

		//����������p�g��A�t�ɂ���ȊO�̎��͌��̎���p�ɖ߂�
		if (GetPlayer()->GetIsFastSpeed())
		{
			if (camera3D->GetFovRadius() >= 50.0f)
			{
				camera3D->SetFovRadius(50.0f);
			}
			else
			{
				camera3D->AddFovRadius(0.1f);
			}
		}
		else
		{
			camera3D->MoveInitFovRadius(0.1f);
		}

		//�~�j�}�b�v�ݒ�
		if (Input::Get()->GetKeyTrigger(DIK_L) || pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_BACK))
		{
			MiniMapNum++;
			if (MiniMapNum == 3)
				MiniMapNum = 0;
		}
		switch (MiniMapNum)
		{
		case 0:
			MiniMapPos = GetPlayer()->Position;
			MiniMapPos.y += 60.0f;
			break;
		case 1:
			MiniMapPos = GetPlayer()->Position;
			MiniMapPos.y += 120.0f;
			break;
		case 2:
			MiniMapPos = GetPlayer()->Position;
			MiniMapPos.y += 300.0f;
			break;
		default:
			break;
		}

		MinimapDir.x = GetPlayer()->Position.x - MiniMapPos.x;
		MinimapDir.y = GetPlayer()->Position.y - MiniMapPos.y;
		MinimapDir.z = GetPlayer()->Position.z - MiniMapPos.z;

		minimapdir = XMVector3Normalize(XMLoadFloat3(&MinimapDir));
		MiniMapCam->SetEye(MiniMapPos);
		MiniMapCam->SetFocus(GetPlayer()->Position);
		//MiniMapCam->SetFocus({0.0f,-1.0f,0.0f});
		//MiniMapCam->SetUp(GetPlayer()->GetForward());
		MiniMapCam->SetUp({ 0.0f,0.0f,1.0f });
		MiniMapCam->Update();

		camera3D->Update(); // �ݒ肵�����W�Ńr���[�ϊ��s����X�V
		break;
	}
	case CScene::TEST_STAGE:
	{
		//�V�[���ݒ�
		if (!WaitFadeSecne)
		{
			//if (CPlayerAttack::Get()->GetEnemyNum() == 1 || !GetPlayer()->GetActive())
			//{
			//	SetFade(FADE_IN);
			//	WaitFadeSecne = true;
			//}
		}

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				if (BackStage)
				{
					//���j���[�Ɉڍs
					SetFade(FADE_OUT);
					WaitFadeSecne = false;
					BackStage = false;
					IsCheckMainMenu = false;
					IsCheckGameStage = false;
					GetPlayer()->BivStop();
					CScene::Get()->SetScene(CScene::MENU);
					//IsStop = false;
				}
				else
				{
					//���U���g��ʂɈڍs
					CameraId = Third;
					if (CPlayerAttack::Get()->GetEnemyNum() == 1)
					{
						SetFade(FADE_OUT);
						WaitFadeSecne = false;
						GetPlayer()->BivStop();
						CScene::Get()->SetScene(CScene::GAMECLEAR);
						return;
					}
					else if (!GetPlayer()->GetActive())
					{
						SetFade(FADE_OUT);
						WaitFadeSecne = false;
						GetPlayer()->BivStop();
						CScene::Get()->SetScene(CScene::GAMEOVER);
						return;
					}
				}
			}
		}

		//�ꎞ��~
		if (Input::Get()->GetKeyTrigger(DIK_ESCAPE))
		{
			SetIsGameStop();
		}
		if (IsStop)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN))
			{
				if (!WaitFadeSecne)
				{
					SetFade(FADE_IN);
					WaitFadeSecne = true;
					BackStage = true;
				}
			}
			return;
		}

		//�G�t�F�N�g�̍X�V����
		CEffectManager::Get()->Update();

		//���C���J�����̍��W��ݒ肷��
		if (GetPlayer()->GetActive())
			SetCameraPos();

		if (IsStartGame)
		{
			//�G�̍X�V����
			CPlayerAttack::Get()->EnemyUpdates();
		}

		//�V�[���̍X�V����
		CScene::Get()->Update();

		if (IsStartGame)
		{
			CPlayerAttack::Get()->Update();
			CEnemyAttack::Get()->Update();
		}


		//����������p�g��A�t�ɂ���ȊO�̎��͌��̎���p�ɖ߂�
		if (GetPlayer()->GetIsFastSpeed())
		{
			if (camera3D->GetFovRadius() >= 60.0f)
			{
				camera3D->SetFovRadius(60.0f);
			}
			else
			{
				camera3D->AddFovRadius(0.2f);
			}
		}
		else
		{
			camera3D->MoveInitFovRadius(0.1f);
		}

		//�~�j�}�b�v�ݒ�
		if (Input::Get()->GetKeyTrigger(DIK_L) || pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_BACK))
		{
			MiniMapNum++;
			if (MiniMapNum == 3)
				MiniMapNum = 0;
		}
		switch (MiniMapNum)
		{
		case 0:
			MiniMapPos = GetPlayer()->Position;
			MiniMapPos.y += 40.0f;
			break;
		case 1:
			MiniMapPos = GetPlayer()->Position;
			MiniMapPos.y += 80.0f;
			break;
		case 2:
			MiniMapPos = GetPlayer()->Position;
			MiniMapPos.y += 160.0f;
			break;
		default:
			break;
		}

		MinimapDir.x = GetPlayer()->Position.x - MiniMapPos.x;
		MinimapDir.y = GetPlayer()->Position.y - MiniMapPos.y;
		MinimapDir.z = GetPlayer()->Position.z - MiniMapPos.z;

		minimapdir = XMVector3Normalize(XMLoadFloat3(&MinimapDir));
		MiniMapCam->SetEye(MiniMapPos);
		MiniMapCam->SetFocus(GetPlayer()->Position);
		//MiniMapCam->SetFocus({0.0f,-1.0f,0.0f});
		//MiniMapCam->SetUp(GetPlayer()->GetForward());
		MiniMapCam->SetUp({ 0.0f,0.0f,1.0f });
		MiniMapCam->Update();

		camera3D->Update(); // �ݒ肵�����W�Ńr���[�ϊ��s����X�V

		break;
	}
	case CScene::GAMECLEAR:
	{
		//�J�����̐ݒ�
		//�J�����ʒu��ݒ�
		camera3D->SetEye({ 0.0f,0.0f,0.0f });
		//�J�����̌����Ă�������ݒ�
		camera3D->SetFocus({ 0.0f,0.0f,10.0f });
		//������x�N�g���ݒ�
		camera3D->SetUp({ 0.0f,1.0f,0.0f });
		//�X�V����
		camera3D->Update(); // �ݒ肵�����W�Ńr���[�ϊ��s����X�V

		//�V�[���̍X�V����
		CScene::Get()->Update();

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				CScene::Get()->SetScene(CScene::MENU);
				SetFade(FADE_OUT);
				WaitFadeSecne = false;
				IsCheckMainMenu = false;
				IsCheckGameStage = false;
			}
		}

		if (Fade != FADE_OUT)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN) || pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_A))
			{
				if (!WaitFadeSecne)
				{
					//XA_Play(SE_MOVETITLE);
					SetFade(FADE_IN);
					WaitFadeSecne = true;
				}
			}
		}
		break;
	}
	case CScene::GAMEOVER:
	{
		//�J�����̐ݒ�
		//�J�����ʒu��ݒ�
		camera3D->SetEye({ 0.0f,0.0f,0.0f });
		//�J�����̌����Ă�������ݒ�
		camera3D->SetFocus({ 0.0f,0.0f,10.0f });
		//������x�N�g���ݒ�
		camera3D->SetUp({ 0.0f,1.0f,0.0f });
		//�X�V����
		camera3D->Update(); // �ݒ肵�����W�Ńr���[�ϊ��s����X�V

		//�V�[���̍X�V����
		CScene::Get()->Update();

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				CScene::Get()->SetScene(CScene::MENU);
				SetFade(FADE_OUT);
				WaitFadeSecne = false;
				IsCheckMainMenu = false;
				IsCheckGameStage = false;
			}
		}

		if (Fade != FADE_OUT)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN) || pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_A))
			{
				if (!WaitFadeSecne)
				{
					//XA_Play(SE_MOVETITLE);
					SetFade(FADE_IN);
					WaitFadeSecne = true;
				}
			}
		}
		break;
	}
	case CScene::RESULT:
	{
		break;
	}
	case CScene::CREATE_STAGE:
	{
		//�V�[���ݒ�

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				if (BackStage)
				{
					//���j���[�Ɉڍs
					SetFade(FADE_OUT);
					WaitFadeSecne = false;
					BackStage = false;
					IsCheckMainMenu = false;
					IsCheckGameStage = false;
					IsStop = false;
					CScene::Get()->SetScene(CScene::MENU);
					//IsStop = false;
				}
			}
		}

		//�ꎞ��~
		if (Input::Get()->GetKeyTrigger(DIK_ESCAPE))
		{
			SetIsGameStop();
		}
		if (IsStop)
		{
			if (Input::Get()->GetKeyTrigger(DIK_RETURN))
			{
				if (!WaitFadeSecne)
				{
					SetFade(FADE_IN);
					WaitFadeSecne = true;
					BackStage = true;
				}
			}
			return;
		}


		//�L�[�{�[�h�ŃJ�����ʒu�ύX
		if (Input::Get()->GetKeyPress(DIK_A))
		{
			//���ړ�
			DebugCamera->MoveRightDebugCamera(false);
		}
		else if (Input::Get()->GetKeyPress(DIK_D))
		{
			//�E�ړ�
			DebugCamera->MoveRightDebugCamera(true);
		}

		if (Input::Get()->GetKeyPress(DIK_W))
		{
			//�O�ړ�
			DebugCamera->MoveForwardDebugCamera(true);
		}
		else if (Input::Get()->GetKeyPress(DIK_S))
		{
			//��ړ�
			DebugCamera->MoveForwardDebugCamera(false);
		}

		//�L�[�{�[�h�ŃJ�����ʒu�ύX
		if (Input::Get()->GetKeyPress(DIK_Z))
		{
			//��ړ�
			DebugCamera->MoveUpDebugCamera(true);
		}
		else if (Input::Get()->GetKeyPress(DIK_C))
		{
			//���ړ�
			DebugCamera->MoveUpDebugCamera(false);
		}

		if (Input::Get()->GetKeyPress(DIK_Q))
		{
			DebugCamera->RotateDebugCamera(false);
		}
		else if (Input::Get()->GetKeyPress(DIK_E))
		{
			DebugCamera->RotateDebugCamera(true);
		}


		DebugCamera->Update(); // �ݒ肵�����W�Ńr���[�ϊ��s����X�V
		break;
	}
	default:
		break;
	}

}

/**
 *�Q�[���̕`�揈��������֐��ł��B
 */
void CGame::Draw()
{
	D3D::Get()->SetViewBord(0);
	ID3D11DeviceContext* d3dContext = D3D::Get()->GetContext();
	D3D::Get()->SetViewBord(1);
	ID3D11DeviceContext* d3dContext2 = D3D::Get()->GetContext();

	D3D::Get()->ClearScreen();

	switch (CScene::Get()->GetNowScene())
	{
	case CScene::TITLE:
	{
		//�eGameObject�̕`��

		SetCamera(camera3D);

		CScene::Get()->Draw3DCamera();

		CEffectManager::Get()->Draw();

		SetCamera(camera2D);

		CScene::Get()->Draw2DCamera();

		pFadePanel->Draw();

		break;
	}
	case CScene::MENU:
	{
		//3d�J�����ɐݒ�
		SetCamera(camera3D);

		SetCamera(camera2D);

		CScene::Get()->Draw2DCamera();

		pFadePanel->Draw();

		break;
	}
	case CScene::STAGE:
	{
		SetCamera(camera3D);

		//CPlayerAttack::Get()->EnemysDraw();
		CScene::Get()->Draw3DCamera();

		CPlayerAttack::Get()->Draw();
		CEnemyAttack::Get()->Draw();

		CEffectManager::Get()->Draw();

		SetCamera(camera2D);
		if (IsStartGame)
		{
			if (GetPlayer()->GetActive())
			{
				//����
				for (int i = 0; i < 10; i++)
				{
					CScene::Get()->GetStageSpeedNums(i)->SetActive(false);
					CScene::Get()->GetStageSpeedNums(i)->Position.x = -1.0f;
					CScene::Get()->GetStageSpeedNums(i)->Position.y = 0.2f;

					CScene::Get()->GetStageAltNums(i)->SetActive(false);
					CScene::Get()->GetStageAltNums(i)->Position.x = 1.0f;
					CScene::Get()->GetStageAltNums(i)->Position.y = 0.2f;
				}
				//�v���C���[�̑��x�v�Z
				MoveSpeedNum = int(CScene::Get()->GetPlayer()->GetMoveSpeed() * 1000.0f);
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum / 100)->SetActive(true);
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum / 100)->Draw();
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum / 100)->Position.x = -1.0f;

				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Position.x = CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Position.x + 0.05f;
				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->SetActive(true);
				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Draw();
				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Position.x = -1.0f;

				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Position.x = CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Position.x + 0.05f * 2.0f;
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->SetActive(true);
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Draw();
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Position.x = -1.0f;

				//�v���C���[�̍��x�v�Z
				AltitudeNum = (int)(CScene::Get()->GetPlayer()->Position.y - CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->Position.y);
				if (AltitudeNum <= 0)
					AltitudeNum = 0;
				CScene::Get()->GetStageAltNums((AltitudeNum / 100))->SetActive(true);
				CScene::Get()->GetStageAltNums((AltitudeNum / 100))->Draw();
				CScene::Get()->GetStageAltNums((AltitudeNum / 100))->Position.x = 1.0f;

				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Position.x = CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Position.x + 0.05f;
				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->SetActive(true);
				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Draw();
				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Position.x = 1.0f;

				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->Position.x = CScene::Get()->GetStageAltNums(AltitudeNum % 10)->Position.x + 0.05f * 2.0f;
				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->SetActive(true);
				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->Draw();
				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->Position.x = 1.0f;
			}
			CScene::Get()->Draw2DCamera();
		}
		else
		{
			RuleUI->Draw();
		}

		CEffectManager::Get()->Draw2D();

		if (IsStop)
			pWaitPanel->Draw();

		pFadePanel->Draw();
		break;
	}
	case CScene::TEST_STAGE:
	{
		SetCamera(camera3D);

		//CPlayerAttack::Get()->EnemysDraw();
		CScene::Get()->Draw3DCamera();

		CPlayerAttack::Get()->Draw();
		CEnemyAttack::Get()->Draw();

		CEffectManager::Get()->Draw();

		SetCamera(camera2D);
		if (IsStartGame)
		{
			if (GetPlayer()->GetActive())
			{
				//����
				for (int i = 0; i < 10; i++)
				{
					CScene::Get()->GetStageSpeedNums(i)->SetActive(false);
					CScene::Get()->GetStageSpeedNums(i)->Position.x = -1.0f;
					CScene::Get()->GetStageSpeedNums(i)->Position.y = 0.2f;

					CScene::Get()->GetStageAltNums(i)->SetActive(false);
					CScene::Get()->GetStageAltNums(i)->Position.x = 1.0f;
					CScene::Get()->GetStageAltNums(i)->Position.y = 0.2f;
				}
				//�v���C���[�̑��x�v�Z
				MoveSpeedNum = int(CScene::Get()->GetPlayer()->GetMoveSpeed() * 1000.0f);
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum / 100)->SetActive(true);
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum / 100)->Draw();
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum / 100)->Position.x = -1.0f;

				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Position.x = CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Position.x + 0.05f;
				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->SetActive(true);
				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Draw();
				CScene::Get()->GetStageSpeedNums((MoveSpeedNum / 10) % 10)->Position.x = -1.0f;

				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Position.x = CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Position.x + 0.05f * 2.0f;
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->SetActive(true);
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Draw();
				CScene::Get()->GetStageSpeedNums(MoveSpeedNum % 10)->Position.x = -1.0f;

				//�v���C���[�̍��x�v�Z
				AltitudeNum = (int)(CScene::Get()->GetPlayer()->Position.y - CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->Position.y);
				if (AltitudeNum <= 0)
					AltitudeNum = 0;
				CScene::Get()->GetStageAltNums((AltitudeNum / 100))->SetActive(true);
				CScene::Get()->GetStageAltNums((AltitudeNum / 100))->Draw();
				CScene::Get()->GetStageAltNums((AltitudeNum / 100))->Position.x = 1.0f;

				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Position.x = CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Position.x + 0.05f;
				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->SetActive(true);
				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Draw();
				CScene::Get()->GetStageAltNums(((AltitudeNum / 10) % 10))->Position.x = 1.0f;

				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->Position.x = CScene::Get()->GetStageAltNums(AltitudeNum % 10)->Position.x + 0.05f * 2.0f;
				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->SetActive(true);
				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->Draw();
				CScene::Get()->GetStageAltNums((AltitudeNum % 10))->Position.x = 1.0f;
			}
			CScene::Get()->Draw2DCamera();
		}

		CEffectManager::Get()->Draw2D();

		//RuleUI->Draw();

		if (IsStop)
			pWaitPanel->Draw();

		//pWaitPanel->Draw();

		pFadePanel->Draw();



		break;
	}
	case CScene::GAMECLEAR:
	{
		//�eGameObject�̕`��

		SetCamera(camera3D);

		CScene::Get()->Draw3DCamera();

		SetCamera(camera2D);

		pFadePanel->Draw();

		break;
	}
	case CScene::GAMEOVER:
	{
		SetCamera(camera3D);

		CScene::Get()->Draw3DCamera();

		SetCamera(camera2D);
		pFadePanel->Draw();
		break;
	}
	case CScene::RESULT:
	{
		//�eGameObject�̕`��

		SetCamera(camera3D);

		SetCamera(camera2D);
		pFadePanel->Draw();

		break;
	}
	case CScene::CREATE_STAGE:
	{

		SetCamera(DebugCamera);
		CScene::Get()->Draw3DCamera();

		CPlayerAttack::Get()->EnemysDraw();


		CPlayerAttack::Get()->Draw();
		CEnemyAttack::Get()->Draw();

		CEffectManager::Get()->Draw();

		SetCamera(camera2D);

		if (IsStop)
			pWaitPanel->Draw();

		pFadePanel->Draw();

		break;
	}
	default:
		break;
	}

	//�f�o�b�O��ʕ`��t���O
	if (Input::Get()->GetKeyTrigger(DIK_P))
	{
		IsDebug = !IsDebug;
	}

	//�f�o�b�OUI�̕`��
	DebugUI::Render();

	//�Ō�Ɉ�񂾂�����
	D3D::Get()->UpdateScreen();
}

CPlayer* CGame::GetPlayer()
{
	return CScene::Get()->GetPlayer();
}

/**
 *���C���J�����̐��ʕ������擾����֐��ł��B
 *
 * @param  ��ƂȂ�v���C���[�ʒu
 * @return ���C���J�����̐��ʕ�����Ԃ��܂��B
 */
XMFLOAT3 CGame::GetMainCameraForward(CGameObject* _player)
{
	//�J�����̍��W�ݒ�
	//�Ǐ]�J�����̈ʒu���v�Z����
	XMFLOAT3 BackForward = _player->GetForward();	//������x�N�g���p
	XMFLOAT3 Forward = _player->GetForward();		//�O�����x�N�g���p
	XMFLOAT3 Up = _player->GetUp();					//������x�N�g��
	//�������t�ɂ��Č������x�N�g�����擾����
	BackForward.x *= -1.0f;
	BackForward.y *= -1.0f;
	BackForward.z *= -1.0f;
	//�J�������W�ɑ��
	cameraPos.x = _player->Position.x + BackForward.x * 5.0f + (Up.x * 2.0f);
	cameraPos.y = _player->Position.y + BackForward.y * 5.0f + (Up.y * 2.0f);
	cameraPos.z = _player->Position.z + BackForward.z * 5.0f + (Up.z * 2.0f);
	//�J�����̌����Ă������
	Forward.x = _player->Position.x + Forward.x * 3.0f;
	Forward.y = _player->Position.y + Forward.y * 3.0f;
	Forward.z = _player->Position.z + Forward.z * 3.0f;

	return Forward;
}

/**
 *�I�u�W�F�N�g�̃����[�X����������֐��ł��B
 */
void CGame::ReleseObject()
{
	NewCameraPos.clear();

	EndObject(pFadePanel);
	EndObject(pWaitPanel);
	EndObject(pShotPosUI);
	EndObject(pCountDownUI);
	EndObject(RuleUI);

	//delete modelManager;
	//modelManager->clear();

	delete camera3D;
	delete camera2D;
	delete MiniMapCam;
	delete DebugCamera;

}

/**
 *�I�u�W�F�N�g��Relese��Delete���s���֐��ł��B
 *
 * @param  ����������I�u�W�F�N�g
 */
void CGame::EndObject(CGameObject* _obj)
{
	if (_obj != nullptr)
	{
		SAFE_RELEASE(_obj->GetModel().texture);
		SAFE_RELEASE(_obj->GetModel().vertexBuffer);
		SAFE_RELEASE(_obj->GetModel().indexBuffer);

		delete _obj;
		_obj = nullptr;
	}
}

/**
 *�~�T�C�����q�b�g��������UI������s���֐��ł��B
 */
void CGame::MisileHitUIActive()
{
	CScene::Get()->Get2DGameObject(CGameObject::TAG::HITMISILE_UI)->SetActive(true);
	CScene::Get()->Get2DGameObject(CGameObject::TAG::HITMISILE_UI)->SetMisileHitSetTime(GetTickCount64() + 2000);
}

/**
* �v���C���[�Ƀ~�T�C���������������ɃJ������h�炷�֐��ł��B
*
* @param id
* 1:�_���[�W�U��
* 2:�X�s�[�h�A�b�v�U��
* 3:
* 4:
*
*/
void CGame::ShakeCamera(const int& _id)
{
	switch (_id)
	{
	case 1:
		IsDamegeShakeCamera = true;
		IsSpeedShakeCamera = false;
		CameraShakeStartTime = GetTickCount64(); 
		break;
	case 2:
		//�_���[�W�U���̕����D��x����
		if (!IsDamegeShakeCamera)
		{
			IsSpeedShakeCamera = true;
			CameraShakeStartTime = GetTickCount64();
		}
		break;
	case 3:
		//IsDamegeShakeCamera = true;
		//CameraShakeStartTime = GetTickCount64();
		break;
	default:
		break;
	}

}

/**
 *�J�������W�̐ݒ���s���֐��ł��B
 */
void CGame::SetCameraPos()
{
	//�Ǐ]�J�����̈ʒu���v�Z����
	XMFLOAT3 BackForward = CScene::Get()->GetPlayer()->GetForward();	//������x�N�g���p
	XMFLOAT3 Forward = CScene::Get()->GetPlayer()->GetForward();		//�O�����x�N�g���p
	XMFLOAT3 Up = CScene::Get()->GetPlayer()->GetUp();					//������x�N�g��
	XMFLOAT3 Right = CScene::Get()->GetPlayer()->GetRight();			//�E�����x�N�g��

	XMFLOAT3 PlayerPos = CScene::Get()->GetPlayer()->Position;

	//�������t�ɂ��Č������x�N�g�����擾����
	BackForward.x *= -1.0f;
	BackForward.y *= -1.0f;
	BackForward.z *= -1.0f;

	switch (CameraId)
	{
	case CGame::Third:
	{
		//�v���C���[�̐��ʕ������擾
		//�J�����̍��W�ݒ�

		//�J������h�炷����
		if (IsDamegeShakeCamera)
		{
			if (GetTickCount64() >= CameraShakeStartTime + (ULONGLONG)SHAKE_CAMERA_TIME)
			{
				IsDamegeShakeCamera = false;
				CameraShakeStartTime = 0;
			}
			else
			{
				// ����������̏�����
				std::random_device rd;  // �n�[�h�E�F�A�̗�����������g�p����
				std::mt19937 rng(rd()); // �����Z���k�E�c�C�X�^�@�Ɋ�Â��[������������

				// �����͈̔͂��w��
				std::uniform_real_distribution<float> distX(0.1f, 0.5f); // 1.0f����2.0f�͈̔͂̐����𐶐�

				// �����͈̔͂��w��
				std::uniform_real_distribution<float> distY(0.1f, 0.5f); // 1.0f����2.0f�͈̔͂̐����𐶐�

				// �����_���Ȑ������擾
				float randomValueX = distX(rng);

				// �����_���Ȑ������擾
				float randomValueY = distY(rng);

				if (IsDamegeShakeright)
				{
					//�J�������W�ɑ��
					cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
					cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
					cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
				}
				else
				{
					Right.x *= -1.0f;
					Right.y *= -1.0f;
					Right.z *= -1.0f;
					//�J�������W�ɑ��
					cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
					cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
					cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
				}

				IsDamegeShakeright = !IsDamegeShakeright;
			}
		}
		else if (IsSpeedShakeCamera)
		{
			if (GetTickCount64() >= CameraShakeStartTime + 1000)
			{
				IsSpeedShakeCamera = false;
				CameraShakeStartTime = 0;
			}

			// ����������̏�����
			std::random_device rd;  // �n�[�h�E�F�A�̗�����������g�p����
			std::mt19937 rng(rd()); // �����Z���k�E�c�C�X�^�@�Ɋ�Â��[������������

			// �����͈̔͂��w��
			std::uniform_real_distribution<float> distX(0.01f, 0.03f); // 1.0f����2.0f�͈̔͂̐����𐶐�

			// �����͈̔͂��w��
			std::uniform_real_distribution<float> distY(0.01f, 0.03f); // 1.0f����2.0f�͈̔͂̐����𐶐�

			// �����_���Ȑ������擾
			float randomValueX = distX(rng);

			// �����_���Ȑ������擾
			float randomValueY = distY(rng);

			if (IsSpeedShakeright)
			{
				//�J�������W�ɑ��
				cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
				cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
				cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
			}
			else
			{
				Right.x *= -1.0f;
				Right.y *= -1.0f;
				Right.z *= -1.0f;
				//�J�������W�ɑ��
				cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
				cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
				cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
			}

			IsSpeedShakeright = !IsSpeedShakeright;
		}
		else
		{
			//�J�������W�ɑ��
			cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * camera3D->GetOffsetCameraPos().x);
			cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.x * camera3D->GetOffsetCameraPos().x);
			cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y) + (Right.x * camera3D->GetOffsetCameraPos().x);
		}

		//�J�����̏����
		XMFLOAT3 PlayerResUp = GetPlayer()->GetUp();

		XMFLOAT3 cameraUp = GetCamera3D()->GetCameraUp();
		XMVECTOR ResCameraUpVector = XMLoadFloat3(&cameraUp);
		//XMVECTOR ResPlayerVector = GetPlayer()->GetNewForward();
		XMVECTOR ResPlayerUpVector = XMLoadFloat3(&PlayerResUp);
		float UpAlpha = camera3D->GetUpAlpha();

		DirectX::XMVECTOR cameraUpDirection = XMVector3Normalize(XMVectorLerp(ResCameraUpVector, ResPlayerUpVector, UpAlpha));

		XMStoreFloat3(&Up, cameraUpDirection);

		//Forward.x = PlayerPos.x + Forward.x * 10.0f;
		//Forward.y = PlayerPos.y + Forward.y * 10.0f;
		//Forward.z = PlayerPos.z + Forward.z * 10.0f;

		//�J�����̌����Ă������
		//���̈ړ����x���J���������x��
		XMFLOAT3 PlayerResForward;
		PlayerResForward.x = PlayerPos.x + Forward.x * 10.0f;
		PlayerResForward.y = PlayerPos.y + Forward.y * 10.0f;
		PlayerResForward.z = PlayerPos.z + Forward.z * 10.0f;

		XMFLOAT3 cameraFocus = GetCamera3D()->GetCameraFocus();
		XMVECTOR ResCameraVector = XMLoadFloat3(&cameraFocus);
		//XMVECTOR ResPlayerVector = GetPlayer()->GetNewForward();
		XMVECTOR ResPlayerVector = XMLoadFloat3(&PlayerResForward);
		float FocusAlpha = camera3D->GetFocusAlpha();

		DirectX::XMVECTOR cameraDirection = XMVectorLerp(ResCameraVector, ResPlayerVector, FocusAlpha);

		XMStoreFloat3(&Forward, cameraDirection);

		//�J�����̐ݒ�
		//�J�����ʒu��ݒ�
		camera3D->SetEye(cameraPos);
		//�J�����̌����Ă�������ݒ�
		camera3D->SetFocus(Forward);
		//������x�N�g���ݒ�
		camera3D->SetUp(Up);

		//camera2D->SetEye(cameraPos);
		//camera2D->SetFocus(Forward);
		//camera2D->SetUp(Up);
		//camera2D->Update();

		break;
	}
	case CGame::First:
		//�J�����̍��W���v���C���[�̑O�ʂɒu��
		//��ƑO�����̓v���C���[����擾

		//�������t�ɂ��Č������x�N�g�����擾����
		BackForward.x *= -1.0f;
		BackForward.y *= -1.0f;
		BackForward.z *= -1.0f;
		//�J�������W�ɑ��
		cameraPos.x = PlayerPos.x;
		cameraPos.y = PlayerPos.y;
		cameraPos.z = PlayerPos.z;
		//�J�����̌����Ă������
		Forward.x = PlayerPos.x + Forward.x * 10.0f;
		Forward.y = PlayerPos.y + Forward.y * 10.0f;
		Forward.z = PlayerPos.z + Forward.z * 10.0f;

		//�J�����̐ݒ�
		//�J�����ʒu��ݒ�
		camera3D->SetEye(cameraPos);
		//�J�����̌����Ă�������ݒ�
		camera3D->SetFocus(Forward);
		//������x�N�g���ݒ�
		camera3D->SetUp(Up);

		break;
	default:
		break;
	}
}

/**
 *�J��������l�̎��_����Ԃ��֐��ł��B
 *
 * @return �J��������l�̎��_����Ԃ��܂��B
 */
bool CGame::GetIsFirst()
{
	if (CameraId == First)
		return true;
	else if (CameraId == Third)
		return false;

	return false;
}

/**
* �G���o��������֐��ł��B
* 
* @param _tag		  �G�̃^�OID
* @param _newposition �o��������I�u�W�F�N�g�̃|�W�V����
* @param _newrotate   �o��������I�u�W�F�N�g�̉�]�p�x
* @param _newscaling  �o��������I�u�W�F�N�g�̃X�P�[��
*/
void CGame::InsertEnemy(int _tag ,DirectX::XMFLOAT3 _newposition, DirectX::XMFLOAT3 _newrotate, DirectX::XMFLOAT3 _newscaling)
{
	CEnemy* Enemy;
	Enemy = new CEnemy();

	CGameObject::TAG Tag = CGameObject::TAG(_tag);

	switch (Tag)
	{
	case CGameObject::FRIGHT_ENEMY:
	{
		//�G���f���쐬
		//model = modelManager->at(L"asset/CHARACTER/Fighter_EnemyJet.obj");
		model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
		Enemy->SetModel(model);

		//�~�j�}�b�v�ݒ�
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//�^�[�Q�b�g���ꂽ�Ƃ��̃~�j�}�b�v���f��
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);

		break;
	}
	case CGameObject::GROUND_ENEMY:
	{
		//�G���f���쐬
		model = modelManager->at(L"asset/CHARACTER/GroundEnemy1.obj");
		Enemy->SetModel(model);

		//�~�j�}�b�v�ݒ�
		model = modelManager->at(L"asset/CHARACTER/MiniMapGroundEnemy.obj");
		Enemy->SetMiniMapModel(model);
		//�^�[�Q�b�g���ꂽ�Ƃ��̃~�j�}�b�v���f��
		model = modelManager->at(L"asset/CHARACTER/MiniMapGroundEnemy.obj");
		Enemy->SetTargetMiniMapModel(model);

		break;
	}
	case CGameObject::FOLLOW_ENEMY:
	{
		////�G���f���쐬
		//model = modelManager->at(L"asset/CHARACTER/F-2.obj");
		model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
		Enemy->SetModel(model);

		//�~�j�}�b�v�ݒ�
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//�^�[�Q�b�g���ꂽ�Ƃ��̃~�j�}�b�v���f��
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);

		break;
	}
	case CGameObject::ENTOURAGE_ENEMY:
	{
			////�G���f���쐬
			//model = modelManager->at(L"asset/CHARACTER/F-2.obj");
			model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
			Enemy->SetModel(model);

			//�~�j�}�b�v�ݒ�
			model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
			Enemy->SetMiniMapModel(model);
			//�^�[�Q�b�g���ꂽ�Ƃ��̃~�j�}�b�v���f��
			model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
			Enemy->SetTargetMiniMapModel(model);
			break;
		}
	case CGameObject::BOSS_ENEMY:
	{
		////�G���f���쐬
		//model = modelManager->at(L"asset/CHARACTER/Fighter_EnemyJet.obj");
		model = modelManager->at(L"asset/CHARACTER/F-2Test2.obj");
		Enemy->SetModel(model);

		//�~�j�}�b�v�ݒ�
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//�^�[�Q�b�g���ꂽ�Ƃ��̃~�j�}�b�v���f��
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);
		ULONGLONG Time = GetTickCount64();
		Time += 3000;
		Enemy->SetTimes(Time);
		break;
	}
	case CGameObject::GUN_ENEMY:
	{
		////�G���f���쐬
		model = modelManager->at(L"asset/CHARACTER/GunEnemy.obj");
		//model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
		Enemy->SetModel(model);

		//�~�j�}�b�v�ݒ�
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//�^�[�Q�b�g���ꂽ�Ƃ��̃~�j�}�b�v���f��
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);

		Enemy->SetGunEnemyNewPos(CPlayerAttack::Get()->GetGunENemyStartPos());
		CPlayerAttack::Get()->AddGunEnemyStartPos();

		break;
	}
	default:
		break;
	}

	//���W�ݒ�
	Enemy->Position = _newposition;
	Enemy->Rotation = _newrotate;
	Enemy->Scaling = _newscaling;
	Enemy->SetInitRotateMatrix();

	//�^�O��ݒ�
	Enemy->SetTag(Tag);

	//�G�̓����蔻��̃��f��
	model = modelManager->at(L"asset/CHARACTER/Enemy.obj");
	Enemy->SetColliderModel(model);

	Enemy->Init();

	CPlayerAttack::Get()->InsertEnemys(Enemy);
}

/**
* �o�ߎ��Ԃ�ݒ肷��֐��ł��B
*/
void CGame::SetNowMoveTime()
{
	ULONGLONG ResTime = GetTickCount64();
	NowMoveTime = (int)(ResTime - StartProjTime);
}

/**
* �o�ߎ��Ԃ�Ԃ��֐��ł��B
*/
int CGame::GetNowMoveTime()
{
	return NowMoveTime;
}

/**
* ���f������ꂽ�֐����擾����
*
* @param  ���f���e�N�X�`���̖��O
* @return �e�N�X�`���̖��O�����ƂɎ擾�������f����Ԃ�
*/
D3D::Model& CGame::GetModel(const std::wstring _moedlname)
{
	// TODO: return �X�e�[�g�����g�������ɑ}�����܂�
	return modelManager->at(_moedlname);
}
