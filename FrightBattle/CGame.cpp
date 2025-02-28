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

//フェード速度
#define FadeSpeed (0.02)

//地面のy座標
#define GROUND_YPOS (-60)

//ミサイルが当たった時にカメラが揺れる時間
#define SHAKE_CAMERA_TIME (500)

CGame::~CGame()
{
	//サウンドの解放処理
	XA_Release();

	//オブジェクトをリリースする
	ReleseObject();
}

CGame* CGame::Get()
{
	static CGame instance;
	return &instance;
}

/**
*ゲームオブジェクト作成とアセット読み込みをする関数です。
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

	//サウンドの初期化(サウンドファイルの読み込みも)
	HRESULT hr;
	hr = XA_Initialize();

	//サウンドの初期化のエラーチェック
	//このif文に入ったら失敗
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "サウンド初期化失敗", "エラー", MB_ICONERROR | MB_OK);
	}

	// モデル読み込み
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

	//ミサイルヒットエフェクト
	model = D3D::CreateSquare({ 0,0 }, { 3,3 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/Effect_01.png");
	modelManager->emplace(L"asset/Effect_01.png", model);

	//ジェットエフェクト
	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/JetEffect_low.png");
	modelManager->emplace(L"asset/JetEffect_low.png", model);

	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/JetEffect_normal.png");
	modelManager->emplace(L"asset/JetEffect_normal.png", model);

	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/JetEffect_high.png");
	modelManager->emplace(L"asset/JetEffect_high.png", model);

	//弾丸ヒットエフェクト
	model = D3D::CreateSquare({ 0,0 }, { 2,2 }, { 0.0714f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/Effect_02.png");
	modelManager->emplace(L"asset/Effect_02.png", model);

	//ミサイルUI
	model = D3D::CreateSquare({ 0,0 }, { 0.15f,0.5f }, { 0.1f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/UI/Misile_UI_Test.png");
	modelManager->emplace(L"asset/UI/Misile_UI_Test.png", model);

	//開始のカウントダウン
	model = D3D::CreateSquare({ 0,0 }, { 1,1 }, { 0.25f, 1.0f });
	model.texture = D3D::LoadTexture(L"asset/UI/StartUI.png");
	modelManager->emplace(L"asset/UI/StartUI.png", model);

	// １枚ポリゴン＋テクスチャのモデル生成
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
 *ゲームオブジェクトの初期化処理をする関数です。
 */
void CGame::Initialize()
{
	//SetGameObjectInit();

	//初期時間設定
	StartProjTime = GetTickCount64();
	SetNowMoveTime();

	//モデル生成

	//ミサイルの情報をセット
	CPlayerAttack::Get()->SetMisileInf(modelManager->at(L"asset/CHARACTER/Misile.obj"), modelManager->at(L"asset/Effect_01.png"), modelManager->at(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });
	//機関銃の弾の情報をセット
	CPlayerAttack::Get()->SetBallInf(modelManager->at(L"asset/CHARACTER/Cube.obj"), modelManager->at(L"asset/Effect_02.png"), { 12,1 });

	CEnemyAttack::Get()->SetMisileInf(modelManager->at(L"asset/CHARACTER/Misile.obj"), modelManager->at(L"asset/Effect_01.png"), modelManager->at(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });

	//パーティクルのモデル設定
	model = modelManager->at(L"asset/CHARACTER/Circle.obj");
	//CParticleManager::Get()->SetParticleModel(model, CParticleManager::MODEL_NAME::MISILE_PARTICLE);
	CPlayerAttack::Get()->SetMisileParticleModel(model);
	CEnemyAttack::Get()->SetMisileParticleModel(model);

	model = modelManager->at(L"asset/UI/GunCircle.png");
	pShotPosUI->SetModel(model);
	pShotPosUI->SetBillboard(true);
	pShotPosUI->SetLighting(false);

	//フェードパネル
	model = modelManager->at(L"asset/FadePanel.png");
	pFadePanel->SetModel(model);
	pFadePanel->SetBillboard(true);
	pFadePanel->SetLighting(false);

	//中止パネル
	model = modelManager->at(L"asset/WaitPanel.png");
	//model = modelManager->at(L"asset/Misile2.png");
	//model = modelManager->at(L"asset/TestBackFire.png");
	pWaitPanel->SetModel(model);
	pWaitPanel->SetBillboard(true);
	pWaitPanel->SetLighting(false);

	//スタートカウントダウンUI
	model = modelManager->at(L"asset/UI/StartUI.png");
	pCountDownUI->SetModel(model);
	pCountDownUI->SetBillboard(true);
	pCountDownUI->SetLighting(false);
	pCountDownUI->SetAnimUVSize(4, 1);

	//ルール表示
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
	////UI設定
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

	//タグの設定

	// カメラを生成
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

	//もしステージ作成シーンなら


	//XA_Play(BGM_TITLE,CScene::Get()->GetBGMVolume(), 1.0f);
}

