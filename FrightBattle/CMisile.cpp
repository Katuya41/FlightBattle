#include "CMisile.h"
#include "CGame.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CScene.h"
#include "CEffect.h"
#include "CEffectManager.h"
#include "CCheckCollision.h"
#include "CPlayerAttack.h"
#include "xa2.h"

#include <random>
#include <iostream>

using namespace std;

using namespace DirectX;

void CMisile::InsertParticle()
{
    //パーティクル追加
    PARTICLE Particle;
    Particle.Object = make_unique<CGameObject>();

    // 乱数生成器の初期化
    //ハードウェアの乱数生成器を使用する
    //メルセンヌ・ツイスタ法に基づく擬似乱数生成器
    std::random_device rd;
    std::mt19937 rng(rd());

    // 乱数の範囲を指定
    std::uniform_real_distribution<float> dist(0.0f, 0.4f);
    float Rnd = dist(rng) - 0.2f;

    Particle.Object->Position = { Position.x + GetRight().x * Rnd + GetUp().x * Rnd,
                                  Position.y + GetRight().y * Rnd + GetUp().y * Rnd,
                                  Position.z + GetRight().z * Rnd + GetUp().z * Rnd,};

    Particle.Object->Scaling = { 0.1f, 0.1f, 0.1f };
    Particle.Object->materialDiffuse.w = 1.0f;
    Particle.Object->SetActive(true);
    Particle.Object->SetLighting(false);
    Particle.Object->SetBillboard(true);
    Particle.Object->SetModel(ParticleModel);
    Particle.DeleteTime = CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time;
    Particle.StartTime = GetTickCount64();
    Particle.Object->SetTag(MISILE_SMOKE);
    Particles.push_back(move(Particle));

    //パーティクル追加
    PARTICLE Particle2;
    Particle2.Object = make_unique<CGameObject>();

    Rnd = dist(rng) - 0.2f;

    Particle2.Object->Position = { Position.x + GetRight().x * Rnd + GetUp().x * Rnd,
                              Position.y + GetRight().y * Rnd + GetUp().y * Rnd,
                              Position.z + GetRight().z * Rnd + GetUp().z * Rnd, };

    Particle2.Object->Scaling = { 0.1f, 0.1f, 0.1f };
    Particle2.Object->materialDiffuse.w = 1.0f;
    Particle2.Object->SetActive(true);
    Particle2.Object->SetLighting(false);
    Particle2.Object->SetBillboard(true);
    Particle2.Object->SetModel(ParticleModel);
    Particle2.DeleteTime = CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time;
    Particle2.StartTime = GetTickCount64();
    Particle2.Object->SetTag(MISILE_SMOKE);
    Particles.push_back(move(Particle2));
}

CMisile::CMisile()
{
    pEffect = new CEffect();
    pEffect->SetBillboard(true);
    pEffect->SetLighting(false);

    IsShotto = false;

    pEffect->Scaling = { 0.5f,0.5f,0.5f };

    ColliderObject = new CGameObject();
    SetColliderModel(CGame::Get()->GetModel(L"asset/CHARACTER/BoxCollider.obj"));
    ColliderObject->Position = Position;
    ColliderObject->Rotation = Rotation;
    ColliderObject->Scaling = Scaling;

    //ミサイルの当たり判定作成
    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);
}

CMisile::~CMisile()
{
    if (pEffect != nullptr)
        delete pEffect;

    //パーティクルの削除
    //スマートポインタの為解放処理は不要
    Particles.clear();
}

