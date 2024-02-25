//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP11A132 15 �����ق̍� 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "keyframe.h"
#include "debugproc.h"
#include "meshfield.h"
#include "particle.h"
#include "boss.h"
#include "guide.h"
#include "collision.h"
#include "deco.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//****************************************************************************
#define	VALUE_MOVE				(2.2f)							// �ړ���
#define	VALUE_DASH				(3.6f)							// �_�b�V�����̈ړ���
#define	COUNT_KNOCKBACK			(5.0f)							// ������΂����J�E���g

#define PLAYER_PARTS_MAX		(8)								// �v���C���[�̃p�[�c�̐�
#define	VALUE_MOVE_CAMERA		(2.0f)							// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA		(XM_PI * 0.01f)					// �J�����̉�]��
#define PAD_LENGTH				(1000.0f)						// �X�e�B�b�N�̌X���̍ő�l
#define PLAYER_HPMAX			(100.0f)						// �}�b�N�X��HP
#define PLAYER_ENERGYMAX		(100.0f)						// �}�b�N�X�̗̑�
#define PLAYER_DAMAGETIME		(30.0f)							// ���G����
#define PLAYER_ENERGYUP			(0.1f)							// �G�i�W�[�񕜗�
#define PLAYER_DASHENERGY		(0.25f)							// �_�b�V���Ɏg���G�i�W�[
#define MIN_POSY_WATER			(10.0f)

// �A�j���[�V�����̃��X�g
enum ANIM_IDLE_LIST
{
	// �ҋ@���
	ANIM_IDLE_LIST_JOINT1 = 0,
	ANIM_IDLE_LIST_JOINT2,
	ANIM_IDLE_LIST_JOINT3,
	ANIM_IDLE_LIST_JOINT4,
	ANIM_IDLE_LIST_JOINT5,
	ANIM_IDLE_LIST_SWORD,
	ANIM_IDLE_LIST_BODY,
	ANIM__LIST_SWORDTIP1,

	// �A�^�b�N
	ANIM_ATTACK_LIST_JOINT1,
	ANIM_ATTACK_LIST_JOINT2,
	ANIM_ATTACK_LIST_JOINT3,
	ANIM_ATTACK_LIST_JOINT4,
	ANIM_ATTACK_LIST_JOINT5,
	ANIM_ATTACK_LIST_SWORD,
	ANIM_ATTACK_LIST_BODY,
	ANIM__LIST_SWORDTIP5,

	// �ガ����
	ANIM_SLICE_LIST_JOINT1,
	ANIM_SLICE_LIST_JOINT2,
	ANIM_SLICE_LIST_JOINT3,
	ANIM_SLICE_LIST_JOINT4,
	ANIM_SLICE_LIST_JOINT5,
	ANIM_SLICE_LIST_SWORD,
	ANIM_SLICE_LIST_BODY,
	ANIM__LIST_SWORDTIP6,

