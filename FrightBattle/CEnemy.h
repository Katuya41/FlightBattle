#pragma once
#include "CGameObject.h"
#include "CPlayerAttack.h"

class CMisile;
//class CPlayerAttack;

/**
 * これはCEnemyクラスの説明です。
 * このクラスは敵の制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CEnemy :
    public CGameObject
{

private:
    //共有変数

    //ジェットエフェクト
    CEffect* eJetEffect;

    //ターゲットされているときのミニマップのモデル
    D3D::Model TargetMiniMapModel;
    //ターゲットされていないミニマップモデルの避難用変数
    D3D::Model NottargetMiniMapModel;
    //ターゲットのオブジェクトの座標
    DirectX::XMFLOAT3 TargetPosition = { 0,0,0 };
    //ターゲットの方向ベクトル
    DirectX::XMVECTOR TargetDir;
    //攻撃できるか
    bool CanAttack = true;
    //ターゲットかどうか
    bool IsTarget = false;
    //プレイヤーをターゲットできているか
    bool IsAttackTarget = false;
    //乱数保持用変数
    int OldRndNum = NULL;
    //プレイヤーとの距離
    double PlayerDistance;
    //プレイヤーの正面ベクトルとの角度
    float ToPlayerRadius = 0.0f;
    //地点移動敵の移動ポイントを入れる変数
    DirectX::XMFLOAT3 MovePos[8];

    //取り巻き敵用変数//

    //攻撃指示が終わったか
    bool EndChaseAndAttack = false;
    //最初に位置調整を行ったか
    bool IsInitChengeBoidPos = false;
    //ボスを守りに向かったか
    bool IsDefenceMove = false;
    //取り巻きのついていく基準の座標
    DirectX::XMFLOAT3 FollowPos = { 0.0f,0.0f,0.0f };
    //ボス取り巻き用座標変数
    DirectX::XMFLOAT3 EntouragePos = { 0.0f,0.0f,0.0f };

    //ボス用雑魚敵召喚フラグ
    bool InsertEntourage = false;
    //取り巻き攻撃フラグ
    bool IsAttackEntourage = false;
    //デバッグ用攻撃フラグ
    bool DebugIsAttack = true;
    //ボス用変数
    //徘徊状態の取り巻きの移動座標を入れる配列
    DirectX::XMFLOAT3 EntourageMovePos[8];

    //機関銃敵//
    //この敵の具体的な行動
    //配列に下記の構造体を入れ、最初から順番に設定していく

    /**
    * 移動先の座標と通過したかの構造体
    */
    struct GUNENEMY_MOVEFLAG
    {
        DirectX::XMFLOAT3 MovePos = {};  //移動先の座標
        bool EndRotate = false;          //移動先への回転終了
        bool EndForwardRotate = false;   //平行回転の終了
        bool IsPassPos = false;          //座標を通過したか
        bool IsInsert = false;           //この座標を設定したか
        ULONGLONG PassTime = 0;          //移動先に到達した時間
        ULONGLONG WaitTime = 5000;       //移動後の停止時間
    };
    std::deque<GUNENEMY_MOVEFLAG> qMoveArray;
    std::vector<DirectX::XMFLOAT3> qResArray;
    //現在の構造体
    GUNENEMY_MOVEFLAG NowFlag;

    //ミサイル用変数//

    //ミサイル発射距離
    ULONGLONG ShotTime = 0;
    //ターゲット開始時間
    ULONGLONG TargetStartTime = 0;
    //発射したミサイルを入れる変数
    CMisile* pMisile;
    //時間計測用変数
    ULONGLONG Times = 0;

    // 機関銃用変数 //
    ULONGLONG GunShotTime = 0;

