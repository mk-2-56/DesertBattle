//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GP11A132 15 �����@�ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "title.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// �e�N�X�`���̐�

#define MENU_FIRST					(303)
#define MENU_INTERVAL				(77)
#define START_WAIT					(8)				// ���ʉ��𕷂��I��邽�߂ɏ����҂�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bg_title.jpg",
	"data/TEXTURE/text_titleSet.png",
	"data/TEXTURE/cursor_titleMenu.png",
};

TITLE g_title[TITLETEX_ALL] =
{
	{TRUE, XMFLOAT3(0.0f, 0.0f, 0.0f), SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 0, 0, 0},	// use, pos, w, h, loop, opacity, scrlSpeed, texNo
	{TRUE, XMFLOAT3(0.0f, 0.0f, 0.0f), SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 0, 0, 1},
	{TRUE, XMFLOAT3(SCREEN_WIDTH / 2 - 156, 300.0f, 0.0f), 308, 52, 1.0f, 1.0f, 0, 0, 2},
};
static BOOL	g_Load = FALSE;
static int g_menu;
static int g_time = 0;
static BOOL g_click;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

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

	g_menu = 0;		// �ŏ��̓^�C�g��
	g_time = 0;
	g_click = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	// ���[�h�̐؂�ւ�
	{
		// ���ɐi��
		// �L�[�{�[�h
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_menu++;
			PlaySound(SOUND_LABEL_SE_menuSelect);
		}
		// �Q�[���p�b�h
		else if (IsButtonTriggered(0, BUTTON_DOWN))
		{
			g_menu++;
			PlaySound(SOUND_LABEL_SE_menuSelect);
		}

		// ��ɐi��
		// �L�[�{�[�h
		if (GetKeyboardTrigger(DIK_UP))
		{
			g_menu--;
			PlaySound(SOUND_LABEL_SE_menuSelect);
		}
		// �Q�[���p�b�h
		else if (IsButtonTriggered(0, BUTTON_UP))
		{
			g_menu--;
			PlaySound(SOUND_LABEL_SE_menuSelect);
		}
	}

	// ���̃��[�h��
	{
		// �L�[�{�[�h
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_click = TRUE;
			PlaySound(SOUND_LABEL_SE_titleClick);
		}

		// �Q�[���p�b�h�œ��͏���
		else if (IsButtonTriggered(0, BUTTON_START))
		{
			g_click = TRUE;
			PlaySound(SOUND_LABEL_SE_titleClick);
		}
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			g_click = TRUE;
			PlaySound(SOUND_LABEL_SE_titleClick);
		}

	}

	if (g_click)
	{
		g_time++;
		if (g_time > START_WAIT)
		{
			SetFade(FADE_OUT, (abs(g_menu) % TITLEMENU_ALL) + 1);		// ��ԍŏ��̃��[�h�̓^�C�g���Ȃ̂�+1����
			g_time = 0;
		}
	}

	g_title[TITLETEX_TEXTCURSOR].pos.y = (float)(MENU_FIRST + (MENU_INTERVAL * (abs(g_menu) % TITLEMENU_ALL)));




#ifdef _DEBUG	// �f�o�b�O����\������

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	for (int i = 0; i < TITLETEX_ALL; i++)
	{
		if (g_title[i].use == TRUE)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_title[i].texNo]);

			float sx = g_title[i].pos.x;
			float sy = g_title[i].pos.y;
			float sw = g_title[i].w;
			float sh = g_title[i].h;

			float tw = g_title[i].loop;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, sx, sy, sw, sh, 0.0f, 0.0f, tw, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}





