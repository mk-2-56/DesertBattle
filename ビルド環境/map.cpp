//=============================================================================
//
// マップ用処理 [map.cpp]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "map.h"
#include "model.h"
#include "player.h"
#include "boss.h"
#include "sprite.h"
#include "meshfield.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(5)								// テクスチャの数

#define	MAP_WIDTH			(160.0f)						// 地図情報
#define	MAP_HEIGHT			(160.0f)						// 地図情報
#define	MAP_X				(SCREEN_WIDTH - MAP_WIDTH - 20)		// 地図情報
#define	MAP_Y				(SCREEN_HEIGHT - MAP_HEIGHT - 20)	// 地図情報

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BOOL							g_Load = FALSE;
static MAP							g_Map[MAX_ICON] = {
	{0.0f, 0.0f, 0.0f, 15.0f, 20.0f, 3},
	{0.0f, 0.0f, 0.0f, 12.0f, 12.0f, 4},
};

static char* g_TexturName[] =
{
	"data/TEXTURE/sandMap.png",
	"data/TEXTURE/waterField.png",
	"data/TEXTURE/mapBg.png",
	"data/TEXTURE/mapIconPlayer.png",
	"data/TEXTURE/mapIconEnemy.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMap(void)
{
	ID3D11Device* pDevice = GetDevice();

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

	//// アイコンのセット
	//for (int i = 0; i < MAX_PLAYER; i++)
	//{
	//	SetMapIcon(ICON_PLAYER, i);			// プレイヤーのアイコンをセット
	//}
	//for (int i = 0; i < MAX_BOSS; i++)
	//{
	//	SetMapIcon(ICON_BOSS, i);				// エネミーのアイコンをセット
	//}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMap(void)
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
void UpdateMap(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < MAX_ICON; i++)
	{
		// アイコンの移動処理
		switch (i)
		{
		case ICON_PLAYER:
			if (player->use)
			{
				XMFLOAT2 posPlayer2D = ConvertToMapSpace(player->pos);
				g_Map[i].x = posPlayer2D.x;
				g_Map[i].y = posPlayer2D.y;
				g_Map[i].dir = player->rot.y;
			}
			break;

		case ICON_BOSS:
			if (boss->use)
			{
				XMFLOAT2 posBoss2D = ConvertToMapSpace(boss->pos);
				g_Map[i].x = posBoss2D.x;
				g_Map[i].y = posBoss2D.y;
				g_Map[i].dir = boss->rot.y;
			}
			break;
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMap(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

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

	// 地図背景の描画
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[FIELD_ALL]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, MAP_X - 4, MAP_Y - 4, MAP_WIDTH * 1.05f, MAP_HEIGHT * 1.05f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 地図の描画
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GetFieldType()]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, MAP_X, MAP_Y, MAP_WIDTH, MAP_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// プレイヤーのアイコンを描画
	if (player->use)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Map[ICON_PLAYER].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, g_Map[ICON_PLAYER].x, g_Map[ICON_PLAYER].y, g_Map[ICON_PLAYER].w, g_Map[ICON_PLAYER].h, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), g_Map[ICON_PLAYER].dir + XM_PI);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// ボスのアイコンを描画
	if (boss->use)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Map[ICON_BOSS].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, g_Map[ICON_BOSS].x, g_Map[ICON_BOSS].y, g_Map[ICON_BOSS].w, g_Map[ICON_BOSS].h, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), g_Map[ICON_PLAYER].dir + XM_PI);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}

//=============================================================================
// 3D座標をマップ座標に変換
//=============================================================================
XMFLOAT2 ConvertToMapSpace(XMFLOAT3 worldPos)
{
	float fieldLength = GetFieldSize();
	XMFLOAT2 mapRate, mapPos;

	mapRate.x = (worldPos.x / fieldLength) * MAP_WIDTH;
	mapRate.y = (-worldPos.z / fieldLength) * MAP_HEIGHT;

	mapPos.x = mapRate.x + MAP_X + (MAP_WIDTH / 2);
	mapPos.y = mapRate.y + MAP_Y + (MAP_HEIGHT / 2);

	return mapPos;
}
