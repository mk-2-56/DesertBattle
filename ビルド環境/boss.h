//=============================================================================
//
// �{�X���� [boss.h]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#pragma once

#include <vector>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_BOSS		(1)					// �{�X�̐�
#define MAX_BOSSHP		(70)				// �{�X��HP�����l

// �A�j���[�V�����̏��
enum BOSSSTATE_LIST
{
	BOSSSTATE_IDLE = 0,
	BOSSSTATE_DIG,
	BOSSSTATE_JUMPOUT,
	BOSSSTATE_CHANGE,
	BOSSSTATE_SWIM,
	BOSSSTATE_FLY,
	BOSSSTATE_WALK,
	BOSSSTATE_JUMP,

	BOSSSTATE_MAX,
};

// �{�X�̑���
enum BOSSATTRIBUTE_LIST
{
	BOSSATTR_SAND = 0,
	BOSSATTR_WATER,
	// BOSSATTR_MAGMA,

	BOSSATTR_MAX,
};

// �q�p�[�c�̃��X�g
enum BOSSPARTS_LIST
{
	BOSSPARTS_BODY = 0,
	BOSSPARTS_HEAD,
	BOSSPARTS_LARMTOP,
	BOSSPARTS_LARMBTM,
	BOSSPARTS_RARMTOP,
	BOSSPARTS_RARMBTM,
	BOSSPARTS_LLEGTOP,
	BOSSPARTS_LLEGBTM,
	BOSSPARTS_RLEGTOP,
	BOSSPARTS_RLEGBTM,
	BOSSPARTS_LWING,
	BOSSPARTS_RWING,
	BOSSPARTS_TAIL,

	BOSSPARTS_MAX,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BOSS
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3			oldPos;				// �O�t���[����pos

	float				spd;				// �ړ��X�s�[�h
	float				dir;				// ����
	float				size;				// �����蔻��̑傫��
	int					state;
	int					hp;
	BOOL				attack;
	BOOL				use;
	BOOL				underGround;		// �n���ɂ���Ƃ���

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��
	int					setTbl;				// �Z�b�g���Ă���e�[�u��

	int					modelIdx;
	int					attribute;			// ���݂̑���

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	BOSS*				parent;				// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X
	std::vector<INTERPOLATION_DATA> blendTbl;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	DISSOLVE			dissolve;	// �f�B�]����
};

struct BOSS_MODEL
{
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBoss(void);
void UninitBoss(void);
void UpdateBoss(void);
void DrawBoss(BOOL shadow = FALSE);

BOSS *GetBoss(void);
XMFLOAT3 ChildBossPartsPos(int no, int index);
void DamageBoss(int no, int damage);
void ResetBossAction(void);