	ANIM_LIST_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InputMovePlayer(int no);
void InputMoveCamera(void);
void InputAttackPlayer(int no);
void InputSlicePlayer(int no);
void MovePlayer(int no);
void AnimFinPlayer(int no, int state);
void SetPlayerTblAnim(int no, int state);
void PlayerAnimation(int i);
void PlayerBlendAnimation(int i);
void LoadBlendAnimTbl(int no);
void PushedPlayer(int no);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
struct PUSH
{
	BOOL			pushed;
	float			spd;
	float			dir;
	int				time;
};

static PUSH			g_Push;									// �G�ɉ�����Ă邩
static BOOL			g_Interval;								// �A�j���[�V�����̃u�����h����
static BOOL			g_Dash;									// �_�b�V������
static BOOL			g_Damaged = FALSE;						// ���G���Ԃ�
static int			g_DamageCnt;
static PLAYER		g_Player[MAX_PLAYER];					// �v���C���[
static PLAYER		g_Parts[MAX_PLAYER][PLAYER_PARTS_MAX];	// �v���C���[�̃p�[�c�p
static PLAYER_MODEL g_Model[MAX_PLAYER][PLAYER_PARTS_MAX];
static LIGHT		g_Light;

// �A�j���[�V�����̃f�B���N�g���̔z��
static char* g_AnimDir[] =
{
	"data/ANIMDATA/player/idle/joint1.csv",
	"data/ANIMDATA/player/idle/joint2.csv",
	"data/ANIMDATA/player/idle/joint3.csv",
	"data/ANIMDATA/player/idle/joint4.csv",
	"data/ANIMDATA/player/idle/joint5.csv",
	"data/ANIMDATA/player/idle/sword.csv",
	"data/ANIMDATA/player/idle/body.csv",
	"data/ANIMDATA/player/swordTip.csv",

	"data/ANIMDATA/player/attack/joint1.csv",
	"data/ANIMDATA/player/attack/joint2.csv",
	"data/ANIMDATA/player/attack/joint3.csv",
	"data/ANIMDATA/player/attack/joint4.csv",
	"data/ANIMDATA/player/attack/joint5.csv",
	"data/ANIMDATA/player/attack/sword.csv",
	"data/ANIMDATA/player/attack/body.csv",
	"data/ANIMDATA/player/swordTip.csv",

	"data/ANIMDATA/player/attackSlice/joint1.csv",
	"data/ANIMDATA/player/attackSlice/joint2.csv",
	"data/ANIMDATA/player/attackSlice/joint3.csv",
	"data/ANIMDATA/player/attackSlice/joint4.csv",
	"data/ANIMDATA/player/attackSlice/joint5.csv",
	"data/ANIMDATA/player/attackSlice/sword.csv",
	"data/ANIMDATA/player/attackSlice/body.csv",
	"data/ANIMDATA/player/swordTip.csv",
};

// ���f���f�[�^�̃f�B���N�g��
static char* g_ModelAdr[] =
{
	"data/MODEL/player/player_joint1.obj",
	"data/MODEL/player/player_joint2.obj",
	"data/MODEL/player/player_joint3.obj",
	"data/MODEL/player/player_joint4.obj",
	"data/MODEL/player/player_joint5.obj",
	"data/MODEL/player/player_sword.obj",
	"data/MODEL/player/player_body.obj",
	"data/MODEL/player/player_swordTip.obj",
};

// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// vector�^��
// �ҋ@���
static std::vector<INTERPOLATION_DATA> idle_joint1;
static std::vector<INTERPOLATION_DATA> idle_joint2;
static std::vector<INTERPOLATION_DATA> idle_joint3;
static std::vector<INTERPOLATION_DATA> idle_joint4;
static std::vector<INTERPOLATION_DATA> idle_joint5;
static std::vector<INTERPOLATION_DATA> idle_sword;
static std::vector<INTERPOLATION_DATA> idle_body;
// ����
static std::vector<INTERPOLATION_DATA> always_swordTip;

// �A�^�b�N
static std::vector<INTERPOLATION_DATA> attack_joint1;
static std::vector<INTERPOLATION_DATA> attack_joint2;
static std::vector<INTERPOLATION_DATA> attack_joint3;
static std::vector<INTERPOLATION_DATA> attack_joint4;
static std::vector<INTERPOLATION_DATA> attack_joint5;
static std::vector<INTERPOLATION_DATA> attack_sword;
static std::vector<INTERPOLATION_DATA> attack_body;

// �ガ����
static std::vector<INTERPOLATION_DATA> slice_joint1;
static std::vector<INTERPOLATION_DATA> slice_joint2;
static std::vector<INTERPOLATION_DATA> slice_joint3;
static std::vector<INTERPOLATION_DATA> slice_joint4;
static std::vector<INTERPOLATION_DATA> slice_joint5;
static std::vector<INTERPOLATION_DATA> slice_sword;
static std::vector<INTERPOLATION_DATA> slice_body;

static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// �ҋ@���
	&idle_joint1,
	&idle_joint2,
	&idle_joint3,
	&idle_joint4,
	&idle_joint5,
	&idle_sword,
	&idle_body,
	&always_swordTip,

	// �A�^�b�N
	&attack_joint1,
	&attack_joint2,
	&attack_joint3,
	&attack_joint4,
	&attack_joint5,
	&attack_sword,
	&attack_body,
	&always_swordTip,

	// �ガ����
	&slice_joint1,
	&slice_joint2,
	&slice_joint3,
	&slice_joint4,
	&slice_joint5,
	&slice_sword,
	&slice_body,
	&always_swordTip,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Push.pushed = FALSE;

