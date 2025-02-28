//=============================================================================
//
// サウンド処理 [xa2.h]
//
//=============================================================================
#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_

#include <xaudio2.h>

// サウンドファイル
typedef enum
{
	BGM_TITLE,				//タイトルBGM
	BGM_STAGE,				//ステージBGM
	BGM_RESULT,				//リザルト時のBGM
	SE_STARTGAME,			//タイトルでのボタンを押したSE
	SE_MOVETITLE,			//リザルトでボタンを押したSE
	SE_MISILESHOTTO,		//ミサイルを打った時のSE
	SE_MISILEHIT,			//ミサイルが当たった時のSE
	SE_LOCKON,				//ロックオンのSE
	SE_ENEMYROCK,			//敵にロックされている
	SE_BUZZER,				//プレイヤーロックオンブザー
	SE_COUNTDOWN,			//カウントダウンSE
	SE_COUNTDOWN2,			//カウントダウンSE
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

// ゲームループ開始前に呼び出すサウンドの初期化処理
HRESULT XA_Initialize(void);

// ゲームループ終了後に呼び出すサウンドの解放処理
void XA_Release(void);

// 引数で指定したサウンドを再生する
void XA_Play(SOUND_LABEL label , const float& _volume, const float& _pitch);

// 引数で指定したサウンドを停止する
void XA_Stop(SOUND_LABEL label);

// 引数で指定したサウンドの再生を再開する
void XA_Resume(SOUND_LABEL label);

#endif
