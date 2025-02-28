#include "CShottoBall.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include "CCheckCollision.h"

using namespace DirectX;

CShottoBall::CShottoBall()
{
    pEffect = new CEffect();
    pEffect->SetBillboard(true);
    pEffect->SetLighting(false);
    pEffect->Scaling = { 3.0f,3.0f,3.0f };

    ColliderObject = new CGameObject();
    SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
    ColliderObject->Position = Position;
    ColliderObject->Rotation = Rotation;
    ColliderObject->Scaling = { 0.1f,0.1f,0.1f };

    //ミサイルの当たり判定作成
    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

CShottoBall::~CShottoBall()
{
    delete pEffect;
    delete Target;
}

void CShottoBall::Update()
{

    //敵の当たり判定取得
    if (Target != nullptr)
    {
        if (Target->GetTag() != INITENEMY)
        {
            EnemyCol = Target->GetColliderObjectOBB();

            if (CCheckCollision::Get()->CheckColBoxOBBOBB(EnemyCol, ColliderBoxOBB))
            {
                isActive = false;
                Target->SetDamege(2);
                CEffectManager::Get()->SetvEffect(pEffect, this->Position, 3, 0.1f);
            }
        }
    }


    CGameObject::SetVector();

    //敵に向かっていく
    //MoveEnemy();

    CGameObject::MoveUpdate();

    ColliderObject->Position = Position;
    ColliderObject->Rotation = Rotation;

    //ミサイルの当たり判定作成
    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

    EnemyCol = CGame::Get()->GetPlayer()->GetColliderObjectOBB();

    //ターゲットとの当たり判定をチェックする
    if (CCheckCollision::Get()->CheckColBoxOBBOBB(ColliderBoxOBB, EnemyCol))
    {
        isActive = false;
        CGame::Get()->GetPlayer()->SetDamege(1);
        CEffectManager::Get()->SetvEffect(pEffect, this->Position, 2, 0.2f);
    }
}

void CShottoBall::Draw()
{
    CGameObject::Draw();
}

void CShottoBall::SetDiviatDir()
{
    //機関銃設定
    //敵のスピードと位置と移動方向を取得
    //プレイヤーの位置とスピードを取得
    //敵のスピードと位置と移動方向から未来の敵座標を計算
    //それに弾のスピードを入れて計算した座標に弾を発射する
    //ここで計算するわけではなく、最初に計算をさせて飛ぶ方向と場所のみ受け取り、
    //それを元に弾を移動させるようにする

    ////ターゲットへの方向ベクトルを取得
    //XMFLOAT3 directionVector;
    //directionVector.x = Target->Position.x - Position.x;
    //directionVector.y = Target->Position.y - Position.y;
    //directionVector.z = Target->Position.z - Position.z;
    //// 正規化された方向ベクトルを取得
    //XMFLOAT3 normalizedDirection;
    //XMStoreFloat3(&normalizedDirection, XMVector3Normalize(XMLoadFloat3(&directionVector)));
    ////敵の移動スピードと移動方向を取得
    //float EnemySpeed = Target->GetMoveSpeed();
    //XMFLOAT3 EnemyPos = Target->Position;
    //XMFLOAT3 EnemyForward = Target->GetForward();
    //XMFLOAT3 EnemyVelocity = { EnemyForward.x * EnemySpeed,EnemyForward.y * EnemySpeed,EnemyForward.z * EnemySpeed };

    ////ターゲットの座標と移動方向を取得
    //XMVECTOR targetPosition = XMLoadFloat3(&Target->Position);
    //XMVECTOR targetVelocity = XMLoadFloat3(&EnemyVelocity);

    ////ターゲットの方向を計算
    //XMVECTOR toTarget = targetPosition - XMLoadFloat3(&Position);
    ////ターゲットまでの距離を計算
    //float distanceToTarget = XMVectorGetX(XMVector3Length(toTarget));

    //// 弾丸がターゲットに到達する時間を計算
    //float timeToTarget = distanceToTarget / ShotSpeed;

    //// 予測位置を計算
    //XMVECTOR predictedPosition = targetPosition + targetVelocity * timeToTarget;

    //// 射撃方向を計算
    //XMVECTOR shootDirection = XMVector3Normalize(predictedPosition - XMLoadFloat3(&Position));

    //////現在の玉の方向とターゲットの方向の角度を内積で計算
    //XMVECTOR res = newForward - shootDirection;
    //float angle = 0.0f;

    //float dotProduct = XMVectorGetX(XMVector3Dot(newForward, shootDirection));
    //angle = acosf(dotProduct);
    ////角度を度数で計算
    //float an = XMConvertToDegrees(angle);

    ////回転させる軸を計算
    //if (an != 0)
    //{
    //    //回転させる軸を計算
    //    XMVECTOR axis = XMVector3Cross(newForward, shootDirection);

    //    XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

    //    //回転行列作成
    //    XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

    //    //現在の回転行列にかけ合わせる
    //    RotateMatrix *= QMT;
    //}

    Rotation = ExtractRotationFromMatrix(RotateMatrix);

    //MoveSpeed = ShotSpeed;
}

XMFLOAT3 CShottoBall::SetDirect()
{
    // オブジェクト間の位置ベクトルを計算
    //方向はターゲット-プレイヤーの位置で求められる
    XMFLOAT3 directionVector;
    directionVector.x = Target->Position.x - Position.x;
    directionVector.y = Target->Position.y - Position.y;
    directionVector.z = Target->Position.z - Position.z;

    // 位置ベクトルを正規化
    vDirection = XMVector3Normalize(XMLoadFloat3(&directionVector));

    // 正規化された方向ベクトルを取得
    XMFLOAT3 normalizedDirection;
    XMStoreFloat3(&normalizedDirection, vDirection);

    return normalizedDirection;
}

void CShottoBall::SetModel(D3D::Model _model)
{
    model = _model;
}

void CShottoBall::SetRotateMatrix()
{
    RotateMatrix = GetIsMatrix();
}

void CShottoBall::SetHitBallColBoxEffect(D3D::Model _model)
{
    pEffect->SetModel(_model);
}

void CShottoBall::SetAnimUVSize(float _u, float _v)
{
    pEffect->SetAnimUVSize(_u, _v);
}
