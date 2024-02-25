//=============================================================================
//
// �w�i���� [bg.cpp]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "bg.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BG		"data/MODEL/bg/bg.obj"			// �ǂݍ��ރ��f����
#define	BG_SPEED		(0.00015f)						// �X�N���[���X�s�[�h

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
XMFLOAT3		g_pos;									// �|���S���̈ʒu
XMFLOAT3		g_rot;									// �|���S���̌���(��])
XMFLOAT3		g_scl;									// �|���S���̑傫��(�X�P�[��)
DX11_MODEL		g_model;
BOOL			g_load;
BOOL			g_use;
float			g_spd;
XMFLOAT4X4		g_mtxWorld;								// ���[���h�}�g���b�N�X


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
{
	LoadModel(MODEL_BG, &g_model);
	g_load = true;
	 
	g_pos = { 0.0f, 50.0f, 0.0f };
	g_rot = { 0.0f, 0.0f, 0.0f };
	g_scl = { 110.0f, 110.0f, 110.0f };
	 
	g_spd  = 0.0f;			// �ړ��X�s�[�h�N���A
	 
	g_use = true;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
{
	// ���f���̉������
	if (g_load)
	{
		UnloadModel(&g_model);
		g_load = false;
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBG(void)
{
	PLAYER* player = GetPlayer();

	// �w�i���X�N���[��
	{
		g_rot.y += BG_SPEED;

		if (g_rot.y > XM_PI)
		{
			g_rot.y -= XM_PI;
		}
	}

	// ���S���v���C���[�̈ʒu�ɍ��킹��
	// g_pos = player->pos;



#ifdef _DEBUG
#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_FRONT);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_scl.x, g_scl.y, g_scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_rot.x, g_rot.y + XM_PI, g_rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_pos.x, g_pos.y, g_pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&g_model);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


