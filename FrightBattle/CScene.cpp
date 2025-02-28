#include "CScene.h"
#include "CGame.h"
#include "xa2.h"
#include "Input.h"
#include "CCheckCollision.h"

#include "CGameObject.h"
#include "CVector.h"
#include "CPlayer.h"
#include "CEnemy.h"
#include "CCamera.h"
#include "CMisile.h"
#include "CMisileUI.h"
#include "CShottoBall.h"
#include "CPlayerAttack.h"
#include "CEnemyAttack.h"
#include "CEffectManager.h"

#include "direct3d.h"
#include "DebugUI.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <random>

using namespace std;

//地面のy座標
#define GROUND_YPOS (-60)

void DebugStage() {

	//ステージのデバッグ//

	if (CGame::Get()->GetIsDebug())
	{
		//プレイヤー関連のデバッグ表示//
		ImGui::Begin("Debug Stage");

		ImGui::Text("Application StartTime %d ms", CGame::Get()->GetStartTime());
		ImGui::Text("Application NowTime %d ms", (int)(GetTickCount64()));
		ImGui::Text("Application Time %d ms", CGame::Get()->GetNowMoveTime());

		ImGui::Text("Distotion", CGame::Get()->GetDist());
		ImGui::InputFloat("Distotion", &CGame::Get()->GetDist(), 0.001f, 0.01f, "%.2f");
		ImGui::SliderFloat("Distotion", &CGame::Get()->GetDist(), 0.0f, 1.0f);

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Stage Camera"))
		{
			//カメラのTransform
			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Transform")) {
				//プレイヤーオブジェクトの座標角度表示
				ImGui::Text("Position: %.3f %.3f %.3f", CGame::Get()->GetCamera3D()->GetCameraPos().x, CGame::Get()->GetCamera3D()->GetCameraPos().y, CGame::Get()->GetCamera3D()->GetCameraPos().z);
				ImGui::Text("Focus: %.3f %.3f %.3f", CGame::Get()->GetCamera3D()->GetCameraFocus().x, CGame::Get()->GetCamera3D()->GetCameraFocus().y, CGame::Get()->GetCamera3D()->GetCameraFocus().z);
				ImGui::Text("Up : %.3f %.3f %.3f", CGame::Get()->GetCamera3D()->GetCameraUp().x, CGame::Get()->GetCamera3D()->GetCameraUp().y, CGame::Get()->GetCamera3D()->GetCameraUp().z);

				ImGui::TreePop();
			}

			//オフセットのX座標
			ImGui::Text("OffSetXPos");
			ImGui::SliderFloat("OffSetXPos", &CGame::Get()->GetCamera3D()->GetOffsetCameraPos().x, -10.0f, 10.0f);

			//オフセットのY座標
			ImGui::Text("OffSetYPos");
			ImGui::SliderFloat("OffSetYPos", &CGame::Get()->GetCamera3D()->GetOffsetCameraPos().y, -10.0f, 10.0f);

			//オフセットのZ座標
			ImGui::Text("OffSetZPos");
			ImGui::SliderFloat("OffSetZPos", &CGame::Get()->GetCamera3D()->GetOffsetCameraPos().z, -10.0f, 10.0f);

			//カメラのフォーカス補完値を設定する
			ImGui::Text("FocusAlpha");
			ImGui::SliderFloat("FocusAlpha", &CGame::Get()->GetCamera3D()->GetFocusAlpha(), 0.0f, 1.0f);

			//カメラのアップ補完値を設定する
			ImGui::Text("UpAlpha");
			ImGui::SliderFloat("UpAlpha", &CGame::Get()->GetCamera3D()->GetUpAlpha(), 0.0f, 1.0f);

			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Player "))
		{
			//パラメータをセーブする
			if (ImGui::Button("Save_Player_Parameter")) {
				CScene::Get()->GetPlayer()->SaveParameter();
			}

			//パラメータをセーブする
			if (ImGui::Button("Save_Misile_Parameter")) {
				CPlayerAttack::Get()->SaveMisileParameter();
			}

			//プレイヤー関連のデバッグ表示//
			if (CScene::Get()->GetPlayer() != nullptr)
			{
				//プレイヤーのTransform
				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::TreeNode("Transform")) {

					//HP
					ImGui::Text("HP: %d", CScene::Get()->GetPlayer()->GetHp());

					//プレイヤーオブジェクトの座標角度表示
					ImGui::Text("Position: %.3f %.3f %.3f", CScene::Get()->GetPlayer()->Position.x, CScene::Get()->GetPlayer()->Position.y, CScene::Get()->GetPlayer()->Position.z);
					ImGui::Text("Rotation: %.3f %.3f %.3f", CScene::Get()->GetPlayer()->Rotation.x, CScene::Get()->GetPlayer()->Rotation.y, CScene::Get()->GetPlayer()->Rotation.z);
					ImGui::Text("Scaling : %.3f %.3f %.3f", CScene::Get()->GetPlayer()->Scaling.x, CScene::Get()->GetPlayer()->Scaling.y, CScene::Get()->GetPlayer()->Scaling.z);

					//プレイヤーオブジェクトの当たり判定の座標大きさ回転表示
					ImGui::Text("Collider Position: %.3f %.3f %.3f", CScene::Get()->GetPlayer()->GetColliderObjectOBB().Center.x, CScene::Get()->GetPlayer()->GetColliderObjectOBB().Center.y, CScene::Get()->GetPlayer()->GetColliderObjectOBB().Center.z);
					ImGui::Text("Collider Rotation: %.3f %.3f %.3f", CScene::Get()->GetPlayer()->GetColliderObjectOBB().Orientation.x, CScene::Get()->GetPlayer()->GetColliderObjectOBB().Orientation.y, CScene::Get()->GetPlayer()->GetColliderObjectOBB().Orientation.z);
					ImGui::Text("Collider Scaling : %.3f %.3f %.3f", CScene::Get()->GetPlayer()->GetColliderObjectOBB().Extents.x, CScene::Get()->GetPlayer()->GetColliderObjectOBB().Extents.y, CScene::Get()->GetPlayer()->GetColliderObjectOBB().Extents.z);
					
					//現在のプレイヤーのスピード状態
					switch (CScene::Get()->GetPlayer()->GetSpeedUpMode())
					{
					case CPlayer::SPEEDUP_MODE::LOW:	ImGui::Text("NowSpeedMode : LOW"); break;
					case CPlayer::SPEEDUP_MODE::NORMAL:	ImGui::Text("NowSpeedMode : NORMAL"); break;
					case CPlayer::SPEEDUP_MODE::HIGH:	ImGui::Text("NowSpeedMode : HIGH"); break;
					default:
						break;
					}

					ImGui::TreePop();
				}

				//CPlayerクラスのパラメーター
				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::TreeNode("CPlayer Class")) {

					//プレイヤーのパラメーター//

					//平行角度への最大回転角度
					ImGui::Text("MaxInitRotate");
					ImGui::SliderFloat("MaxInitRotate", &CScene::Get()->GetPlayer()->GetPlayerParameter().MaxInitRotate, 0.0f, 10.0f);

					//X軸回転の速度
					ImGui::Text("RotateX_Speed");
					ImGui::SliderFloat("RotateX_Speed", &CScene::Get()->GetPlayer()->GetPlayerParameter().RotateX_StickSpeed, 0.0f, 5.0f);

					//Z軸回転の速度
					ImGui::Text("RotateZ_Speed");
					ImGui::SliderFloat("RotateZ_Speed", &CScene::Get()->GetPlayer()->GetPlayerParameter().RotateZ_StickSpeed, 0.0f, 5.0f);

					//Y軸回転の速度
					ImGui::Text("Yow_Rotate_Speed");
					ImGui::SliderFloat("Yow_Rotate_Speed", &CScene::Get()->GetPlayer()->GetPlayerParameter().Yow_Rotate_Speed, 0.0f, 5.0f);

					//X軸の慣性
					ImGui::Text("Max_SlipX");
					ImGui::SliderFloat("Max_SlipX", &CScene::Get()->GetPlayer()->GetPlayerParameter().Max_SlipX, 0.0f, 2.0f);

					//Z軸の慣性
					ImGui::Text("Max_SlipZ");
					ImGui::SliderFloat("Max_SlipZ", &CScene::Get()->GetPlayer()->GetPlayerParameter().Max_SlipZ, 0.0f, 2.0f);

					//前後移動加速度
					ImGui::Text("Move_Add_Speed");
					ImGui::SliderFloat("Move_Add_Speed", &CScene::Get()->GetPlayer()->GetPlayerParameter().Move_Add_Speed, 0.0f, 0.01f);

					//左右移動加速度
					ImGui::Text("Right_Add_Speed");
					ImGui::SliderFloat("Right_Add_Speed", &CScene::Get()->GetPlayer()->GetPlayerParameter().Right_Move_Speed, 0.0f, 0.1f);

					//左右最高移動速度
					ImGui::Text("Max_Right_Move_Speed");
					ImGui::SliderFloat("Max_Right_Move_Speed", &CScene::Get()->GetPlayer()->GetPlayerParameter().Max_Right_Move_Speed, 0.0f, 0.1f);

					//最大速度
					ImGui::Text("Max_MoveSpeed");
					ImGui::SliderFloat("Max_MoveSpeed", &CScene::Get()->GetPlayer()->GetPlayerParameter().Max_MoveSpeed, 0.0f, 2.0f);

					//最低速度
					ImGui::Text("Min_MoveSpeed");
					ImGui::SliderFloat("Min_MoveSpeed", &CScene::Get()->GetPlayer()->GetPlayerParameter().Min_MoveSpeed, 0.0f, 0.5f);

					ImGui::TreePop();
				}

				//CPlayerAttackクラスのパラメーター
				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::TreeNode("CPlayerAttack Class")) {

					//パラメーター//

					//CMisileクラスのパラメーター
					ImGui::SetNextItemOpen(false, ImGuiCond_Once);
					if (ImGui::TreeNode("CMisile Class")) {

						//プレイヤーミサイルの最大回転角度
						ImGui::Text("MaxRotate");
						ImGui::SliderFloat("MaxRotate", &CPlayerAttack::Get()->GetMisileParameter().MaxRotate, 0.0f, 4.0f);

						//プレイヤーミサイルの最低回転角度
						ImGui::Text("MinRotate");
						ImGui::SliderFloat("MinRotate", &CPlayerAttack::Get()->GetMisileParameter().MinRotate, 0.0f, 2.0f);

						//敵ミサイルの最低回転角度
						ImGui::Text("Enemy_Min_Rotate");
						ImGui::SliderFloat("Enemy_Min_Rotate", &CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Rotate, 0.0f, 2.0f);

						//敵ミサイルの最大回転角度
						ImGui::Text("Enemy_Max_Rotate");
						ImGui::SliderFloat("Enemy_Max_Rotate", &CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Rotate, 0.0f, 4.0f);

						//ミサイルの低基準速度
						ImGui::Text("Min_Speed");
						ImGui::SliderFloat("Min_Speed", &CPlayerAttack::Get()->GetMisileParameter().Min_Speed, 0.0f, 1.0f);

						//ミサイルの最高速度
						ImGui::Text("Max_Speed");
						ImGui::SliderFloat("Max_Speed", &CPlayerAttack::Get()->GetMisileParameter().Max_Speed, 0.0f, 2.0f);

						//基準値以下速度時の加速度
						ImGui::Text("Min_Add_Speed");
						ImGui::SliderFloat("Min_Add_Speed", &CPlayerAttack::Get()->GetMisileParameter().Min_Add_Speed, 0.0f, 0.1f);

						//基準値以上速度時の加速度
						ImGui::Text("Max_Add_Speed");
						ImGui::SliderFloat("Max_Add_Speed", &CPlayerAttack::Get()->GetMisileParameter().Max_Add_Speed, 0.0f, 0.1f);

						//敵基準値以下速度時の加速度
						ImGui::Text("Enemy_Min_Add_Speed");
						ImGui::SliderFloat("Enemy_Min_Add_Speed", &CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Add_Speed, 0.0f, 0.05f);

						//敵基準値以上速度時の加速度
						ImGui::Text("Enemy_Max_Add_Speed");
						ImGui::SliderFloat("Enemy_Max_Add_Speed", &CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Add_Speed, 0.0f, 0.05f);

						//ミサイルの生存時間
						ImGui::Text("Delete_Misile_Time");
						ImGui::SliderInt("Delete_Misile_Time", &CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time, 0, 10000);

						//パーティクルの出現間隔
						ImGui::Text("Particle_Cooltime");
						ImGui::SliderFloat("Particle_Cooltime", &CPlayerAttack::Get()->GetMisileParameter().Particle_Cooltime, 0.0f, 30.0f);

						//パーティクルの消える速度
						ImGui::Text("Particle_OutTime");
						ImGui::SliderFloat("Particle_OutTime", &CPlayerAttack::Get()->GetMisileParameter().Particle_OutTime, 0.0f, 0.1f);

						//パーティクルの生存時間
						ImGui::Text("Particle_DeleteTime");
						ImGui::SliderFloat("Particle_DeleteTime", &CPlayerAttack::Get()->GetMisileParameter().Particle_AddScale, 0.0f, 5000.0f);

						//ミサイルのチャージ速度
						ImGui::Text("Misile_Charge_Speed");
						ImGui::SliderFloat("Misile_Charge_Speed", &CPlayerAttack::Get()->GetMisileParameter().Misile_Charge_Speed, 0.0f, 0.1f);

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				//ターゲットを配列で取得
				std::list<CEnemy*> lTarget;
				CPlayerAttack::Get()->GetTargets(lTarget);

				//ターゲットを階層構造で表示
				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::TreeNode("Targets")) {
					int Id = 1;
					for (auto It = lTarget.begin(); It != lTarget.end(); It++)
					{

						if ((*It)->GetTag() == CGameObject::TAG::INITENEMY)
						{
							ImGui::Text("Not Target");
						}
						else
						{
							// 敵番号付きのノードタイトルを作成
							std::string nodeName = "Target " + std::to_string(Id);

							ImGui::SetNextItemOpen(false, ImGuiCond_Once);
							if (ImGui::TreeNode(nodeName.c_str()))
							{
								ImGui::Text("HP: %d", (*It)->GetHp());
								ImGui::Text("Position: %f %f %f", (*It)->Position.x, (*It)->Position.y, (*It)->Position.z);
								ImGui::Text("Rotation: %f %f %f", (*It)->Rotation.x, (*It)->Rotation.y, (*It)->Rotation.z);
								ImGui::TreePop();
							}

							Id++;
						}

					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		//敵関連のデバッグ表示//

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Enemys"))
		{
			//パラメータをセーブする
			if (ImGui::Button("Save_Enemy_Parameter")) {
				CPlayerAttack::Get()->SaveEnemyParameter();
			}

			//敵を配列で取得
			std::list<CEnemy*> lEnemys;
			CPlayerAttack::Get()->GetEnemys(lEnemys);

			//敵の数
			ImGui::Text("EnemyNum : %d", int(lEnemys.size()));

			//ImGui::Text("MaxRotation : %f", CPlayerAttack::Get()->GetEnemyMaxRotation());
			//ImGui::SliderFloat("EnemysMaxRotation", &CPlayerAttack::Get()->GetEnemyMaxRotation(), 0.0f, 10.0f);

			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Fright_Enemy")) {

				ImGui::Text("Fright_MaxSpeed");
				ImGui::SliderFloat("Fright_MaxSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Fright_MaxSpeed, 0.0f, 1.0f);

				ImGui::Text("Fright_AddSpeed");
				ImGui::SliderFloat("Fright_AddSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Fright_AddSpeed, 0.0f, 0.01f);

				ImGui::Text("Fright_MaxRotate");
				ImGui::SliderFloat("Fright_MaxRotate", &CPlayerAttack::Get()->GetEnemyParameter().Fright_MaxRotate, 0.0f, 10.0f);

				ImGui::TreePop();
			}

			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Follow_Enemy")) {

				ImGui::Text("Follow_MaxSpeed");
				ImGui::SliderFloat("Follow_MaxSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxSpeed, 0.0f, 1.0f);

				ImGui::Text("Follow_AddSpeed");
				ImGui::SliderFloat("Follow_AddSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Follow_AddSpeed, 0.0f, 0.01f);

				ImGui::Text("Follow_MaxRotate");
				ImGui::SliderFloat("Follow_MaxRotate", &CPlayerAttack::Get()->GetEnemyParameter().Follow_MaxRotate, 0.0f, 10.0f);

				ImGui::TreePop();
			}

			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Entourage_Enemy")) {

				ImGui::Text("Entourage_MaxSpeed");
				ImGui::SliderFloat("Entourage_MaxSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxSpeed, 0.0f, 1.0f);

				ImGui::Text("Entourage_AddSpeed");
				ImGui::SliderFloat("Entourage_AddSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Entourage_AddSpeed, 0.0f, 0.01f);

				ImGui::Text("Entourage_MovePos_Dis");
				ImGui::SliderFloat("Entourage_MovePos_Dis", &CPlayerAttack::Get()->GetEnemyParameter().Entourage_Move_Pos_Dis, 0.0f, 100.0f);

				ImGui::Text("Entourage_MaxRotate");
				ImGui::SliderFloat("Entourage_MaxRotate", &CPlayerAttack::Get()->GetEnemyParameter().Entourage_MaxRotate, 0.0f, 10.0f);

				ImGui::TreePop();
			}

			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Boss_Enemy")) {

				ImGui::Text("Boss_MaxSpeed");
				ImGui::SliderFloat("Boss_MaxSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxSpeed, 0.0f, 1.0f);

				ImGui::Text("Boss_AddSpeed");
				ImGui::SliderFloat("Boss_AddSpeed", &CPlayerAttack::Get()->GetEnemyParameter().Boss_AddSpeed, 0.0f, 0.01f);

				ImGui::Text("Boss_MaxRotate");
				ImGui::SliderFloat("Boss_MaxRotate", &CPlayerAttack::Get()->GetEnemyParameter().Boss_MaxRotate, 0.0f, 1.0f);

				//取り巻きに攻撃支持を行うか
				
				for (auto Enemy : lEnemys)
				{
					if (Enemy->GetTag() == CGameObject::TAG::BOSS_ENEMY)
					{
						ImGui::Checkbox("BossEnemy Attack Flag", &Enemy->GetDebugIsAttack());
					}
				}


				ImGui::TreePop();
			}

			////敵の移動座標を変更
			//if (ImGui::Button("ChengeMovePosition")) {
			//	CPlayerAttack::Get()->ChengeEnemysMovePos();
			//}

			////新しく敵を追加
			//if (ImGui::Button("Add Enemy")) {
			//	CGame::Get()->InsertEnemy(0);
			//}

			//敵を階層構造で表示
			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Enemys ")) {
				int Id = 1;
				for (auto It = lEnemys.begin(); It != lEnemys.end(); It++)
				{
					if ((*It)->GetTag() != CGameObject::TAG::INITENEMY)
					{
						// 敵番号付きのノードタイトルを作成
						std::string nodeName = "Enemy " + std::to_string(Id);

						ImGui::SetNextItemOpen(false, ImGuiCond_Once);
						if (ImGui::TreeNode(nodeName.c_str()))
						{
							ImGui::SetNextItemOpen(false, ImGuiCond_Once);
							if (ImGui::TreeNode("Transform")) {
								ImGui::Text("HP: %d", (*It)->GetHp());
								ImGui::Text("Position: %f %f %f", (*It)->Position.x, (*It)->Position.y, (*It)->Position.z);
								ImGui::Text("Rotation: %f %f %f", (*It)->Rotation.x, (*It)->Rotation.y, (*It)->Rotation.z);
								ImGui::Text("MoveSpeed: %f", (*It)->GetMoveSpeed());

								ImGui::TreePop();
							}

							//敵の状態
							switch ((*It)->GetEnemyState())
							{
							case CEnemy::MOVE: ImGui::Text("MOVE"); break;
							case CEnemy::MOVE_FORWARD:ImGui::Text("MOVE_FORWARD"); break;
							case CEnemy::ISTARGET:ImGui::Text("ISTARGET"); break;
							case CEnemy::CHASE:ImGui::Text("CHASE"); break;
							case CEnemy::ATTACK:ImGui::Text("ATTACK"); break;
							case CEnemy::RUN:ImGui::Text("RUN"); break;
							case CEnemy::ENTOURAGE_DEFENCE:ImGui::Text("ENTOURAGE_DEFENCE"); break;
							case CEnemy::ENTOURAGE_FOLLOW:ImGui::Text("ENTOURAGE_FOLLOW"); break;
							case CEnemy::ENTOURAGE_ATTACK:ImGui::Text("ENTOURAGE_ATTACK"); break;
							default:
								break;
							}

							//ImGui::Text("TargetStartTime: %f", (float)(*It)->GetTargetTime());

							//優先度
							ImGui::Text("FastPoint : %d", (*It)->Fastpoint);

							//カメラに映っているか
							if ((*It)->GetIsCameraRef())
								ImGui::Text("IsCamera : true");
							else
								ImGui::Text("IsCamera : false");

							//if ((*It)->GetIsCenterCameraRef())
							//	ImGui::Text("IsCenterCamera : true");
							//else
							//	ImGui::Text("IsCenterCamera : false");

							//ターゲットされているか
							if ((*It)->GetIsTarget())
								ImGui::Text("IsTarget : true");
							else
								ImGui::Text("IsTarget : false");

							//攻撃可能か
							if ((*It)->GetCanAttack())
								ImGui::Text("GetCanAttack : true");
							else
								ImGui::Text("GetCanAttack : false");

							//カメラの視線ベクトルとそこから敵へのベクトルの角度
							ImGui::Text("ToCameraRadius : %f" , (*It)->GetToPlayerRadius());

							//プレイヤーとの距離
							ImGui::Text("PlayerDistance : %f", (*It)->GetPlayerDistance());

							//敵を削除
							if (ImGui::Button("Destroy Enemy")) {
								(*It)->SetDamege((*It)->GetHp());
							}

							////オブジェクトの表示非表示
							//if (ImGui::Button("Set Active")) {
							//	(*It)->SetActive(!(*It)->GetActive());
							//}

							ImGui::TreePop();
						}

						Id++;
					}
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}


		//建物関係のデバッグ表示//

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Objects"))
		{
			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
			if (ImGui::TreeNode("Cteate New Objects"))
			{
				// X座標、Y座標、Z座標を個別に入力
				ImGui::Text("NewObjectPosition");
				ImGui::InputFloat("PosX", &CScene::Get()->GetNewObjectPos().x, 0.1f, 1.0f, "%.2f");
				ImGui::InputFloat("PosY", &CScene::Get()->GetNewObjectPos().y, 0.1f, 1.0f, "%.2f");
				ImGui::InputFloat("PosZ", &CScene::Get()->GetNewObjectPos().z, 0.1f, 1.0f, "%.2f");

				ImGui::Text("NewObjectRotation");
				ImGui::InputFloat("RotX", &CScene::Get()->GetNewObjectRot().x, 0.1f, 1.0f, "%.2f");
				ImGui::InputFloat("RotY", &CScene::Get()->GetNewObjectRot().y, 0.1f, 1.0f, "%.2f");
				ImGui::InputFloat("RotZ", &CScene::Get()->GetNewObjectRot().z, 0.1f, 1.0f, "%.2f");

				ImGui::Text("NewObjectScalling");
				ImGui::InputFloat("ScaX", &CScene::Get()->GetNewObjectSca().x, 0.1f, 1.0f, "%.2f");
				ImGui::InputFloat("ScaY", &CScene::Get()->GetNewObjectSca().y, 0.1f, 1.0f, "%.2f");
				ImGui::InputFloat("ScaZ", &CScene::Get()->GetNewObjectSca().z, 0.1f, 1.0f, "%.2f");

				//新しくオブジェクトを追加
				if (ImGui::Button("Add Object")) {
					DirectX::XMFLOAT3 NewPos = CScene::Get()->GetNewObjectPos();
					DirectX::XMFLOAT3 NewRot = CScene::Get()->GetNewObjectRot();
					DirectX::XMFLOAT3 NewSca = CScene::Get()->GetNewObjectSca();

					CGameObject* pBills1 = new CGameObject();
					pBills1->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/Bill.obj"));
					CScene::Get()->SetGameObjectInf(L"asset/CHARACTER/Bill.obj", false, true, NewPos, NewRot, NewSca, 1.0f, CGameObject::TAG::OBJECT, pBills1, false);
				}
				ImGui::TreePop();
			}


			//オブジェクトを配列で取得
			std::list<CGameObject*> lGameObject;
			CScene::Get()->Get3DObject(lGameObject);
			int Id = 1;
			for (auto It = lGameObject.begin(); It != lGameObject.end(); It++)
			{

				if ((*It)->GetTag() == CGameObject::TAG::OBJECT)
				{
					// 敵番号付きのノードタイトルを作成
					std::string nodeName = "Object " + std::to_string(Id);

					ImGui::SetNextItemOpen(false, ImGuiCond_Once);
					if (ImGui::TreeNode(nodeName.c_str()))
					{
						// X座標、Y座標、Z座標を個別に入力
						ImGui::InputFloat("Pos_X", &(*It)->Position.x, 0.1f, 1.0f, "%.2f");
						ImGui::InputFloat("Pos_Y", &(*It)->Position.y, 0.1f, 1.0f, "%.2f");
						ImGui::InputFloat("Pos_Z", &(*It)->Position.z, 0.1f, 1.0f, "%.2f");

						// X座標、Y座標、Z座標を個別に入力
						ImGui::InputFloat("Sca_X", &(*It)->Scaling.x, 0.1f, 1.0f, "%.2f");
						ImGui::InputFloat("Sca_Y", &(*It)->Scaling.y, 0.1f, 1.0f, "%.2f");
						ImGui::InputFloat("Sca_Z", &(*It)->Scaling.z, 0.1f, 1.0f, "%.2f");

						//オブジェクトの表示非表示
						if (ImGui::Button("Set Active")) {
							(*It)->SetActive(!(*It)->GetActive());
						}

						ImGui::TreePop();
					}
					Id++;
				}
				else if ((*It)->GetTag() == CGameObject::TAG::GROUND)
				{
					// 敵番号付きのノードタイトルを作成
					std::string nodeName = "Ground";

					ImGui::SetNextItemOpen(false, ImGuiCond_Once);
					if (ImGui::TreeNode(nodeName.c_str()))
					{
						// X座標、Y座標、Z座標を個別に入力
						ImGui::InputFloat("X", &(*It)->Position.x, 0.1f, 1.0f, "%.2f");
						ImGui::InputFloat("Y", &(*It)->Position.y, 0.1f, 1.0f, "%.2f");
						ImGui::InputFloat("Z", &(*It)->Position.z, 0.1f, 1.0f, "%.2f");

						//オブジェクトの表示非表示
						if (ImGui::Button("Set Active")) {
							(*It)->SetActive(!(*It)->GetActive());
						}

						ImGui::TreePop();
					}
					Id++;
				}
			}
			ImGui::TreePop();
		}


		//攻撃関連のデバッグ表示//

		//ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		//if (ImGui::TreeNode("Misile"))
		//{
		//	//オブジェクトを配列で取得
		//	std::list<CMisile*> lMisileObject;
		//	CPlayerAttack::Get()->GetMisiles(lMisileObject);
		//	int Id = 1;
		//	if (lMisileObject.begin() == lMisileObject.end())
		//	{
		//		ImGui::Text("Not Misile");
		//	}
		//	else
		//	{
		//		for (auto It = lMisileObject.begin(); It != lMisileObject.end(); It++)
		//		{
		//			// 敵番号付きのノードタイトルを作成
		//			std::string nodeName = "Misile " + std::to_string(Id);

		//			ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		//			if (ImGui::TreeNode(nodeName.c_str()))
		//			{
		//				// X座標、Y座標、Z座標を個別に入力
		//				ImGui::InputFloat("X", &(*It)->Position.x, 0.1f, 1.0f, "%.2f");
		//				ImGui::InputFloat("Y", &(*It)->Position.y, 0.1f, 1.0f, "%.2f");
		//				ImGui::InputFloat("Z", &(*It)->Position.z, 0.1f, 1.0f, "%.2f");

		//				//オブジェクトの表示非表示
		//				if (ImGui::Button("Set Active")) {
		//					(*It)->SetActive(!(*It)->GetActive());
		//				}

		//				ImGui::TreePop();
		//			}
		//			Id++;
		//		}
		//	}
		//	ImGui::TreePop();
		//}

		ImGui::End();
	}
}

void CreateStage()
{
	//プレイヤー関連のデバッグ表示//
	ImGui::Begin("Debug Create");

	ImGui::Text("DebugCamera Position: %f %f %f", CGame::Get()->GetDebugCamera()->GetCameraPos().x, CGame::Get()->GetDebugCamera()->GetCameraPos().y, CGame::Get()->GetDebugCamera()->GetCameraPos().z);
	ImGui::Text("DebugCamera Focus: %f %f %f", CGame::Get()->GetDebugCamera()->GetCameraFocus().x, CGame::Get()->GetDebugCamera()->GetCameraFocus().y, CGame::Get()->GetDebugCamera()->GetCameraFocus().z);
	//ImGui::InputFloat("DebugCamera MoveSpeed", &CGame::Get()->GetDebugCamera()->GetDebugMoveSpeed(), 0.1f, 1.0f, "%.2f");
	ImGui::SliderFloat("DebugCamera MoveSpeed", &CGame::Get()->GetDebugCamera()->GetDebugMoveSpeed(), 0.0f, 1.0f);
	ImGui::SliderFloat("DebugCamera RotateSpeed", &CGame::Get()->GetDebugCamera()->GetDebugRotateSpeed(), 0.0f, 10.0f);

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Player "))
	{
		//プレイヤー関連のデバッグ表示//
		if (CScene::Get()->GetPlayer() != nullptr)
		{
			ImGui::Text("Position: %f %f %f", CScene::Get()->GetPlayer()->Position.x, CScene::Get()->GetPlayer()->Position.y, CScene::Get()->GetPlayer()->Position.z);

			//ターゲットを配列で取得
			std::list<CEnemy*> lTarget;
			CPlayerAttack::Get()->GetTargets(lTarget);

			//プレイヤーオブジェクトを削除
			if (ImGui::Button("Delete Player")) 
			{
				CScene::Get()->ClearLPlayer();
			}

		}
		else
		{
			// X座標、Y座標、Z座標を個別に入力
			ImGui::Text("NewPlayerPosition");
			ImGui::InputFloat("PosX", &CScene::Get()->GetNewPlayerPos().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosY", &CScene::Get()->GetNewPlayerPos().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosZ", &CScene::Get()->GetNewPlayerPos().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewPlayerRotation");
			ImGui::InputFloat("RotX", &CScene::Get()->GetNewPlayerRot().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("RotY", &CScene::Get()->GetNewPlayerRot().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("RotZ", &CScene::Get()->GetNewPlayerRot().z, 0.1f, 1.0f, "%.2f");

			//新しくプレイヤーオブジェクトを追加
			if (ImGui::Button("Add Player")) {
				//プレイヤー
				CPlayer* pPlayer = new CPlayer();
				pPlayer->SetAnimUvSize({ 10, 1 });
				pPlayer->SetRockUI(CGame::Get()->GetModel(L"asset/UI/RockPlayer.png"), CGame::Get()->GetModel(L"asset/UI/RockOnPlayer.png"));
				pPlayer->SetHitMisileEffect(CGame::Get()->GetModel(L"asset/Effect_01.png"));
				pPlayer->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/PlayerMiniMap.obj"));
				pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_Jet.obj"));
				pPlayer->SetTag(CGameObject::TAG::PLAYER);
				pPlayer->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
				//座標や大きさ設定
				pPlayer->Position = CScene::Get()->GetNewPlayerPos();
				pPlayer->Rotation = CScene::Get()->GetNewPlayerRot();
				pPlayer->Scaling = { 1.0f, 1.0f, 1.0f };
				CScene::Get()->SetDrawPlayer(true);
				CScene::Get()->InsertPlayer(pPlayer);
			}
		}
		ImGui::TreePop();
	}

	//敵関連のデバッグ表示//

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Enemys"))
	{
		//敵を配列で取得
		std::list<CEnemy*> lEnemys;
		CPlayerAttack::Get()->GetEnemys(lEnemys);

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Create New FrightEnemy"))
		{
			// X座標、Y座標、Z座標を個別に入力
			ImGui::Text("NewObjectPosition");
			ImGui::InputFloat("PosX", &CScene::Get()->GetNewFrightEnemyPos().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosY", &CScene::Get()->GetNewFrightEnemyPos().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosZ", &CScene::Get()->GetNewFrightEnemyPos().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewObjectScalling");
			ImGui::InputFloat("ScaX", &CScene::Get()->GetNewFrightEnemySca().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaY", &CScene::Get()->GetNewFrightEnemySca().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaZ", &CScene::Get()->GetNewFrightEnemySca().z, 0.1f, 1.0f, "%.2f");

			//新しく敵を追加
			if (ImGui::Button("Add Enemy")) {
				DirectX::XMFLOAT3 NewPos = CScene::Get()->GetNewFrightEnemyPos();
				DirectX::XMFLOAT3 NewSca = CScene::Get()->GetNewFrightEnemySca();
				CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, NewPos, {0.0f,0.0f,0.0f}, NewSca);
			}
			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Create New GroundEnemy"))
		{
			// X座標、Y座標、Z座標を個別に入力
			ImGui::Text("NewObjectPosition");
			ImGui::InputFloat("PosX", &CScene::Get()->GetNewGroundEnemyPos().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosY", &CScene::Get()->GetNewGroundEnemyPos().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosZ", &CScene::Get()->GetNewGroundEnemyPos().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewObjectScalling");
			ImGui::InputFloat("ScaX", &CScene::Get()->GetNewGroundEnemySca().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaY", &CScene::Get()->GetNewGroundEnemySca().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaZ", &CScene::Get()->GetNewGroundEnemySca().z, 0.1f, 1.0f, "%.2f");

			//新しく敵を追加
			if (ImGui::Button("Add Enemy")) {
				DirectX::XMFLOAT3 NewPos = CScene::Get()->GetNewGroundEnemyPos();
				DirectX::XMFLOAT3 NewSca = CScene::Get()->GetNewGroundEnemySca();
				CGame::Get()->InsertEnemy(CGameObject::TAG::GROUND_ENEMY,NewPos, { 0.0f,0.0f,0.0f }, NewSca);
			}
			ImGui::TreePop();
		}



		//敵を階層構造で表示
		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Enemys ")) {
			int Id = 1;
			for (auto It = lEnemys.begin(); It != lEnemys.end(); It++)
			{
				if ((*It)->GetTag() != CGameObject::TAG::INITENEMY && (*It)->GetActive())
				{
					// 敵番号付きのノードタイトルを作成
					std::string nodeName = "Enemy " + std::to_string(Id);

					ImGui::SetNextItemOpen(false, ImGuiCond_Once);
					if (ImGui::TreeNode(nodeName.c_str()))
					{
						ImGui::Text("HP: %d", (*It)->GetHp());
						ImGui::Text("Position: %f %f %f", (*It)->Position.x, (*It)->Position.y, (*It)->Position.z);
						ImGui::Text("Rotation: %f %f %f", (*It)->Rotation.x, (*It)->Rotation.y, (*It)->Rotation.z);

						//敵を削除
						if (ImGui::Button("Destroy Enemy")) {
							(*It)->SetDamege((*It)->GetHp());
						}

						//オブジェクトの表示非表示
						if (ImGui::Button("Set Active")) {
							(*It)->SetActive(!(*It)->GetActive());
						}

						ImGui::TreePop();
					}

					Id++;
				}
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	//建物関係のデバッグ表示//

	ImGui::SetNextItemOpen(false, ImGuiCond_Once);
	if (ImGui::TreeNode("Objects"))
	{
		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Create New Object"))
		{
			// X座標、Y座標、Z座標を個別に入力
			ImGui::Text("NewObjectPosition");
			ImGui::InputFloat("PosX", &CScene::Get()->GetNewObjectPos().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosY", &CScene::Get()->GetNewObjectPos().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosZ", &CScene::Get()->GetNewObjectPos().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewObjectRotation");
			ImGui::InputFloat("RotX", &CScene::Get()->GetNewObjectRot().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("RotY", &CScene::Get()->GetNewObjectRot().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("RotZ", &CScene::Get()->GetNewObjectRot().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewObjectScalling");
			ImGui::InputFloat("ScaX", &CScene::Get()->GetNewObjectSca().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaY", &CScene::Get()->GetNewObjectSca().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaZ", &CScene::Get()->GetNewObjectSca().z, 0.1f, 1.0f, "%.2f");

			//新しくオブジェクトを追加
			if (ImGui::Button("Add Object")) {
				DirectX::XMFLOAT3 NewPos = CScene::Get()->GetNewObjectPos();
				DirectX::XMFLOAT3 NewRot = CScene::Get()->GetNewObjectRot();
				DirectX::XMFLOAT3 NewSca = CScene::Get()->GetNewObjectSca();

				CGameObject* pBills1 = new CGameObject();
				pBills1->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/Cube.obj"));
				CScene::Get()->SetGameObjectInf(L"asset/CHARACTER/Cube.obj", false, true, NewPos, NewRot, NewSca, 1.0f, CGameObject::TAG::OBJECT, pBills1, false);
			}
			ImGui::TreePop();
		}

		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::TreeNode("Create New Cloud"))
		{
			// X座標、Y座標、Z座標を個別に入力
			ImGui::Text("NewObjectPosition");
			ImGui::InputFloat("PosX", &CScene::Get()->GetNewObjectPos().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosY", &CScene::Get()->GetNewObjectPos().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("PosZ", &CScene::Get()->GetNewObjectPos().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewObjectRotation");
			ImGui::InputFloat("RotX", &CScene::Get()->GetNewObjectRot().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("RotY", &CScene::Get()->GetNewObjectRot().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("RotZ", &CScene::Get()->GetNewObjectRot().z, 0.1f, 1.0f, "%.2f");

			ImGui::Text("NewObjectScalling");
			ImGui::InputFloat("ScaX", &CScene::Get()->GetNewObjectSca().x, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaY", &CScene::Get()->GetNewObjectSca().y, 0.1f, 1.0f, "%.2f");
			ImGui::InputFloat("ScaZ", &CScene::Get()->GetNewObjectSca().z, 0.1f, 1.0f, "%.2f");

			//新しくオブジェクトを追加
			if (ImGui::Button("Add Object")) {
				DirectX::XMFLOAT3 NewPos = CScene::Get()->GetNewObjectPos();
				DirectX::XMFLOAT3 NewRot = CScene::Get()->GetNewObjectRot();
				DirectX::XMFLOAT3 NewSca = CScene::Get()->GetNewObjectSca();

				CGameObject* pCloud = new CGameObject();
				pCloud->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/Clooud.obj"));
				CScene::Get()->SetGameObjectInf(L"asset/CHARACTER/Clooud.obj", false, false, NewPos, NewRot, NewSca, 0.6f, CGameObject::TAG::CLOUD, pCloud, false);
			}
			ImGui::TreePop();
		}

		//オブジェクトを配列で取得
		std::list<CGameObject*> lGameObject;
		CScene::Get()->Get3DObject(lGameObject);
		int Id = 1;
		for (auto It = lGameObject.begin(); It != lGameObject.end(); It++)
		{
			if ((*It)->GetTag() == CGameObject::TAG::OBJECT && (*It)->GetActive())
			{
				// 敵番号付きのノードタイトルを作成
				std::string nodeName = "Object " + std::to_string(Id);

				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::TreeNode(nodeName.c_str()))
				{
					// X座標、Y座標、Z座標を個別に入力
					ImGui::InputFloat("X", &(*It)->Position.x, 0.1f, 1.0f, "%.2f");
					ImGui::InputFloat("Y", &(*It)->Position.y, 0.1f, 1.0f, "%.2f");
					ImGui::InputFloat("Z", &(*It)->Position.z, 0.1f, 1.0f, "%.2f");

					//敵を削除
					if (ImGui::Button("Destroy Object")) {
						(*It)->SetActive(!(*It)->GetActive());
					}

					ImGui::TreePop();
				}
				Id++;
			}
			else if ((*It)->GetTag() == CGameObject::TAG::GROUND)
			{
				// 敵番号付きのノードタイトルを作成
				std::string nodeName = "Ground";

				ImGui::SetNextItemOpen(false, ImGuiCond_Once);
				if (ImGui::TreeNode(nodeName.c_str()))
				{
					// X座標、Y座標、Z座標を個別に入力
					ImGui::InputFloat("X", &(*It)->Position.x, 0.1f, 1.0f, "%.2f");
					ImGui::InputFloat("Y", &(*It)->Position.y, 0.1f, 1.0f, "%.2f");
					ImGui::InputFloat("Z", &(*It)->Position.z, 0.1f, 1.0f, "%.2f");

					ImGui::TreePop();
				}
				Id++;
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

CScene::CScene()
{
	Scene = TITLE;
	//Scene = MENU;
	//Scene = STAGE_03;
	//Scene = TEST_STAGE;
	//Scene = GAMECLEAR;
	//Scene = GAMEOVER;
	SetScene(Scene);
}

CScene::~CScene()
{
	//オブジェクトの解放処理
	ClearGameObject();
}

void CScene::SetGameObjectInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting, DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw, CGameObject::TAG _tag, CGameObject* _gameobject, bool _isui)
{
	_gameobject->SetModel(CGame::Get()->GetModel(_moedlname));
	_gameobject->SetModelName(_moedlname);
	_gameobject->SetBillboard(_isbillboar);
	_gameobject->SetLighting(_islighting);
	_gameobject->Position = _position;
	_gameobject->Rotation = _rotation;
	_gameobject->Scaling = _scaling;
	_gameobject->materialDiffuse.w = _materialw;
	_gameobject->SetTag(_tag);
	if (_tag == CGameObject::TAG::GROUND || _tag == CGameObject::TAG::OBJECT)
	{
		_gameobject->SetInitRotateMatrix();
		_gameobject->SetLinear();
		_gameobject->SetColliderBoxAABB( _position, _scaling);
		_gameobject->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
	}
	else if (_tag == CGameObject::TAG::SUN)
	{
		CGame::Get()->SetSunPosition(_position);
	}
	else if (_tag == CGameObject::TAG::MINIMAP_UI)
	{
		_gameobject->SetMiniMapModel(CGame::Get()->GetModel(L"asset/UI/MapFrame.png"));
		_gameobject->SetIsMiniMapUpdate(true);
		_gameobject->SetActive(false);
	}


	if (_isui)
	{
		lGameUI.push_back(_gameobject);
	}
	else
	{
		lGameObject.push_back(_gameobject);
	}
}

void CScene::SetGameObjectInfTest(bool _isbillboar, bool _islighting, DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw, CGameObject::TAG _tag, bool _isui)
{
	switch (_tag)
	{
	case CGameObject::NONE:
	{
		break;
	}
	case CGameObject::BACK_SKY_CIRCLE:
	{
		CGameObject* Object = new CGameObject();
		Object->SetCullingMode(false);
		Object->SetModelName(L"asset/CHARACTER/BackCircle.obj");
		Object->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/BackCircle.obj"));
		Object->SetBillboard(_isbillboar);
		Object->SetLighting(_islighting);
		Object->Position = _position;
		Object->Rotation = _rotation;
		Object->Scaling = _scaling;
		Object->materialDiffuse.w = _materialw;
		Object->SetTag(_tag);
		if (_isui)
		{
			lGameUI.push_back(Object);
		}
		else
		{
			lGameObject.push_back(Object);
		}
		break;
	}
	case CGameObject::GROUND:
	{
		CGameObject* Object = new CGameObject();
		Object->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapGround.obj"));
		Object->SetIsMiniMapUpdate(true);
		Object->SetModelName(L"asset/Wave.png");
		Object->SetModel(CGame::Get()->GetModel(L"asset/Wave.png"));
		Object->SetBillboard(_isbillboar);
		Object->SetLighting(_islighting);
		Object->Position = _position;
		Object->Rotation = _rotation;
		Object->Scaling = _scaling;
		Object->materialDiffuse.w = _materialw;
		Object->SetTag(_tag);

		Object->SetInitRotateMatrix();
		Object->SetLinear();
		Object->SetColliderBoxAABB(_position, _scaling);
		Object->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));

		if (_isui)
		{
			lGameUI.push_back(Object);
		}
		else
		{
			lGameObject.push_back(Object);
		}
		break;
	}
	case CGameObject::PLAYER:
	{
		CPlayer* pPlayer = new CPlayer();
		pPlayer->SetAnimUvSize({ 10, 1 });
		pPlayer->SetRockUI(CGame::Get()->GetModel(L"asset/UI/RockPlayer.png"), CGame::Get()->GetModel(L"asset/UI/RockOnPlayer.png"));
		pPlayer->SetHitMisileEffect(CGame::Get()->GetModel(L"asset/Effect_01.png"));
		pPlayer->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/PlayerMiniMap.obj"));
		pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_Jet.obj"));
		pPlayer->SetTag(CGameObject::TAG::PLAYER);
		pPlayer->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
		//座標や大きさ設定
		pPlayer->Position = { 0.0f,0.0f,00.0f };
		pPlayer->Rotation = { 0.0f,0.0f,0.0f };
		pPlayer->Scaling = { 1.0f, 1.0f, 1.0f };
		IsDrawPlayer = true;
		lPlayer.push_back(pPlayer);
		break;
	}
	case CGameObject::VECTOR:
	{
		CVector* Object = new CVector();
		Object->SetModelName(L"asset/CHARACTER/Vectol.obj");
		Object->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Vectol.obj"));
		Object->SetBillboard(_isbillboar);
		Object->SetLighting(_islighting);
		Object->Position = _position;
		Object->Rotation = _rotation;
		Object->Scaling = _scaling;
		Object->materialDiffuse.w = _materialw;
		Object->SetTag(_tag);
		if (_isui)
		{
			lGameUI.push_back(Object);
		}
		else
		{
			lGameObject.push_back(Object);
		}
		break;
	}
	case CGameObject::OBJECT:
	{
		CGameObject* Object = new CGameObject();
		Object->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/Cube.obj"));
		Object->SetModelName(L"asset/CHARACTER/Bill.obj");
		Object->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Bill.obj"));
		Object->SetIsMiniMapUpdate(true);
		Object->SetBillboard(_isbillboar);
		Object->SetLighting(_islighting);
		Object->Position = _position;
		Object->Rotation = _rotation;
		Object->Scaling = _scaling;
		Object->materialDiffuse.w = _materialw;
		Object->SetTag(_tag);
		Object->SetColliderBoxAABB(_position, _scaling);
		Object->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
		if (_isui)
		{
			lGameUI.push_back(Object);
		}
		else
		{
			lGameObject.push_back(Object);
		}
		break;
	}
	case CGameObject::CLOUD:
	{
		CGameObject* Object = new CGameObject();
		Object->SetModelName(L"asset/CHARACTER/Clooud.obj");
		Object->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Clooud.obj"));
		Object->SetBillboard(_isbillboar);
		Object->SetLighting(_islighting);
		Object->Position = _position;
		Object->Rotation = _rotation;
		Object->Scaling = _scaling;
		Object->materialDiffuse.w = _materialw;
		Object->SetTag(_tag);
		Object->SetInitRotateMatrix();
		//Object->SetColliderBoxAABB();
		//Object->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
		if (_isui)
		{
			lGameUI.push_back(Object);
		}
		else
		{
			lGameObject.push_back(Object);
		}
		break;
	}
	case CGameObject::GROUND_ENEMY:
	{
		CGame::Get()->InsertEnemy(CGameObject::TAG::GROUND_ENEMY,_position, { 0.0f,0.0f,0.0f }, _scaling);
		break;
	}
	case CGameObject::FRIGHT_ENEMY:
	{
		CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, _position, { 0.0f,0.0f,0.0f }, _scaling);
		break;
	}
	case CGameObject::FOLLOW_ENEMY:
	{
		CGame::Get()->InsertEnemy(CGameObject::TAG::FOLLOW_ENEMY, _position, _rotation, _scaling);
		break;
	}
	case CGameObject::BOSS_ENEMY:
	{
		CGame::Get()->InsertEnemy(CGameObject::TAG::BOSS_ENEMY, _position, { 0.0f,0.0f,0.0f }, _scaling);
		break;
	}
	case CGameObject::MISILE_UI:
	{
		CMisileUI* pMisileUI = new CMisileUI();
		pMisileUI->SetBillboard(_isbillboar);
		pMisileUI->SetLighting(_islighting);
		pMisileUI->SetModel(CGame::Get()->GetModel(L"asset/UI/Misile_UI.png"));
		pMisileUI->Position = _position;
		pMisileUI->Rotation = _rotation;
		pMisileUI->Scaling = _scaling;
		pMisileUI->materialDiffuse.w = _materialw;
		pMisileUI->SetEffectPos(_position);
		pMisileUI->SetEffectScale({ 1.08f,1.14f,0.1f });
		pMisileUI->SetTag(_tag);
		pMisileUI->SetMisileUIInf(CGame::Get()->GetModel(L"asset/UI/Misile_UI_Test.png"), DirectX::XMFLOAT2(10, 1));
		plMisileUI.push_back(pMisileUI);
		break;
	}
	case CGameObject::BOSSNAME_UI:
	{
		CGameObject* pBossNameUI = new CGameObject();
		SetGameObjectInf(L"asset/UI/BossUI.png", false, true, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 1.0f, CGameObject::TAG::BOSSNAME_UI, pBossNameUI, false);
		break;
	}
	case CGameObject::BOSSHP_UI:
	{
		CGameObject* pBossHPUI = new CGameObject();
		SetGameObjectInf(L"asset/UI/BossHpUI.png", false, true, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 1.0f, CGameObject::TAG::BOSSHP_UI, pBossHPUI, false);
		break;
	}
	case CGameObject::MAX:
	default:
		break;
	}
	//_gameobject->SetModelName(_moedlname);

}

void CScene::GetTextValue(std::string _stagename)
{
	// 読み込むファイルの名前
	//std::ifstream file("Stage/TestStage.txt");
	std::ifstream file(_stagename);

	if (!file.is_open()) {
		// 読み込みに失敗した場合
		MessageBoxA(NULL, "Miss Open File", "エラー",
			MB_OK | MB_ICONERROR);
	}

	////ファイルの中身をテストテキスト変数に取得
	////これで一行取得できる
	//file >> TestText;

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

		//テスト文字列
		//2,L"asset/CHARACTER/Ground.obj",0,1,0.0f,-60.0f,0.0f,0.0f,0.0f,0.0f,100.0f,1.0f,100.0f,1.0f,0

		//中身をガンマで区切っている
		while (getline(stream, line, ','))
		{
			// 文字列長を計算
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);

			// ワイド文字列用のバッファを確保
			wstring wstr(bufferSize, L'\0');  // ヌル文字で初期化

			switch (EGetText)
			{
			case CScene::TAG:SGetText.TagId = std::stoi(line); IsInsert = true; break;
			case CScene::BILLBORD:SGetText.IsBillbord = std::stoi(line); break;
			case CScene::LIGHTING:SGetText.IsLighting = std::stoi(line); break;
			case CScene::POSX:SGetText.PositionX = std::stof(line); break;
			case CScene::POSY:SGetText.PositionY = std::stof(line); break;
			case CScene::POSZ:SGetText.PositionZ = std::stof(line); break;
			case CScene::ROTX:SGetText.RotationX = std::stof(line); break;
			case CScene::ROTY:SGetText.RotationY = std::stof(line); break;
			case CScene::ROTZ:SGetText.RotationZ = std::stof(line); break;
			case CScene::SCAX:SGetText.ScalingX = std::stof(line); break;
			case CScene::SCAY:SGetText.ScalingY = std::stof(line); break;
			case CScene::SCAZ:SGetText.ScalingZ = std::stof(line); break;
			case CScene::MATERIAL:SGetText.MaterialDiffuseW = std::stof(line); break;
			case CScene::ISUI:SGetText.Isui = std::stoi(line); break;
			case CScene::END:
				break;
			default:
				break;
			}
			EGetText = static_cast<ENUM_GET_TEXT>(static_cast<int>(EGetText) + 1);
		}
		EGetText = TAG;
		if (IsInsert)
		{
			SetGameObjectInfTest(SGetText.IsBillbord, SGetText.IsLighting,
				{ SGetText.PositionX,SGetText.PositionY,SGetText.PositionZ }, { SGetText.RotationX,SGetText.RotationY,SGetText.RotationZ }, { SGetText.ScalingX,SGetText.ScalingY,SGetText.ScalingZ },
				SGetText.MaterialDiffuseW, static_cast<CGameObject::TAG>(SGetText.TagId), SGetText.Isui);
		}

	}

	file.close();
}

void CScene::SetTextValue(std::string _stagename)
{

	// 書き込み用にファイルを再オープン
	//'std::ios::app` で追記モード
	//'std::ios::trunc'で上書きモード
	std::ofstream outFile("Stage/TestText.txt", std::ios::trunc);
	//std::ofstream outFile(_stagename, std::ios::trunc);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open file for writing.\n";
		assert(outFile.is_open() && "Failed to open file for writing");
	}

	auto It = lGameObject.begin();

	// 書き込むテキスト//
	while (It != lGameObject.end())
	{
		if ((*It)->GetActive())
		{
			//一列開ける
			SGetText.TagId = static_cast<int>((*It)->GetTag());
			SGetText.IsBillbord = static_cast<int>((*It)->GetIsbillBord());
			SGetText.IsLighting = static_cast<int>((*It)->GetIsLighting());
			SGetText.PositionX = (*It)->Position.x;
			SGetText.PositionY = (*It)->Position.y;
			SGetText.PositionZ = (*It)->Position.z;
			SGetText.RotationX = (*It)->Rotation.x;
			SGetText.RotationY = (*It)->Rotation.y;
			SGetText.RotationZ = (*It)->Rotation.z;
			SGetText.ScalingX = (*It)->Scaling.x;
			SGetText.ScalingY = (*It)->Scaling.y;
			SGetText.ScalingZ = (*It)->Scaling.z;
			SGetText.MaterialDiffuseW = (*It)->materialDiffuse.w;
			SGetText.Isui = static_cast<int>((*It)->GetIsUI());

			string IsTag = to_string(SGetText.TagId) + ",";
			string IsBillBord = to_string(SGetText.IsBillbord) + ",";
			string IsLighting = to_string(SGetText.IsLighting) + ",";
			string Position = to_string(SGetText.PositionX) + "," + to_string(SGetText.PositionY) + "," + to_string(SGetText.PositionZ) + ",";
			string Rotation = to_string(SGetText.RotationX) + "," + to_string(SGetText.RotationY) + "," + to_string(SGetText.RotationZ) + ",";
			string Scaling = to_string(SGetText.ScalingX) + "," + to_string(SGetText.ScalingY) + "," + to_string(SGetText.ScalingZ) + ",";
			string MaterialDiffuseW = to_string(SGetText.MaterialDiffuseW) + ",";
			string IsUI = to_string(SGetText.Isui);

			string PlayerInf = IsTag + IsBillBord + IsLighting + Position + Rotation + Scaling + MaterialDiffuseW + IsUI + "\n";

			// ファイルにテキストを書き込む
			outFile << PlayerInf;
		}
		It++;
	}

	//敵を配列で取得
	std::list<CEnemy*> lEnemys;
	CPlayerAttack::Get()->GetEnemys(lEnemys);
	auto ENemyIt = lEnemys.begin();

	while (ENemyIt != lEnemys.end())
	{
		if ((*ENemyIt)->GetActive())
		{
			//一列開ける
			SGetText.TagId = static_cast<int>((*ENemyIt)->GetTag());
			SGetText.IsBillbord = static_cast<int>((*ENemyIt)->GetIsbillBord());
			SGetText.IsLighting = static_cast<int>((*ENemyIt)->GetIsLighting());
			SGetText.PositionX = (*ENemyIt)->Position.x;
			SGetText.PositionY = (*ENemyIt)->Position.y;
			SGetText.PositionZ = (*ENemyIt)->Position.z;
			SGetText.RotationX = (*ENemyIt)->Rotation.x;
			SGetText.RotationY = (*ENemyIt)->Rotation.y;
			SGetText.RotationZ = (*ENemyIt)->Rotation.z;
			SGetText.ScalingX = (*ENemyIt)->Scaling.x;
			SGetText.ScalingY = (*ENemyIt)->Scaling.y;
			SGetText.ScalingZ = (*ENemyIt)->Scaling.z;
			SGetText.MaterialDiffuseW = (*ENemyIt)->materialDiffuse.w;
			SGetText.Isui = static_cast<int>((*ENemyIt)->GetIsUI());

			string IsTag = to_string(SGetText.TagId) + ",";
			string IsBillBord = to_string(SGetText.IsBillbord) + ",";
			string IsLighting = to_string(SGetText.IsLighting) + ",";
			string Position = to_string(SGetText.PositionX) + "," + to_string(SGetText.PositionY) + "," + to_string(SGetText.PositionZ) + ",";
			string Rotation = to_string(SGetText.RotationX) + "," + to_string(SGetText.RotationY) + "," + to_string(SGetText.RotationZ) + ",";
			string Scaling = to_string(SGetText.ScalingX) + "," + to_string(SGetText.ScalingY) + "," + to_string(SGetText.ScalingZ) + ",";
			string MaterialDiffuseW = to_string(SGetText.MaterialDiffuseW) + ",";
			string IsUI = to_string(SGetText.Isui);

			string PlayerInf = IsTag + IsBillBord + IsLighting + Position + Rotation + Scaling + MaterialDiffuseW + IsUI + "\n";

			// ファイルにテキストを書き込む
			outFile << PlayerInf;
		}
		ENemyIt++;
	}


	outFile.close();
}

void CScene::SetTextFileParameter(std::string _stagename,int _num)
{
	//ファイルを開く
	std::ofstream outFile(_stagename, std::ios::trunc);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open file for writing.\n";
		assert(outFile.is_open() && "Failed to open file for writing");
	}



	//ファイルを閉じる
	outFile.close();
}

void CScene::SetGameObjectSppedNumsInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting,
								DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw, 
								CGameObject::TAG _tag, CGameObject* _gameobject, int _num)
{
	_gameobject->SetModel(CGame::Get()->GetModel(_moedlname));
	_gameobject->SetBillboard(_isbillboar);
	_gameobject->SetLighting(_islighting);
	_gameobject->Position = _position;
	_gameobject->Rotation = _rotation;
	_gameobject->Scaling = _scaling;
	_gameobject->materialDiffuse.w = _materialw;
	_gameobject->SetTag(_tag);
	lGameSpeedNums[_num] = _gameobject;
	//lGameSpeedNums.push_back(_gameobject);
}

void CScene::SetGameObjectAltNumsInf(const std::wstring _moedlname, bool _isbillboar, bool _islighting, 
								DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scaling, float _materialw, 
								CGameObject::TAG _tag, CGameObject* _gameobject, int _num)
{
	_gameobject->SetModel(CGame::Get()->GetModel(_moedlname));
	_gameobject->SetBillboard(_isbillboar);
	_gameobject->SetLighting(_islighting);
	_gameobject->Position = _position;
	_gameobject->Rotation = _rotation;
	_gameobject->Scaling = _scaling;
	_gameobject->materialDiffuse.w = _materialw;
	_gameobject->SetTag(_tag);
	lGameAltNums[_num] = _gameobject;
	//lGameAltNums.push_back(_gameobject);
}

CScene* CScene::Get()
{
	static CScene instance;
	return &instance;
}

void CScene::Update()
{
	//if (pPlayer != nullptr)
	//{
	//	pPlayer->Update();
	//}

	if ((int)lPlayer.size() != 0)
	{
		(*lPlayer.begin())->Update();
	}

	//パーティクルを並び替える(カメラから位置が遠いものを先に描画)
	//std::sort(lGameObject.begin(), lGameObject.end(), [](const CGameObject& a, const CGameObject& b) {
	//	return a.CheckDistance(nullptr, CGame::Get()->GetCameraPos()) > b.CheckDistance(nullptr, CGame::Get()->GetCameraPos());
	//	});

	lGameObject.sort([](CGameObject* a, CGameObject* b) {
		return a->CheckDistance(nullptr, CGame::Get()->GetCameraPos()) > b->CheckDistance(nullptr, CGame::Get()->GetCameraPos());
		});

	for (auto GameObject : lGameObject)
	{
		GameObject->Update();
	}

	for (auto GameObjectUi : lGameUI)
	{
		GameObjectUi->Update();
	}

	for (auto MisileUI : plMisileUI)
	{
		MisileUI->Update();
	}
}

void CScene::SetScene(SCENE _scene)
{
	//シーンを取得
	Scene = _scene;

	IsDrawPlayer = false;

	//現在のオブジェクトを解放
	ClearGameObject();

	CGame::Get()->GetCamera3D()->InitFovRadius();

	switch (Scene)
	{
	case CScene::TITLE:
		XA_Play(BGM_TITLE,GetBGMVolume(), 1.0f);
		SetTitleScene();
		break;
	case CScene::MENU:
		CGame::Get()->SetIsGameStageStart(false);
		XA_Stop(BGM_STAGE);
		XA_Stop(BGM_RESULT);
		XA_Resume(BGM_TITLE);
		SetMenuScene();
		break;
	case CScene::STAGE:
		break;
	case CScene::TEST_STAGE:
		CGame::Get()->SetIsGameStageStart(true);
		SetTestStageScene();
		break;
	case CScene::STAGE_01:
		XA_Stop(BGM_TITLE); 
		XA_Play(BGM_STAGE, GetBGMVolume(), 1.0f);
		//XA_Resume(BGM_STAGE);
		SetStageScene("Stage/Stage1.txt");
		Scene = STAGE;
		//SetStage01Scene();
		break;
	case CScene::STAGE_02:
		XA_Stop(BGM_TITLE);
		XA_Play(BGM_STAGE, GetBGMVolume(), 1.0f);
		//XA_Resume(BGM_STAGE);
		SetStageScene("Stage/Stage2.txt");
		Scene = STAGE;
		//SetStage02Scene();
		break;
	case CScene::STAGE_03:
		XA_Stop(BGM_TITLE);
		XA_Play(BGM_STAGE, GetBGMVolume(), 1.0f);
		//XA_Resume(BGM_STAGE);
		SetStageScene("Stage/Stage3.txt");
		Scene = STAGE;
		//SetStage03Scene();
		break;
	case CScene::STAGE_04:
		XA_Stop(BGM_TITLE);
		XA_Play(BGM_STAGE, GetBGMVolume(), 1.0f);
		SetStageScene("Stage/Stage4.txt");
		Scene = STAGE;
		//SetStage04Scene();
		break;
	case CScene::RESULT:
		XA_Stop(BGM_STAGE);
		XA_Play(BGM_RESULT, GetBGMVolume(), 1.0f);
		//XA_Resume(BGM_TITLE);
		SetGameClearScene();
		break;
	case CScene::GAMEOVER:
		XA_Stop(BGM_STAGE);
		XA_Play(BGM_RESULT, GetBGMVolume(), 1.0f);
		//XA_Resume(BGM_TITLE);
		SetGameOverScene();
		break;
	case CScene::GAMECLEAR:
		XA_Stop(BGM_STAGE);
		XA_Play(BGM_RESULT, GetBGMVolume(), 1.0f);
		//XA_Resume(BGM_TITLE);
		SetGameClearScene();
		break;
	case CScene::CREATE_STAGE:
		XA_Resume(BGM_TITLE);
		SetCreateStageScene();
		break;
	default:
		break;
	}
}

CGameObject* CScene::Get3DGameObject(CGameObject::TAG _objecttag)
{
	for (auto GameObject : lGameObject)
	{
		if (GameObject->GetTag() == _objecttag)
		{
			return GameObject;
		}
	}

	return nullptr;
}

CGameObject* CScene::Get2DGameObject(CGameObject::TAG _objecttag)
{
	for (auto GameObject : lGameUI)
	{
		if (GameObject->GetTag() == _objecttag)
		{
			return GameObject;
		}
	}

	return nullptr;
}

CPlayer* CScene::GetPlayer()
{
	if ((int)lPlayer.size() != 0)
	{
		return (*lPlayer.begin());
	}

	return nullptr;
}

void CScene::GetCloudObject(std::list<CGameObject*>& _cloudlist)
{
	_cloudlist.clear();
	for (auto Object : lGameObject)
	{
		if (Object->GetTag() == CGameObject::TAG::CLOUD)
		{
			_cloudlist.push_back(Object);
		}
	}
}

void CScene::InsertCloudObject()
{
	// 乱数生成器の初期化
	std::random_device rd;
	std::mt19937 rng(rd());

	// 乱数の範囲を指定
	std::uniform_real_distribution<float> dist(-5.0f, 5.0f);

	DirectX::XMFLOAT3 NewPos = { dist(rng) ,0.0f,10.0f };

	//雲
	CGameObject* pCloud = new CGameObject();
	SetGameObjectInf(L"asset/CHARACTER/Clooud.obj", false, false, NewPos, { 0.0f,0.0f,0.0f }, { 0.6f, 1.0f, 1.0f }, 0.6f, CGameObject::TAG::CLOUD, pCloud, false);

}

CGameObject* CScene::GetStageSpeedNums(int _numsid)
{
	if (lGameSpeedNums[_numsid] != nullptr)
	{
		return lGameSpeedNums[_numsid];
	}

	return nullptr;
}

CGameObject* CScene::GetStageAltNums(int _numsid)
{
	if (lGameAltNums[_numsid] != nullptr)
	{
		return lGameAltNums[_numsid];
	}

	return nullptr;
}

CGameObject* CScene::GetActiveMisileUI()
{
	for (auto GameUI : lGameUI)
	{
		if (GameUI->GetTag() == CGameObject::TAG::MISILE_UI && GameUI->materialDiffuse.w >= 1.0f  /*GameUI->GetActive() == true*/)
		{
			return GameUI;
		}
	}

	return nullptr;
}

CMisileUI* CScene::GetActiveMisileUITest(bool _isfour)
{
	//ミサイルUIのリストからActiveの物を取得する
	int cnt = 0;
	for (auto MisileUI : plMisileUI)
	{
		if (MisileUI->GetActive())
		{
			return MisileUI;
		}

		cnt++;

		if (!_isfour)
		{
			if (cnt > 1)
			{
				return nullptr;
			}
		}
	}

	return nullptr;
}

int CScene::CheckFourMisileActive()
{
	int Cnt = 0;

	for (auto MisileUI : plMisileUI)
	{
		if (MisileUI->GetActive())
		{
			Cnt++;
		}
	}

	return Cnt;
}

void CScene::SetMisileUICharge(const int _num)
{
	int Cnt = 0;
	for (auto MisileUI : plMisileUI)
	{
		if (MisileUI->GetActive())
		{
			MisileUI->SetMisileUIStateCharge();
			Cnt++;
		}

		if (Cnt >= _num)
		{
			return;
		}
	}
}

void CScene::ResetMisileUI()
{
	for (auto MisileUI : plMisileUI)
	{
		MisileUI->ResetMisileUI();
	}
}

void CScene::ClearGameObject()
{
	for (auto GameUI : lGameUI)
	{
		if (GameUI != nullptr)
		{
			delete GameUI;
			GameUI = nullptr;
		}
	}
	lGameUI.clear();

	for (auto BossHpUI : lBossHPUI)
	{
		if (BossHpUI != nullptr)
		{
			delete BossHpUI;
			BossHpUI = nullptr;
		}
	}
	lBossHPUI.clear();

	for (auto GameObject : lGameObject)
	{
		if (GameObject != nullptr)
		{
			delete GameObject;
			GameObject = nullptr;
		}
	}
	lGameObject.clear();

	for (auto Player : lPlayer)
	{
		if (Player != nullptr)
		{
			Player->ClearMoveTargetList();
			delete Player;
			Player = nullptr;
		}
	}
	lPlayer.clear();

	for (auto Nums : lGameSpeedNums)
	{
		if (Nums.second != nullptr)
		{
			delete Nums.second;
			Nums.second = nullptr;
		}
	}
	lGameSpeedNums.clear();

	for (auto Nums : lGameAltNums)
	{
		if (Nums.second != nullptr)
		{
			delete Nums.second;
			Nums.second = nullptr;
		}
	}
	lGameAltNums.clear();

	for (auto MisileUI : plMisileUI)
	{
		delete MisileUI;
		MisileUI = nullptr;
	}
	plMisileUI.clear();

	//ターゲットの情報をクリア
	CPlayerAttack::Get()->ResetPlayerAttack();

	//エフェクトクリア
	CEffectManager::Get()->ClearEffect();

	////敵情報クリア
	//CPlayerAttack::Get()->ClearEnemys();
}

void CScene::SetTitleScene()
{
	//背景パネル
	CGameObject* pBackSky = new CGameObject();
	SetGameObjectInf(L"asset/sky_01.png", false, false, { 0.0f,-9.0f,50.0f }, { 0.0f,0.0f,0.0f }, { 100.0f, 60.0f, 30.0f }, 1.0f, CGameObject::TAG::BACK_SKY_FLAT, pBackSky,false);

	//プレイヤー
	CPlayer* pPlayer = new CPlayer();
	pPlayer->SetAnimUvSize({ 10, 1 });
	pPlayer->SetRockUI(CGame::Get()->GetModel(L"asset/UI/RockPlayer.png"), CGame::Get()->GetModel(L"asset/UI/RockOnPlayer.png"));
	pPlayer->SetHitMisileEffect(CGame::Get()->GetModel(L"asset/Effect_01.png"));
	pPlayer->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/PlayerMiniMap.obj"));
	pPlayer->SetParticleModel(CGame::Get()->GetModel(L"asset/Smoke.png"));
	//pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_Jet.obj"));
	pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/F-2Test.obj"));
	pPlayer->SetTag(CGameObject::TAG::PLAYER);
	pPlayer->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
	//座標や大きさ設定
	pPlayer->Position = { 0.0f,0.0f,00.0f };
	pPlayer->Rotation = { 0.0f,0.0f,0.0f };
	pPlayer->Scaling = { 1.5f, 1.5f, 1.5f };
	IsDrawPlayer = true;
	lPlayer.push_back(pPlayer);

	//タイトルパネル
	CGameObject* pTitlePanel = new CGameObject();
	SetGameObjectInf(L"asset/UI/Title_UI_01.png", true, false, { 0.0f,0.6f,0.0f }, { 0.0f,0.0f,0.0f }, { 4.0f, 1.3f, 0.1f }, 1.0f, CGameObject::TAG::UI, pTitlePanel, true);

	//プレスAボタン
	CGameObject* pPressAButton = new CGameObject();
	SetGameObjectInf(L"asset/UI/PressAButton.png", true, false, { 0.0f,-1.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 2.0f, 0.3f, 0.1f }, 1.0f, CGameObject::TAG::PRESS_UI, pPressAButton, true);

}

void CScene::SetMenuScene()
{
	//背景
	CGameObject* pMenuPanel = new CGameObject();
	pMenuPanel->SetModel(CGame::Get()->GetModel(L"asset/UI/Menu_Panel.png"));
	pMenuPanel->SetLighting(false);
	pMenuPanel->SetBillboard(true);
	pMenuPanel->Position = { 0.0f,0.0f,0.1f };
	pMenuPanel->Rotation = { 0.0f,0.0f,0.0f };
	pMenuPanel->Scaling = { 5.0f, 4.0f, 1.0f };
	pMenuPanel->materialDiffuse.w = 1.0f;
	pMenuPanel->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuPanel);

	//メニューカーソル
	CGameObject* pMenuCarsol = new CGameObject();
	pMenuCarsol->SetModel(CGame::Get()->GetModel(L"asset/UI/MenuCarsol.png"));
	pMenuCarsol->SetLighting(false);
	pMenuCarsol->SetBillboard(true);
	pMenuCarsol->Position = { -0.95f,0.0f,0.0f };
	pMenuCarsol->Rotation = { 0.0f,0.0f,0.0f };
	pMenuCarsol->Scaling = { 0.1f, 0.1f, 0.1f };
	pMenuCarsol->materialDiffuse.w = 1.0f;
	pMenuCarsol->SetTag(CGameObject::TAG::CARSOL);
	lGameUI.push_back(pMenuCarsol);

	//メニューGameEnd
	CGameObject* pMenuGameEnd = new CGameObject();
	pMenuGameEnd->SetModel(CGame::Get()->GetModel(L"asset/UI/GameEnd.png"));
	pMenuGameEnd->SetLighting(false);
	pMenuGameEnd->SetBillboard(true);
	pMenuGameEnd->Position = { -0.5f,-0.5f,0.0f };
	pMenuGameEnd->Rotation = { 0.0f,0.0f,0.0f };
	pMenuGameEnd->Scaling = { 0.8f, 0.4f, 0.1f };
	pMenuGameEnd->materialDiffuse.w = 1.0f;
	pMenuGameEnd->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuGameEnd);

	//メニューGameStart
	CGameObject* pMenuGameStart = new CGameObject();
	pMenuGameStart->SetModel(CGame::Get()->GetModel(L"asset/UI/GameStart.png"));
	pMenuGameStart->SetLighting(false);
	pMenuGameStart->SetBillboard(true);
	pMenuGameStart->Position = { -0.5f,0.0f,0.0f };
	pMenuGameStart->Rotation = { 0.0f,0.0f,0.0f };
	pMenuGameStart->Scaling = { 0.8f, 0.4f, 0.1f };
	pMenuGameStart->materialDiffuse.w = 1.0f;
	pMenuGameStart->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuGameStart);

	//メニューA
	CGameObject* pMenuUI_A = new CGameObject();
	pMenuUI_A->SetModel(CGame::Get()->GetModel(L"asset/UI/A_UI.png"));
	pMenuUI_A->SetLighting(false);
	pMenuUI_A->SetBillboard(true);
	pMenuUI_A->Position = { -2.2f,-1.2f,0.0f };
	pMenuUI_A->Rotation = { 0.0f,0.0f,0.0f };
	pMenuUI_A->Scaling = { 0.2f, 0.2f, 0.1f };
	pMenuUI_A->materialDiffuse.w = 1.0f;
	pMenuUI_A->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuUI_A);

	//メニューB
	CGameObject* pMenuUI_B = new CGameObject();
	pMenuUI_B->SetModel(CGame::Get()->GetModel(L"asset/UI/B_UI.png"));
	pMenuUI_B->SetLighting(false);
	pMenuUI_B->SetBillboard(true);
	pMenuUI_B->Position = { -1.5f,-1.2f,0.0f };
	pMenuUI_B->Rotation = { 0.0f,0.0f,0.0f };
	pMenuUI_B->Scaling = { 0.2f, 0.2f, 0.1f };
	pMenuUI_B->materialDiffuse.w = 1.0f;
	pMenuUI_B->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuUI_B);

	//メニューEnter
	CGameObject* pMenuUI_Enter = new CGameObject();
	pMenuUI_Enter->SetModel(CGame::Get()->GetModel(L"asset/UI/Enter_UI.png"));
	pMenuUI_Enter->SetLighting(false);
	pMenuUI_Enter->SetBillboard(true);
	pMenuUI_Enter->Position = { -2.0f,-1.2f,0.0f };
	pMenuUI_Enter->Rotation = { 0.0f,0.0f,0.0f };
	pMenuUI_Enter->Scaling = { 0.2f, 0.1f, 0.1f };
	pMenuUI_Enter->materialDiffuse.w = 1.0f;
	pMenuUI_Enter->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuUI_Enter);

	//メニューBack
	CGameObject* pMenuUI_Back = new CGameObject();
	pMenuUI_Back->SetModel(CGame::Get()->GetModel(L"asset/UI/Back_UI.png"));
	pMenuUI_Back->SetLighting(false);
	pMenuUI_Back->SetBillboard(true);
	pMenuUI_Back->Position = { -1.3f,-1.2f,0.0f };
	pMenuUI_Back->Rotation = { 0.0f,0.0f,0.0f };
	pMenuUI_Back->Scaling = { 0.2f, 0.1f, 0.1f };
	pMenuUI_Back->materialDiffuse.w = 1.0f;
	pMenuUI_Back->SetTag(CGameObject::TAG::UI);
	lGameUI.push_back(pMenuUI_Back);

	CGameObject* pMenuStage[5];
	for (int i = 0; i < 5; i++)
	{
		pMenuStage[i] = new CGameObject();
		pMenuStage[i]->SetLighting(false);
		pMenuStage[i]->SetBillboard(true);
		pMenuStage[i]->materialDiffuse.w = 1.0f;
		pMenuStage[i]->SetTag(CGameObject::TAG::UI);
	}

	pMenuStage[0]->SetModel(CGame::Get()->GetModel(L"asset/UI/STAGE_01.png"));
	pMenuStage[1]->SetModel(CGame::Get()->GetModel(L"asset/UI/STAGE_02.png"));
	pMenuStage[2]->SetModel(CGame::Get()->GetModel(L"asset/UI/STAGE_03.png"));
	pMenuStage[3]->SetModel(CGame::Get()->GetModel(L"asset/UI/STAGE_04.png"));
	pMenuStage[4]->SetModel(CGame::Get()->GetModel(L"asset/UI/STAGE_05.png"));

	pMenuStage[0]->Position = { 0.5f,0.5f,0.0f };
	pMenuStage[0]->Rotation = { 0.0f,0.0f,0.0f };
	pMenuStage[0]->Scaling = { 0.6f, 0.2f, 0.1f };

	lGameUI.push_back(pMenuStage[0]);

	//メニューStage
	for (int i = 1; i < 4; i++)
	{
		pMenuStage[i]->Position = pMenuStage[0]->Position;
		pMenuStage[i]->Position.y -= (i * 0.4f);
		pMenuStage[i]->Rotation = { 0.5f,0.0f,0.0f };
		pMenuStage[i]->Scaling = { 0.6f, 0.2f, 0.1f };
		pMenuStage[i]->materialDiffuse.w = 1.0f;
		lGameUI.push_back(pMenuStage[i]);
	}

}

void CScene::SetStageScene(std::string _stagename)
{
	//モデル設定//

	//ミサイルの情報をセット
	CPlayerAttack::Get()->SetMisileInf(CGame::Get()->GetModel(L"asset/CHARACTER/Misile.obj"), CGame::Get()->GetModel(L"asset/Effect_01.png"), CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });
	//機関銃の弾の情報をセット
	CPlayerAttack::Get()->SetBallInf(CGame::Get()->GetModel(L"asset/CHARACTER/Cube.obj"), CGame::Get()->GetModel(L"asset/Effect_02.png"), { 12,1 });
	//敵のミサイル情報をセット
	CEnemyAttack::Get()->SetMisileInf(CGame::Get()->GetModel(L"asset/CHARACTER/Misile.obj"), CGame::Get()->GetModel(L"asset/Effect_01.png"), CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });
	//敵の機関銃情報をセット
	CEnemyAttack::Get()->SetGunInf(CGame::Get()->GetModel(L"asset/CHARACTER/Cube.obj"), CGame::Get()->GetModel(L"asset/Effect_02.png"), { 12,1 });
	//ミサイルのパーティクルのモデル設定
	CPlayerAttack::Get()->SetMisileParticleModel(CGame::Get()->GetModel(L"asset/Smoke.png"));
	CEnemyAttack::Get()->SetMisileParticleModel(CGame::Get()->GetModel(L"asset/Smoke.png"));

	//ターゲットの情報をクリア
	CPlayerAttack::Get()->ResetPlayerAttack();

	//エフェクトクリア
	CEffectManager::Get()->ClearEffect();

	if (CGame::Get()->GetIsStop())
		CGame::Get()->SetIsGameStop();


	//3Dオブジェクト//

	//初期敵
	CEnemy* pInitEnemy = new CEnemy();
	pInitEnemy->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_EnemyJet.obj"));
	pInitEnemy->Init();
	pInitEnemy->Position = { 0.0f,0.0f,10.0f };
	pInitEnemy->Rotation = { 0.0f,0.0f,0.0f };
	pInitEnemy->Scaling = { 1.0f,1.0f,1.0f };
	pInitEnemy->SetInitRotateMatrix();
	pInitEnemy->SetTag(CGameObject::TAG::INITENEMY);
	//pInitEnemy->SetDrawTarget(false);
	pInitEnemy->SetCanAttack(false);
	pInitEnemy->SetActive(false);

	CPlayerAttack::Get()->ClearEnemys();

	//敵を格納する
	CPlayerAttack::Get()->InsertEnemys(pInitEnemy);

	//プレイヤー
	CPlayer* pPlayer = new CPlayer();
	pPlayer->SetAnimUvSize({ 10, 1 });
	pPlayer->SetRockUI(CGame::Get()->GetModel(L"asset/UI/RockPlayer.png"), CGame::Get()->GetModel(L"asset/UI/RockOnPlayer.png"));
	pPlayer->SetHitMisileEffect(CGame::Get()->GetModel(L"asset/Effect_01.png"));
	pPlayer->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/PlayerMiniMap.obj"));
	pPlayer->SetParticleModel(CGame::Get()->GetModel(L"asset/Smoke.png"));
	//pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_Jet.obj"));
	pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/F-2Test.obj"));
	pPlayer->SetTag(CGameObject::TAG::PLAYER);
	pPlayer->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
	//座標や大きさ設定
	pPlayer->Position = { 0.0f,0.0f,-30.0f };
	pPlayer->Rotation = { 0.0f,0.0f,0.0f };
	pPlayer->Scaling = { 1.5f, 1.5f, 1.5f };
	pPlayer->SetMoveSpeed(0.1f);
	IsDrawPlayer = true;
	lPlayer.push_back(pPlayer);

	//ファイルからオブジェクト情報を取得
	GetTextValue(_stagename);

	//UI//

	//ミニマップUI
	CGameObject* pMiniMapUI = new CGameObject();
	SetGameObjectInf(L"asset/None.png", false, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 1.0f, CGameObject::TAG::MINIMAP_UI, pMiniMapUI, false);

	//画面中央UI
	CGameObject* pCenterUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Center_UI.png", true, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.13f,0.1f,0.1f }, 1.0f, CGameObject::TAG::CENTER_UI, pCenterUI, true);

	//スピードフレーム
	CGameObject* pSpeedFrame = new CGameObject();
	SetGameObjectInf(L"asset/UI/STAGE_FRAME.png", true, false, { -0.94f,0.25f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.3f, 0.25f, 0.1f }, 1.0f, CGameObject::TAG::UI, pSpeedFrame, true);

	//高度フレーム
	CGameObject* pAltFrame = new CGameObject();
	SetGameObjectInf(L"asset/UI/STAGE_FRAME.png", true, false, { 1.05f,0.25f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.3f, 0.25f, 0.1f }, 1.0f, CGameObject::TAG::UI, pAltFrame, true);

	//スピードUI
	CGameObject* pSpeed_UI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Stage_SPEED.png", true, false, { -0.95f,0.3f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.25f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::UI, pSpeed_UI, true);

	//高度UI
	CGameObject* pAlt_UI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Stage_ALT.png", true, false, { 1.05f,0.3f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.2f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::UI, pAlt_UI, true);

	//プレイヤーUI
	CGameObject* pFightJetUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/FightJet_UI_01.png", true, false, { 1.8f,-1.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.4f, 0.4f, 0.1f }, 1.0f, CGameObject::TAG::PLAYER_UI, pFightJetUI, true);

	//ミサイルUI
	SetGameObjectInfTest(true, false, { 1.53f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);
	SetGameObjectInfTest(true, false, { 2.07f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);
	SetGameObjectInfTest(true, false, { 1.36f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);
	SetGameObjectInfTest(true, false, { 2.24f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);

	//攻撃UI
	CGameObject* pXUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/X_UI.png", true, false, { -2.3f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.1f,0.1f,0.1f }, 1.0f, CGameObject::TAG::UI, pXUI, true);

	//攻撃UI2
	CGameObject* pAttackUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Attack_UI.png", true, false, { -2.14f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.2f,0.15f,0.1f }, 1.0f, CGameObject::TAG::UI, pAttackUI, true);

	//ターゲット変更UI
	CGameObject* pYUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Y_UI.png", true, false, { -1.9f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.1f,0.1f,0.1f }, 1.0f, CGameObject::TAG::UI, pYUI, true);

	//ターゲット変更UI2
	CGameObject* pChengeTargetUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/ChengeTarget_UI.png", true, false, { -1.6f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.4f,0.15f,0.1f }, 1.0f, CGameObject::TAG::UI, pChengeTargetUI, true);

	//現在の攻撃情報
	CGameObject* pAttackInf = new CGameObject();
	SetGameObjectInf(L"asset/UI/Stage_MSL.png", true, false, { 1.8f,-0.7f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.4f, 0.2f, 0.1f }, 1.0f, CGameObject::TAG::ATTACKINF_UI, pAttackInf, true);

	//ミサイルヒットUI
	CGameObject* pHitMisileUI = new CGameObject();
	pHitMisileUI->SetActive(false);
	SetGameObjectInf(L"asset/UI/MISILE_HIT_UI.png", true, false, { 0.0f,-0.3f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.8f, 0.2f, 1.0f }, 1.0f, CGameObject::TAG::HITMISILE_UI, pHitMisileUI, true);

	//警告UI
	CGameObject* pCaveatUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Warning.png", true, false, { 0.0f,0.1f,-0.1f }, { 0.0f,0.0f,0.0f }, { 1.3f, 0.3f, 0.1f }, 0.6f, CGameObject::TAG::CAVEAT_UI, pCaveatUI, true);

	//ボスがいるなら
	if (CPlayerAttack::Get()->GetBossEnemy() != nullptr)
	{
		//ボスの名前
		CGameObject* pBossNameUI = new CGameObject();
		SetGameObjectInf(L"asset/UI/BossUI.png", false, true, { -1.5f,1.2f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.8f,0.3f,0.1f }, 1.0f, CGameObject::TAG::BOSSNAME_UI, pBossNameUI, true);

		float InitXPos = -0.8f;

		//ボスのHP
		for (int i = 0; i < CPlayerAttack::Get()->GetBossEnemy()->GetHp(); i += 10)
		{
			CGameObject* pBossHPUI = new CGameObject();
			SetGameObjectInf(L"asset/UI/BossHpUI.png", false, false, { InitXPos,1.2f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.2f,0.2f,0.1f }, 1.0f, CGameObject::TAG::BOSSHP_UI, pBossHPUI, true);

			InitXPos += 0.2f;
		}

		//CGameObject* pBossHPUI = new CGameObject();
		//SetGameObjectInf(L"asset/UI/BossHpUI.png", false, true, { -0.8f,1.2f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.3f,0.2f,0.1f }, 1.0f, CGameObject::TAG::BOSSHP_UI, pBossHPUI, true);
	}

	//スピード表示をする数字群
	CGameObject* pSpeedNums[10];
	for (int i = 0; i < 10; i++)
		pSpeedNums[i] = new CGameObject();
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_0.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[0], 0);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_1.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[1], 1);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_2.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[2], 2);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_3.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[3], 3);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_4.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[4], 4);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_5.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[5], 5);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_6.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[6], 6);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_7.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[7], 7);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_8.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[8], 8);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_9.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[9], 9);

	//高度表示をする数字群
	CGameObject* pAltNums[10];
	for (int i = 0; i < 10; i++)
		pAltNums[i] = new CGameObject();
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_0.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[0], 0);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_1.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[1], 1);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_2.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[2], 2);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_3.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[3], 3);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_4.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[4], 4);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_5.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[5], 5);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_6.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[6], 6);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_7.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[7], 7);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_8.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[8], 8);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_9.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[9], 9);

	CPlayerAttack::Get()->ClearMisileParticles();

	if (!IsCreateStageDebugWindoe)
	{
		// デバッグ表示関数登録
		DebugUI::RedistDebugFunction(DebugStage);
		IsCreateStageDebugWindoe = true;
	}

}

