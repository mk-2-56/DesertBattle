//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "effect.h"
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	EFFECT_WIDTH		(70.0f)			// ���_�T�C�Y
#define	EFFECT_HEIGHT		(70.0f)			// ���_�T�C�Y

#define	MAX_EFFECT			(3)				// �ő吔
#define TEX_DEVX			(5)				// x�������̃e�N�X�`��������
#define TEX_DEVY			(2)			// y�������̃e�N�X�`��������
#define MAX_INTERVAL		(30.0f)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	BOOL		use;			// �g�p���Ă��邩�ǂ���
	float		animCnt;		// �A�j���[�V�����J�E���g

} EFFECT;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexEffect(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static VERTEX_3D* g_Vertex = NULL;
static EFFECT				g_Effect[MAX_EFFECT];						// �G�t�F�N�g
static BOOL					g_AlpaTest;									// �A���t�@�e�X�gON/OFF
static float				g_IntervalCnt;

static int					g_TexNo;									// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/effect.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
{
	g_Vertex = new VERTEX_3D[4];
	MakeVertexEffect();

	// �e�N�X�`������
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

	// �G�t�F�N�g�̏�����
	for(int i = 0; i < MAX_EFFECT; i++)
	{
		ZeroMemory(&g_Effect[i].material, sizeof(g_Effect[i].material));
		g_Effect[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Effect[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Effect[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Effect[i].fWidth = EFFECT_WIDTH;
		g_Effect[i].fHeight = EFFECT_HEIGHT;
		g_Effect[i].animCnt = 0.0f;
		g_Effect[i].use = FALSE;
	}

	g_AlpaTest = TRUE;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEffect(void)
{
	for(int i = 0; i < MAX_EFFECT; i++)
	{
		if(g_Effect[i].use)
		{
			float tw = 1.0f / TEX_DEVX;
			float th = 1.0f / TEX_DEVY;
			float tx = ((int)(g_Effect[i].animCnt) % TEX_DEVX) * tw;	// �e�N�X�`���̍���X���W
			float ty = ((int)(g_Effect[i].animCnt) / TEX_DEVX) * th;	// �e�N�X�`���̍���Y���W

			// ���_�����X�V
			g_Vertex[0].TexCoord = XMFLOAT2(tx, ty);
			g_Vertex[1].TexCoord = XMFLOAT2(tx + tw, ty);
			g_Vertex[2].TexCoord = XMFLOAT2(tx, ty + th);
			g_Vertex[3].TexCoord = XMFLOAT2(tx + tw, ty + th);

			// ���_�o�b�t�@�ɒl���Z�b�g����
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			// �S���_�����㏑��
			memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * 4);

			GetDeviceContext()->Unmap(g_VertexBuffer, 0);

			// �A�j���[�V������i�߂�
			g_Effect[i].animCnt += 0.3f;

			if ((int)(g_Effect[i].animCnt) > TEX_DEVX * TEX_DEVY)
			{	// �A�j���[�V�������I��������
				g_Effect[i].use = FALSE;
			}
		}
	}

#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	//if(GetKeyboardTrigger(DIK_F1))
	//{
	//	g_AlpaTest = g_AlpaTest ? FALSE: TRUE;
	//}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
{
	// ���e�X�g�ݒ�
	if (g_AlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_EFFECT; i++)
	{
		if(g_Effect[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Effect[i].scl.x, g_Effect[i].scl.y, g_Effect[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Effect[i].pos.x, g_Effect[i].pos.y, g_Effect[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Effect[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexEffect(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	float fWidth = EFFECT_WIDTH;
	float fHeight = EFFECT_HEIGHT;

	// ���_���W�̐ݒ�
	g_Vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	g_Vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	g_Vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	g_Vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	g_Vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	g_Vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	g_Vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	g_Vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	g_Vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	g_Vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	g_Vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * 4);
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �G�t�F�N�g�̃p�����[�^���Z�b�g
//=============================================================================
int SetEffect(XMFLOAT3 pos, XMFLOAT4 col)
{
	int nIdxTree = -1;
	float scl,randPos;

	for(int i = 0; i < MAX_EFFECT; i++)
	{
		scl = 0.7f + (rand() % 5 / 10);
		randPos = (float)(rand() % 10);

		if(!g_Effect[i].use)
		{
			g_Effect[i].pos = XMFLOAT3(pos.x + randPos, pos.y - 40.0f + randPos, pos.z);
			g_Effect[i].scl = XMFLOAT3(scl, scl, scl);
			g_Effect[i].animCnt = 0.0f;
			g_Effect[i].use = TRUE;

			nIdxTree = i;
		}
	}

	return nIdxTree;
}

