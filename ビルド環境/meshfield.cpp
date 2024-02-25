//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.cpp]
// Author : GP11A132 15 �����ق̍�
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX		(2)				// �e�N�X�`���̐�

#define COORD(z, x) (z) * (g_nNumBlockXField + 1) + (x) //�z��ԍ������߂�

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11Buffer					*g_IndexBuffer = NULL;	// �C���f�b�N�X�o�b�t�@
// static WIPE							g_Wipe;				// ���C�v

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;				// �e�N�X�`���ԍ�
static int							g_FieldType;			// ���݂̃t�B�[���h�^�C�v

static XMFLOAT3		g_posField;								// �|���S���\���ʒu�̒��S���W
static XMFLOAT3		g_rotField;								// �|���S���̉�]�p

static int			g_nNumBlockXField, g_nNumBlockZField;	// �u���b�N��
static int			g_nNumVertexField;						// �����_��	
static int			g_nNumVertexIndexField;					// ���C���f�b�N�X��
static int			g_nNumPolygonField;						// ���|���S����
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// �u���b�N�T�C�Y
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

// �g�̏���

static VERTEX_3D	*g_Vertex = NULL;

// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
static XMFLOAT3		g_Center;					// �g�̔����ꏊ
static float		g_Time = 0.0f;				// �g�̌o�ߎ���
static float		g_wave_frequency  = 2.0f;	// �g�̎��g��
static float		g_wave_correction = 0.02f;	// �g�̋����␳
static float		g_wave_amplitude  = 40.0f;	// �g�̐U��

static BOOL			g_Load = FALSE;
static BOOL			g_FieldChange = FALSE;		// �t�B�[���h���ω�����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
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
	
	g_FieldType = FIELD_SAND;	// �ŏ��͍���

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

	// ���̃t�B�[���h��y���W�ۑ��p
	g_dataPosY = new float[g_nNumVertexField];

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
			float randHeight = (float)((rand() % 10 + 10) / 9);
			g_Vertex[COORD(z, x)].randHeight = randHeight;
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


	// �t�B�[���h�̃��Z�b�g
	ResetField(g_FieldType);

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
void UninitMeshField(void)
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

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMeshField(void)
{
	BOSS* boss = GetBoss();
	CAMERA* camera = GetCamera();

	// �{�X�̒n���̓����ɍ��킹��
	if (boss->underGround)
	{
		g_Center = boss->pos;
		for (int z = 0; z < g_nNumBlockZField; z++)
		{
			for (int x = 0; x < g_nNumBlockXField; x++)
			{
				float dx = g_Vertex[COORD(z, x)].Position.x - g_Center.x;
				float dz = g_Vertex[COORD(z, x)].Position.z - g_Center.z;

				// �g��̒��S�_����̋����𓾂�
				float len = (float)sqrt(dx * dx + dz * dz);

				// �g�̍������Asin�֐��œ���
				//  �g�̍����@= sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
				if (len < 100.0f)
				{
					g_Vertex[COORD(z, x)].Position.y = g_wave_amplitude * g_Vertex[COORD(z, x)].randHeight;
				}
			}
		}
	}

	// �t�B�[���h�`�F���W���̓���
	if (g_FieldChange)
	{
		BOOL changeComp = TRUE;
		for (int z = 0; z < g_nNumBlockZField; z++)
		{
			for (int x = 0; x < g_nNumBlockXField; x++)
			{
				// ���̍��W�Ǝ��̍��W�̍��������񂾂񌸂炷
				float diff = g_dataPosY[COORD(z, x)] - g_Vertex[COORD(z, x)].Position.y;
				
				g_Vertex[COORD(z, x)].Position.y += diff * 0.015f;
				
				// ��������萔�ȏ゠��Ȃ�
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

		// �S�Ă̒��_�̑J�ڂ�����������
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


	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// �S���_�����㏑��
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeshField()
{
	// SetWipe(WIPE_MODE_TEXWIPE, XMFLOAT4(1.0f, 0.0f, -1.0f, 0.0f), g_wipeSize);

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
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_FieldType]);			// �}�b�v(1����)
	
	//// �Ō�̃t�B�[���h����Ȃ����2���ڂɃ}�b�v���Z�b�g���Ă���(2����)
	//if(g_FieldType != FIELD_ALL - 1) GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[g_FieldType + 1]);

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

	// SetWipe(WIPE_MODE_NONE, XMFLOAT4(1.0f, 0.0f, 1.0f, 0.0f), 0.0f);
}


// �t�B�[���h�Ƃ̏Փ˔���
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

	// �^����ꂽ���W�̏����ォ��A���փ��C���΂�
	start.y += 100.0f;
	end.y -= 1000.0f;

	// ����������������ׂɑS�����ł͂Ȃ����W����K�v�ȃ|���S��������o��
	float fz = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;			// �t�B�[���h�S�̂̔����̒���
	float fx = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	int sz = (int)((-start.z+fz) / g_fBlockSizeZField);					// ������ꏊ�̒��_���z��̉��Ԗڂ�
	int sx = (int)(( start.x+fx) / g_fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;

	if ((sz < 0) || (sz > g_nNumBlockZField-1) ||
		(sx < 0) || (sx > g_nNumBlockXField-1))
	{	// �t�B�[���h�͈̔͊O�ɋ�����
		*Normal = {0.0f, 1.0f, 0.0f};
		return FALSE;
	}

	// �K�v�����������J��Ԃ�(���肵�����I�u�W�F�N�g�̐^���̃|���S���̂�=1��)
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			// �^���̎l�p�`�|���S���p�̒��_
			XMFLOAT3 p0 = g_Vertex[COORD(z, x)].Position;
			XMFLOAT3 p1 = g_Vertex[COORD(z, x+1)].Position;
			XMFLOAT3 p2 = g_Vertex[COORD(z+1, x)].Position;
			XMFLOAT3 p3 = g_Vertex[COORD(z+1, x+1)].Position;

			// �O�p�|���S��������2�����̓����蔻��
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
// �t�B�[���h�̕ӂ̒�����Ԃ�
//=============================================================================
float GetFieldSize(void)
{
	float fieldLength;

	fieldLength = g_nNumBlockXField * g_fBlockSizeXField;

	return fieldLength;
}

//=============================================================================
// �t�B�[���h���ω�����
//=============================================================================
BOOL GetFieldChange(void)
{
	return g_FieldChange;
}

//=============================================================================
// �t�B�[���h�^�C�v�̎擾
//=============================================================================
int GetFieldType(void)
{
	return g_FieldType;
}

//=============================================================================
// �t�B�[���h�����Z�b�g
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
		printf("�t�@�C���G���[�I");
	}
	else
	{
		// �t�@�C����ǂݍ���ŏ���������
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

		// �I��
		fclose(file);
		printf("�I��");
		if (type != FIELD_SAND)
		{
			PlaySound(SOUND_LABEL_SE_earthquake);
			g_FieldChange = TRUE;
		}
	}
}