public:
    /**
    * 空を飛ぶ敵の状態を示す列挙型です。
    */
    enum ENEMY_STATE
    {
        MOVE,           //指定ポイントを動き回る
        MOVE_FORWARD,   //まっすぐ飛び続ける
        ISTARGET,       //プレイヤーにターゲットされている
        CHASE,          //プレイヤーを追いかける
        ATTACK,         //プレイヤーを攻撃する
        RUN,            //プレイヤーが攻撃してきたときに逃げる
        ENTOURAGE_FOLLOW,   //取り巻きボス追従
        ENTOURAGE_FRIGHT,   //取り巻きの徘徊
        ENTOURAGE_DEFENCE,  //取り巻きボス防御
        ENTOURAGE_ATTACK,   //取り巻きプレイヤー攻撃
        GUN_ENEMT,          //機関銃敵
    };

    CPlayerAttack::GUNENEMYSTARTPOS GunENemyStartPos = CPlayerAttack::GUNENEMYSTARTPOS::NoneVerTex;

    //優先度
    int Fastpoint = 0;

    //ターゲットされるまでの時間変数
    LONGLONG TargetTime = 0;
    //ターゲットにされた時間
    LONGLONG OldTargetTime = 0;

private:
    //状態
    ENEMY_STATE EnemyState = MOVE;

    /**
    * 乱数生成
    * 
    * @param   最低範囲
    * @param   最大範囲
    * @param   取得した乱数
    */
    void GetRandomNum(float _maxnum , float _minnum ,float& _getnum);

    /**
    * カメラに映っているかを確かめる関数です。
    */
    void CheckRefCamera();

    /**
    * メインカメラの中心付近に映っているかを確かめる関数です。
    */
    void CheckRefCameraCenter();

    /**
    * 空を飛ぶ敵のMOVE更新関数です
    */
    void StateMoveUpdate(float _maxrotate);

    /**
    * 空を飛ぶ敵のMOVE_FORWARD更新関数です。
    */
    void StateMoveForwardUpdate(float _maxrotate);

    /**
    * 空を飛ぶ敵のISTARGET更新関数です
    */
    void StateIsTargetUpdate();

    /**
    * 空を飛ぶ敵のATTACK更新関数です
    */
    bool StateAttackUpdate();

    /**
    * 空を飛ぶ敵のRUN更新関数です
    */
    void StateRunUpdate();

    /**
    * 取り巻き敵のENTOURAGE_FOLLOW関数です。
    */
    void StateFollowEntourageUpdate(float _maxspeed);

    /**
    * 取り巻き敵のENTOURAGE_DEFENCE関数です。
    */
    void StateDefenceEntourageUpdate();

    /**
    * 取り巻き敵のENTOURAGE_ATTACK関数です。
    */
    void StateAttackEntourageUpdate();

