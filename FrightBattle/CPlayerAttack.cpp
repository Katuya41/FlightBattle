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

//玉が消えるまでの時間
#define DeleteBallTime (1000)

//玉のクールタイム
#define ShottoBallCoolTime (100)

//玉のスピード
#define GUN_SPEED (0.6f)

//ターゲット可能距離
#define MAX_TARGET_DISTANCE (150)

//プレイヤーの各武装の発射可能距離
#define MAX_PLAYER_MISILETARGET_DISTANCE (150)
#define MAX_PLAYER_FOURMISILE_DISTANCE (150)
#define MAX_PLAYER_BULLETTARGET_DISTANCE (30)

//ターゲット完了時間
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
	//ファイルからパラメータを読み込む
	//一行目は説明なので無視
	//入れる用の変数を設定しておく(構造体で)

	// 読み込むファイルの名前

	std::ifstream file("Parameter/Enemy.txt");

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
	//ファイルからパラメータを読み込む
//一行目は説明なので無視
//入れる用の変数を設定しておく(構造体で)

// 読み込むファイルの名前

	std::ifstream file("Parameter/Misile.txt");

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
		// デバッグ表示関数登録
		EnemyFrame = DebugUI::RedistDebugFunction([&]() {
			DrawTargetLine();
			});
		CreateEnemyFrame = true;
	}

	//敵とミサイルのパラメータをファイルから取得する
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
				delete Enemy->Enemy;  // RESENEMY の内部メモリを解放
				Enemy->Enemy = nullptr;
			}
			delete Enemy;  // RESENEMY 自体を解放
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
				// ワールド座標をベクトル形式に変換
				XMVECTOR worldPosition = XMLoadFloat3(&(*it)->Enemy->Position);

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
				float rectSize = 30.0f; // 枠のサイズ
				rectSize = rectSize * (1.0f / clipZ); // スケーリング後の枠サイズ
				rectSize = std::clamp(rectSize, 30.0f, 100.0f); // 最小10, 最大100の制約
				DirectX::XMFLOAT2 EnemyFrameLeft = { screenPos.x - rectSize * 0.5f,screenPos.y - rectSize * 0.5f };
				DirectX::XMFLOAT2 EnemyFrameRight = { screenPos.x + rectSize * 0.5f, screenPos.y + rectSize * 0.5f };

				if ((*it)->Enemy->GetIsTarget())
				{
					if ((*it)->Enemy->GetCanAttack())
					{
						ImVec2 Vertex[4];
						Vertex[0] = ImVec2(screenPos.x, screenPos.y - rectSize / 2.0f); // 上頂点
						Vertex[1] = ImVec2(screenPos.x + rectSize / 2.0f, screenPos.y); // 右頂点
						Vertex[2] = ImVec2(screenPos.x, screenPos.y + rectSize / 2.0f); // 下頂点
						Vertex[3] = ImVec2(screenPos.x - rectSize / 2.0f, screenPos.y); // 左頂点
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
	//複数の敵をターゲットする場合、常に優先度チェックをする
	if (!IsOneTarget)
	{
		SetTarget(false);
	}

	//ターゲット更新処理
	TargetUpdate();

	//ミサイル更新処理
	MisileUpdate();
}

void CPlayerAttack::Draw()
{
	MisileDraw();

	MisileParticleDraw();
}

void CPlayerAttack::MisileUpdate()
{
	//ミサイルの更新処理
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
						delete (*it)->Enemy;  // RESENEMY の内部メモリを解放
						(*it)->Enemy = nullptr;
					}
					delete (*it);  // RESENEMY 自体を解放
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

	//機関銃敵専用更新
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
	//敵を描画
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		(*it)->Enemy->Draw();
	}

	//敵のデバッグ用当たり判定オブジェクトを描画
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
		//攻撃可能であり、攻撃範囲内か
		if ((*lTargetEnemys.begin())->GetCanAttack() && MAX_PLAYER_MISILETARGET_DISTANCE > (*lTargetEnemys.begin())->GetPlayerDistance())
			AttackMisile(_spornobj, (*lTargetEnemys.begin()), true);
		else
			AttackMisile(_spornobj, (*lTargetEnemys.begin()), false);
		break;
	case CPlayerAttack::ONETARGET_FOURMISILE:
		//攻撃可能であり、攻撃範囲内か
		if ((*lTargetEnemys.begin())->GetCanAttack() && MAX_PLAYER_MISILETARGET_DISTANCE > (*lTargetEnemys.begin())->GetPlayerDistance())
		{
			if (CScene::Get()->CheckFourMisileActive() == 4)
			{
				for (int i = 1; i < 5; i++)
					AttackFourMisile(_spornobj, (*lTargetEnemys.begin()), i, true,false);

				//ミサイルUIの状態を変更
				CScene::Get()->SetMisileUICharge(4);
			}

		}
		else
		{
			if (CScene::Get()->CheckFourMisileActive() == 4)
			{
				for (int i = 1; i < 5; i++)
					AttackFourMisile(_spornobj, (*lTargetEnemys.begin()), i, false,false);

				//ミサイルUIの状態を変更
				CScene::Get()->SetMisileUICharge(4);
			}
		}
		break;
	case CPlayerAttack::FOURTARGET_FOURMISILE:
		int Num = 0;
		for (auto It = lTargetEnemys.begin(); It != lTargetEnemys.end(); It++)
		{
			//攻撃可能であり、攻撃範囲内か
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
	//取得したミサイルUIがnullptrなら打てない
	CMisileUI* MisileUI = CScene::Get()->GetActiveMisileUITest(false);
	if (MisileUI == nullptr)
	{
		return;
	}

	//CMisileUI* MisileUI = CScene::Get()->GetActiveMisileUI();

	unique_ptr<CMisile> resunique;
	resunique = make_unique<CMisile>();
	resunique->SetModel(MisileModel);							//モデル
	resunique->SetMiniMapModel(MiniMapMisileModel);			//ミニマップのモデル
	resunique->SetParticleModel(MisileParticleModel);			//ミサイルのパーティクルのモデル
	resunique->Scaling = { 0.2f, 0.2f, 0.2f };				//大きさ設定
	resunique->SetTag(CGameObject::MISILE);					//タグの設定
	resunique->SetMisileState(CMisile::MISILE_STATE::READY);
	resunique->SetUv_Size(MisileHitUVSize);
	resunique->SetIsMoveEnemy(_moveenemy);

	resunique->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -0.5f,
					   _spornobj->Position.y + _spornobj->GetUp().y * -0.5f,
					   _spornobj->Position.z + _spornobj->GetUp().z * -0.5f });				//スポーンする座標

	resunique->SetSpornRotate(_spornobj->Rotation);			//角度設定
	resunique->SetInitRotateMatrix();							//初期行列設定
	resunique->SetVector();									//現在のvector計算
	resunique->SetMoveSpeed(_spornobj->GetMoveSpeed());		//初期速度
	resunique->SetTarget(_target);							//ターゲット情報取得
	resunique->SetShotTime(GetTickCount64());					//発車時刻の設定
	resunique->SetMisileParticleTime(GetTickCount64());
	resunique->SetIsShotPlayer();								//放ったオブジェクトがプレイヤーか
	resunique->SetHitMisileEffect(MisileHitModel);			//ミサイルの当たった時のエフェクトテクスチャ
	XA_Play(SE_MISILESHOTTO, CScene::Get()->GetSeVolume(), 1.0f);

	////ミサイルUIの非アクティブ化
	//MisileUI->materialDiffuse.w = 0.5f;
	//MisileUI->SetMisileShotTime(GetTickCount64());

	MisileUI->SetMisileUIStateCharge();

	vuMisile.push_back(move(resunique));

}

