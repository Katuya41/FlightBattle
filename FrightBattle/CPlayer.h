#pragma once
#include "CGameObject.h"
#include "deque"
class CEnemy;
class CEffect;

/**
 * これはCPlayerクラスの説明です。
 * このクラスはプレイヤー制御で使用されます。
 *
 * @author 吉村括哉
 * @since 2024-7-10
 */
class CPlayer :
    public CGameObject
{
private:
    //もっているエフェクト情報
    CEffect* pEffect;
    CEffect* eJetEffect;

    //敵にターゲットされたときに使用するモデル
    std::unique_ptr<CGameObject> pEnemyRock;
    std::unique_ptr<CGameObject> pRockModel;
    std::unique_ptr<CGameObject> pRockOnModel;

    //警告判定用のオブジェクト
    std::unique_ptr<CGameObject> pCaveatObject;

    //シーンがステージ
    bool IsStageScene = false;

    //発射判定
    bool IsMisileShotto = false;

    //玉発射判定
    bool IsBallShotto = false;

    //カメラを動かしているか
    bool IsMoveCamera = false;

    //ターゲットUIを表示するか
    bool IsDrawRockUI = false;

    //ロックオンされているか
    //これは敵にロックされているかで変更される
    bool IsEnemyRock = false;

    //これはミサイルによって変更される
    //ミサイルの正面方向にプレイヤーがいる場合にtrueになる
    bool IsRockOn = false;

    //警告UIを描画するか
    bool IsDrawCaveat = false;

    //作成したターゲットUIのID
    int TargetUIId = 0;

    //回転させる角度
    DirectX::XMFLOAT3 Angle = { 0.0f,0.0f,0.0f };

    //ターゲットの座標格納用変数
    DirectX::XMFLOAT3 TargetPos = { 0,0,0 };

    //カメラ回転用変数
    DirectX::XMFLOAT3 CameraMoveRotate = { 0.0f,0.0f,0.0f };

    //振動の時間用
    ULONGLONG BivTime;

    //加速しているフラグ2
    bool OldFastSpeed = false;

    //加速している時間
    ULONGLONG OldSpeedUpTime = 0;
    ULONGLONG SpeedUpTime = 0;

    //警告SEを定期的に鳴らすのに使うタイマー
    ULONGLONG EnemyRockTime = 0;

    //敵をターゲットしたときの移動するUI
    struct MOVETARGET_UI
    {
        DirectX::XMFLOAT3 Position = {};    //UIの座標
        bool IsDraw = false;                //描画するか
        CEnemy* pTargetObject;              //ターゲットオブジェクト
    };
    std::list<MOVETARGET_UI> plMoveTargetUI;

    //パーティクルの構造体です
    struct PARTICLE
    {
        CGameObject* Object;        //パーティクルのオブジェクト
        float CoolTime;				//クールタイム
        ULONGLONG DeleteTime;		//消去時間
        ULONGLONG StartTime;		//開始時間
    };

    //パーティクルの生成間隔測定用変数
    LONGLONG OldInsertParticleTime;

    //パーティクルのモデル
    D3D::Model ParticleModel;

    //パーティクルのオブジェクトを格納する変数
    std::deque<PARTICLE> qParticles;

public:
    /**
    * 加速状態の列挙型です
    */
    enum SPEEDUP_MODE
    {
        LOW,
        NORMAL,
        HIGH,
    };

private:
    SPEEDUP_MODE SpeedUpMode = LOW;

private:

    /**
    * パラメータを入れる構造体です。
    */
    struct PARAMETER
    {
        float MaxInitRotate;        //平行角度への最大回転角度
        float Max_SlipZ;            //最大Z慣性
        float Max_SlipX;            //最大X慣性
        float Particle_DeleteTime;  //パーティクルの生存時間
        float Particle_CoolTime_Low;    //LOW状態のパーティクル出現頻度
        float Particle_CoolTime_Normal; //NORMAL状態のパーティクル出現頻度
        float Particle_CoolTime_High;   //HIGH状態のパーティクル出現頻度
        float Move_Add_Speed;       //移動加速度
        float NoMove_AddSpeed;      //加速してない状態の加減速
        float Right_Move_Speed;     //左右移動速度
        float Max_Right_Move_Speed; //左右最高移動速度
        float Max_MoveSpeed;        //最大速度
        float Min_MoveSpeed;        //最低速度
        float RotateX_StickSpeed;    //スティックでのX軸回転速度
        float RotateZ_StickSpeed;   //スティックでのZ軸回転速度
        float Yow_Rotate_Speed;     //ヨー回転速度
    };
    PARAMETER Parameter;

    enum PARAM_TAG
    {
        MAXINITROTATE,
        MAXSLIPZ,
        MAXSLIPX,
        PARTICLEDELETETIME,
        PARTICLECOOLTIMELOW,
        PARTICLECOOLTIMENORMAL,
        PARTICLECOOLTIMEHIGH,
        ADDSPEED,
        NOMOVEADDSPEED,
        RIGHTMOVESPEED,
        MAXRIGHTMOVESPEED,
        MAXSPEED,
        MINSPEED,
        ROTATESTICKSPEED,
        ROTATEZSTICKSPEED,
        YOWROTATESPEED,
        MAX,
    };
    PARAM_TAG ParamTag = MAXINITROTATE;

    /**
    * パラメータをファイルから読み込む
    */
    void InsertParameter();

    /**
    * プレイヤーのパーティクル更新処理
    */
    void PaeticleUpdate();

    /**
    * パーティクルを追加する関数です。
    */
    void InsertParticle();

    //入力判定
    void CheckInput(DirectX::XMFLOAT3& _angle);

    /**
    * 回転更新処理
    */
    void RotateUpdate();

    /**
    * プレイヤーのZ軸を地面と平行にする関数です。
    */
    void SetInitRotate();

    /**
    * コントローラーの振動更新処理を行う関数です。
    */
    void BivUpdate();

    /**
    * 無操作時の機体の姿勢制御
    */
    void SetNotInputRotate();

public:

    CPlayer();
    ~CPlayer();

    //格納関数//

    /**
    * ミサイルのヒットテクスチャを設定する関数です。
    */
    void SetHitMisileEffect(D3D::Model _model);

    /**
    * ターゲットされたときのUIを格納する関数です。
    */
    void SetRockUI(D3D::Model& _rockmodel, D3D::Model& _rockonmodel);

    /**
    * パーティクルのモデルを設定する関数です。
    *
    * @param 設定するパーティクルのモデル
    */
    void SetParticleModel(D3D::Model _model) { ParticleModel = _model; }

    void SetAnimUvSize(DirectX::XMFLOAT2 _uv);

    ////////

    /**
    * 更新処理を行う関数です。
    */
    void Update() override;

    /**
    * 移動処理を行う関数です。
    */
    void MoveUpdate() override;

    /**
    * パーティクルの描画処理を行う関数です。
    */
    void DrawParticle();

    /**
    * ターゲットされたときのUIを表示する関数です。
    */
    void DrawTargetUI();

    /**
    * カメラを動かしている(周囲を見渡している)かを取得する関数です。
    * 
    * @return カメラを動かしているか
    */
    bool GetIsMoveCamera() { return IsMoveCamera; }

    /**
    * コントローラーの振動強制終了する関数です。
    */
    void BivStop();

    /**
    * 振動時間を設定する関数です。
    * 
    * @param  振動終了時間
    */
    void SetBivTime(ULONGLONG _stoptime);

    /**
    * ターゲット状態設定
    */
    void SetIsDrawRockUI(bool _isdrawui) { IsDrawRockUI = _isdrawui; }

    /**
    * ターゲットロック状態設定
    */
    void SetIsRockOn(bool _isrock) { IsRockOn = _isrock; }

    /**
    * 敵にロックされているかの状態変更
    */
    void SetIsEnemyRock(bool _isenemyrock) { IsEnemyRock = _isenemyrock; }

    /**
    * 警告を描画するかを返す関数です。
    */
    bool GetIsDrawCaveat() { return IsDrawCaveat; }

    /**
    * シーンがタイトルかを設定する関数です。
    */
    void SetIsStageScene(bool _isstage) { IsStageScene = _isstage; }

    /**
    * 移動するターゲットUiを新しく作成する関数です。
    */
    void InsertMovetargetUI(CEnemy* _targetobject);

    /**
    * ターゲットUIを更新する関数です。
    */
    void TargetLineUpdate();

    /**
    * ターゲットUIを描画する関数です。
    */
    void DrawTargetLine();

    /**
    * 移動するターゲットUIのリストをクリアする関数です。
    */
    void ClearMoveTargetList();

    /**
    * 移動するターゲットの中の特定のオブジェクトを削除する関数です。
    */
    void DeleteMoveTargetList(CEnemy* _enemy);

    /**
    * 現在のスピード状態を取得する関数です。
    */
    SPEEDUP_MODE& GetSpeedUpMode() { return SpeedUpMode; }

    /**
    * 現在のパラメータを返す関数です
    * 
    * @return 現在のパラメータの構造体
    */
    PARAMETER& GetPlayerParameter() { return Parameter; }

    /**
    * パラメータをセーブする関数です。
    */
    void SaveParameter();

    //void CheckControllerTrigger();
};