void CScene::SetTestStageScene()
{
	//モデル設定//

	//ミサイルの情報をセット
	CPlayerAttack::Get()->SetMisileInf(CGame::Get()->GetModel(L"asset/CHARACTER/Misile.obj"), CGame::Get()->GetModel(L"asset/Effect_01.png"), CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });
	//機関銃の弾の情報をセット
	CPlayerAttack::Get()->SetBallInf(CGame::Get()->GetModel(L"asset/CHARACTER/Cube.obj"), CGame::Get()->GetModel(L"asset/Effect_02.png"), { 12,1 });
	//敵のミサイル情報をセット
	CEnemyAttack::Get()->SetMisileInf(CGame::Get()->GetModel(L"asset/CHARACTER/Misile.obj"), CGame::Get()->GetModel(L"asset/Effect_01.png"), CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapMisile.obj"), { 10,1 });
	//敵の機関銃情報をセット
	CEnemyAttack::Get()->SetGunInf(CGame::Get()->GetModel(L"asset/CHARACTER/Cube.obj"), CGame::Get()->GetModel(L"asset/Effect_02.png"), { 12,1 });
	//ミサイルのパーティクルのモデル設定
	CPlayerAttack::Get()->SetMisileParticleModel(CGame::Get()->GetModel(L"asset/Smoke.png"));
	CEnemyAttack::Get()->SetMisileParticleModel(CGame::Get()->GetModel(L"asset/Smoke.png"));

	//ターゲットの情報をクリア
	CPlayerAttack::Get()->ResetPlayerAttack();

	//エフェクトクリア
	CEffectManager::Get()->ClearEffect();

	if (CGame::Get()->GetIsStop())
		CGame::Get()->SetIsGameStop();


	//3Dオブジェクト//

	////空
	//CGameObject* pBackSkyCircle = new CGameObject();
	//pBackSkyCircle->SetCullingMode(false);
	//SetGameObjectInf(L"asset/CHARACTER/BackCircle.obj", false, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 900.0f, 900.0f, 900.0f }, 1.0f, CGameObject::TAG::BACK_SKY_CIRCLE, pBackSkyCircle, false);

	////テストバックファイア
	//CGameObject* pSun = new CGameObject();
	//SetGameObjectInf(L"asset/CHARACTER/Sun.obj", true, false, { 0.0f,60.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 10.0f, 10.0f, 10.0f }, 1.0f, CGameObject::TAG::SUN, pSun, false);


	//プレイヤー
	CPlayer* pPlayer = new CPlayer();
	pPlayer->SetAnimUvSize({ 10, 1 });
	pPlayer->SetRockUI(CGame::Get()->GetModel(L"asset/UI/RockPlayer.png"), CGame::Get()->GetModel(L"asset/UI/RockOnPlayer.png"));
	pPlayer->SetHitMisileEffect(CGame::Get()->GetModel(L"asset/Effect_01.png"));
	pPlayer->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/PlayerMiniMap.obj"));
	pPlayer->SetParticleModel(CGame::Get()->GetModel(L"asset/SpeedUpPart.png"));
	//pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_Jet.obj"));
	pPlayer->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/F-2Test.obj"));
	pPlayer->SetTag(CGameObject::TAG::PLAYER);
	pPlayer->SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
	//座標や大きさ設定
	pPlayer->Position = { 0.0f,0.0f,-30.0f };
	pPlayer->Rotation = { 0.0f,0.0f,0.0f };
	pPlayer->Scaling = { 1.5f, 1.5f, 1.5f };
	pPlayer->SetMoveSpeed(0.1f);
	IsDrawPlayer = true;
	lPlayer.push_back(pPlayer);

	//矢印
	CVector* pVector = new CVector();
	SetGameObjectInf(L"asset/CHARACTER/Vectol.obj", false, true, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.1f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::VECTOR, pVector, false);

	//地面
	CGameObject* pGround = new CGameObject();
	pGround->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapGround.obj"));
	pGround->SetIsMiniMapUpdate(true);
	SetGameObjectInf(L"asset/Wave.png", false, true, { 0.0f,(float)GROUND_YPOS,0.0f }, { 90.0f,0.0f,0.0f }, { 300.0f, 300.0f, 1.0f }, 1.0f, CGameObject::TAG::GROUND, pGround, false);

	////地面
	//CGameObject* pBill = new CGameObject();
	//SetGameObjectInf(L"asset/CHARACTER/Bill.obj", false, true, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 10.0f, 10.0f, 10.0f }, 1.0f, CGameObject::TAG::OBJECT, pBill, false);


	//初期敵
	CEnemy* pInitEnemy = new CEnemy();
	pInitEnemy->SetModel(CGame::Get()->GetModel(L"asset/CHARACTER/Fighter_EnemyJet.obj"));
	pInitEnemy->Init();
	pInitEnemy->Position = { 0.0f,0.0f,10.0f };
	pInitEnemy->Rotation = { 0.0f,0.0f,0.0f };
	pInitEnemy->Scaling = { 1.0f,1.0f,1.0f };
	pInitEnemy->SetInitRotateMatrix();
	pInitEnemy->SetTag(CGameObject::TAG::INITENEMY);
	//pInitEnemy->SetDrawTarget(false);
	pInitEnemy->SetCanAttack(false);
	pInitEnemy->SetActive(false);

	CPlayerAttack::Get()->ClearEnemys();

	//敵を格納する
	CPlayerAttack::Get()->InsertEnemys(pInitEnemy);

	//敵の手動での呼び出し//

	//機関銃敵
	//CGameObject* pGunEnemy = new CGameObject();
	//SetGameObjectInf(L"asset/CHARACTER/GunEnemy.obj", false, true, { 0.0f,0.0f,40.0f }, { 0.0f,0.0f,0.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f, CGameObject::TAG::GUN_ENEMY, pGunEnemy, false);


	//CGame::Get()->InsertEnemy(CGameObject::TAG::FOLLOW_ENEMY, { 0.0f,0.0f,10.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f });
	//CGame::Get()->InsertEnemy(8);
	//CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f });
	//CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f });
	//CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f });
	//CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f });
	//CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f });
	CGame::Get()->InsertEnemy(CGameObject::TAG::FRIGHT_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.5f,1.5f,1.5f });
	//CGame::Get()->InsertEnemy(CGameObject::TAG::BOSS_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f });

	//for (int i = 0; i < 5; i++)
	//{
	//	CGame::Get()->InsertEnemy(CGameObject::TAG::GUN_ENEMY, { 0.0f,0.0f,20.0f }, { 0.0f,0.0f,0.0f }, { 0.5f, 0.5f, 0.5f });
	//}

	//UI//

	//ミニマップUI
	CGameObject* pMiniMapUI = new CGameObject();
	SetGameObjectInf(L"asset/None.png", false, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 1.0f, CGameObject::TAG::MINIMAP_UI, pMiniMapUI, false);

	//画面中央UI
	CGameObject* pCenterUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Center_UI.png", true, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.13f,0.1f,0.1f }, 1.0f, CGameObject::TAG::CENTER_UI, pCenterUI, true);

	//スピードフレーム
	CGameObject* pSpeedFrame = new CGameObject();
	SetGameObjectInf(L"asset/UI/STAGE_FRAME.png", true, false, { -0.94f,0.25f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.3f, 0.25f, 0.1f }, 1.0f, CGameObject::TAG::UI, pSpeedFrame, true);

	//高度フレーム
	CGameObject* pAltFrame = new CGameObject();
	SetGameObjectInf(L"asset/UI/STAGE_FRAME.png", true, false, { 1.05f,0.25f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.3f, 0.25f, 0.1f }, 1.0f, CGameObject::TAG::UI, pAltFrame, true);

	//スピードUI
	CGameObject* pSpeed_UI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Stage_SPEED.png", true, false, { -0.95f,0.3f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.25f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::UI, pSpeed_UI, true);

	//高度UI
	CGameObject* pAlt_UI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Stage_ALT.png", true, false, { 1.05f,0.3f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.2f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::UI, pAlt_UI, true);

	//プレイヤーUI
	CGameObject* pFightJetUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/FightJet_UI_01.png", true, false, { 1.8f,-1.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.4f, 0.4f, 0.1f }, 1.0f, CGameObject::TAG::PLAYER_UI, pFightJetUI, true);

	//ミサイルUI
	SetGameObjectInfTest(true, false, { 1.53f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);
	SetGameObjectInfTest(true, false, { 2.07f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);
	SetGameObjectInfTest(true, false, { 1.36f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);
	SetGameObjectInfTest(true, false, { 2.24f,-1.0f,-0.2f }, { 0.0f,0.0f,0.0f }, { 0.17f,0.57f,0.1f }, 1.0f, CGameObject::TAG::MISILE_UI, true);

	//攻撃UI
	CGameObject* pXUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/X_UI.png", true, false, { -2.3f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.1f,0.1f,0.1f }, 1.0f, CGameObject::TAG::UI, pXUI, true);

	//攻撃UI2
	CGameObject* pAttackUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Attack_UI.png", true, false, { -2.14f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.2f,0.15f,0.1f }, 1.0f, CGameObject::TAG::UI, pAttackUI, true);

	//ターゲット変更UI
	CGameObject* pYUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Y_UI.png", true, false, { -1.9f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.1f,0.1f,0.1f }, 1.0f, CGameObject::TAG::UI, pYUI, true);

	//ターゲット変更UI2
	CGameObject* pChengeTargetUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/ChengeTarget_UI.png", true, false, { -1.6f,-1.25f,-0.4f }, { 0.0f,0.0f,0.0f }, { 0.4f,0.15f,0.1f }, 1.0f, CGameObject::TAG::UI, pChengeTargetUI, true);

	//現在の攻撃情報
	CGameObject* pAttackInf = new CGameObject();
	SetGameObjectInf(L"asset/UI/Stage_MSL.png", true, false, { 1.8f,-0.7f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.4f, 0.2f, 0.1f }, 1.0f, CGameObject::TAG::ATTACKINF_UI, pAttackInf, true);

	//ミサイルヒットUI
	CGameObject* pHitMisileUI = new CGameObject();
	pHitMisileUI->SetActive(false);
	SetGameObjectInf(L"asset/UI/MISILE_HIT_UI.png", true, false, { 0.0f,-0.3f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.8f, 0.2f, 1.0f }, 1.0f, CGameObject::TAG::HITMISILE_UI, pHitMisileUI, true);

	//警告UI
	CGameObject* pCaveatUI = new CGameObject();
	SetGameObjectInf(L"asset/UI/Warning.png", true, false, { 0.0f,0.1f,-0.1f }, { 0.0f,0.0f,0.0f }, { 1.3f, 0.3f, 0.1f }, 0.6f, CGameObject::TAG::CAVEAT_UI, pCaveatUI, true);

	//ボスがいるなら
	if (CPlayerAttack::Get()->GetBossEnemy() != nullptr)
	{
		//ボスの名前
		CGameObject* pBossNameUI = new CGameObject();
		SetGameObjectInf(L"asset/UI/BossUI.png", false, false, { -1.5f,1.2f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.8f,0.3f,0.1f }, 1.0f, CGameObject::TAG::BOSSNAME_UI, pBossNameUI, true);

		float InitXPos = -0.8f;

		//ボスのHP
		for (int i = 0; i < CPlayerAttack::Get()->GetBossEnemy()->GetHp(); i += 10)
		{
			CGameObject* pBossHPUI = new CGameObject();
			SetGameObjectInf(L"asset/UI/BossHpUI.png", false, false, { InitXPos,1.2f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.2f,0.2f,0.1f }, 1.0f, CGameObject::TAG::BOSSHP_UI, pBossHPUI, true);
		
			InitXPos += 0.2f;
		}

		//CGameObject* pBossHPUI = new CGameObject();
		//SetGameObjectInf(L"asset/UI/BossHpUI.png", false, true, { -0.8f,1.2f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.3f,0.2f,0.1f }, 1.0f, CGameObject::TAG::BOSSHP_UI, pBossHPUI, true);
	}

	//スピード表示をする数字群
	CGameObject* pSpeedNums[10];
	for (int i = 0; i < 10; i++)
		pSpeedNums[i] = new CGameObject();
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_0.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[0], 0);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_1.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[1], 1);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_2.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[2], 2);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_3.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[3], 3);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_4.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[4], 4);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_5.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[5], 5);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_6.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[6], 6);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_7.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[7], 7);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_8.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[8], 8);
	SetGameObjectSppedNumsInf(L"asset/NUM/Stage_9.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::SPEED_NUM, pSpeedNums[9], 9);

	//高度表示をする数字群
	CGameObject* pAltNums[10];
	for (int i = 0; i < 10; i++)
		pAltNums[i] = new CGameObject();
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_0.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[0], 0);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_1.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[1], 1);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_2.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[2], 2);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_3.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[3], 3);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_4.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[4], 4);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_5.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[5], 5);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_6.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[6], 6);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_7.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[7], 7);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_8.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[8], 8);
	SetGameObjectAltNumsInf(L"asset/NUM/Stage_9.png", true, false, { 0.0f,0.0f,-0.1f }, { 0.0f,0.0f,0.0f }, { 0.05f, 0.1f, 0.1f }, 1.0f, CGameObject::TAG::ALT_NUM, pAltNums[9], 9);

	CPlayerAttack::Get()->ClearMisileParticles();

	if (!IsCreateStageDebugWindoe)
	{
		// デバッグ表示関数登録
		DebugUI::RedistDebugFunction(DebugStage);
		IsCreateStageDebugWindoe = true;
	}

}