void CMisile::Update()
{
    //カメラに映っているか
    ChackRefCamera();

    //パーティクルの更新処理
    ParticleUpdate();

    if (Particles.size() == 0 && isActive == false)
    {
        Particles.clear();
        IsEnd = true;
    }

    if (GetTickCount64() >= ShotTime + CPlayerAttack::Get()->GetMisileParameter().Delete_Misile_Time)
    {
        isActive = false;
        Target->SetIsAttacked(false);
    }

    if (isActive == false)
        return;


    //パーティクル作成
    if (GetTickCount64() >= OldParticleTime + (ULONGLONG)CPlayerAttack::Get()->GetMisileParameter().Particle_Cooltime)
    {
        //パーティクルを追加
        InsertParticle();
        OldParticleTime = GetTickCount64();

        //CParticleManager::Get()->SetParticle(CParticleManager::MODEL_NAME::MISILE_PARTICLE, Position);
    }

    CGameObject::SetVector();

    //敵に向かっていく
    MoveUpdate();

    CGameObject::MoveUpdate();

    if (IsMoveEnemy)
    {
        //当たり判定
        //敵が初期敵じゃなければ
        if (Target->GetTag() != INITENEMY && isActive)
        {
            //敵の当たり判定取得
            if (Target->GetIsColAABB())
            {
                EnemyColAABB = Target->GetColliderObjectAABB();
            }
            else
            {
                EnemyColOBB = Target->GetColliderObjectOBB();
            }

            Target->SetIsAttacked(true);

            //敵がアクティブなら
            if (Target->GetActive())
            {
                //ターゲットとの距離を計算
                TargetDistance = CheckDistance(Target, {});

                ColliderObject->Position = Position;
                ColliderObject->Rotation = Rotation;
                ColliderObject->Scaling = Scaling;

                //ミサイルの当たり判定作成
                SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

                if (Target->GetIsColAABB())
                {
                    //ターゲットとの当たり判定をチェックする
                    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, EnemyColAABB))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        Target->SetIsAttacked(false);
                        Target->SetDamege(10);
                        CEffectManager::Get()->SetvEffect(pEffect, this->Position, 2, 0.1f);
                        MisileState = WAIT;

                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });

                        //UIを表示するか

                        if (IsDrawHitUI || Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY || 
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->MisileHitUIActive();

                        //コントローラーを振動させるか
                        if (Target->GetTag() == CGameObject::TAG::PLAYER)
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);
                        else if (Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY ||
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);

                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(),1.0f);
                    }
                }
                else
                {
                    ColliderObject->Position = Position;
                    ColliderObject->Rotation = Rotation;
                    ColliderObject->Scaling = Scaling;

                    //ミサイルの当たり判定作成
                    SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

                    //ターゲットとの当たり判定をチェックする
                    if (CCheckCollision::Get()->CheckColBoxOBBOBB(ColliderBoxOBB, EnemyColOBB))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        Target->SetIsAttacked(false);
                        Target->SetDamege(10);
                        CEffectManager::Get()->SetvEffect(pEffect, this->Position, 2, 0.1f);
                        MisileState = WAIT;

                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });

                        //UIを表示するか
                        if (IsDrawHitUI || Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY || 
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->MisileHitUIActive();

                        //コントローラーを振動させるか
                        if (Target->GetTag() == CGameObject::TAG::PLAYER)
                        {
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);
                            CGame::Get()->ShakeCamera(1);
                        }
                        else if (Target->GetTag() == FRIGHT_ENEMY || Target->GetTag() == GROUND_ENEMY || Target->GetTag() == FOLLOW_ENEMY ||
                            Target->GetTag() == ENTOURAGE_ENEMY || Target->GetTag() == BOSS_ENEMY)
                            CGame::Get()->GetPlayer()->SetBivTime(GetTickCount64() + 1000);
                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
                    }
                }

                //地面との当たり判定
                for (auto Collider : mColliderBoxsAABB[TAG::GROUND])
                {
                    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        Target->SetIsAttacked(false);
                        CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                        //ミサイルの当たり判定作成
                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
                    }
                }

                //建物との当たり判定
                for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
                {
                    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
                    {
                        isActive = false;
                        IsDrawMiniMapModel = false;
                        CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                        XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
                    }
                }

            }
        }
    }
    else
    {
        ColliderObject->Position = Position;
        ColliderObject->Rotation = Rotation;
        ColliderObject->Scaling = Scaling;

        //ミサイルの当たり判定作成
        SetColliderBoxOBB(ColliderObject->Position, ColliderObject->Scaling);

        if (CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND) != nullptr)
        {
            DirectX::BoundingBox GroundCol = CScene::Get()->Get3DGameObject(CGameObject::TAG::GROUND)->GetColliderObjectAABB();
            if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, GroundCol))
            {
                isActive = false;
                IsDrawMiniMapModel = false;
                Target->SetIsAttacked(false);
                CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                //ミサイルの当たり判定作成
                SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
            }
        }

        ////地面との当たり判定
        //for (auto Collider : mColliderBoxsAABB[TAG::GROUND])
        //{
        //    if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
        //    {
        //        isActive = false;
        //        IsDrawMiniMapModel = false;
        //        Target->SetIsAttacked(false);
        //        CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
        //        //ミサイルの当たり判定作成
        //        SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
        //        //XA_Play(SE_MISILEHIT);
        //    }
        //}

        //建物との当たり判定
        for (auto Collider : mColliderBoxsAABB[TAG::OBJECT])
        {
            if (CCheckCollision::Get()->CheckColBoxOBBAABB(ColliderBoxOBB, Collider))
            {
                isActive = false;
                IsDrawMiniMapModel = false;
                Target->SetIsAttacked(false);
                CEffectManager::Get()->SetvEffect(pEffect, Position, 2, 0.1f);
                SetColliderBoxOBB(ColliderObject->Position, { 0.0f,0.0f,0.0f });
                XA_Play(SE_MISILEHIT, CScene::Get()->GetSeVolume(), 1.0f);
            }
        }
    }

    //ミニマップのアップデート
    if (isActive)
        MiniMapObjectUpdate();

}

void CMisile::Draw()
{
    if (isActive)
        CGameObject::Draw();
}

