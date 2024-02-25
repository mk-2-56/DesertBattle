//=============================================================================
//
// �t�F�[�h���� [fade.cpp]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "model.h"
#include "boss.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�

#define	FADE_RATE					(0.02f)			// �t�F�[�h�W��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

FADE							g_Fade = FADE_IN;			// �t�F�[�h�̏��
int								g_ModeNext;					// ���̃��[�h
XMFLOAT4						g_Color;					// �t�F�[�h�̃J���[�i���l�j
static BOOL						g_soundFade;
static int						g_soundNext;

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitFade(void)
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


	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_Fade  = FADE_IN;
	g_soundFade = FADE_NONE;
	g_Color = { 1.0, 0.0, 0.0, 1.0 };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFade(void)
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
void UpdateFade(void)
{

	if (g_Fade != FADE_NONE)
	{// �t�F�[�h������
		if (g_Fade == FADE_OUT)
		{// �t�F�[�h�A�E�g����
			g_Color.w += FADE_RATE;		// ���l�����Z���ĉ�ʂ������Ă���
			float volume = (float)(1.0 - g_Color.w);
			SetVolumeSound(volume);
			if (g_Color.w >= 1.0f)
			{
				BOSS* boss = GetBoss();

				// ���Ă���Ȃ�S���~�߂�
				StopSound();

				// �{�X��use��FALSE�ɂ��Ă���
				boss->use = FALSE;

				// �t�F�[�h�C�������ɐ؂�ւ�
				g_Color.w = 1.0f;
				SetFade(FADE_IN, g_ModeNext);

				// ���[�h��ݒ�
				SetMode(g_ModeNext);
			}

		}
		else if (g_Fade == FADE_IN)
		{// �t�F�[�h�C������
			g_Color.w -= FADE_RATE;		// ���l�����Z���ĉ�ʂ𕂂��オ�点��
			float volume = (float)(1.0 - g_Color.w);
			SetVolumeSound(volume);
			if (g_Color.w <= 0.0f)
			{
				// �t�F�[�h�����I��
				g_Color.w = 0.0f;
				SetFade(FADE_NONE, g_ModeNext);
			}

		}
	}

	if (g_soundFade != FADE_NONE)
	{// �t�F�[�h������
		if (g_soundFade == FADE_OUT)
		{// �t�F�[�h�A�E�g����
			g_Color.w += FADE_RATE;
			float volume = (float)(1.0 - g_Color.w);
			SetVolumeSound(volume);
			if (g_Color.w >= 1.0f)
			{
				// ���Ă���Ȃ�S���~�߂�
				StopSound();
				PlaySound(g_soundNext);

				// �t�F�[�h�C�������ɐ؂�ւ�
				g_Color.w = 1.0f;
				SetSoundFade(FADE_IN, g_soundNext);
			}

		}
		else if (g_soundFade == FADE_IN)
		{// �t�F�[�h�C������
			g_Color.w -= FADE_RATE;
			float volume = (float)(1.0 - g_Color.w);
			SetVolumeSound(volume);
			if (g_Color.w <= 0.0f)
			{   // �t�F�[�h�����I��
				g_Color.w = 0.0f;
				SetSoundFade(FADE_NONE, g_soundNext);
			}

		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	// PrintDebugProc("\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// �t�F�[�h���Ȃ��̂Ȃ�`�悵�Ȃ�

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


	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH/2, TEXTURE_WIDTH/2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
			g_Color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


}


//=============================================================================
// �t�F�[�h�̏�Ԑݒ�
//=============================================================================
void SetFade(FADE fade, int modeNext)
{
	g_Fade = fade;
	g_ModeNext = modeNext;
}

//=============================================================================
// �t�F�[�h�̏�Ԏ擾
//=============================================================================
FADE GetFade(void)
{
	return g_Fade;
}

//=============================================================================
// �T�E���h�̂݃t�F�[�h����
//=============================================================================
void SetSoundFade(FADE fade, int nextSong)
{
	g_soundFade = fade;
	g_soundNext = nextSong;
}