	// ���f���̓Ǎ�
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			LoadModel(g_ModelAdr[j], &g_Model[i][j].model);
			g_Model[i][j].load = TRUE;
		}
	}

	// �A�j���[�V�����f�[�^���t�@�C������ǂݍ���
	for (int i = 0; i < ANIM_LIST_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}

	// �v���C���[�̏�����
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_Player[i].pos = XMFLOAT3(0.0f, PLAYER_OFFSET_Y, PLAYER_FIRST_Z);
		g_Player[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Player[i].spd = 0.0f;						// �ړ��X�s�[�h�N���A
		g_Player[i].use = TRUE;						// TRUE:�����Ă�
		g_Player[i].attack = ATKSTATE_NONE;			// �A�^�b�N����

		g_Player[i].state = STATE_IDLE;
		g_Player[i].setTbl = STATE_IDLE;

		g_Player[i].hp = PLAYER_HPMAX;
		g_Player[i].energy = PLAYER_ENERGYMAX;

		g_Player[i].parent = NULL;					// �{�́i�e�j�Ȃ̂�NULL������


		// �K�w�A�j���[�V�����̏�����
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = TRUE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Parts[i][j].modelIdx = j;

			// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
			GetModelDiffuse(&g_Model[i][g_Parts[i][j].modelIdx].model, &g_Parts[i][j].diffuse[0]);

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[i][j].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[i][j].tblNo = j + ANIM_IDLE_LIST_JOINT1;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[i][j].tblMax = (int)g_TblAdr[g_Parts[i][j].tblNo]->size();			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g


			// �e�q�֌W
			switch (j)
			{
			case PLAYERPARTS_JOINT1:
				g_Parts[i][j].parent = &g_Player[i];
				break;

			case PLAYERPARTS_JOINT2:
				g_Parts[i][j].parent = &g_Parts[i][PLAYERPARTS_JOINT1];
				break;

			case PLAYERPARTS_JOINT3:
				g_Parts[i][j].parent = &g_Parts[i][PLAYERPARTS_JOINT2];
				break;

			case PLAYERPARTS_JOINT4:
				g_Parts[i][j].parent = &g_Parts[i][PLAYERPARTS_JOINT3];
				break;

			case PLAYERPARTS_JOINT5:
				g_Parts[i][j].parent = &g_Parts[i][PLAYERPARTS_JOINT4];
				break;

			case PLAYERPARTS_SWORD:
			case PLAYERPARTS_BODY:
				g_Parts[i][j].parent = &g_Parts[i][PLAYERPARTS_JOINT5];
				break;

			case PLAYERPARTS_SWORDTIP:
				g_Parts[i][j].parent = &g_Parts[i][PLAYERPARTS_SWORD];
				break;

			default:
				break;
			}
		}
		// �N�H�[�^�j�I���̏�����
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());

	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// ���f���̉������
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			if (g_Model[i][j].load == FALSE) continue;

			UnloadModel(&g_Model[i][j].model);
			g_Parts[i][j].blendTbl.clear();
			g_Model[i][j].load = FALSE;
		}
	}

	// �A�j���[�V�����e�[�u�����t�@�C�����珉����
	for (int i = 0; i < ANIM_LIST_MAX; i++)
	{
		g_TblAdr[i]->clear();
	}

	
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use)
		{
			g_Dash = FALSE;								// �_�b�V���͏����l��FALSE

			InputMoveCamera();							// �E�X�e�B�b�N�ŃJ��������

			switch (g_Player[i].state)					// ���݂̃X�e�[�g�ɂ���đ���𕪂���
			{
			case STATE_IDLE:
				if (!g_Push.pushed) InputMovePlayer(i);
				InputAttackPlayer(i);
				InputSlicePlayer(i);
				break;

			default:
				break;
			}

			// �A�j���[�V�������Z�b�g
			AnimFinPlayer(i, g_Player[i].state);
			SetPlayerTblAnim(i, g_Player[i].state);

			// �u�����h�����ǂ����Ő��`��Ԃ��킯��
			(g_Interval) ? PlayerBlendAnimation(i) : PlayerAnimation(i);

			if (g_Push.pushed)
			{
				PushedPlayer(i);
				g_Push.time++;

				if (g_Push.time > COUNT_KNOCKBACK)
				{
					g_Push.pushed = FALSE;
					g_Push.time = 0;
				}
			}

			// ���������Č���������
			g_Player[i].spd *= 0.7f;
			g_Push.spd *= 0.7f;


			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g��
			BOOL ans = RayHitField(g_Player[i].pos, &HitPosition, &Normal);

			BOOL gravity = TRUE;
			float distance = 0.0f;

			if (ans)
			{
				g_Player[i].pos.y = HitPosition.y + PLAYER_OFFSET_Y;
				distance = g_Player[i].pos.y - HitPosition.y;

				// �n�ʂɖ��܂�̂�h�~
				if ((distance <= 10.0f) && (distance >= -100.0f))
				{
					gravity = FALSE;
				}

				// �n�ʂɖ��܂��Ă�Ȃ�o��
				if (distance < 0.0f)
				{
					g_Player[i].pos.y -= distance - PLAYER_OFFSET_Y;
				}
			}

			if (gravity == TRUE)
			{
				// �d�͂�t����
				g_Player[i].pos.y -= 1.0f;
			}
			//else
			//{
			//	g_Player.pos.y = PLAYER_OFFSET_Y;
			//	Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			//}


			// �p������

			XMVECTOR vx, nvx, up;
			XMVECTOR quat;
			float len, angle;

			g_Player[i].UpVector = Normal;
			up = { 0.0f, 1.0f, 0.0f, 0.0f };
			vx = XMVector3Cross(up, XMLoadFloat3(&g_Player[i].UpVector));

			nvx = XMVector3Length(vx);
			XMStoreFloat(&len, nvx);
			nvx = XMVector3Normalize(vx);
			angle = asinf(len);
			quat = XMQuaternionRotationNormal(nvx, angle);

			quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player[i].Quaternion), quat, 0.05f);
			XMStoreFloat4(&g_Player[i].Quaternion, quat);


			// �G�i�W�[�n
			{
				// �_�b�V�����̓G�i�W�[������
				if (g_Dash) g_Player[i].energy -= PLAYER_DASHENERGY;

				// ���R��
				if (g_Player[i].energy < PLAYER_ENERGYMAX) g_Player[i].energy += PLAYER_ENERGYUP;
			}

			// �_���[�W�n
			{
				if (g_Damaged)
				{
					g_DamageCnt++;
					
					if (g_DamageCnt > PLAYER_DAMAGETIME)
					{
						g_DamageCnt = 0;
						g_Damaged = FALSE;
					}
				}
			}

			// ��������̓����蔻��`�F�b�N�n(�v���C���[���ŕK�v�Ȃ���)
			{
				// ���t�B�[���h�̎��͐��ɓ���Ȃ�
				if ((GetFieldType() == FIELD_WATER) && (g_Player[i].pos.y <= MIN_POSY_WATER)) g_Player[i].pos = g_Player[i].oldPos;

				// ���t�B�[���h�ȊO�ł͒P���Ƀ}�b�v�̊O�ɏo��Ȃ�
				else if (((g_Player[i].pos.x < GetFieldSize() / 2 * -1) || (g_Player[i].pos.x > GetFieldSize() / 2)) ||
					((g_Player[i].pos.z < GetFieldSize() / 2 * -1) || (g_Player[i].pos.z > GetFieldSize() / 2)))
				{
					g_Player[i].pos = g_Player[i].oldPos;
				}
			}

