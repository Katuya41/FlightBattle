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

//メモ
//プレイヤージェットエフェクト
//LOW	:始点.1.0,1.0,2.0 終点.2.0,2.0,4.0
//NORMAl:始点.1.5,1.5,2.0 終点.2.5,2.5,7.0
//HIGH	:始点.2.5,2.5,2.0 終点.4.0,4.0,7.0

using namespace DirectX;

using namespace std;

void CPlayer::InsertParameter()
{
	//ファイルからパラメータを読み込む
	//一行目は説明なので無視
	//入れる用の変数を設定しておく(構造体で)

	// 読み込むファイルの名前

	std::ifstream file("Parameter/Player.txt");

	//ファイルオープン
	if (!file.is_open()) {
		// 読み込みに失敗した場合
		MessageBoxA(NULL, "Miss Open File", "エラー",
			MB_OK | MB_ICONERROR);
	}

	bool IsInsert = false;

	//ファイルが終わるまで読み込み
	//ファイルの末尾かどうか判定
	while (!file.eof())
	{
		string bufferstream;

		//ファイルの対象の行の中身をbufferstreamに入れる
		file >> bufferstream;
		//getline関数を使うために変換
		istringstream stream(bufferstream);

		string line;

		IsInsert = false;

		//中身をガンマで区切っている
		while (getline(stream, line, ','))
		{
			// 文字列長を計算
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);

			// ワイド文字列用のバッファを確保
			wstring wstr(bufferSize, L'\0');  // ヌル文字で初期化

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
	//パーティクルを並び替える(カメラから位置が遠いものを先に描画)
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

	// 出現位置を決める乱数
	std::random_device rd;
	std::mt19937 rng(rd());

	// 乱数の範囲を指定
	std::uniform_real_distribution<float> dist(0.0f, 0.4f);

	//DirectX::XMFLOAT2 AddPos = { dist(rng) - 0.1f ,dist(rng) - 0.1f };
	DirectX::XMFLOAT2 AddPos = { dist(rng) - 0.2f ,0.1f };

	//パーティクル追加
	PARTICLE Particle;
	Particle.Object = new CGameObject();

	// 行列生成
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	rotationMatrix.r[0] = newRIGHT;       // X軸（右方向）
	rotationMatrix.r[1] = newUP;          // Y軸（上方向）
	rotationMatrix.r[2] = newForward;    // Z軸（後ろ方向）
	rotationMatrix.r[3] = XMVectorSet(0, 0, 0, 1); // 平行移動はなし

	//オイラー角度計算
	DirectX::XMFLOAT3 ResRotation = ExtractRotationFromMatrix(rotationMatrix);

	// パーティクルの回転に適用
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

	//座標や大きさ設定
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

	//パラメータ取得
	InsertParameter();

	OldInsertParticleTime = GetTickCount64();

	// デバッグ表示関数登録
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

	//ステージシーンか
	if (CScene::Get()->GetNowScene() != CScene::SCENE::TITLE)
	{
		//プレイヤーのダメージ状態を表示
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

		//ステージで開始したか
		if (CGame::Get()->GetIsStartStage())
		{
			//現在の軸を計算
			CGameObject::SetVector();

			//コントローラー接続確認
			GetControllerState();

			IsInput = false;
			//コントローラーの入力判定
			DoControllerEvent();
			//角度に更新角度を設定
			CheckInput(Angle);

			//コントローラーの振動
			BivUpdate();

			//自動操縦だったら
			if (IsAutoPilot)
			{
				//平行回転角度に移行
				SetInitRotate();
			}

			//加速しているとき
			if (IsFastSpeed)
			{
				//一個前の状態が加速していないとき
				if (!OldFastSpeed)
				{
					//基準時間と現在時間を取得
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
					//一個前の状態が加速しているとき
					//一個前の時間と現在の時間から加速している時間を計算
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
				//加速していないとき

				//一個前の状態が加速しているとき
				if (OldFastSpeed)
				{
					//基準時間と現在時間を取得
					OldSpeedUpTime = GetTickCount64();
					SpeedUpTime = 0;
					OldFastSpeed = false;
				}
				else
				{
					//一個前の状態が加速しているとき
					//一個前の時間と現在の時間から加速している時間を計算
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

			//回転更新処理
			RotateUpdate();

			//兵器種類変更
			if (Input::Get()->GetKeyTrigger(DIK_X) ||
				CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_X))
			{
				CPlayerAttack::Get()->ChengeAttackweapon();
			}

			//攻撃開始
			if ((Input::Get()->GetKeyTrigger(DIK_SPACE) ||
				CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_B)))
			{
				CPlayerAttack::Get()->AttackTarget(this);
			}

			//ターゲット変更
			if (Input::Get()->GetKeyTrigger(DIK_C) || CheckControllerTrigger(CGameObject::BOTTUN_NAME::CONTROLLER_Y))
			{
				CPlayerAttack::Get()->SetTarget(false);
			}

			//移動処理
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

			//移動処理
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

	//当たり判定の座標角度設定//

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

	//当たり判定設定
	SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

	//地面と警告判定の当たり判定
	if (CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND) != nullptr)
	{
		DirectX::BoundingBox GroundCol = CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->GetColliderObjectAABB();
		if (CCheckCollision::Get()->CheckColBoxOBBAABB(CaveatBox, GroundCol))
		{
			IsDrawCaveat = true;
		}
	}

	//建物と警告判定の当たり判定
	for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
	{
		if (CCheckCollision::Get()->CheckColBoxOBBAABB(CaveatBox, Collider))
		{
			IsDrawCaveat = true;
		}
	}

	//地面との当たり判定
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

	////地面とプレイヤーの当たり判定
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

	//建物とプレイヤーの当たり判定
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

	////パーティクル作成
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
	//	//パーティクルを追加
	//	InsertParticle();
	//	OldInsertParticleTime = GetTickCount64();
	//}

	////パーティクル更新処理
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

	//右移動
	XMFLOAT3 Right = GetRight();
	Position.x += Right.x * RightMoveSpeed;
	Position.y += Right.y * RightMoveSpeed;
	Position.z += Right.z * RightMoveSpeed;


	CGameObject::MoveUpdate();
}

void CPlayer::RotateUpdate()
{
	////入力がない場合
	//if (!IsInput)
	//{
	//	//入力がない場合、機体の向いている正面角度によって、角度を変更する
	//	//1:地面側を向いている場合、速度が速くなる
	//	//2:Z軸が傾いているとき、それに対応して徐々にY軸回転を行う
	//	//3:機体の上下が逆(機体の上方向が地面)の場合、機体の正面は急速に地面の方を向く
	//	SetNotInputRotate();
	//}

	//角度を徐々に0に近づける
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

	//任意軸で回転させる
	//クォータニオンでの回転
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

	//現在の軸を計算
	CGameObject::SetVector();

}

void CPlayer::CheckInput(XMFLOAT3& _angle)
{
	bool ChengeSpeed = false;
	IsFastSpeed = false;
	IsAutoPilot = false;
	IsVeryRotate = false;

	//UPとDOWAの同時押しの場合、急速な機体の回転ができる
	//しかし、上昇には速度が必要になり、下降では速度が早くなる
	//速度がない時に上昇しようとしたとき、機体の向きは急速に地面を向くようになる
	//機体の速度設定
	if (Input::Get()->GetKeyPress(DIK_UP) && Input::Get()->GetKeyPress(DIK_DOWN))
	{
		IsVeryRotate = true;
	}
	else if (Input::Get()->GetKeyPress(DIK_UP))
	{
		//速度の増加
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
		//速度の低下
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

	//コントローラーが接続されている
	if (DwResult == ERROR_SUCCESS)
	{
		//前進
		if (State.Gamepad.bRightTrigger && State.Gamepad.bLeftTrigger)
		{
			//急旋回モード
			IsVeryRotate = true;

			//速度の低下
			ChengeSpeed = true;
			SetAddMoveSpeed(-(float)Parameter.Move_Add_Speed);
			if (MoveSpeed < 0.0f)
			{
				SetMoveSpeed(0.0f);
			}
		}
		else if (State.Gamepad.bRightTrigger)
		{
			//速度の増加
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
			//速度の低下
			ChengeSpeed = true;
			SetAddMoveSpeed(-(float)Parameter.Move_Add_Speed);
			if (MoveSpeed < 0.0f)
			{
				SetMoveSpeed(0.0f);
			}
		}

		//RBとLBの同時押し
		if (iPad_leftshoulder && iPad_rightshoulder)
		{
			IsAutoPilot = true;
			return;
		}

		//ヨー回転
		if (iPad_leftshoulder)
		{
			RightMoveSpeed -= Parameter.Right_Move_Speed;
			if (RightMoveSpeed <= -Parameter.Max_Right_Move_Speed)
				RightMoveSpeed = -Parameter.Max_Right_Move_Speed;

			_angle.y = -(float)Parameter.Yow_Rotate_Speed;
			IsInput = true;
		}
		else if (iPad_rightshoulder)	//LBボタン(右移動)
		{
			RightMoveSpeed += Parameter.Right_Move_Speed;
			if (RightMoveSpeed >= Parameter.Max_Right_Move_Speed)
				RightMoveSpeed = Parameter.Max_Right_Move_Speed;

			_angle.y = (float)Parameter.Yow_Rotate_Speed;
			IsInput = true;
		}

		//右スティック
		//カメラを回せる
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

		//左スティックの上下又は左右が反対の値より大きかったら、
		//逆のスティックの回転スピードを減少させる
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

		//左左右スティック
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

		//左上下スティック
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

		//左十字キーの左右
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

		//左十字キーの上下
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

	//プレイヤー操作
	if (Input::Get()->GetKeyPress(DIK_A))
	{
		//急旋回モードの時
		if (IsVeryRotate)
		{
			//プレイヤーの上ベクトルが空側を向いているなら速度が下がっていく
			//その分、回転速度が早くなる(具体的には通常の2倍ぐらい
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
			//プレイヤーの下ベクトルが空側を向いているなら速度が下がっていく
			//その分、回転速度が早くなる(具体的には通常の2倍ぐらい
			//逆に下ベクトルが地面側なら、速度が徐々に上がっていく
			//回転速度も2倍くらいで
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

	//スピード操作がない場合、スピードが一定になる処理
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

		//定期的に警告SEを鳴らす
		if (GetTickCount64() >= EnemyRockTime + 1000)
		{
			EnemyRockTime = GetTickCount64();
			XA_Play(SE_ENEMYROCK, CScene::Get()->GetSeVolume(),1.0f);
		}
	}

	//ターゲットされている
	if (IsDrawRockUI)
	{
		//ロックオンされているか
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
	//入力がない場合、ロールを0に変更する

	DirectX::XMFLOAT3 fTargetDir = { 0.0f,1.0f,0.0f };
	DirectX::XMVECTOR vTargetDir = XMVector3Normalize(XMLoadFloat3(&fTargetDir));

	float angle = 0.0f;

	//回転させる角度を計算
	float dotProduct = XMVectorGetX(XMVector3Dot(newUP, vTargetDir));
	angle = acosf(dotProduct);

	//角度を度数で計算
	float an = XMConvertToDegrees(angle);
	//angle = XMConvertToRadians(angle);

	if (an > 0.0f)
	{
		//回転させる軸を計算
		XMVECTOR axis = XMVector3Cross(newUP, vTargetDir);

		if (an > Parameter.MaxInitRotate)
		{
			angle = XMConvertToRadians(Parameter.MaxInitRotate);
		}

		XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

		//回転行列作成
		XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

		//現在の回転行列にかけ合わせる
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

	//振動するか
	if (GetTickCount64() < BivTime || !isActive)
	{
		//振動中
		//ゲームパッドのAまたはBを押した時の処理：ゲームパッドの振動
		vibration.wLeftMotorSpeed = 65535;
		XInputSetState(0, &vibration);
	}
	else  
	{
		//振動ストップ
		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;
		XInputSetState(0, &vibration);
	}
}

void CPlayer::BivStop()
{
	//振動ストップ
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
	//機体のZ軸回転と地面との並行性を調べる
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

	//ターゲット対象外のものを削除
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

	//配列に格納
	plMoveTargetUI.push_back(MovetargetUI);
}

void CPlayer::TargetLineUpdate()
{
	//最初に出現位置を設定
	//その後、出現位置からターゲット位置までの方向ベクトルを取得
	//方向ベクトルにそって移動させる
	for (auto It = plMoveTargetUI.begin(); It != plMoveTargetUI.end(); It++)
	{
		//if (!It->pTargetObject->GetIsCameraRef())
		//{
		//	It->IsDraw = false;
		//	It->Position = { 0.0f,0.0f,0.0f };
		//	plMoveTargetUI.erase(It);
		//	break;
		//}

		//複数ターゲットをする場合
		if (!CPlayerAttack::Get()->GetIsOnetarget())
		{
			//ターゲットしている敵がターゲット状態か
			if (!(It)->pTargetObject->GetIsTarget())
			{
				(It)->pTargetObject->SetCanAttack(false);
				(It)->pTargetObject->SetIsTarget(false);
				It->IsDraw = false;
				It->Position = { 0.0f,0.0f,0.0f };
				//ターゲット状態ではない
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

		//カメラに映っていなかったら描画しない
		if (!It->pTargetObject->GetIsCameraRef())
		{
			It->IsDraw = false;
		}

		//ターゲットがカメラに映っている状態および距離が150以内のときに移動
		if ((It)->pTargetObject->GetIsCameraRef() && (It)->pTargetObject->GetPlayerDistance() <= 150)
		{
			(It)->IsDraw = true;

			//移動速度
			const float constantSpeed = 60.0f;

			// 時間経過を考慮する（例：deltaTime を渡されると仮定）
			float deltaTime = 0.016f; // 1フレームの時間（例：60FPSの場合）

			// ターゲットの現在位置と速度を取得
			XMVECTOR vPosition = XMLoadFloat3(&(It)->Position);
			XMVECTOR vTargetPosition = XMLoadFloat3(&(It)->pTargetObject->Position);

			// ターゲットまでの方向ベクトルを計算
			XMVECTOR direction = XMVectorSubtract(vTargetPosition, vPosition);
			direction = XMVector3Normalize(direction);

			// ターゲットまでの残り距離を計算
			XMVECTOR totalDistance = XMVectorSubtract(vTargetPosition, vPosition);
			float distance = XMVectorGetX(XMVector3Length(totalDistance));

			// ターゲットが近づきすぎた場合の補正
			if (distance <= constantSpeed * deltaTime) {
				// ターゲットに直接到達
				XMStoreFloat3(&(It)->Position, vTargetPosition);
				(It)->IsDraw = false;
				(It)->pTargetObject->SetCanAttack(true);
			}
			else {
				// 一定速度で移動
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
					// ワールド座標をベクトル形式に変換
					XMVECTOR worldPosition = XMLoadFloat3(&TargetUI.Position);

					// ビュー行列とプロジェクション行列を掛け合わせる
					XMMATRIX viewProj = XMMatrixMultiply(CGame::Get()->GetCamera3D()->GetViewMatrix(), CGame::Get()->GetCamera3D()->GetProjectionMatrix());

					// ワールド座標をクリップ空間に変換
					XMVECTOR clipSpacePos = XMVector3TransformCoord(worldPosition, viewProj);

					// クリップ空間座標を正規化デバイス座標に変換 (-1 ～ 1 の範囲)
					float clipX = XMVectorGetX(clipSpacePos);
					float clipY = XMVectorGetY(clipSpacePos);
					float clipZ = XMVectorGetZ(clipSpacePos); // 距離の取得

					// 正規化デバイス座標をスクリーン座標に変換
					XMFLOAT2 screenPos;
					screenPos.x = (clipX + 1.0f) * 0.5f * 1264.0f;
					screenPos.y = (1.0f - clipY) * 0.5f * 681.0f; // Y軸は逆方向

					// UIを描画 (例: ImGui を使用)
					float rectSize = 50.0f; // 枠のサイズ
					rectSize = rectSize * clipZ * 0.5f; // スケーリング後の枠サイズ
					DirectX::XMFLOAT2 EnemyFrameLeft = { screenPos.x - rectSize * 0.5f,screenPos.y - rectSize * 0.5f };
					DirectX::XMFLOAT2 EnemyFrameRight = { screenPos.x + rectSize * 0.5f, screenPos.y + rectSize * 0.5f };

					ImVec2 Vertex[4];
					Vertex[0] = ImVec2(screenPos.x, screenPos.y - rectSize / 2.0f); // 上頂点
					Vertex[1] = ImVec2(screenPos.x + rectSize / 2.0f, screenPos.y); // 右頂点
					Vertex[2] = ImVec2(screenPos.x, screenPos.y + rectSize / 2.0f); // 下頂点
					Vertex[3] = ImVec2(screenPos.x - rectSize / 2.0f, screenPos.y); // 左頂点
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
	//パラメータのファイルを開く
	std::ofstream outFile("Parameter/Player.txt", std::ios::trunc);

	//ファイルオープン
	if (!outFile.is_open()) {
		// 読み込みに失敗した場合
		MessageBoxA(NULL, "Miss Open File", "エラー",
			MB_OK | MB_ICONERROR);
	}

	// 書き込むテキスト//
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

	// ファイルにテキストを書き込む
	outFile << ParameterText;

	outFile.close();
}