/**
 *ゲームの更新処理をする関数です。
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
			//フェードパネル
			pFadePanel->materialDiffuse.w += (float)FadeSpeed;
		}
		else if (pFadePanel->materialDiffuse.w >= 1)
		{
			//フェードパネル
			pFadePanel->materialDiffuse.w = 1.0f;
			SetFade(NO_FADE);
		}
		break;
	case CGame::FADE_OUT:
		if (pFadePanel->materialDiffuse.w > 0)
		{
			//フェードパネル
			pFadePanel->materialDiffuse.w -= (float)FadeSpeed;

		}
		else if (pFadePanel->materialDiffuse.w <= 0)
		{
			//フェードパネルのUI
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
		//雲を動かす
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
				// 乱数生成器の初期化
				std::random_device rd;
				std::mt19937 rng(rd());

				// 乱数の範囲を指定
				std::uniform_real_distribution<float> dist(-5.0f, 5.0f);

				Cloud->Position.x = dist(rng);
				Cloud->Position.z = 10.0f;
			}
		}

		//カメラの設定
		//カメラ位置を設定
		camera3D->SetEye(cameraPos);
		//カメラの向いている方向設定
		camera3D->SetFocus({ 0.0f,0.0f,10.0f });
		//上向きベクトル設定
		camera3D->SetUp({ 0.0f,1.0f,0.0f });
		//更新処理
		camera3D->Update(); // 設定した座標でビュー変換行列を更新

		//シーンの更新処理
		CScene::Get()->Update();

		//エフェクトの更新処理
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
				//もし兵器が最大値じゃなかったら
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


		//次のシーンに移動
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
				//不正なメニュー番号じゃなければ
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
					//メインメニュー決定
					IsCheckMainMenu = true;
					GameStage = GMENU_STAGE1;
					if (CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL) != nullptr)
						CScene::Get()->Get2DGameObject(CGameObject::TAG::CARSOL)->Position = { 0.1f,0.5f,0.0f };
					//pMenuCarsol->Position = { 0.2f,0.5f,0.0f };
				}
				else if (IsCheckMainMenu && !IsCheckGameStage)
				{
					XA_Play(SE_STARTGAME, CScene::Get()->GetSeVolume(), 1.0f);

					//ステージ決定
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

		//シーンの更新処理
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

			//カウントダウンのアニメーション終了
			if (pCountDownUI->GetAnimPlay() == false)
			{
				IsStartGame = true;
			}
		}

		//シーン設定
		if (!WaitFadeSecne)
		{
			//敵の数が指定数以下またはプレイヤーがいなくなったならフェード以降
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
					//メニューに移行
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
					//リザルト画面に移行
					CameraId = Third;
					//敵の数が1又はプレイヤーのアクティブがfalse
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

		//一時停止
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

		//視点切り替え
		if (pFadePanel->CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_MENU) ||
			Input::Get()->GetKeyTrigger(DIK_Y))
		{
			if (CameraId == First)
				CameraId = Third;
			else if (CameraId == Third)
				CameraId = First;
		}

		//エフェクトの更新処理
		CEffectManager::Get()->Update();

		if (IsStartGame)
		{
			//敵の更新処理
			CPlayerAttack::Get()->EnemyUpdates();
		}

		////メインカメラの座標を設定する
		//if (GetPlayer()->GetActive())
		//	SetCameraPos();

		//シーンの更新処理
		CScene::Get()->Update();

		//メインカメラの座標を設定する
		if (GetPlayer()->GetActive())
			SetCameraPos();

		if (IsStartGame)
		{
			CPlayerAttack::Get()->Update();
			CEnemyAttack::Get()->Update();
		}

		//加速時視野角拡大、逆にそれ以外の時は元の視野角に戻る
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

		//ミニマップ設定
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

		camera3D->Update(); // 設定した座標でビュー変換行列を更新
		break;
	}
	case CScene::TEST_STAGE:
	{
		//シーン設定
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
					//メニューに移行
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
					//リザルト画面に移行
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

		//一時停止
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

		//エフェクトの更新処理
		CEffectManager::Get()->Update();

		//メインカメラの座標を設定する
		if (GetPlayer()->GetActive())
			SetCameraPos();

		if (IsStartGame)
		{
			//敵の更新処理
			CPlayerAttack::Get()->EnemyUpdates();
		}

		//シーンの更新処理
		CScene::Get()->Update();

		if (IsStartGame)
		{
			CPlayerAttack::Get()->Update();
			CEnemyAttack::Get()->Update();
		}


		//加速時視野角拡大、逆にそれ以外の時は元の視野角に戻る
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

		//ミニマップ設定
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

		camera3D->Update(); // 設定した座標でビュー変換行列を更新

		break;
	}
	case CScene::GAMECLEAR:
	{
		//カメラの設定
		//カメラ位置を設定
		camera3D->SetEye({ 0.0f,0.0f,0.0f });
		//カメラの向いている方向設定
		camera3D->SetFocus({ 0.0f,0.0f,10.0f });
		//上向きベクトル設定
		camera3D->SetUp({ 0.0f,1.0f,0.0f });
		//更新処理
		camera3D->Update(); // 設定した座標でビュー変換行列を更新

		//シーンの更新処理
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
		//カメラの設定
		//カメラ位置を設定
		camera3D->SetEye({ 0.0f,0.0f,0.0f });
		//カメラの向いている方向設定
		camera3D->SetFocus({ 0.0f,0.0f,10.0f });
		//上向きベクトル設定
		camera3D->SetUp({ 0.0f,1.0f,0.0f });
		//更新処理
		camera3D->Update(); // 設定した座標でビュー変換行列を更新

		//シーンの更新処理
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
		//シーン設定

		if (WaitFadeSecne)
		{
			if (Fade == NO_FADE)
			{
				if (BackStage)
				{
					//メニューに移行
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

		//一時停止
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


		//キーボードでカメラ位置変更
		if (Input::Get()->GetKeyPress(DIK_A))
		{
			//左移動
			DebugCamera->MoveRightDebugCamera(false);
		}
		else if (Input::Get()->GetKeyPress(DIK_D))
		{
			//右移動
			DebugCamera->MoveRightDebugCamera(true);
		}

		if (Input::Get()->GetKeyPress(DIK_W))
		{
			//前移動
			DebugCamera->MoveForwardDebugCamera(true);
		}
		else if (Input::Get()->GetKeyPress(DIK_S))
		{
			//後移動
			DebugCamera->MoveForwardDebugCamera(false);
		}

		//キーボードでカメラ位置変更
		if (Input::Get()->GetKeyPress(DIK_Z))
		{
			//上移動
			DebugCamera->MoveUpDebugCamera(true);
		}
		else if (Input::Get()->GetKeyPress(DIK_C))
		{
			//下移動
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


		DebugCamera->Update(); // 設定した座標でビュー変換行列を更新
		break;
	}
	default:
		break;
	}

}

/**
 *ゲームの描画処理をする関数です。
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
		//各GameObjectの描画

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
		//3dカメラに設定
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
				//数字
				for (int i = 0; i < 10; i++)
				{
					CScene::Get()->GetStageSpeedNums(i)->SetActive(false);
					CScene::Get()->GetStageSpeedNums(i)->Position.x = -1.0f;
					CScene::Get()->GetStageSpeedNums(i)->Position.y = 0.2f;

					CScene::Get()->GetStageAltNums(i)->SetActive(false);
					CScene::Get()->GetStageAltNums(i)->Position.x = 1.0f;
					CScene::Get()->GetStageAltNums(i)->Position.y = 0.2f;
				}
				//プレイヤーの速度計算
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

				//プレイヤーの高度計算
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
				//数字
				for (int i = 0; i < 10; i++)
				{
					CScene::Get()->GetStageSpeedNums(i)->SetActive(false);
					CScene::Get()->GetStageSpeedNums(i)->Position.x = -1.0f;
					CScene::Get()->GetStageSpeedNums(i)->Position.y = 0.2f;

					CScene::Get()->GetStageAltNums(i)->SetActive(false);
					CScene::Get()->GetStageAltNums(i)->Position.x = 1.0f;
					CScene::Get()->GetStageAltNums(i)->Position.y = 0.2f;
				}
				//プレイヤーの速度計算
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

				//プレイヤーの高度計算
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
		//各GameObjectの描画

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
		//各GameObjectの描画

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

	//デバッグ画面描画フラグ
	if (Input::Get()->GetKeyTrigger(DIK_P))
	{
		IsDebug = !IsDebug;
	}

	//デバッグUIの描画
	DebugUI::Render();

	//最後に一回だけ処理
	D3D::Get()->UpdateScreen();
}

CPlayer* CGame::GetPlayer()
{
	return CScene::Get()->GetPlayer();
}

/**
 *メインカメラの正面方向を取得する関数です。
 *
 * @param  基準となるプレイヤー位置
 * @return メインカメラの正面方向を返します。
 */