#ifdef _DEBUG
			// �f�o�b�O�\��
			PrintDebugProc("Player X:%f Y: %f Z:%f rotY: %f pushed: %d\n", g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z, g_Player[i].rot.y, g_Push.pushed);

			if (GetKeyboardPress(DIK_R))
			{
				g_Player[i].pos.x = g_Player[i].pos.y = g_Player[i].pos.z = 0.0f;
				g_Player[i].spd = 0.0f;
			}
#endif

		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use)
		{
			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

			// �J�����O����
			SetCullingMode(CULL_MODE_NONE);

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �N�H�[�^�j�I���𔽉f
			quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
			mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);

			// �K�w�A�j���[�V����
			for (int j = 0; j < PLAYER_PARTS_MAX; j++)
			{
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);

				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
					// ��
					// g_Player.mtxWorld���w���Ă���
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// �g���Ă���Ȃ珈������
				if (g_Parts[i][j].use == FALSE) continue;

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);

				// ���f���`��
				if (j != PLAYER_PARTS_MAX - 1) DrawModel(&g_Model[i][g_Parts[i][j].modelIdx].model);
			}

			// �J�����O�ݒ��߂�
			SetCullingMode(CULL_MODE_BACK);

		}
	}
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

BOOL GetPushed(void)
{
	return g_Push.pushed;
}

