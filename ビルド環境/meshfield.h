//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum FIELD_LIST
{
	FIELD_SAND = 0,
	FIELD_WATER,

	FIELD_ALL,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField();

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);
float GetFieldSize(void);
BOOL GetFieldChange(void);
int GetFieldType(void);
void ResetField(int type);