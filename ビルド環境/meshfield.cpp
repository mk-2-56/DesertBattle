//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"
#include "model.h"
#include "boss.h"
#include "player.h"
#include "camera.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(2)				// テクスチャの数

#define COORD(z, x) (z) * (g_nNumBlockXField + 1) + (x) //配列番号を求める

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11Buffer					*g_IndexBuffer = NULL;	// インデックスバッファ
// static WIPE							g_Wipe;				// ワイプ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号
static int							g_FieldType;			// 現在のフィールドタイプ

static XMFLOAT3		g_posField;								// ポリゴン表示位置の中心座標
static XMFLOAT3		g_rotField;								// ポリゴンの回転角

static int			g_nNumBlockXField, g_nNumBlockZField;	// ブロック数
static int			g_nNumVertexField;						// 総頂点数	
static int			g_nNumVertexIndexField;					// 総インデックス数
static int			g_nNumPolygonField;						// 総ポリゴン数
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// ブロックサイズ
static float		*g_dataPosY = NULL;

static char* g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/sand2.png",
	"data/TEXTURE/waterField.png",
};

static char* g_FieldAdr[] =
{
	"data/FIELDDATA/sandField.csv",
	"data/FIELDDATA/waterField.csv",
};

// 波の処理

static VERTEX_3D	*g_Vertex = NULL;

// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
static XMFLOAT3		g_Center;					// 波の発生場所
static float		g_Time = 0.0f;				// 波の経過時間
static float		g_wave_frequency  = 2.0f;	// 波の周波数
static float		g_wave_correction = 0.02f;	// 波の距離補正
static float		g_wave_amplitude  = 40.0f;	// 波の振幅

