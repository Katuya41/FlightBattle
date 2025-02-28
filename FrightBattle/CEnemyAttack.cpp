#include"CEnemyAttack.h"
#include <DirectXMath.h>
#include "CCamera.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CMisile.h"
#include "CShottoBall.h"
#include "CEffect.h"
#include "CScene.h"
#include "xa2.h"
#include "Input.h"

//ミサイルの初期速度
#define MISILE_SPEED (0.2)

//玉が消えるまでの時間
#define DeleteBallTime (5000)

//玉のスピード
#define GUN_SPEED (0.6f)

//プレイヤー警告用距離
#define MAX_PLAYER_MISILETARGET_DISTANCE (200)

CEnemyAttack::CEnemyAttack()
{
}

CEnemyAttack::~CEnemyAttack()
{
	Misiles.clear();

	for (auto ShotBall : vShottoBall)
	{
		delete ShotBall;
	}
	vShottoBall.clear();
}

void CEnemyAttack::MisileUpdate()
{
	IsDrawPlayerRock = false;
	IsClosePlayerDis = false;

	//ミサイルの更新処理
	for (auto i = Misiles.begin(); i != Misiles.end(); i++)
	{
		if ((*i)->GetMisileEnd())
		{
			Misiles.erase(i);
			break;
		}
		else
		{
			(*i)->Update();

			if ((*i)->GetActive() && (*i)->GetIsMoveTarget())
			{
				//距離が100以上の場合は黄色警告で、
				//距離が100以内かつミサイルがプレイヤーの方向を向いていた場合に赤色警告になる
				if ((*i)->CheckDistance(CGame::Get()->GetPlayer(), {}) <= ((float)MAX_PLAYER_MISILETARGET_DISTANCE))
				{
					IsDrawPlayerRock = true;
					IsClosePlayerDis = false;
					if ((*i)->GetTargetDistance() <= ((float)MAX_PLAYER_MISILETARGET_DISTANCE / 2.0f) &&
						(*i)->GetIsMoveTarget())
					{
						IsClosePlayerDis = true;
					}
				}
			}
		}
	}

	//プレイヤーロックモデルを表示
	if (IsDrawPlayerRock)
		CGame::Get()->GetPlayer()->SetIsDrawRockUI(true);
	else
		CGame::Get()->GetPlayer()->SetIsDrawRockUI(false);

	//ロックモデルの色を設定
	if(IsClosePlayerDis)
		CGame::Get()->GetPlayer()->SetIsRockOn(true);		//赤
	else
		CGame::Get()->GetPlayer()->SetIsRockOn(false);		//黄色

}

void CEnemyAttack::GunUpdate()
{
	for (auto i = vShottoBall.begin(); i != vShottoBall.end(); i++)
	{
		if (GetTickCount64() >= (*i)->ShotTime + DeleteBallTime || !(*i)->GetActive())
		{
			vShottoBall.erase(i);
			break;
		}
		else
		{
			(*i)->Update();
		}
	}
}

void CEnemyAttack::TargetUpdate()
{
	//敵のターゲットは、地面敵の場合、攻撃可能距離内にいるかのみ
}

void CEnemyAttack::MisileDraw()
{
	for (auto it = Misiles.begin(); it != Misiles.end(); it++)
	{
		(*it)->Draw();

		if (CScene::Get()->GetIsDrawCollider())
		{
			(*it)->DrawColliderObject();
		}
	}
}

void CEnemyAttack::MisileParticleDraw()
{
	for (auto it = Misiles.begin(); it != Misiles.end(); it++)
	{
		(*it)->DrawParticle();
	}

	//for (auto Misile : Misiles)
	//{
	//	Misile->DrawParticle();
	//}
}

void CEnemyAttack::GunDraw()
{
	for (auto ShotBall : vShottoBall)
	{
		ShotBall->Draw();

		if (CScene::Get()->GetIsDrawCollider())
		{
			ShotBall->DrawColliderObject();
		}
	}
}