//=============================================================================
// �L�[�{�[�h�̓��͔���
//=============================================================================
BOOL AnyKeyPress(void)
{
	// �ǂꂩ�������ꂽ��
	return (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_DOWN));
}


//=============================================================================
// �X�e�B�b�N�̓��͔���
//=============================================================================
BOOL AnyStickPress(void)
{
	// �ǂꂩ�������ꂽ��
	return ((IsButtonPressed(0, BUTTON_LEFT)) || (IsButtonPressed(0, BUTTON_RIGHT)) || (IsButtonPressed(0, BUTTON_UP)) || (IsButtonPressed(0, BUTTON_DOWN)));
}


//=============================================================================
// �v���C���[�̍s��
//=============================================================================
void InputMovePlayer(int no)
{
	DIJOYSTATE2* dijs = GetButtonAmount();
	CAMERA* cam = GetCamera();

	// �ړ�����
	// �Q�[���p�b�h�܂��̓L�[�{�[�h�ŉ��炩�̓��͂������
	if ((AnyKeyPress()) || (AnyStickPress()))
	{
		// x,y���̕��������߂�p
		float dx = 0.0f;
		float dz = 0.0f;

		// �L�[�{�[�h�̏�����������
		if (AnyKeyPress())
		{
			if (GetKeyboardPress(DIK_LEFT)) dx = -1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) dx = 1.0f;
			if (GetKeyboardPress(DIK_UP)) dz = 1.0f;
			if (GetKeyboardPress(DIK_DOWN)) dz = -1.0f;
			if ((GetKeyboardPress(DIK_SPACE)) && (g_Player[no].energy >= PLAYER_DASHENERGY)) g_Dash = TRUE;			// �_�b�V����
		}

		// �Q�[���p�b�h�̏�����������
		if (AnyStickPress())
		{
			if ((IsButtonPressed(0, BUTTON_RIGHT)) || (IsButtonPressed(0, BUTTON_LEFT)))
			{
				dx = dijs->lX / PAD_LENGTH;
			}
			if ((IsButtonPressed(0, BUTTON_DOWN)) || (IsButtonPressed(0, BUTTON_UP)))
			{
				dz = (dijs->lY / PAD_LENGTH) * -1;
			}
			if ((IsButtonPressed(0, BUTTON_R) && (g_Player[no].energy >= PLAYER_DASHENERGY))) g_Dash = TRUE;			// �_�b�V����
		}

		g_Player[no].spd = (g_Dash) ? VALUE_DASH : VALUE_MOVE;

		// x��,z���ɂ��������͂�i�ޕ����̊p�x�ɂ���
		g_Player[no].dir = atan2f(dx * -1, dz * -1);

		MovePlayer(no);										// ���͂����ƂɈړ�����
	}
}

// �E�X�e�B�b�N�ŃJ��������
void InputMoveCamera(void)
{
	CAMERA* cam = GetCamera();

	if (IsButtonPressed(0, BUTTON_RRIGHT))
	{
		cam->rot.y += VALUE_ROTATE_CAMERA;
		if (cam->rot.y > XM_PI)
		{
			cam->rot.y -= XM_PI * 2.0f;
		}

		cam->at.x = cam->pos.x + sinf(cam->rot.y) * cam->len;
		cam->at.z = cam->pos.z + cosf(cam->rot.y) * cam->len;
	}
	else if (IsButtonPressed(0, BUTTON_RLEFT))
	{
		cam->rot.y -= VALUE_ROTATE_CAMERA;
		if (cam->rot.y < -XM_PI)
		{
			cam->rot.y += XM_PI * 2.0f;
		}

		cam->at.x = cam->pos.x + sinf(cam->rot.y) * cam->len;
		cam->at.z = cam->pos.z + cosf(cam->rot.y) * cam->len;
	}
	if (IsButtonPressed(0, BUTTON_RDOWN))
	{
		cam->pos.y += VALUE_MOVE_CAMERA;
	}
	else if (IsButtonPressed(0, BUTTON_RUP))
	{
		cam->pos.y -= VALUE_MOVE_CAMERA;
	}
}