XMFLOAT3 CGame::GetMainCameraForward(CGameObject* _player)
{
	//カメラの座標設定
	//追従カメラの位置を計算する
	XMFLOAT3 BackForward = _player->GetForward();	//後方向ベクトル用
	XMFLOAT3 Forward = _player->GetForward();		//前方向ベクトル用
	XMFLOAT3 Up = _player->GetUp();					//上方向ベクトル
	//符号を逆にして後ろ向きベクトルを取得する
	BackForward.x *= -1.0f;
	BackForward.y *= -1.0f;
	BackForward.z *= -1.0f;
	//カメラ座標に代入
	cameraPos.x = _player->Position.x + BackForward.x * 5.0f + (Up.x * 2.0f);
	cameraPos.y = _player->Position.y + BackForward.y * 5.0f + (Up.y * 2.0f);
	cameraPos.z = _player->Position.z + BackForward.z * 5.0f + (Up.z * 2.0f);
	//カメラの向いている方向
	Forward.x = _player->Position.x + Forward.x * 3.0f;
	Forward.y = _player->Position.y + Forward.y * 3.0f;
	Forward.z = _player->Position.z + Forward.z * 3.0f;

	return Forward;
}

/**
 *オブジェクトのリリース処理をする関数です。
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
 *オブジェクトのReleseとDeleteを行う関数です。
 *
 * @param  解放したいオブジェクト
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
 *ミサイルがヒットした時のUI制御を行う関数です。
 */
