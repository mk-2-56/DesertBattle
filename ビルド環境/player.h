//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#pragma once
#include "model.h"

#include <vector>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER				(1)					// �v���C���[�̐�
#define PLAYER_OFFSET_Y			(7.0f)				// �v���C���[�̑��������킹��
#define PLAYER_FIRST_X			(100.0f)				// �v���C���[�̏������WX
#define PLAYER_FIRST_Z			(-200.0f)			// �v���C���[�̏������WZ


// �A�j���[�V�����̏��
enum STATE_LIST
{
	STATE_IDLE = 0,
	STATE_ATK,
	STATE_SLICE,

	STATE_MAX,
};

// �U���̏��
enum ATKSTATE_LIST
{
	ATKSTATE_NONE = 0,
	ATKSTATE_NOHIT,
	ATKSTATE_HIT,

	ATKSTATE_MAX,
};

// �q�p�[�c�̃��X�g
enum PLAYERPARTS_LIST
{
	PLAYERPARTS_JOINT1 = 0,
	PLAYERPARTS_JOINT2,
	PLAYERPARTS_JOINT3,
	PLAYERPARTS_JOINT4,
	PLAYERPARTS_JOINT5,
	PLAYERPARTS_SWORD,
	PLAYERPARTS_BODY,
	PLAYERPARTS_SWORDTIP,

	PLAYERPARTS_MAX,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3			oldPos;				// �O�t���[����pos

	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X

	float				spd;				// �ړ��X�s�[�h
	float				dir;				// ����
	int					state;				// �X�e�[�g(��ԁj
	int					attack;				// �U�����
	BOOL				use;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��
	int					setTbl;				// �Z�b�g���Ă���e�[�u��

	int					modelIdx;

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER*				parent;				// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F
	std::vector<INTERPOLATION_DATA> blendTbl;

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	// �X�e�[�^�X
	float				hp;					// HP
	float				energy;				// �G�l���M�[
	float				damageRot;			// ������ԕ���
};

struct PLAYER_MODEL
{
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
PLAYER* GetPlayer(void);
BOOL GetPushed(void);

// �L�[�{�[�h����
BOOL AnyKeyPress(void);
// �X�e�B�b�N����
BOOL AnyStickPress(void);

// �����蔻��֘A
XMFLOAT3 ChildPlayerPartsPos(int no, int index);
void SetPlayerPush(float rot, float speed);

// �X�e�[�^�X�֘A
float GetPlayerHP(int no);
float GetPlayerEnergy(int no);
void DamagePlayer(int no, int damage);