void CPlayerAttack::AttackFourMisile(CGameObject* _spornobj, CEnemy* _target, int _misilenum, bool _moveenemy, bool _checkmisileui)
{
	//ミサイルの座標用変数
	float ResNum = 0.0f;
	for (int i = 0; i < _misilenum; i++)
		ResNum += 0.5f;

	if (_checkmisileui)
	{
		//取得したミサイルUIがnullptrなら打てない
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
	res->SetModel(MisileModel);							//モデル
	res->SetMiniMapModel(MiniMapMisileModel);			//ミニマップのモデル
	res->SetParticleModel(MisileParticleModel);			//ミサイルのパーティクルのモデル
	res->Scaling = { 0.2f, 0.2f, 0.2f };				//大きさ設定
	res->SetTag(CGameObject::MISILE);					//タグの設定
	res->SetMisileState(CMisile::MISILE_STATE::READY);
	res->SetUv_Size(MisileHitUVSize);
	res->SetIsMoveEnemy(_moveenemy);

	//ミサイルの出現位置をずらす
	if (_misilenum < 3)
	{
		res->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -0.5f + (_spornobj->GetRight().x * 0.5f),
						   _spornobj->Position.y + _spornobj->GetUp().y * -0.5f + (_spornobj->GetRight().y * 0.5f),
						   _spornobj->Position.z + _spornobj->GetUp().z * -0.5f + (_spornobj->GetRight().z * 0.5f) });				//スポーンする座標
	}
	else
	{
		res->SetSpornPos({ _spornobj->Position.x + _spornobj->GetUp().x * -0.5f + (_spornobj->GetRight().x * -0.5f),
						   _spornobj->Position.y + _spornobj->GetUp().y * -0.5f + (_spornobj->GetRight().y * -0.5f),
						   _spornobj->Position.z + _spornobj->GetUp().z * -0.5f + (_spornobj->GetRight().z * -0.5f) });				//スポーンする座標
	}

	//ミサイルの初期角度を変更
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
			//クォータニオンでの回転
			Qr = XMQuaternionRotationAxis(XMLoadFloat3(&ResRight), XMConvertToRadians(AngleX));
			Qmt = XMMatrixRotationQuaternion(Qr);
		}
		else
		{
			if (_misilenum == 1)
				AngleY = 25.0f;
			else if (_misilenum == 3)
				AngleY = -25.0f;
			//クォータニオンでの回転
			Qr = XMQuaternionRotationAxis(XMLoadFloat3(&ResUp), XMConvertToRadians(AngleY));
			Qmt = XMMatrixRotationQuaternion(Qr);
		}

		ResMatrix *= Qmt;
		ResRotate = _spornobj->ExtractRotationFromMatrix(ResMatrix);
		res->SetSpornRotate(ResRotate);		//角度設定
	}
	else if(AttackId == FOURTARGET_FOURMISILE)
	{
		res->SetSpornRotate(_spornobj->Rotation);		//角度設定
	}

	res->SetInitRotateMatrix();							//初期行列設定
	res->SetVector();									//現在のvector計算
	res->SetMoveSpeed(_spornobj->GetMoveSpeed());		//初期速度
	res->SetTarget(_target);							//ターゲット情報取得
	res->SetShotTime(GetTickCount64());					//発車時刻の設定
	res->SetIsShotPlayer();								//放ったオブジェクトがプレイヤーか
	res->SetMisileParticleTime(GetTickCount64());
	res->SetHitMisileEffect(MisileHitModel);			//ミサイルの当たった時のエフェクトテクスチャ
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
		// デバッグ表示関数登録
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
	//配列作成
	//このなかに優先度の入った敵の情報を入れる
	std::vector<RESENEMY*> vResEnemys;
	//std::vector<std::unique_ptr<RESENEMY>> vResEnemys;
	InsertResEnemys(vResEnemys);

	//

	//優先度で降順にする(これで優先度が高いものを最初のほうにできる)
	std::sort(vResEnemys.begin(), vResEnemys.end(), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) { return _enemy1->FastPoint > _enemy2->FastPoint; });

	//std::sort(vResEnemys.begin(), vResEnemys.end(), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) 
	//	{ 
	//		if(_enemy1->FastPoint != _enemy2->FastPoint)
	//			return _enemy1->FastPoint > _enemy2->FastPoint; 
	//	
	//		return _enemy1->Enemy->GetToPlayerRadius() < _enemy1->Enemy->GetToPlayerRadius();
	//	});

	//最初と最後のイテレータを取得
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

	//優先度が5,4の物をカメラからのベクトルでソート
	if (ResEndIt != vResEnemys.end())
		std::sort(ResStartIt, std::next(ResEndIt), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) { return _enemy1->Enemy->GetToPlayerRadius() < _enemy2->Enemy->GetToPlayerRadius(); });
	else
		std::sort(ResStartIt, vResEnemys.end(), [](const RESENEMY* _enemy1, const RESENEMY* _enemy2) { return _enemy1->Enemy->GetToPlayerRadius() < _enemy2->Enemy->GetToPlayerRadius(); });

	//4ミサイル攻撃の時、ターゲットの優先度が4未満の時、ターゲットを外す
	if (!IsOneTarget)
	{
		//すべてのターゲット状態の敵のうち、優先度が低いものをターゲット状態から外す
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

		//すべてのターゲット状態の敵のうち、優先度が低いものをターゲット状態から外す
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

	//現在のすべての敵がターゲットされているかと攻撃可能かを確認
	//ターゲット配列の敵と敵配列の敵を比べ、
	//ターゲットされているのにターゲット配列の中身にない場合になくす
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		bool End = false;

		if ((*it)->Enemy->GetIsTarget() || (*it)->Enemy->GetCanAttack())
		{
			for (auto itTarget = lTargetEnemys.begin(); itTarget != lTargetEnemys.end(); itTarget++)
			{
				//この中でターゲット配列と同じなら抜ける
				if ((*it)->Enemy == (*itTarget))
				{
					End = true;
					break;
				}
			}

			if (End)
				break;

			//ここに来たら、ターゲット状態又は攻撃可能状態だけど、
			//ターゲット配列にないこと
			(*it)->Enemy->SetIsTarget(false);
			(*it)->Enemy->SetCanAttack(false);
			(*it)->Enemy->SetTargetMiniMapObjectModel(false);
		}
	}

	//ターゲットをリセットするか
	if (_resettime)
	{
		//ターゲットした敵のターゲット時間をリセット
		for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
		{
			//ターゲットの場合
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

	//現在のターゲット数を設定
	TargetNum = int(lTargetEnemys.size());

	//ターゲット可能数が単体か複数か
	if (IsOneTarget)
	{
		//ターゲット配列に要素が入っているか
		if (TargetNum > 0)
		{
			//すでにターゲットしていたら
			auto It = lTargetEnemys.begin();

			//優先度が高いものからターゲットされているかを調べる
			for (auto it = vResEnemys.begin(); it != vResEnemys.end(); it++)
			{
				//ターゲットされているか
				if (!(*it)->Enemy->GetIsTarget())
				{
					//旧ターゲットをターゲットから外す
					(*It)->SetTargetMiniMapObjectModel(false);
					(*It)->SetIsTarget(false);
					(*It)->SetCanAttack(false);

					//新ターゲットをターゲット配列に設定
					(*It) = (*it)->Enemy;

					//新ターゲットをターゲットとして設定
					(*It)->SetIsTarget(true);
					(*It)->SetTargetMiniMapObjectModel(true);
					TargetNum = int(lTargetEnemys.size());
					break;
				}
			}
		}
		else
		{
			//新ターゲットをターゲット配列に設定
			(*vResEnemys.begin())->Enemy->SetIsTarget(true);
			(*vResEnemys.begin())->Enemy->SetTargetMiniMapObjectModel(true);
			lTargetEnemys.push_back((*vResEnemys.begin())->Enemy);
			TargetNum = int(lTargetEnemys.size());
		}

		//ターゲットライン作成
		CGame::Get()->GetPlayer()->InsertMovetargetUI((*lTargetEnemys.begin()));
	}
	else
	{
		for (auto it = vResEnemys.begin(); it != vResEnemys.end(); it++)
		{
			//優先度が4以上のもののみ対象
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

		//もし敵がいない場合、InitEnemyをターゲットに入れる
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
	////中身をクリア
	//for (int i = 0; i < 6; i++)
	//{
	//	vEnemy3[i].clear();
	//}

	////敵を優先度ごとに配列に格納
	//for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	//{
	//	//HPが0以上
	//	if ((*it)->Enemy->GetHp() > 0 && (*it)->Enemy->GetTag() != CGameObject::TAG::INITENEMY)
	//	{
	//		//中央の視錐台に映っている
	//		if ((*it)->Enemy->GetIsCenterCameraRef() && (*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
	//		{
	//			(*it)->FastPoint = 5;
	//			(*it)->Enemy->Fastpoint = 5;
	//			vEnemy3[5].push_back((*it));
	//		}
	//		else if ((*it)->Enemy->GetIsCameraRef())
	//		{
	//			//カメラに映っている
	//			//プレイヤーとの距離がターゲット可能範囲内なら
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
	//			//プレイヤーとの距離がターゲット可能範囲内なら
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
	//中身をクリア
	_enemys.clear();

	//敵を優先度ごとに配列に格納
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		//HPが0以上
		if ((*it)->Enemy->GetHp() > 0 && (*it)->Enemy->GetTag() != CGameObject::TAG::INITENEMY)
		{
			////中央の視錐台に映っている
			//if ((*it)->Enemy->GetIsCenterCameraRef() && (*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
			//{
			//	(*it)->FastPoint = 5;
			//	(*it)->Enemy->Fastpoint = 5;
			//	_enemys.push_back((*it));
			//}

			if ((*it)->Enemy->GetIsCameraRef())
			{
				//カメラに映っている
				//プレイヤーとの距離がターゲット可能範囲内なら
				if ((*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
				{
					//プレイヤーとの距離が20以下
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
				//プレイヤーとの距離がターゲット可能範囲内なら
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
//	//中身をクリア
//	_enemys.clear();
//
//	//敵を優先度ごとに配列に格納
//	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
//	{
//		//HPが0以上
//		if ((*it)->Enemy->GetHp() > 0 && (*it)->Enemy->GetTag() != CGameObject::TAG::INITENEMY)
//		{
//			//中央の視錐台に映っている
//			if ((*it)->Enemy->GetIsCenterCameraRef() && (*it)->Enemy->GetPlayerDistance() <= MAX_TARGET_DISTANCE)
//			{
//				(*it)->FastPoint = 5;
//				(*it)->Enemy->Fastpoint = 5;
//				_enemys.push_back((*it));
//			}
//			else if ((*it)->Enemy->GetIsCameraRef())
//			{
//				//カメラに映っている
//				//プレイヤーとの距離がターゲット可能範囲内なら
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
//				//プレイヤーとの距離がターゲット可能範囲内なら
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
				delete Enemy->Enemy;  // RESENEMY の内部メモリを解放
				Enemy->Enemy = nullptr;
			}
			delete Enemy;  // RESENEMY 自体を解放
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
	//もし兵器が最大値じゃなかったら
	AttackId = static_cast<ATTACK_ID>(static_cast<int>(AttackId) + 1);

	//プレイヤー側のターゲットUIリセット
	CGame::Get()->GetPlayer()->ClearMoveTargetList();

	if (AttackId == MAX)
		AttackId = MISILE;

	//変更した後の攻撃ID
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

	//ここで、攻撃支持が行われている敵を確認する
	Num = GetEntourageAttackNum();
	if (Num < 0)
		Num = 0;

	if (Num >= 2)
		return;

	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		if (Num >= 2)
			return;

		//ターゲットされていないかつ攻撃指示が出ていない敵かつ
		//防衛追従取り巻きじゃないときに指示
		if (!(*it)->Enemy->GetIsTarget() && (*it)->Enemy->GetIsEndChaseAttack() &&
			(*it)->Enemy->GetEnemyState() != CEnemy::ENEMY_STATE::ENTOURAGE_FOLLOW
			&& (*it)->Enemy->GetEnemyState() != CEnemy::ENEMY_STATE::ENTOURAGE_DEFENCE)
		{
			//状態変更
			(*it)->Enemy->SetEnemyState(CEnemy::ENEMY_STATE::ENTOURAGE_ATTACK);
			//新しい取り巻きの指定座標設定
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
		//全ての敵のうち、取り巻きであり、追従敵のみを防衛敵に変更する
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
		//全ての敵のうち、取り巻きであり、防衛敵のみを追従敵に変更する
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
	//先頭を末尾に移動
	ResArray.push_back(ResArray.front());
	ResArray.pop_front();
}

void CPlayerAttack::CheckGunEnemysPass()
{
	//全ての敵から
	for (auto it = vEnemys.begin(); it != vEnemys.end(); it++)
	{
		//機関銃敵の内
		if ((*it)->Enemy->GetTag() == CGameObject::TAG::GUN_ENEMY)
		{
			//移動完了していない場合、
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
	//パラメータのファイルを開く
	std::ofstream outFile("Parameter/Enemy.txt", std::ios::trunc);

	//ファイルオープン
	if (!outFile.is_open()) {
		// 読み込みに失敗した場合
		MessageBoxA(NULL, "Miss Open File", "エラー",
			MB_OK | MB_ICONERROR);
	}

	// 書き込むテキスト//
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

	// ファイルにテキストを書き込む
	outFile << ParameterText;

	outFile.close();
}

void CPlayerAttack::SaveMisileParameter()
{
	//パラメータのファイルを開く
	std::ofstream outFile("Parameter/Misile.txt", std::ios::trunc);

	//ファイルオープン
	if (!outFile.is_open()) {
		// 読み込みに失敗した場合
		MessageBoxA(NULL, "Miss Open File", "エラー",
			MB_OK | MB_ICONERROR);
	}

	// 書き込むテキスト//
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

	// ファイルにテキストを書き込む
	outFile << ParameterText;

	outFile.close();
}
