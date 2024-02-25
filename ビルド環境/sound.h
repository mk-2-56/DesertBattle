//=============================================================================
//
// サウンド処理 [sound.h]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_TITLE,		// タイトル画面
	SOUND_LABEL_BGM_GAME,		// ゲーム画面
	SOUND_LABEL_BGM_TUTORIAL,	// チュートリアル画面
	SOUND_LABEL_SE_gameOver,	// ゲームオーバー
	SOUND_LABEL_SE_gameClear,	// ゲームクリア
	SOUND_LABEL_SE_slash01,		// 斬撃音
	SOUND_LABEL_SE_earthquake,	// フィールドチェンジ中
	SOUND_LABEL_SE_titleClick,	// スタート音
	SOUND_LABEL_SE_menuSelect,	// カーソル移動音

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

void SetVolumeSound(float volume);
void SetVolumeSound(int label, float volume);

void SetFrequencyRatio(float pitch);
void SetFrequencyRatio(int label, float pitch);