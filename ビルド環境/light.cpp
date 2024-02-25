//=============================================================================
//
// ライト処理 [light.cpp]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DIRLIGHT_INDEX 0	//扱うライトの番号


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = FALSE;
static XMFLOAT4X4 g_LightView;

//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Direction = XMFLOAT3(-1.0f, -1.0f, 1.0f);		// 光の向き
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	g_Light[0].Enable = TRUE;									// このライトをON
	SetLight(0, &g_Light[0]);									// これで設定している



	// フォグの初期化（霧の効果）
	//g_Fog.FogStart = 100.0f;									// 視点からこの距離離れるとフォグがかかり始める
	//g_Fog.FogEnd   = 250.0f;									// ここまで離れるとフォグの色で見えなくなる
	//g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// フォグの色
	//SetFog(&g_Fog);
	//SetFogEnable(g_FogEnable);				// 他の場所もチェックする shadow

}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{



}

//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL GetFogEnable(void)
{
	return(g_FogEnable);
}

//=============================================================================
// shadowMap用
//=============================================================================
// ライトからの視点を確保
void SetLightView(void)
{
	SHADOW shadow;
	shadow.Type = SHADOW_TYPE_SOFT;

	if (shadow.Type != 0)
	{
		const static XMFLOAT3 tempPos = { 0.0f ,0.0f ,0.0f };
		const static XMFLOAT3 Up = { 0.0f, 1.0f, 0.0f };
		const static XMVECTOR zeroPos = XMLoadFloat3(&tempPos);
		const static XMVECTOR WorldUp = XMLoadFloat3(&Up);
		XMVECTOR lightDir = XMLoadFloat3(&g_Light[DIRLIGHT_INDEX].Direction);
		XMVECTOR lightPosition = XMVectorSubtract(zeroPos, XMVectorScale(lightDir, 500.0f));

		XMMATRIX mtxView;
		mtxView = XMMatrixLookAtLH(lightPosition, zeroPos, WorldUp);

		XMMATRIX mtxProjection;
		mtxProjection = XMMatrixOrthographicOffCenterLH(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -2500.0f, 2500.0f);

		shadow.LightViewProj = XMMatrixMultiply(mtxView, mtxProjection);
		XMStoreFloat4x4(&g_LightView, mtxView);
	}

	SetShadow(&shadow);
}