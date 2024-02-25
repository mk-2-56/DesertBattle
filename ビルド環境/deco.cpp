//=============================================================================
//
// 装飾処理 [deco.cpp]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "deco.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(2)			// テクスチャの数
#define ALPHA_PLUS					(0.005f)		// 透過じゃなくなる度合い

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static DECO		g_Deco[DECO_ALL] = {
	{FALSE, 645, 334},
	{FALSE, 833, 555},
};

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/loog_failed.png",
	"data/TEXTURE/logo_clear.png",
};

static BOOL						g_Load = FALSE;
static float					g_alpha = 0.0f;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDeco(void)
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

	g_alpha = 0.0f;

	for (int i = 0; i < DECO_ALL; i++)
	{
		g_Deco[i].use = FALSE;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitDeco(void)
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
void UpdateDeco(void)
{
	for (int i = 0; i < DECO_ALL; i++)
	{
		if (g_Deco[i].use)
		{
			g_alpha += ALPHA_PLUS;

			if (g_alpha >= 1.0f)
			{
				g_alpha = 1.0f;

				if (GetKeyboardTrigger(DIK_RETURN))
				{// Enter押したら、ステージを切り替える
					SetFade(FADE_OUT, MODE_TITLE);
				}
				// ゲームパッドで入力処理
				else if (IsButtonTriggered(0, BUTTON_START))
				{
					SetFade(FADE_OUT, MODE_TITLE);
				}
				else if (IsButtonTriggered(0, BUTTON_B))
				{
					SetFade(FADE_OUT, MODE_TITLE);
				}
			}
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
void DrawDeco(void)
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

	for (int i = 0; i < DECO_ALL; i++)
	{
		if (g_Deco[i].use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, g_Deco[i].w, g_Deco[i].h, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, g_alpha));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// 情報の取得
//=============================================================================
DECO* GetDeco(void)
{
	return &g_Deco[0];
}
