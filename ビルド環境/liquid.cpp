//=============================================================================
//
// �t�̂̏��� [liquid.cpp]
// Author : GP111A132 15 �����@�ق̍�
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "liquid.h"
#include "meshfield.h"
#include "renderer.h"
#include "model.h"
#include "boss.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX		(2)				// �e�N�X�`���̐�

#define COORD(z, x) (z) * (g_nNumBlockXField + 1) + (x) // �z��ԍ������߂�
#define DAMP_PARAM  (0.001f)							// ����,�ǂꂾ�������ɔg���Â��ɂȂ邩 �f���^T��T�ɂ��Ă���
#define T			(0.03f)								// �g���������̎���T

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetWaveWater(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11Buffer					*g_IndexBuffer = NULL;	// �C���f�b�N�X�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;				// �e�N�X�`���ԍ�

static XMFLOAT3		g_posField;								// �|���S���\���ʒu�̒��S���W
static XMFLOAT3		g_rotField;								// �|���S���̉�]�p

static int			g_nNumBlockXField, g_nNumBlockZField;	// �u���b�N��
static int			g_nNumVertexField;						// �����_��	
static int			g_nNumVertexIndexField;					// ���C���f�b�N�X��
static int			g_nNumPolygonField;						// ���|���S����
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// �u���b�N�T�C�Y
static float		g_counter = 0.0f;

static char* g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/wave1.png",
	"data/TEXTURE/wave2.png",
};


// �g�̏���

static VERTEX_3D	*g_Vertex = NULL;

// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
static XMFLOAT3		g_Center;					// �g�̔����ꏊ
static float		g_Time = 0.0f;				// �g�̌o�ߎ���
static float		g_wave_frequency  = 2.0f;	// �g�̎��g��
static float		g_wave_correction = 0.02f;	// �g�̋����␳
static float		g_wave_amplitude  = 20.0f;	// �g�̐U��

static BOOL			g_Load = FALSE;

// �ߋ��̓_�̍���(�z��̍ŏ��̓_�݂����Ȃ���)
static float* pointOLD = NULL;

// ���݂̓_�̍���
static float* pointNOW = NULL;

// �����̓_�̍���
static float* pointNEXT = NULL;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitLiquid(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// �|���S���\���ʒu�̒��S���W��ݒ�
	g_posField = pos;

	g_rotField = rot;

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
	
	// �u���b�N���̐ݒ�
	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;

	// ���_���̐ݒ�
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;	

	// �|���S�����̐ݒ�
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// �u���b�N�T�C�Y�̐ݒ�
	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;


	// ���_�����������ɍ���Ă����i�g�ׁ̈j
	// �g�̏���
	// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �g�̔����ꏊ
	g_Time = 0.1f;								// �g�̌o�ߎ���(�{�Ɓ[�Ƃœ����O���ɂȂ�)
	g_wave_frequency = 1.0f;					// �g�̎��g���i�㉺�^���̑����j
	g_wave_correction = 0.02f;					// �g�̋����␳�i�ς��Ȃ��Ă��ǂ��Ǝv���j
	g_wave_amplitude = 60.0f;					// �g�̐U��(�g�̍���)

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			g_Vertex[COORD(z, x)].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[COORD(z, x)].Position.y = 0.0f;
			g_Vertex[COORD(z, x)].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			// �@���̐ݒ�
			g_Vertex[COORD(z, x)].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// ���ˌ��̐ݒ�
			g_Vertex[COORD(z, x)].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			float texSizeX = 1.0f / g_nNumBlockXField;
			float texSizeZ = 1.0f / g_nNumBlockZField;
			g_Vertex[COORD(z, x)].TexCoord.x = texSizeX * x;
			g_Vertex[COORD(z, x)].TexCoord.y = texSizeZ * z;
		}

	}

	// �g��������

	// �����_���g����
	pointOLD = new float[g_nNumVertexField];
	pointNOW = new float[g_nNumVertexField];
	pointNEXT = new float[g_nNumVertexField];

	// �g���������̏�����
	{
		for (int i = 0; i < g_nNumVertexField; i++)
		{
			pointOLD[i] = 0.0f;
			pointNOW[i] = 0.0f;
			pointNEXT[i] = 0.0f;

		}
	}

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �C���f�b�N�X�o�b�t�@����
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//���_�o�b�t�@�̒��g�𖄂߂�

		// ���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�

		// �C���f�b�N�X�o�b�t�@�̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if(nCntVtxZ > 0)
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
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
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
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
// �I������
//=============================================================================
void UninitLiquid(void)
{
	if (g_Load == FALSE) return;

	// �C���f�b�N�X�o�b�t�@�̉��
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	// new���Z�q���g�����̂�delete
	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}
	if (pointOLD)
	{
		delete[] pointOLD;
		pointOLD = NULL;
	}
	if (pointNOW)
	{
		delete[] pointNOW;
		pointNOW = NULL;
	}
	if (pointNEXT)
	{
		delete[] pointNEXT;
		pointNEXT = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateLiquid(void)
{
	// �g��������
	if (GetFieldType() == FIELD_WATER)
	{
		SetWaveWater();
	}

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// �S���_���𖈉�㏑��
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawLiquid(void)
{
	BOSS* boss = GetBoss();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	material.noTexSampling = (boss->attribute == BOSSATTR_WATER) ? 2 : 0;
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);		// �g�p(2����)
	GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[1]);		// �g�p(3����)

	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	// �|���S���̕`��
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}