static BOOL			g_Load = FALSE;
static BOOL			g_FieldChange = FALSE;		// フィールドが変化中か

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// ポリゴン表示位置の中心座標を設定
	g_posField = pos;

	g_rotField = rot;

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
	
	g_FieldType = FIELD_SAND;	// 最初は砂漠

	// ブロック数の設定
	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;

	// 頂点数の設定
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;	
	
	// ポリゴン数の設定
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;

	// 次のフィールドのy座標保存用
	g_dataPosY = new float[g_nNumVertexField];

	// 頂点情報をメモリに作っておく（波の為）
	// 波の処理
	// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 波の発生場所
	g_Time = 0.1f;								// 波の経過時間(＋とーとで内側外側になる)
	g_wave_frequency = 1.0f;					// 波の周波数（上下運動の速さ）
	g_wave_correction = 0.02f;					// 波の距離補正（変えなくても良いと思う）
	g_wave_amplitude = 60.0f;					// 波の振幅(波の高さ)

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			float randHeight = (float)((rand() % 10 + 10) / 9);
			g_Vertex[COORD(z, x)].randHeight = randHeight;
			g_Vertex[COORD(z, x)].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[COORD(z, x)].Position.y = 0.0f;
			g_Vertex[COORD(z, x)].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			// 法線の設定
			g_Vertex[COORD(z, x)].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			g_Vertex[COORD(z, x)].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			float texSizeX = 1.0f / g_nNumBlockXField;
			float texSizeZ = 1.0f / g_nNumBlockZField;
			g_Vertex[COORD(z, x)].TexCoord.x = texSizeX * x;
			g_Vertex[COORD(z, x)].TexCoord.y = texSizeZ * z;
		}

	}


	// フィールドのリセット
	ResetField(g_FieldType);

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//頂点バッファの中身を埋める

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める

		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if(nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for(int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if(nCntVtxZ < (g_nNumBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}
		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if (g_Load == FALSE) return;

	// インデックスバッファの解放
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
	BOSS* boss = GetBoss();
	CAMERA* camera = GetCamera();

	// ボスの地中の動きに合わせる
	if (boss->underGround)
	{
		g_Center = boss->pos;
		for (int z = 0; z < g_nNumBlockZField; z++)
		{
			for (int x = 0; x < g_nNumBlockXField; x++)
			{
				float dx = g_Vertex[COORD(z, x)].Position.x - g_Center.x;
				float dz = g_Vertex[COORD(z, x)].Position.z - g_Center.z;

				// 波紋の中心点からの距離を得る
				float len = (float)sqrt(dx * dx + dz * dz);

				// 波の高さを、sin関数で得る
				//  波の高さ　= sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
				if (len < 100.0f)
				{
					g_Vertex[COORD(z, x)].Position.y = g_wave_amplitude * g_Vertex[COORD(z, x)].randHeight;
				}
			}
		}
	}

	// フィールドチェンジ中の動き
	if (g_FieldChange)
	{
		BOOL changeComp = TRUE;
		for (int z = 0; z < g_nNumBlockZField; z++)
		{
			for (int x = 0; x < g_nNumBlockXField; x++)
			{
				// 今の座標と次の座標の差分をだんだん減らす
				float diff = g_dataPosY[COORD(z, x)] - g_Vertex[COORD(z, x)].Position.y;
				
				g_Vertex[COORD(z, x)].Position.y += diff * 0.015f;
				
				// 差分が一定数以上あるなら
				if (fabsf(diff) > 1.0f)
				{
					changeComp = FALSE;
				}
				else
				{
					g_Vertex[COORD(z, x)].Position.y = g_dataPosY[COORD(z, x)];
				}
			}
		}

		// 全ての頂点の遷移が完了したら
		if (changeComp == TRUE)
		{
			g_FieldChange = FALSE;
			g_FieldType = boss->attribute;
			camera->len = LEN_CAM;
			camera->pos.y = POS_Y_CAM;
			boss->attribute = BOSSATTR_WATER;
			boss->state = BOSSSTATE_SWIM;
			boss->hp = MAX_BOSSHP;
			ResetBossAction();
		}
	}


	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// 全頂点情報を上書き
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField()
{
	// SetWipe(WIPE_MODE_TEXWIPE, XMFLOAT4(1.0f, 0.0f, -1.0f, 0.0f), g_wipeSize);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_FieldType]);			// マップ(1枚目)
	
	//// 最後のフィールドじゃなければ2枚目にマップをセットしておく(2枚目)
	//if(g_FieldType != FIELD_ALL - 1) GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[g_FieldType + 1]);

	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);


	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);

	// SetWipe(WIPE_MODE_NONE, XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f), 0.0f);
}


// フィールドとの衝突判定
BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;

	if (start.x == 0.0f)
	{
		start.x += 0.5f;
		end.x += 0.5f;
	}
	if (start.z == 0.0f)
	{
		start.z -= 0.5f;
		end.z -= 0.5f;
	}

	// 与えられた座標の少し上から、下へレイを飛ばす
	start.y += 100.0f;
	end.y -= 1000.0f;

	// 処理を高速化する為に全検索ではなく座標から必要なポリゴンを割り出す
	float fz = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;			// フィールド全体の半分の長さ
	float fx = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	int sz = (int)((-start.z+fz) / g_fBlockSizeZField);					// 今いる場所の頂点が配列の何番目か
	int sx = (int)(( start.x+fx) / g_fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;

	if ((sz < 0) || (sz > g_nNumBlockZField-1) ||
		(sx < 0) || (sx > g_nNumBlockXField-1))
	{	// フィールドの範囲外に居たら
		*Normal = {0.0f, 1.0f, 0.0f};
		return FALSE;
	}

	// 必要数分検索を繰り返す(判定したいオブジェクトの真下のポリゴンのみ=1個分)
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			// 真下の四角形ポリゴン用の頂点
			XMFLOAT3 p0 = g_Vertex[COORD(z, x)].Position;
			XMFLOAT3 p1 = g_Vertex[COORD(z, x+1)].Position;
			XMFLOAT3 p2 = g_Vertex[COORD(z+1, x)].Position;
			XMFLOAT3 p3 = g_Vertex[COORD(z+1, x+1)].Position;

			// 三角ポリゴンだから2枚分の当たり判定
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}


	return FALSE;
}

//=============================================================================
// フィールドの辺の長さを返す
//=============================================================================
float GetFieldSize(void)
{
	float fieldLength;

	fieldLength = g_nNumBlockXField * g_fBlockSizeXField;

	return fieldLength;
}

//=============================================================================
// フィールドが変化中か
//=============================================================================
BOOL GetFieldChange(void)
{
	return g_FieldChange;
}

//=============================================================================
// フィールドタイプの取得
//=============================================================================
int GetFieldType(void)
{
	return g_FieldType;
}

//=============================================================================
// フィールドをリセット
//=============================================================================
void ResetField(int type)
{
	char* fileName = g_FieldAdr[type];
	FILE* file;
	char line[256];
	float tx, ty, tz;
	int count = 0;

	if ((file = fopen(fileName, "r")) == NULL)
	{
		printf("ファイルエラー！");
	}
	else
	{
		// ファイルを読み込んで初期化する
		fgets(line, sizeof(line), file);

		while (fgets(line, sizeof(line), file) != NULL)
		{
			(void)sscanf(line, "%f,%f,%f", &tx, &ty, &tz);

			if (type == FIELD_SAND)
			{
				g_Vertex[count].Position.y = ty;
			}
			else
			{
				g_dataPosY[count] = ty;
			}
			count++;
		}

		// 終了
		fclose(file);
		printf("終了");
		if (type != FIELD_SAND)
		{
			PlaySound(SOUND_LABEL_SE_earthquake);
			g_FieldChange = TRUE;
		}
	}
}
