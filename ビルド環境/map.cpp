//=============================================================================
//
// �}�b�v�p���� [map.cpp]
// Author : GP11A132 15 �����@�ق̍�
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(5)								// �e�N�X�`���̐�

#define	MAP_WIDTH			(160.0f)						// �n�}���
#define	MAP_HEIGHT			(160.0f)						// �n�}���
#define	MAP_X				(SCREEN_WIDTH - MAP_WIDTH - 20)		// �n�}���
#define	MAP_Y				(SCREEN_HEIGHT - MAP_HEIGHT - 20)	// �n�}���

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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
// ����������
//=============================================================================
HRESULT InitMap(void)
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

	//// �A�C�R���̃Z�b�g
	//for (int i = 0; i < MAX_PLAYER; i++)
	//{
	//	SetMapIcon(ICON_PLAYER, i);			// �v���C���[�̃A�C�R�����Z�b�g
	//}
	//for (int i = 0; i < MAX_BOSS; i++)
	//{
	//	SetMapIcon(ICON_BOSS, i);				// �G�l�~�[�̃A�C�R�����Z�b�g
	//}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateMap(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

	for (int i = 0; i < MAX_ICON; i++)
	{
		// �A�C�R���̈ړ�����
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
// �`�揈��
//=============================================================================
void DrawMap(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();

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

	// �n�}�w�i�̕`��
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[FIELD_ALL]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, MAP_X - 4, MAP_Y - 4, MAP_WIDTH * 1.05f, MAP_HEIGHT * 1.05f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �n�}�̕`��
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[GetFieldType()]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, MAP_X, MAP_Y, MAP_WIDTH, MAP_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �v���C���[�̃A�C�R����`��
	if (player->use)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Map[ICON_PLAYER].texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, g_Map[ICON_PLAYER].x, g_Map[ICON_PLAYER].y, g_Map[ICON_PLAYER].w, g_Map[ICON_PLAYER].h, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), g_Map[ICON_PLAYER].dir + XM_PI);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �{�X�̃A�C�R����`��
	if (boss->use)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Map[ICON_BOSS].texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, g_Map[ICON_BOSS].x, g_Map[ICON_BOSS].y, g_Map[ICON_BOSS].w, g_Map[ICON_BOSS].h, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), g_Map[ICON_PLAYER].dir + XM_PI);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}

//=============================================================================
// 3D���W���}�b�v���W�ɕϊ�
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