//=============================================================================
// �g��������
//=============================================================================
void SetWaveWater(void)
{
	BOSS* boss = GetBoss();

	// �g�̔���
	if(((int)(g_counter) % 180 == 0) && (boss->state == BOSSSTATE_SWIM))
	{
		for (int z = 1; z < g_nNumBlockZField - 1; z++)
		{
			for (int x = 1; x < g_nNumBlockXField - 1; x++)
			{
				// �����O����
				float bossPosX = boss->pos.x - sinf(boss->rot.y) * 100.0f;
				float bossPosZ = boss->pos.z - cosf(boss->rot.y) * 100.0f;
				float dx = g_Vertex[COORD(z, x)].Position.x - bossPosX;
				float dz = g_Vertex[COORD(z, x)].Position.z - bossPosZ;

				//�g��̒��S�_����̋����𓾂�
				float len = (float)sqrt(dx * dx + dz * dz);


				// �g�̋���
				float a = (float)exp(-len / 5) * 20;

				// �����Ŕg������Ă���
				pointNOW[COORD(z, x)] += a;
			}
		}

		for (int z = 0; z < g_nNumBlockZField; z++)
		{
			pointNOW[COORD(z, 0)] = pointNOW[COORD(z, g_nNumBlockXField - 1)] = 0.0f;
		}

		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			pointNOW[COORD(0, x)] = pointNOW[COORD(g_nNumBlockZField - 1, x)] = 0.0f;
		}
	}

	//�[�����ȊO�̓_ ���菬�����z����Q�Ƃ��Ă��� �ׂ����Ȃ��Ƃ���h�����߂炵��
	for (int z = 1; z < g_nNumBlockZField - 1; z++)
	{
		// ���Ɖߋ��̓_���疢���̓_�𓱂��o���Ă�
		for (int x = 1; x < g_nNumBlockXField - 1; x++)
		{
			// �����̎�
			float damp = -DAMP_PARAM * (pointNOW[COORD(z, x)] - pointOLD[COORD(z, x)]);

			// �g���������̒��g
			pointNEXT[COORD(z, x)] =
				2.0f * pointNOW[COORD(z, x)] - pointOLD[COORD(z, x)]
				+ T * (pointNOW[COORD(z, x + 1)] + pointNOW[COORD(z, x - 1)]
					+ pointNOW[COORD(z + 1, x)] + pointNOW[COORD(z - 1, x)]
					- 4.0f * pointNOW[COORD(z, x)]) + damp;

		}
	}

	////���E����	�w�肵���͈͊O�ɔg��₽����
	//for (int z = g_nNumBlockZField/4; z < g_nNumBlockZField * 3/4; z++)
	//{
	//    for (int x = g_nNumBlockXField/4; x < g_nNumBlockXField * 3 / 4; x++)
	//    {
	//        pointNEXT[COORD(z, x)] = pointNEXT[COORD(z, x)] = 0.0f;
	//    }
	//}


//�_�̍X�V 1�t���[�����Ƃɓ_�̍X�V�����Ă���
	for (int i = 0; i < g_nNumVertexField; i++)
	{
		pointOLD[i] = pointNOW[i];
		pointNOW[i] = pointNEXT[i];
	}

	//���_�ɑ��
	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 1; x < g_nNumBlockXField - 1; x++)
		{
			g_Vertex[COORD(z, x)].Position.y = pointNOW[COORD(z, x)];
		}
	}

	g_counter++;
}