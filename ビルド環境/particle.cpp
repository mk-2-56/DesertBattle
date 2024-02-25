//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : GP11A132 15 高橋　ほの香
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "particle.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(2)			// テクスチャの数

#define	PARTICLE_SIZE_X		(40.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y		(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_PARTICLE	(5.0f)		// 移動速度

#define	MAX_PARTICLE		(500)		// パーティクル最大数

#define	DISP_SHADOW						// 影の表示
#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	int				nIdxShadow;		// 影ID
	int				life;			// 寿命
	BOOL			use;			// 使用しているかどうか
	int				type;			// パーティクルタイプ

} PARTICLE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static PARTICLE					g_Particle[MAX_PARTICLE];		// パーティクルワーク
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.png",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_Particle[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Particle[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Particle[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_Particle[nCntParticle].material, sizeof(g_Particle[nCntParticle].material));
		g_Particle[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Particle[nCntParticle].nIdxShadow = -1;
		g_Particle[nCntParticle].life = 0;
		g_Particle[nCntParticle].use = FALSE;
		g_Particle[nCntParticle].type = PARTICLE_SAND;
	}

	//g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	//for (int i = 0; i < 100; i++)
	//{
	//	PLAYER* player = GetPlayer();
	//	float theta = (float)((rand() % 628 - 314) * 0.01f);
	//	float phi = (float)((rand() % 628 - 314) * 0.01f);

	//	float radius = 200.0f;
	//	float x = player->pos.x + radius * sinf(phi) * cosf(theta);
	//	float y = player->pos.y + radius * cosf(phi);
	//	float z = player->pos.z + radius * sinf(phi) * sinf(theta);

	//	XMFLOAT3 pos = { x, y, z };

	//	XMVECTOR vTPos = XMLoadFloat3(&pos);
	//	XMVECTOR vPPos = XMLoadFloat3(&player->pos);

	//	XMVECTOR v = vTPos - vPPos;

	//	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	//	XMStoreFloat3(&move, v);

	//	SetParticle(pos, move, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 100, PARTICLE_EVAID, 0.1f);

	//}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	//PLAYER *pPlayer = GetPlayer();
	//g_posBase = pPlayer->pos;

	{
		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (g_Particle[i].use)
			{// 使用中

				if (g_Particle[i].type == PARTICLE_SAND)
				{

					g_Particle[i].pos.x += g_Particle[i].move.x / 2;
					g_Particle[i].pos.z += g_Particle[i].move.z / 2;

					g_Particle[i].pos.y += g_Particle[i].move.y / 4;
					//if (g_Particle[i].pos.y <= g_Particle[i].fSizeY / 2)
					//{// 着地した
					//	g_Particle[i].pos.y = g_Particle[i].fSizeY / 2;
					//	g_Particle[i].move.y = -g_Particle[i].move.y * 0.75f;
					//}

					g_Particle[i].move.x += (0.0f - g_Particle[i].move.x) * 0.05f;
					g_Particle[i].move.y -= 0.1f;
					g_Particle[i].move.z += (0.0f - g_Particle[i].move.z) * 0.05f;

#ifdef DISP_SHADOW
					if (g_Particle[i].nIdxShadow != -1)
					{// 影使用中
						float colA;

						// 影の位置設定
						SetPositionShadow(g_Particle[i].nIdxShadow, XMFLOAT3(g_Particle[i].pos.x, 0.1f, g_Particle[i].pos.z));

						// 影の色の設定
						colA = g_Particle[i].material.Diffuse.w;
						SetColorShadow(g_Particle[i].nIdxShadow, XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
					}
#endif

					g_Particle[i].life--;
					if (g_Particle[i].life <= 0)
					{
						g_Particle[i].use = FALSE;
						g_Particle[i].nIdxShadow = -1;
					}
					else
					{
						if (g_Particle[i].life <= 80)
						{
							g_Particle[i].material.Diffuse.x = 0.8f - (float)(80 - g_Particle[i].life) / 80 * 0.8f;
							g_Particle[i].material.Diffuse.y = 0.7f - (float)(80 - g_Particle[i].life) / 80 * 0.7f;
							g_Particle[i].material.Diffuse.z = 0.2f - (float)(80 - g_Particle[i].life) / 80 * 0.2f;
						}

						if (g_Particle[i].life <= 20)
						{
							// α値設定
							g_Particle[i].material.Diffuse.w -= 0.05f;
							if (g_Particle[i].material.Diffuse.w < 0.0f)
							{
								g_Particle[i].material.Diffuse.w = 0.0f;
							}
						}
					}
				}

				else if (g_Particle[i].type == PARTICLE_EVAID)
				{
					g_Particle[i].pos.x += g_Particle[i].move.x * 0.1f;
					g_Particle[i].pos.z += g_Particle[i].move.z * 0.1f;
					g_Particle[i].pos.y += g_Particle[i].move.y * 0.1f;

					g_Particle[i].life--;
					if (g_Particle[i].life <= 0)
					{
						g_Particle[i].use = FALSE;
					}
					else
					{
						if (g_Particle[i].life <= 20)
						{
							// α値設定
							g_Particle[i].material.Diffuse.w -= 0.05f;
							if (g_Particle[i].material.Diffuse.w < 0.0f) g_Particle[i].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	//SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_Particle[nCntParticle].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Particle[nCntParticle].scale.x, g_Particle[nCntParticle].scale.y, g_Particle[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Particle[nCntParticle].pos.x, g_Particle[nCntParticle].pos.y, g_Particle[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_Particle[nCntParticle].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	//SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	SetFogEnable(GetFogEnable());

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_Particle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, int life, int type, float scale)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_Particle[nCntParticle].use)
		{
			g_Particle[nCntParticle].pos = pos;
			g_Particle[nCntParticle].rot = { 0.0f, 0.0f, 0.0f };
			g_Particle[nCntParticle].scale = { scale, scale, scale };
			g_Particle[nCntParticle].move = move;
			g_Particle[nCntParticle].material.Diffuse = col;
			g_Particle[nCntParticle].life = life;
			g_Particle[nCntParticle].use = TRUE;
			g_Particle[nCntParticle].type= type;

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// 影の設定
			g_Particle[nCntParticle].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
			if (g_Particle[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_Particle[nCntParticle].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