CMisile* CEnemyAttack::AttackMisile(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation, CPlayer* _target, bool _moveenemy)
{
	std::unique_ptr<CMisile> res = std::make_unique<CMisile>();
	//CMisile* res = new CMisile();
	res->SetModel(MisileModel);							//モデル
	res->SetMiniMapModel(MiniMapMisileModel);			//ミニマップモデル
	res->SetParticleModel(MisileParticleModel);			//ミサイルのパーティクルのモデル
	res->Scaling = { 0.2f, 0.2f, 0.2f };				//大きさ設定
	res->SetTag(CGameObject::MISILE);					//タグの設定
	res->SetMisileState(CMisile::MISILE_STATE::READY);
	res->SetUv_Size(MisileHitUVSize);
	res->SetIsMoveEnemy(_moveenemy);
	res->SetSpornPos(_spornpos);

	res->SetSpornRotate(_rotation);						//角度設定
	//res->SetSpornRotate(_spornobj->Rotation);			//角度設定
	res->SetInitRotateMatrix();							//初期行列設定
	res->SetVector();									//現在のvector計算
	//res->SetMoveSpeed(0.0f);							//初期速度
	res->SetMoveSpeed((float)MISILE_SPEED);
	res->SetTarget(_target);							//ターゲット情報取得
	res->SetShotTime(GetTickCount64());					//発車時刻の設定
	res->SetMisileParticleTime(GetTickCount64());
	res->SetHitMisileEffect(MisileHitModel);			//ミサイルの当たった時のエフェクトテクスチャ

	Misiles.push_back(std::move(res));

	return res.get();
}

void CEnemyAttack::AttackShotBall(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation)
{
	CShottoBall* s = new CShottoBall();
	s->SetModel(GunModel);							//モデル
	s->Scaling = { 0.1f, 0.1f, 0.1f };				//大きさ設定
	s->SetMoveSpeed(GUN_SPEED);						//移動速度を設定する
	s->SetSpornPos(_spornpos);						//スポーンする座標
	s->SetSpornRotate(_rotation);					//角度設定
	s->SetRotateMatrix();							//初期行列設定
	s->SetVector();									//現在のvector計算
	//s->SetTarget(CGame::Get()->GetPlayer());		//ターゲット情報取得
	s->SetTag(CGameObject::GUN);					//タグの設定
	s->ShotTime = GetTickCount64();					//発車時刻の設定
	s->SetHitBallColBoxEffect(GunHitModel);			//玉の当たった時のエフェクトテクスチャ
	s->SetAnimUVSize(GunHitUVSize.x, GunHitUVSize.y);
	s->SetDiviatDir();

	vShottoBall.push_back(s);
}

CEnemyAttack* CEnemyAttack::Get()
{
	static CEnemyAttack instance;
	return &instance;
}

void CEnemyAttack::Update()
{
	//ミサイル更新処理
	MisileUpdate();

	//機関銃更新処理
	GunUpdate();
}

void CEnemyAttack::Draw()
{
	MisileDraw();

	GunDraw();

	MisileParticleDraw();
}

void CEnemyAttack::MiniMapMisileDraw()
{
	for (auto it = Misiles.begin(); it != Misiles.end(); it++)
	{
		(*it)->DrawMiniMapObject();
	}
}

CMisile* CEnemyAttack::AttackTarget(ATTACK_ID _attackid , DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _rotation)
{
	switch (_attackid)
	{
	case CEnemyAttack::MISILE:
		return AttackMisile(_spornpos, _rotation, CGame::Get()->GetPlayer(),  true);
		break;
	case CEnemyAttack::GUN:
		AttackShotBall(_spornpos, _rotation);
		break;
	case CEnemyAttack::MAX:
		break;
	default:
		break;
	}

	return nullptr;

}

void CEnemyAttack::SetMisileInf(D3D::Model _misilemodel, D3D::Model _misilhitemodel, D3D::Model _minimapmisilemodel, DirectX::XMFLOAT2 _misilehituvsize)
{
	MisileModel = _misilemodel;
	MisileHitModel = _misilhitemodel;
	MisileHitUVSize = _misilehituvsize;
	MiniMapMisileModel = _minimapmisilemodel;
}

void CEnemyAttack::SetGunInf(D3D::Model _gunmodel, D3D::Model _gunhitmodel, DirectX::XMFLOAT2 _ballhituvsize)
{
	GunModel = _gunmodel;
	GunHitModel = _gunhitmodel;
	GunHitUVSize = _ballhituvsize;
}

void CEnemyAttack::GetShotBalls(std::list<CShottoBall*>& _shotballs)
{
	_shotballs.clear();

	for (auto ShotBall : vShottoBall)
	{
		_shotballs.push_back(ShotBall);
	}
}
