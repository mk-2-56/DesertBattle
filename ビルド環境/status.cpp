//=============================================================================
//
// �X�e�[�^�XUI���� [status.cpp]
// Author : GP11A132 15 �����@�ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "status.h"
#include "sprite.h"
#include "model.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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
static FILL						g_Fill;								// �Q�[�W�h��Ԃ��p

//=============================================================================
// ����������
//=============================================================================
HRESULT InitStatus(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
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
// �I������
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
// �X�V����
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

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStatus(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
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

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, g_Status[i].x, g_Status[i].y, g_Status[i].w, g_Status[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		SetFillEnable(false);
	}
}