void CGame::MisileHitUIActive()
{
	CScene::Get()->Get2DGameObject(CGameObject::TAG::HITMISILE_UI)->SetActive(true);
	CScene::Get()->Get2DGameObject(CGameObject::TAG::HITMISILE_UI)->SetMisileHitSetTime(GetTickCount64() + 2000);
}

/**
* プレイヤーにミサイルが当たった時にカメラを揺らす関数です。
*
* @param id
* 1:ダメージ振動
* 2:スピードアップ振動
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
		//ダメージ振動の方が優先度が上
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
 *カメラ座標の設定を行う関数です。
 */
void CGame::SetCameraPos()
{
	//追従カメラの位置を計算する
	XMFLOAT3 BackForward = CScene::Get()->GetPlayer()->GetForward();	//後方向ベクトル用
	XMFLOAT3 Forward = CScene::Get()->GetPlayer()->GetForward();		//前方向ベクトル用
	XMFLOAT3 Up = CScene::Get()->GetPlayer()->GetUp();					//上方向ベクトル
	XMFLOAT3 Right = CScene::Get()->GetPlayer()->GetRight();			//右方向ベクトル

	XMFLOAT3 PlayerPos = CScene::Get()->GetPlayer()->Position;

	//符号を逆にして後ろ向きベクトルを取得する
	BackForward.x *= -1.0f;
	BackForward.y *= -1.0f;
	BackForward.z *= -1.0f;

	switch (CameraId)
	{
	case CGame::Third:
	{
		//プレイヤーの正面方向を取得
		//カメラの座標設定

		//カメラを揺らす処理
		if (IsDamegeShakeCamera)
		{
			if (GetTickCount64() >= CameraShakeStartTime + (ULONGLONG)SHAKE_CAMERA_TIME)
			{
				IsDamegeShakeCamera = false;
				CameraShakeStartTime = 0;
			}
			else
			{
				// 乱数生成器の初期化
				std::random_device rd;  // ハードウェアの乱数生成器を使用する
				std::mt19937 rng(rd()); // メルセンヌ・ツイスタ法に基づく擬似乱数生成器

				// 乱数の範囲を指定
				std::uniform_real_distribution<float> distX(0.1f, 0.5f); // 1.0fから2.0fの範囲の整数を生成

				// 乱数の範囲を指定
				std::uniform_real_distribution<float> distY(0.1f, 0.5f); // 1.0fから2.0fの範囲の整数を生成

				// ランダムな整数を取得
				float randomValueX = distX(rng);

				// ランダムな整数を取得
				float randomValueY = distY(rng);

				if (IsDamegeShakeright)
				{
					//カメラ座標に代入
					cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
					cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
					cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
				}
				else
				{
					Right.x *= -1.0f;
					Right.y *= -1.0f;
					Right.z *= -1.0f;
					//カメラ座標に代入
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

			// 乱数生成器の初期化
			std::random_device rd;  // ハードウェアの乱数生成器を使用する
			std::mt19937 rng(rd()); // メルセンヌ・ツイスタ法に基づく擬似乱数生成器

			// 乱数の範囲を指定
			std::uniform_real_distribution<float> distX(0.01f, 0.03f); // 1.0fから2.0fの範囲の整数を生成

			// 乱数の範囲を指定
			std::uniform_real_distribution<float> distY(0.01f, 0.03f); // 1.0fから2.0fの範囲の整数を生成

			// ランダムな整数を取得
			float randomValueX = distX(rng);

			// ランダムな整数を取得
			float randomValueY = distY(rng);

			if (IsSpeedShakeright)
			{
				//カメラ座標に代入
				cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
				cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
				cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
			}
			else
			{
				Right.x *= -1.0f;
				Right.y *= -1.0f;
				Right.z *= -1.0f;
				//カメラ座標に代入
				cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * randomValueX);
				cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.y * randomValueY);
				cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y);
			}

			IsSpeedShakeright = !IsSpeedShakeright;
		}
		else
		{
			//カメラ座標に代入
			cameraPos.x = PlayerPos.x + (BackForward.x * camera3D->GetOffsetCameraPos().z) + (Up.x * camera3D->GetOffsetCameraPos().y) + (Right.x * camera3D->GetOffsetCameraPos().x);
			cameraPos.y = PlayerPos.y + (BackForward.y * camera3D->GetOffsetCameraPos().z) + (Up.y * camera3D->GetOffsetCameraPos().y) + (Right.x * camera3D->GetOffsetCameraPos().x);
			cameraPos.z = PlayerPos.z + (BackForward.z * camera3D->GetOffsetCameraPos().z) + (Up.z * camera3D->GetOffsetCameraPos().y) + (Right.x * camera3D->GetOffsetCameraPos().x);
		}

		//カメラの上方向
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

		//カメラの向いている方向
		//この移動速度をカメラだけ遅く
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

		//カメラの設定
		//カメラ位置を設定
		camera3D->SetEye(cameraPos);
		//カメラの向いている方向設定
		camera3D->SetFocus(Forward);
		//上向きベクトル設定
		camera3D->SetUp(Up);

		//camera2D->SetEye(cameraPos);
		//camera2D->SetFocus(Forward);
		//camera2D->SetUp(Up);
		//camera2D->Update();

		break;
	}
	case CGame::First:
		//カメラの座標をプレイヤーの前面に置く
		//上と前方向はプレイヤーから取得

		//符号を逆にして後ろ向きベクトルを取得する
		BackForward.x *= -1.0f;
		BackForward.y *= -1.0f;
		BackForward.z *= -1.0f;
		//カメラ座標に代入
		cameraPos.x = PlayerPos.x;
		cameraPos.y = PlayerPos.y;
		cameraPos.z = PlayerPos.z;
		//カメラの向いている方向
		Forward.x = PlayerPos.x + Forward.x * 10.0f;
		Forward.y = PlayerPos.y + Forward.y * 10.0f;
		Forward.z = PlayerPos.z + Forward.z * 10.0f;

		//カメラの設定
		//カメラ位置を設定
		camera3D->SetEye(cameraPos);
		//カメラの向いている方向設定
		camera3D->SetFocus(Forward);
		//上向きベクトル設定
		camera3D->SetUp(Up);

		break;
	default:
		break;
	}
}