void CMisile::ParticleUpdate()
{
    //パーティクルの中身がnullptrのオブジェクトを削除する
    //remove_ifによってObjectの中身がnullptrの物をeraseしている
    Particles.erase(std::remove_if(Particles.begin(), Particles.end(), [](const PARTICLE& p) {
        return p.Object == nullptr;
        }), Particles.end());

    //パーティクルを並び替える(カメラから位置が遠いものを先に描画)
    if (!Particles.empty())
    {
        std::sort(Particles.begin(), Particles.end(), [](const PARTICLE& a, const PARTICLE& b) {
            return a.Object->CheckDistance(nullptr, CGame::Get()->GetCameraPos()) > b.Object->CheckDistance(nullptr, CGame::Get()->GetCameraPos());
            });
    }

    //パーティクルのアップデート
    //徐々に大きくしていく、徐々に薄くしていく
    for (auto it = Particles.begin(); it != Particles.end(); ) // `it++` は最後に処理
    {
        //中身があるか
        if (it->Object)
        {
            //カメラに映っているか判定
            it->Object->ChackRefCamera();

            //徐々に薄くしている
            it->Object->materialDiffuse.w -= (float)CPlayerAttack::Get()->GetMisileParameter().Particle_OutTime;

            //一定の大きさまで大きくする
            if (it->Object->Scaling.x < 0.4f)
                it->Object->Scaling = { it->Object->Scaling.x + 0.001f, it->Object->Scaling.y + 0.001f ,it->Object->Scaling.z + 0.001f };

            //アルファ値が0になったら、削除
            if (it->Object->materialDiffuse.w <= 0.0f) {
                it = Particles.erase(it);       
                continue;
            }
        }

        ++it; // 消さなかった場合だけ進める 
    }
}

void CMisile::DrawParticle()
{
    for (auto it = Particles.begin(); it != Particles.end(); it++)
    {
        //カメラに映っている場合のみ描画
        if ((*it).Object->GetIsCameraRef())
            (*it).Object->Draw();
    }

    //ChengeShader(0);

}

void CMisile::MoveUpdate()
{
    //敵が初期敵じゃなければ
    if (IsMoveEnemy)
    {
        //方向ベクトルを取得
        fDirection = SetDirect();

        //現在のミサイルの方向とターゲットの方向の角度を内積で計算
        XMVECTOR res = newForward - vDirection;
        float angle = 0.0f;

        float dotProduct = XMVectorGetX(XMVector3Dot(newForward, vDirection));
        angle = acosf(dotProduct);
        //角度を度数で計算
        float an = XMConvertToDegrees(angle);
        //angle = XMConvertToRadians(angle);

        //MoveSpeed = MISILE_SPEED;

        if (an >= 60 && MoveSpeed >= CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            IsMoveEnemy = false;
        }

        //回転させる軸を計算
        if (an > 0.0f)
        {
            //回転させる軸を計算
            XMVECTOR axis = XMVector3Cross(newForward, vDirection);

            //撃ったオブジェクトがプレイヤー
            if (IsShotPlayer)
            {
                //スピードが初期規定速度以下の場合、回転角度は最大角度で設定
                if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().MaxRotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().MaxRotate);
                    }
                }
                else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().MinRotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().MinRotate);
                    }
                }
            }
            else
            {
                //スピードが初期規定速度以下の場合、回転角度は最大角度で設定
                if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Rotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Rotate);
                    }
                }
                else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
                {
                    if (an > (float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Rotate)
                    {
                        angle = XMConvertToRadians((float)CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Rotate);
                    }
                }
            }



            XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, angle);

            //回転行列作成
            XMMATRIX QMT = XMMatrixRotationQuaternion(rotationQuaternion);

            //現在の回転行列にかけ合わせる
            RotateMatrix *= QMT;
        }
    }

    Rotation = ExtractRotationFromMatrix(RotateMatrix);

    //撃ったオブジェクトがプレイヤー
    //加速度設定
    if (IsShotPlayer)
    {
        if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Min_Add_Speed;
            MoveSpeed += AddSpeed;
        }
        else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Max_Add_Speed;
            MoveSpeed += AddSpeed;
        }

        if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Max_Speed)
        {
            MoveSpeed = CPlayerAttack::Get()->GetMisileParameter().Max_Speed;
        }
    }
    else
    {
        if (MoveSpeed < CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Enemy_Min_Add_Speed;
            MoveSpeed += AddSpeed;
        }
        else if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Min_Speed)
        {
            AddSpeed = CPlayerAttack::Get()->GetMisileParameter().Enemy_Max_Add_Speed;
            MoveSpeed += AddSpeed;
        }

        if (MoveSpeed > CPlayerAttack::Get()->GetMisileParameter().Max_Speed)
        {
            MoveSpeed = CPlayerAttack::Get()->GetMisileParameter().Max_Speed;
        }
    }
}

XMFLOAT3 CMisile::SetDirect()
{
    // オブジェクト間の位置ベクトルを計算
    //方向はターゲット - 自分の位置で求められる
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

void CMisile::SetHitMisileEffect(D3D::Model _model)
{
    pEffect->SetModel(_model);
}

void CMisile::SetUv_Size(DirectX::XMFLOAT2 _uv)
{
    pEffect->SetAnimUVSize(_uv.x, _uv.y);
}