void CScene::SetGameClearScene()
{
	//背景パネル
	CGameObject* pBackSky = new CGameObject();
	SetGameObjectInf(L"asset/sky_01.png", false, false, { 0.0f,-5.0f,50.0f }, { 0.0f,0.0f,0.0f }, { 80.0f, 60.0f, 30.0f },1.0f, CGameObject::TAG::UI, pBackSky,false);

	//ゲームクリアパネル
	CGameObject* pEndPanel = new CGameObject();
	SetGameObjectInf(L"asset/UI/GameClear_01.png", true, false, { 0.0f,2.0f,10.0f }, { 0.0f,0.0f,0.0f }, { 12.0f, 3.0f, 1.0f }, 1.0f, CGameObject::TAG::UI, pEndPanel, false);

	//プレスAボタン
	CGameObject* pPressAButton = new CGameObject();
	SetGameObjectInf(L"asset/UI/PressAButton.png", true, false, { 0.0f,-2.0f,10.0f }, { 0.0f,0.0f,0.0f }, { 6.0f, 0.8f, 1.0f }, 1.0f, CGameObject::TAG::PRESS_UI, pPressAButton, false);

}

void CScene::SetGameOverScene()
{
	//背景パネル
	CGameObject* pBackSky = new CGameObject();
	SetGameObjectInf(L"asset/sky_01.png", false, false, { 0.0f,-5.0f,50.0f }, { 0.0f,0.0f,0.0f }, { 80.0f, 60.0f, 30.0f }, 1.0f, CGameObject::TAG::UI, pBackSky, false);

	//ゲームクリアパネル
	CGameObject* pEndPanel = new CGameObject();
	SetGameObjectInf(L"asset/UI/GameOver.png", true, false, { 0.0f,2.0f,10.0f }, { 0.0f,0.0f,0.0f }, { 12.0f, 3.0f, 1.0f }, 1.0f, CGameObject::TAG::UI, pEndPanel, false);

	//プレスAボタン
	CGameObject* pPressAButton = new CGameObject();
	SetGameObjectInf(L"asset/UI/PressAButton.png", true, false, { 0.0f,-2.0f,10.0f }, { 0.0f,0.0f,0.0f }, { 6.0f, 0.8f, 1.0f }, 1.0f, CGameObject::TAG::PRESS_UI, pPressAButton, false);

}

