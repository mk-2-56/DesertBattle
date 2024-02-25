//=============================================================================
//
// カメラ処理 [camera.h]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#pragma once


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(75.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-150.0f)		// カメラの初期位置(Z座標)
#define LEN_CAM				(150.0f)		// カメラの初期距離

struct CAMERA
{
	BOOL				reset;			// カメラリセット中かのフラグ
	XMFLOAT4X4			mtxView;		// ビューマトリックス
	XMFLOAT4X4			mtxInvView;		// ビューマトリックス
	XMFLOAT4X4			mtxProjection;	// プロジェクションマトリックス
	
	XMFLOAT3			pos;			// カメラの視点(位置)
	XMFLOAT3			oldPos;			// 1フレーム前の視点を保存
	XMFLOAT3			at;				// カメラの注視点
	XMFLOAT3			up;				// カメラの上方向ベクトル
	XMFLOAT3			rot;			// カメラの回転
	
	float				len;			// カメラの視点と注視点の距離

};


enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_NONE,

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA *GetCamera(void);

void SetViewPort(int type);
int GetViewPortType(void);

void SetCameraAT(XMFLOAT3 pos);