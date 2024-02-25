//=============================================================================
//
// ステータスUI処理 [status.cpp]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "status.h"
#include "sprite.h"
#include "model.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(2)		// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static STATUS		g_Status[STATUS_ALL] = {
	{130.0f, 36.0f, 245.0f, 14.0f, 0, 100},
	{130.0f, 86.0f, 130.0f, 9.0f, 0, 100},
	{5.0f, 5.0f, 380.0f, 110.0f, 1, 100},
};

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bar.png",
	"data/TEXTURE/playerStatus.png",
};

static BOOL						g_Load = FALSE;
static FILL						g_Fill;								// ゲージ塗りつぶし用

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStatus(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStatus(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStatus(void)
{
	for (int i = 0; i < STATUS_ALL; i++)
	{
		switch (i)
		{
		case STATUS_HP:
			g_Status[i].percent = GetPlayerHP(0);
			break;

		case STATUS_ENERGY:
			g_Status[i].percent = GetPlayerEnergy(0);
			break;

		default:
			break;
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStatus(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < STATUS_ALL; i++)
	{
		if (i != STATUS_PIC)
		{
			SetFillEnable(true);
			g_Fill.FillColor = (i == STATUS_HP) ? XMFLOAT4(0.02f, 0.79f, 0.96f, 1.0f) : XMFLOAT4(1.0f, 0.556f, 0.035f, 1.0f);
			g_Fill.Size = g_Status[i].x + (g_Status[i].w * g_Status[i].percent);
			SetFillChange(&g_Fill);
		}

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Status[i].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, g_Status[i].x, g_Status[i].y, g_Status[i].w, g_Status[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		SetFillEnable(false);
	}
}