// ���͂����ƂɈړ�������
void MovePlayer(int no)
{
	// Key���͂���������ړ���������
	CAMERA* cam = GetCamera();
	g_Player[no].oldPos = g_Player[no].pos;

	// �����������Ƀv���C���[���ړ�������
	if (g_Player[no].spd > 0.0f)
	{
		g_Player[no].rot.y = g_Player[no].dir + cam->rot.y;

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player[no].pos.x -= sinf(g_Player[no].rot.y) * g_Player[no].spd;
		g_Player[no].pos.z -= cosf(g_Player[no].rot.y) * g_Player[no].spd;

		// �{�X�Ƃ̓����蔻��
		BOSS* boss = GetBoss();
		GUIDE* guide = GetGuide();
		for (int j = 0; j < MAX_BOSS; j++)
		{
			//�L���t���O���`�F�b�N����
			if (boss[j].use == FALSE) continue;

			for (int k = GUIDE_BOSSHEAD; k < MAX_GUIDE; k++)
			{
				// �{�X����Ԃ���ꂽ��v���C���[�͉������
				BOOL ans = CollisionBC(guide[k].pos, guide[GUIDE_PLAYER].pos, guide[k].size, guide[GUIDE_PLAYER].size);
				if (ans)
				{
					g_Player[no].pos = g_Player[no].oldPos;		// ���̏ꏊ�ɖ߂�
				}
			}
		}
	}
}

// �U������
void InputAttackPlayer(int no)
{
	if ((GetKeyboardTrigger(DIK_G)) || (IsButtonPressed(0, BUTTON_Y)))
	{
		g_Player[no].state = STATE_ATK;
	}
}

// �ガ�����U��
void InputSlicePlayer(int no)
{
	if ((GetKeyboardTrigger(DIK_F)) || (IsButtonPressed(0, BUTTON_B)))
	{
		g_Player[no].state = STATE_SLICE;
	}
}

// �A�j���[�V�������I��炷
void AnimFinPlayer(int no, int state)
{
	// ��ԑJ��
	{
		BOOL ans = TRUE;

		// ���ׂẴp�[�c�̍ő�e�[�u������ -1 �ɂȂ��Ă���ꍇ(���A�j���[�V�������I�����Ă���ꍇ)�A��ԑJ�ڂ�����
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[no][i].tblMax != -1) ans = FALSE;
		}

		if (ans == TRUE)
		{
			// �u�����h�A�j���[�V�������I������Ƃ���Ȃ�
			if (g_Interval)
			{
				for (int i = 0; i < PLAYER_PARTS_MAX; i++)
				{
					if (g_Parts[no][i].use == FALSE) continue;

					g_Parts[no][i].time = 0.0f;
					g_Parts[no][i].tblNo = i + (PLAYER_PARTS_MAX * state);
					g_Parts[no][i].tblMax = (int)g_TblAdr[g_Parts[no][i].tblNo]->size();
				}

				g_Player[no].setTbl = state;
				g_Interval = FALSE;			// �u�����h�A�j���[�V�������I��炷
			}
			else
			{
				g_Player[no].state = STATE_IDLE;
			}
		}
	}
}

// �A�j���[�V�������Z�b�g
void SetPlayerTblAnim(int no, int state)
{
	// ���̃X�e�[�g�ƖړI�̃X�e�[�g����v���ĂȂ����
	if (((g_Player[no].setTbl) != (state)) && (!g_Interval))
	{
		// �u�����h�A�j���[�V�������s��
		LoadBlendAnimTbl(no);
		g_Interval = TRUE;
		g_Player[no].attack = ATKSTATE_NONE;
	}
}

// �u�����h�A�j���[�V�����p�̃e�[�u���̒��g���쐬
void LoadBlendAnimTbl(int no)
{
	float frame = 10.0f;

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �e�[�u���̒��g���N���A
		g_Parts[no][i].blendTbl.clear();

		g_Parts[no][i].time = 0.0f;
		g_Parts[no][i].tblNo = i;

		// ���݂̃p�[�c�����e�[�u���ɒǉ�
		g_Parts[no][i].blendTbl.push_back(
			{
			XMFLOAT3(g_Parts[no][i].pos.x, g_Parts[no][i].pos.y, g_Parts[no][i].pos.z),
			XMFLOAT3(g_Parts[no][i].rot.x, g_Parts[no][i].rot.y, g_Parts[no][i].rot.z),
			XMFLOAT3(g_Parts[no][i].scl.x, g_Parts[no][i].scl.y, g_Parts[no][i].scl.z),
			frame
			}
		);

		// ���̃A�j���[�V�����̃p�[�c�̍ŏ��̏����e�[�u���ɒǉ�
		INTERPOLATION_DATA nextData = g_TblAdr[i + (PLAYER_PARTS_MAX * g_Player[no].state)]->at(0);
		g_Parts[no][i].blendTbl.push_back(
			{
			XMFLOAT3(nextData.pos.x, nextData.pos.y, nextData.pos.z),
			XMFLOAT3(nextData.rot.x, nextData.rot.y, nextData.rot.z),
			XMFLOAT3(nextData.scl.x, nextData.scl.y, nextData.scl.z),
			1.0f
			}
		);
		g_Parts[no][i].tblMax = (int)g_Parts[no][i].blendTbl.size();
	}
}

