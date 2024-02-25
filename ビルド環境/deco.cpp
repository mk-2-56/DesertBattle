//=============================================================================
//
// �������� [deco.cpp]
// Author : GP11A132 15 �����@�ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "deco.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(2)			// �e�N�X�`���̐�
#define ALPHA_PLUS					(0.005f)		// ���߂���Ȃ��Ȃ�x����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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
// ����������
//=============================================================================
HRESULT InitDeco(void)
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

	g_alpha = 0.0f;

	for (int i = 0; i < DECO_ALL; i++)
	{
		g_Deco[i].use = FALSE;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
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
				{// Enter��������A�X�e�[�W��؂�ւ���
					SetFade(FADE_OUT, MODE_TITLE);
				}
				// �Q�[���p�b�h�œ��͏���
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

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDeco(void)
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

	for (int i = 0; i < DECO_ALL; i++)
	{
		if (g_Deco[i].use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, g_Deco[i].w, g_Deco[i].h, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, g_alpha));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}

//=============================================================================
// ���̎擾
//=============================================================================
DECO* GetDeco(void)
{
	return &g_Deco[0];
}
