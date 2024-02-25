//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_TITLE,		// �^�C�g�����
	SOUND_LABEL_BGM_GAME,		// �Q�[�����
	SOUND_LABEL_BGM_TUTORIAL,	// �`���[�g���A�����
	SOUND_LABEL_SE_gameOver,	// �Q�[���I�[�o�[
	SOUND_LABEL_SE_gameClear,	// �Q�[���N���A
	SOUND_LABEL_SE_slash01,		// �a����
	SOUND_LABEL_SE_earthquake,	// �t�B�[���h�`�F���W��
	SOUND_LABEL_SE_titleClick,	// �X�^�[�g��
	SOUND_LABEL_SE_menuSelect,	// �J�[�\���ړ���

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
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