void PlayerAnimation(int i)
{
	// �K�w�A�j���[�V����
	for (int j = 0; j < PLAYER_PARTS_MAX; j++)
	{
		// �ʏ�̐��`���
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[i][j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i][j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;				// �ړ���e�[�u���̔ԍ������߂Ă���
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[i][j].tblNo];	// �s���e�[�u���̃A�h���X���擾(vector�^��)

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[i][j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���

			if ((int)g_Parts[i][j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[i][j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���

				switch (g_Player[i].setTbl)
				{
				case STATE_ATK:
				case STATE_SLICE:
					g_Parts[i][j].tblMax = -1;

					break;
				}
			}
		}
	}

	if (g_Player[i].attack != ATKSTATE_HIT)					// �U�����܂��������ĂȂ����
	{
		switch (g_Player[i].state)
		{
		case STATE_ATK:
			if (g_Parts[i][PLAYERPARTS_SWORD].time > 3) g_Player[i].attack = ATKSTATE_NOHIT;
			break;

		case STATE_SLICE:
			if (g_Parts[i][PLAYERPARTS_BODY].time < 1) g_Player[i].attack = ATKSTATE_NOHIT;
			break;
		}
	}
}

void PlayerBlendAnimation(int i)
{
	// �K�w�A�j���[�V����
	for (int j = 0; j < PLAYER_PARTS_MAX; j++)
	{
		// �A�j���[�V�����̃u�����h�p���`���
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[i][j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i][j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;				// �ړ���e�[�u���̔ԍ������߂Ă���
			std::vector<INTERPOLATION_DATA> tbl = g_Parts[i][j].blendTbl;	// �s���e�[�u���̃A�h���X���擾(vector�^��)

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[i][j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���

			if ((int)g_Parts[i][j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[i][j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				g_Parts[i][j].tblMax = -1;
			}
		}
	}
}

//=============================================================================
// �����蔻��֌W
//=============================================================================
//�q�p�[�c�̃��[���h���W
XMFLOAT3 ChildPlayerPartsPos(int no, int index)
{
	// �p�[�c�̎q�̕t�����̍��W�����߂Ă���
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[no][index].mtxWorld);

	// ���߂����W�����Ă�
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//�q�p�[�c�̃��[���h���W��Ԃ�
	return pos;
}

// �{�X�ɓ���������
void PushedPlayer(int no)
{
	// �����������Ɍ������Ĉړ�����
	g_Player[no].pos.x += sinf(g_Player[no].rot.y) * g_Push.spd;
	g_Player[no].pos.z += cosf(g_Player[no].rot.y) * g_Push.spd;
}

//=============================================================================
// �v���C���[�̃X�e�[�^�X�֌W
//=============================================================================
float GetPlayerHP(int no)
{
	float percent = g_Player[no].hp / PLAYER_HPMAX;

	return percent;
}

float GetPlayerEnergy(int no)
{
	float percent = g_Player[no].energy / PLAYER_ENERGYMAX;

	return percent;
}

void SetPlayerPush(float rot, float speed)
{
	g_Push.spd = speed;
	g_Push.dir = rot;
	g_Push.pushed = TRUE;
}

void DamagePlayer(int no, int damage)
{
	// ���G���Ԃ���Ȃ����
	if (!g_Damaged)
	{
		g_Player[no].hp -= damage;
		g_Damaged = TRUE;

		// hp���Ȃ��Ȃ�����FALSE��
		if (g_Player[no].hp <= 0.0f)
		{
			DECO* deco = GetDeco();

			g_Player[no].use = FALSE;
			SetMonochromeEnable(true);
			deco[DECO_FAILED].use = TRUE;
			SetSoundFade(FADE_OUT, SOUND_LABEL_SE_gameOver);
		}
	}
}