void CScene::SetCreateStageScene()
{
	//ターゲットの情報をクリア
	CPlayerAttack::Get()->ResetPlayerAttack();

	//エフェクトクリア
	CEffectManager::Get()->ClearEffect();

	//敵情報クリア
	CPlayerAttack::Get()->ClearEnemys();

	//プレイヤー配列をクリア
	lPlayer.clear();

	//地面
	CGameObject* pGround = new CGameObject();
	pGround->SetMiniMapModel(CGame::Get()->GetModel(L"asset/CHARACTER/MiniMapGround.obj"));
	pGround->SetIsMiniMapUpdate(true);
	pGround->SetColliderBoxAABB();
	SetGameObjectInf(L"asset/CHARACTER/Ground.obj", false, true, { 0.0f,(float)GROUND_YPOS,0.0f }, { 0.0f,0.0f,0.0f }, { 100.0f, 1.0f, 100.0f }, 1.0f, CGameObject::TAG::GROUND, pGround, false);

	if (!IsCreateCreateStageWindow)
	{
		// デバッグ表示関数登録
		DebugUI::RedistDebugFunction(CreateStage);
		IsCreateCreateStageWindow = true;
	}

}

void CScene::Draw3DCamera()
{
	if (Scene == STAGE || Scene == TEST_STAGE)
	{
		if (Get3DGameObject(CGameObject::TAG::BACK_SKY_CIRCLE) != nullptr)
		{
			Get3DGameObject(CGameObject::TAG::BACK_SKY_CIRCLE)->Draw();
		}

		if (Get3DGameObject(CGameObject::TAG::GROUND) != nullptr)
		{
			Get3DGameObject(CGameObject::TAG::GROUND)->Draw();
			if (IsDrawCollider)
			{
				Get3DGameObject(CGameObject::TAG::GROUND)->DrawColliderObject();
			}
		}

	}


	for (auto GameObject : lGameObject)
	{
		if (GameObject->GetTag() == CGameObject::TAG::GROUND || GameObject->GetTag() == CGameObject::TAG::BACK_SKY_CIRCLE)
		{
			continue;
		}

		GameObject->Draw();

		if (IsDrawCollider)
		{
			GameObject->DrawColliderObject();
		}
	}

	CPlayerAttack::Get()->EnemysDraw();

	if (IsDrawPlayer)
	{
		if (lPlayer.begin() != lPlayer.end())
		{
			(*lPlayer.begin())->Draw();
			(*lPlayer.begin())->DrawParticle();


			if (IsDrawCollider)
			{
				(*lPlayer.begin())->DrawColliderObject();
			}

			(*lPlayer.begin())->TargetLineUpdate();

		}
	}
}

