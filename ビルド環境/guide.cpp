//=============================================================================
//
// ��������p�K�C�h���� [guide.cpp]
// Author : GP11A 15 �����ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "boss.h"
#include "guide.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_GUIDE			"data/MODEL/sphere/sphere.obj"		// �ǂݍ��ރ��f����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static GUIDE			g_Guide[MAX_GUIDE];				// �K�C�h�p����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGuide(void)
{
	for (int i = 0; i < MAX_GUIDE; i++)
	{
		LoadModel(MODEL_GUIDE, &g_Guide[i].model);
		g_Guide[i].pos = { 0.0f, 0.0f, 0.0f };

		switch(i)
		{
		case GUIDE_SWORD:
			g_Guide[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			break;

		case GUIDE_PLAYER:
			g_Guide[i].scl = XMFLOAT3(1.1f, 1.1f, 1.1f);
			break;

		case GUIDE_BOSSHEAD:
			g_Guide[i].scl = XMFLOAT3(6.5f, 6.5f, 6.5f);
			break;

		case GUIDE_BOSSTAIL:
			g_Guide[i].scl = XMFLOAT3(6.0f, 6.0f, 6.0f);
			break;
		}

		g_Guide[i].size = g_Guide[i].scl.x * 10.0f;

#ifdef _DEBUG
		for (int j = 0; j < g_Guide[i].model.SubsetNum; j++)
		{
			if (i <= GUIDE_PLAYER)
			{
				SetModelDiffuse(&g_Guide[i].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.6f));
			}
			else
			{
				SetModelDiffuse(&g_Guide[i].model, j, XMFLOAT4(0.0f, 0.0f, 1.0f, 0.6f));
			}
		}
#endif

		g_Guide[i].use = TRUE;
		g_Guide[i].load = TRUE;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGuide(void)
{

	for (int i = 0; i < MAX_GUIDE; i++)
	{
		if (g_Guide[i].load)
		{
			UnloadModel(&g_Guide[i].model);
			g_Guide[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGuide(void)
{
	XMFLOAT3 swordPos = ChildPlayerPartsPos(0, PLAYERPARTS_SWORD);
	XMFLOAT3 swordTipPos = ChildPlayerPartsPos(0, PLAYERPARTS_SWORDTIP);
	XMFLOAT3 bossBodyPos = ChildBossPartsPos(0, BOSSPARTS_BODY);
	XMFLOAT3 bossHeadPos = ChildBossPartsPos(0, BOSSPARTS_HEAD);
	XMFLOAT3 bossTailPos = ChildBossPartsPos(0, BOSSPARTS_TAIL);
	XMFLOAT3 dir = { 0.0f, 0.0f, 0.0f };
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_GUIDE; i++)
	{
		if (g_Guide[i].use == FALSE) continue;

		switch (i)
		{
		case GUIDE_SWORD:
			// �匕�̓����蔻��

			// ���̕t�������猕��ւ̃x�N�g�����o��
			XMVECTOR vA = XMLoadFloat3(&swordPos);
			XMVECTOR vB = XMLoadFloat3(&swordTipPos);

			// vAB�̐��K��
			XMVECTOR vAB = XMVector3Normalize(vB - vA);
			XMStoreFloat3(&dir, vAB);

			g_Guide[i].pos.x = swordPos.x + dir.x * 30.0f;
			g_Guide[i].pos.y = swordPos.y + dir.y * 30.0f;
			g_Guide[i].pos.z = swordPos.z + dir.z * 30.0f;
			break;

		case GUIDE_PLAYER:
			// �v���C���[�̓����蔻��
			g_Guide[i].pos = player[0].pos;
			g_Guide[i].pos.y = player[0].pos.y + 15.0f;
			break;

			// ��������{�X�̓����蔻��
		case GUIDE_BOSSHEAD:
			g_Guide[i].pos = bossHeadPos;
			break;

		case GUIDE_BOSSTAIL:
			g_Guide[i].pos = bossTailPos;
			break;
		}
	}




#ifdef _DEBUG

#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGuide(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_GUIDE; i++)
	{
		if (g_Guide[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Guide[i].scl.x, g_Guide[i].scl.y, g_Guide[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Guide[i].pos.x, g_Guide[i].pos.y, g_Guide[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Guide[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Guide[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �Փ˖ڈ��̎擾
//=============================================================================
GUIDE* GetGuide(void)
{
	return &g_Guide[0];
}