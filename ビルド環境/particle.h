//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#pragma once

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static XMFLOAT3					g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 1.0f;			// 基準の幅
static float					g_fHeightBase = 2.0f;			// 基準の高さ

enum PERTICLE_LIST
{
	PARTICLE_SAND = 0,
	PARTICLE_EVAID,

	PARTICLE_ALL,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, int nLife, int type, float scale);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