void CScene::Draw2DCamera()
{
	//ミニマップ
	if (Scene == STAGE || Scene == TEST_STAGE)
	{
		//ミニマップに映すもの
		//Get3DGameObject(CGameObject::TAG::GROUND)->DrawMiniMapObject();
		for (auto GameObject : lGameObject)
		{
			GameObject->DrawMiniMapObject();
		}
		GetPlayer()->DrawMiniMapObject();

		CPlayerAttack::Get()->MiniMapEnemyDraw();
		CPlayerAttack::Get()->MiniMapMisileDraw();

		CEnemyAttack::Get()->MiniMapMisileDraw();
	}

	int MisieCnt = 0;
	int BossHpCnt = 0;

	//for (auto BossHpUI : lBossHPUI)
	//{
	//	BossHpUI->Draw();
	//	BossHpCnt += 10;

	//	if (BossHpCnt >= CPlayerAttack::Get()->GetBossEnemy()->GetHp())
	//	{
	//		break;
	//	}
	//}

	for (auto GameObject : lGameUI)
	{
		if (GameObject->GetTag() == CGameObject::TAG::CAVEAT_UI)
		{
			//GameObject->Draw();
			if (lPlayer.begin() != lPlayer.end())
			{
				if ((*lPlayer.begin())->GetIsDrawCaveat())
				{
					GameObject->Draw();
				}
			}
		}
		else if (GameObject->GetTag() == CGameObject::TAG::MISILE_UI)
		{
			switch (CPlayerAttack::Get()->GetAttackId())
			{
			case CPlayerAttack::ATTACK_ID::MISILE:

				if (MisieCnt <= 1)
				{
					GameObject->Draw();
				}
				else
					GameObject->materialDiffuse.w = 0.0f;

				MisieCnt++;
				break;
			case CPlayerAttack::ATTACK_ID::ONETARGET_FOURMISILE:
				GameObject->Draw();
				break;
			case CPlayerAttack::ATTACK_ID::FOURTARGET_FOURMISILE:
				GameObject->Draw();
				break;
			default:
				break;
			}
		}
		else if (GameObject->GetTag() == CGameObject::TAG::BOSSHP_UI)
		{
			if (CPlayerAttack::Get()->GetBossEnemy() != nullptr)
			{
				if (BossHpCnt < CPlayerAttack::Get()->GetBossEnemy()->GetHp())
				{
					GameObject->Draw();
					BossHpCnt += 10;
				}
				else
				{
					GameObject->SetActive(false);
				}
			}
		}
		else
		{
			GameObject->Draw();
		}

		//GameObject->Draw();
	}

	int MisileUICnt = 0;
	for (auto MisileUI : plMisileUI)
	{
		MisileUI->Draw();

		MisileUICnt++;

		if (CPlayerAttack::Get()->GetAttackId() == CPlayerAttack::ATTACK_ID::MISILE && MisileUICnt > 1)
		{
			break;
		}
	}

	//if (pPlayer != nullptr)
	//	pPlayer->DrawTargetUI();

	if (IsDrawPlayer)
	{
		(*lPlayer.begin())->DrawTargetUI();
		//(*lPlayer.begin())->DrawColliderObject();
	}
}

void CScene::Get3DObject(std::list<CGameObject*>& _3dobject)
{
	_3dobject.clear();

	for (auto Object : lGameObject)
	{
		_3dobject.push_back(Object);
	}
}