public:
    /**
    *CEnemyのコンストラクタです。
    */
    CEnemy();

    /**
    *CEnemyのデストラクタです。
    */
    ~CEnemy();

    /**
    *敵情報の初期化を行う関数です。
    */
    void Init();

    /**
    *更新処理を行う関数です。
    */
    void Update() override;

    /**
    * 空を飛んでいる敵の更新処理
    */
    void UpdateFrightEnemy();

    /**
    * 地上の敵の更新処理
    */
    void UpdateGroundEnemy();

    /**
    * プレイヤー追従の敵の更新処理
    */
    void UpdateFollowEnemy();

    /**
    * ボスの取り巻き敵の更新処理
    */
    void UpdateEntourageEnemy();

    /**
    * ボスの更新処理
    */
    void UpdateBossEnemy();

    /**
    * 機関銃敵の更新処理
    */
    void UpdateGunEnemy();

    /**
    *描画処理を行う関数です。
    */
    void Draw() override;

    /**
    *ターゲットかどうかを設定する関数です。
    * 
    * @param ターゲットかどうか
    */
    void SetIsTarget(bool _istarget) { IsTarget = _istarget; }

    /**
    *攻撃できるかを設定する関数です。
    *
    * @param ターゲットにできるか
    */
    void SetCanAttack(bool _cantarget) { CanAttack = _cantarget; }

    /**
    *ターゲットに攻撃できるかを取得する関数です。
    *
    * @return ターゲットに攻撃できるか
    */
    bool GetCanAttack() { return CanAttack; }

    /**
    * 敵がターゲットしているかを返す関数です。
    */
    bool GetIsAttackTarget() { return IsAttackTarget; }

    /**
    *ターゲットされているか返すを取得する関数です。
    *
    * @return ターゲットされているか
    */
    bool GetIsTarget() { return IsTarget; }

    /**
    *ターゲットの座標を設定する関数です。
    *
    * @return ターゲットの座標
    */
    void TargetPos(DirectX::XMFLOAT3 _targetpos) { TargetPosition = _targetpos; }

    /**
    *移動先の座標を設定する関数です。
    *
    * @param  移動ポイント
    * @param  番号
    */
    void SetMovePos(const DirectX::XMFLOAT3& _movepos, const int& _num) { MovePos[_num] = _movepos; }

    /**
    * 取り巻きの移動先の座標を設定する関数です。
    *
    * @param  移動ポイント
    * @param  番号
    */
    void SetEntourageMovePos(const DirectX::XMFLOAT3& _movepos, const int& _num) { EntourageMovePos[_num] = _movepos; }

    /**
    * 取り巻きの移動先座標の配列を取得する関数です。
    * 
    * @param  取得する座標のID
    */
    DirectX::XMFLOAT3& GetEntourageMovePos(int _id) { return EntourageMovePos[_id]; }

    /**
    *移動先の方向を計算する関数です。
    *
    * @param 移動先の座標
    */
    void SetDirPos(DirectX::XMFLOAT3 _targetpos);

    /**
    *移動先の座標に到達したかを確認する関数です。
    *
    * @param  _targetpos  移動先の座標
    * @param  _allowdis   許容する距離
    * @return 移動先の座標に到達したか
    */
    bool CheckMovePos(DirectX::XMFLOAT3 _targetpos,float _allowdis);

    /**
    *プレイヤーとの距離を返す関数です。
    *
    * @return プレイヤーとの距離
    */
    double GetPlayerDistance() { return PlayerDistance; }

    /**
    * ターゲットされているときのミニマップのモデルを設定する関数です。
    * 
    * @param  ターゲットされているときのミニマップのモデル
    */
    void SetTargetMiniMapModel(D3D::Model _model) { TargetMiniMapModel = _model; }

    /**
    * ミニマップ用オブジェクトのモデルを変更する関数です。
    * 
    * @param  ターゲットされているか
    */
    void SetTargetMiniMapObjectModel(bool _istarget);

    /**
    * ミサイルの発射時間を設定する
    */
    void SetMisileShotTime(ULONGLONG _time) { ShotTime = _time; }

    /**
    * 新しい移動先を取得
    */
    void SetNewMovePosition();

    /**
    * 現在の状態を取得する関数です。
    */
    ENEMY_STATE& GetEnemyState() { return EnemyState; }

    /**
    * 現在の状態を設定する関数です。
    */
    void SetEnemyState(ENEMY_STATE _newstate);

    /**
    * 取り巻き敵の位置を調整させる関数です。
    */
    void SetNewEntouragePos() { IsInitChengeBoidPos = false; }

    /**
    * ターゲットタイムを返す
    */
    ULONGLONG& GetTargetTime() { return TargetStartTime; }

    /**
    * 時間用変数に時間を設定する
    */
    void SetTimes(ULONGLONG _time) { Times = _time; }

    /**
    * 攻撃を行う敵に攻撃支持を設定
    */
    void SetIsEndChaseAttack() { EndChaseAndAttack = false; }

    /**
    * 攻撃支持がでているか返す
    */
    bool GetIsEndChaseAttack() { return EndChaseAndAttack; }

    /**
    * プレイヤーの正面ベクトルとプレイヤーから敵へのベクトルの角度を返す関数です。
    */
    float GetToPlayerRadius();

    /**
    * 取り巻きよう関数
    * 
    * 移動予定座標を変更する関数です。
    */
    void ChengeFollowPos(DirectX::XMFLOAT3& _addpos);

    /**
    * 機関銃敵の移動が終了しているかを返す関数
    */
    bool GetGunEnemyIsPass() { return NowFlag.EndForwardRotate; }

    /**
    * 機関銃敵の新しい座標設定
    * 
    * @param  _startpos 開始頂点番号
    */
    void SetGunEnemyNewPos(CPlayerAttack::GUNENEMYSTARTPOS _startpos);

    /**
    * デバッグ用攻撃フラグ取得関数
    */
    bool& GetDebugIsAttack() { return DebugIsAttack; }

    ///**
    //* 最高速度を返す関数です。
    //*/
    //float& GetMaxSpeed() { return MaxSpeed; }

    ///**
    //* 加速度を返す関数です。
    //*/
    //float& GetAddSpeed() { return AddSpeed; }
};