//=============================================================================
//
// ステータスUI処理 [status.h]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum STATUS_LIST
{
	STATUS_HP = 0,
	STATUS_ENERGY,
	STATUS_PIC,
	
	STATUS_ALL,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct STATUS
{
	float			x, y;					// 位置
	float			w, h;					// サイズ
	int				texNo;					// テクスチャ
	float			percent;				// ゲージの割合
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStatus(void);
void UninitStatus(void);
void UpdateStatus(void);
void DrawStatus(void);