/**
 *カメラが一人称視点かを返す関数です。
 *
 * @return カメラが一人称視点かを返します。
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
* 敵を出現させる関数です。
* 
* @param _tag		  敵のタグID
* @param _newposition 出現させるオブジェクトのポジション
* @param _newrotate   出現させるオブジェクトの回転角度
* @param _newscaling  出現させるオブジェクトのスケール
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
		//敵モデル作成
		//model = modelManager->at(L"asset/CHARACTER/Fighter_EnemyJet.obj");
		model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
		Enemy->SetModel(model);

		//ミニマップ設定
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//ターゲットされたときのミニマップモデル
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);

		break;
	}
	case CGameObject::GROUND_ENEMY:
	{
		//敵モデル作成
		model = modelManager->at(L"asset/CHARACTER/GroundEnemy1.obj");
		Enemy->SetModel(model);

		//ミニマップ設定
		model = modelManager->at(L"asset/CHARACTER/MiniMapGroundEnemy.obj");
		Enemy->SetMiniMapModel(model);
		//ターゲットされたときのミニマップモデル
		model = modelManager->at(L"asset/CHARACTER/MiniMapGroundEnemy.obj");
		Enemy->SetTargetMiniMapModel(model);

		break;
	}
	case CGameObject::FOLLOW_ENEMY:
	{
		////敵モデル作成
		//model = modelManager->at(L"asset/CHARACTER/F-2.obj");
		model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
		Enemy->SetModel(model);

		//ミニマップ設定
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//ターゲットされたときのミニマップモデル
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);

		break;
	}
	case CGameObject::ENTOURAGE_ENEMY:
	{
			////敵モデル作成
			//model = modelManager->at(L"asset/CHARACTER/F-2.obj");
			model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
			Enemy->SetModel(model);

			//ミニマップ設定
			model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
			Enemy->SetMiniMapModel(model);
			//ターゲットされたときのミニマップモデル
			model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
			Enemy->SetTargetMiniMapModel(model);
			break;
		}
	case CGameObject::BOSS_ENEMY:
	{
		////敵モデル作成
		//model = modelManager->at(L"asset/CHARACTER/Fighter_EnemyJet.obj");
		model = modelManager->at(L"asset/CHARACTER/F-2Test2.obj");
		Enemy->SetModel(model);

		//ミニマップ設定
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//ターゲットされたときのミニマップモデル
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);
		ULONGLONG Time = GetTickCount64();
		Time += 3000;
		Enemy->SetTimes(Time);
		break;
	}
	case CGameObject::GUN_ENEMY:
	{
		////敵モデル作成
		model = modelManager->at(L"asset/CHARACTER/GunEnemy.obj");
		//model = modelManager->at(L"asset/CHARACTER/SU_27.obj");
		Enemy->SetModel(model);

		//ミニマップ設定
		model = modelManager->at(L"asset/CHARACTER/EnemyMiniMap.obj");
		Enemy->SetMiniMapModel(model);
		//ターゲットされたときのミニマップモデル
		model = modelManager->at(L"asset/CHARACTER/TargetEnemyMiniMap.obj");
		Enemy->SetTargetMiniMapModel(model);

		Enemy->SetGunEnemyNewPos(CPlayerAttack::Get()->GetGunENemyStartPos());
		CPlayerAttack::Get()->AddGunEnemyStartPos();

		break;
	}
	default:
		break;
	}

	//座標設定
	Enemy->Position = _newposition;
	Enemy->Rotation = _newrotate;
	Enemy->Scaling = _newscaling;
	Enemy->SetInitRotateMatrix();

	//タグを設定
	Enemy->SetTag(Tag);

	//敵の当たり判定のモデル
	model = modelManager->at(L"asset/CHARACTER/Enemy.obj");
	Enemy->SetColliderModel(model);

	Enemy->Init();

	CPlayerAttack::Get()->InsertEnemys(Enemy);
}

/**
* 経過時間を設定する関数です。
*/
void CGame::SetNowMoveTime()
{
	ULONGLONG ResTime = GetTickCount64();
	NowMoveTime = (int)(ResTime - StartProjTime);
}

/**
* 経過時間を返す関数です。
*/
int CGame::GetNowMoveTime()
{
	return NowMoveTime;
}

/**
* モデルを入れた関数を取得する
*
* @param  モデルテクスチャの名前
* @return テクスチャの名前をもとに取得したモデルを返す
*/
D3D::Model& CGame::GetModel(const std::wstring _moedlname)
{
	// TODO: return ステートメントをここに挿入します
	return modelManager->at(_moedlname);
}
