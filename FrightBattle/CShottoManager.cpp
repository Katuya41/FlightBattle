#include "CShottoManager.h"
#include <DirectXMath.h>
#include "CShottoBall.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CCamera.h"

//玉が消えるまでの時間
#define DeleteBallTime (1000)

//玉のクールタイム
#define ShottoBallCoolTime (100)

//玉のスピード
#define GUN_SPEED (0.3f)

using namespace DirectX;

CShottoManager* CShottoManager::Get()
{
	static CShottoManager instance;
	return &instance;
}

void CShottoManager::Update()
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

void CShottoManager::Draw()
{
	for (auto i = vShottoBall.begin(); i != vShottoBall.end(); i++)
	{
		(*i)->Draw();
	}
}

void CShottoManager::CheckShotPos(DirectX::XMFLOAT3 _spornpos, CEnemy* _target)
{
    //ターゲットへの方向ベクトルを取得
    XMFLOAT3 directionVector;
    directionVector.x = _target->Position.x - _spornpos.x;
    directionVector.y = _target->Position.y - _spornpos.y;
    directionVector.z = _target->Position.z - _spornpos.z;
    // 正規化された方向ベクトルを取得
    XMFLOAT3 normalizedDirection;
    XMStoreFloat3(&normalizedDirection, XMVector3Normalize(XMLoadFloat3(&directionVector)));
    //敵の移動スピードと移動方向を取得
    float EnemySpeed = _target->GetMoveSpeed();
    XMFLOAT3 EnemyPos = _target->Position;
    XMFLOAT3 EnemyForward = _target->GetForward();
    XMFLOAT3 EnemyVelocity = { EnemyForward.x * EnemySpeed,EnemyForward.y * EnemySpeed,EnemyForward.z * EnemySpeed };
    XMFLOAT3 CameraPos = CGame::Get()->GetCamera3D()->GetCameraPos();

    //ターゲットの座標と移動方向を取得
    XMVECTOR targetPosition = XMLoadFloat3(&_target->Position);
    XMVECTOR targetVelocity = XMLoadFloat3(&EnemyVelocity);

    //ターゲットの方向を計算
    XMVECTOR toTarget = targetPosition - XMLoadFloat3(&_spornpos);
    //XMVECTOR toTarget = targetPosition - XMLoadFloat3(&CameraPos);
    //ターゲットまでの距離を計算
    float distanceToTarget = XMVectorGetX(XMVector3Length(toTarget));

    // 弾丸がターゲットに到達する時間を計算
    float timeToTarget = distanceToTarget / GUN_SPEED;

    // 予測位置を計算
    XMVECTOR predictedPosition = targetPosition + targetVelocity * timeToTarget;

    // 射撃方向を計算
    //XMVECTOR shootDirection = XMVector3Normalize(predictedPosition - XMLoadFloat3(&CameraPos));
    XMVECTOR shootDirection = XMVector3Normalize(predictedPosition - XMLoadFloat3(&_spornpos));

    XMStoreFloat3(&ShotPos ,predictedPosition);
}

void CShottoManager::SetBallInf(DirectX::XMFLOAT3 _spornpos, DirectX::XMFLOAT3 _spornrotate, CEnemy* _target)
{
	//玉を撃てるかチェックする
	if (BallShottoTime == 0 || GetTickCount64() >= BallShottoTime + (ULONGLONG)ShottoBallCoolTime)
	{
		CShottoBall* s = new CShottoBall();
		s->SetModel(model);						//モデル
		s->Scaling = { 0.1f, 0.1f, 0.1f };		//大きさ設定
		s->SetShotSpeed(GUN_SPEED);
		s->SetSpornPos(_spornpos);				//スポーンする座標
		s->SetSpornRotate(_spornrotate);		//角度設定
		s->SetRotateMatrix();					//初期行列設定
		s->SetVector();							//現在のvector計算
		s->SetTarget(_target);					//ターゲット情報取得
		s->SetTag(CGameObject::MISILE);			//タグの設定
		s->ShotTime = GetTickCount64();			//発車時刻の設定
		s->SetHitBallColBoxEffect(BallHitModel);		//玉の当たった時のエフェクトテクスチャ
		s->SetAnimUVSize(UVSize.x, UVSize.y);
		s->SetDiviatDir();

		vShottoBall.push_back(s);

		//玉を撃った時間をセットする
		BallShottoTime = GetTickCount64();
	}
}

