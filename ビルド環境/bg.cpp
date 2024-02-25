//=============================================================================
//
// 背景処理 [bg.cpp]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BG		"data/MODEL/bg/bg.obj"			// 読み込むモデル名
#define	BG_SPEED		(0.00015f)						// スクロールスピード

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
XMFLOAT3		g_pos;									// ポリゴンの位置
XMFLOAT3		g_rot;									// ポリゴンの向き(回転)
XMFLOAT3		g_scl;									// ポリゴンの大きさ(スケール)
DX11_MODEL		g_model;
BOOL			g_load;
BOOL			g_use;
float			g_spd;
XMFLOAT4X4		g_mtxWorld;								// ワールドマトリックス


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
{
	LoadModel(MODEL_BG, &g_model);
	g_load = true;
	 
	g_pos = { 0.0f, 50.0f, 0.0f };
	g_rot = { 0.0f, 0.0f, 0.0f };
	g_scl = { 110.0f, 110.0f, 110.0f };
	 
	g_spd  = 0.0f;			// 移動スピードクリア
	 
	g_use = true;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
{
	// モデルの解放処理
	if (g_load)
	{
		UnloadModel(&g_model);
		g_load = false;
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBG(void)
{
	PLAYER* player = GetPlayer();

	// 背景をスクロール
	{
		g_rot.y += BG_SPEED;

		if (g_rot.y > XM_PI)
		{
			g_rot.y -= XM_PI;
		}
	}

	// 中心をプレイヤーの位置に合わせる
	// g_pos = player->pos;



#ifdef _DEBUG
#endif


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_FRONT);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_scl.x, g_scl.y, g_scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rot.x, g_rot.y + XM_PI, g_rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_pos.x, g_pos.y, g_pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_model);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


