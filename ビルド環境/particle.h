//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : GP11A132 15 �����@�ق̍�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 1.0f;			// ��̕�
static float					g_fHeightBase = 2.0f;			// ��̍���

enum PERTICLE_LIST
{
	PARTICLE_SAND = 0,
	PARTICLE_EVAID,

	PARTICLE_ALL,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, int nLife, int type, float scale);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

