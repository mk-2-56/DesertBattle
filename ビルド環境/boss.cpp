//=============================================================================
//
// �{�X���� [boss.cpp]
// Author : GP11A 015 �����ق̍�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "keyframe.h"
#include "meshfield.h"
#include "boss.h"
#include "player.h"
#include "sound.h"
#include "fade.h"
#include "deco.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define BOSS_HEIGHT			(170.0f)					// �{�X�̐g��
#define BOSS_SWIMY			(-60.0f)					// �j���ł�Ƃ���y���W
#define SPEED_UNDERGROUND	(7.0f)						// �n���ł̃z�[�~���O�X�s�[�h
#define TIME_UNDERGROUND	(120.0f)					// �ő�n���z�[�~���O�^�C��
#define BOSS_OFFSET_Y		(3.0f)						// �G�l�~�[�̑��������킹��
#define BOSS_GRAVITY		(10.0f)						// �{�X�ɂ�����d�͂̑傫��
#define BOSS_UPPOWER		(17.0f)						// �n�ʂ����яo����
#define COUNTMAX_WALK		(60.0f)						// �����J�E���gMAX
#define COUNTMAX_WALK2		(160.0f)					// �����J�E���gMAX2
#define COUNTMAX_IDLE		(30.0f)						// �ҋ@�J�E���gMAX
#define COUNTMAX_SWIM		(1000.0f)					// �j���J�E���gMAX

#define ELLIPSE_LONGR		(950.0f)					// �ȉ~�̒���
#define ELLIPSE_SHORTR		(550.0f)					// �ȉ~�̒���
#define SWIM_SPEED			(800.0f)					// �j���X�s�[�h

// �A�j���[�V�����̃��X�g
enum BOSSANIM_LIST
{
	// �ҋ@���
	ANIM_IDLE_LIST_BODY = 0,
	ANIM_IDLE_LIST_HEAD,
	ANIM_IDLE_LIST_LARMTOP,
	ANIM_IDLE_LIST_LARMBTM,
	ANIM_IDLE_LIST_RARMTOP,
	ANIM_IDLE_LIST_RARMBTM,
	ANIM_IDLE_LIST_LLEGTOP,
	ANIM_IDLE_LIST_LLEGBTM,
	ANIM_IDLE_LIST_RLEGTOP,
	ANIM_IDLE_LIST_RLEGBTM,
	ANIM_IDLE_LIST_LWING,
	ANIM_IDLE_LIST_RWING,
	ANIM_IDLE_LIST_TAIL,

	// �@����
	ANIM_DIG_LIST_BODY,
	ANIM_DIG_LIST_HEAD,
	ANIM_DIG_LIST_LARMTOP,
	ANIM_DIG_LIST_LARMBTM,
	ANIM_DIG_LIST_RARMTOP,
	ANIM_DIG_LIST_RARMBTM,
	ANIM_DIG_LIST_LLEGTOP,
	ANIM_DIG_LIST_LLEGBTM,
	ANIM_DIG_LIST_RLEGTOP,
	ANIM_DIG_LIST_RLEGBTM,
	ANIM_DIG_LIST_LWING,
	ANIM_DIG_LIST_RWING,
	ANIM_DIG_LIST_TAIL,

	// �n�ʂ����яo�����
	ANIM_JUMOPUT_LIST_BODY,
	ANIM_JUMOPUT_LIST_HEAD,
	ANIM_JUMOPUT_LIST_LARMTOP,
	ANIM_JUMOPUT_LIST_LARMBTM,
	ANIM_JUMOPUT_LIST_RARMTOP,
	ANIM_JUMOPUT_LIST_RARMBTM,
	ANIM_JUMOPUT_LIST_LLEGTOP,
	ANIM_JUMOPUT_LIST_LLEGBTM,
	ANIM_JUMOPUT_LIST_RLEGTOP,
	ANIM_JUMOPUT_LIST_RLEGBTM,
	ANIM_JUMOPUT_LIST_LWING,
	ANIM_JUMOPUT_LIST_RWING,
	ANIM_JUMOPUT_LIST_TAIL,

	// �����ω�
	ANIM_CHANGE_LIST_BODY,
	ANIM_CHANGE_LIST_HEAD,
	ANIM_CHANGE_LIST_LARMTOP,
	ANIM_CHANGE_LIST_LARMBTM,
	ANIM_CHANGE_LIST_RARMTOP,
	ANIM_CHANGE_LIST_RARMBTM,
	ANIM_CHANGE_LIST_LLEGTOP,
	ANIM_CHANGE_LIST_LLEGBTM,
	ANIM_CHANGE_LIST_RLEGTOP,
	ANIM_CHANGE_LIST_RLEGBTM,
	ANIM_CHANGE_LIST_LWING,
	ANIM_CHANGE_LIST_RWING,
	ANIM_CHANGE_LIST_TAIL,

	// �j��
	ANIM_SWIM_LIST_BODY,
	ANIM_SWIM_LIST_HEAD,
	ANIM_SWIM_LIST_LARMTOP,
	ANIM_SWIM_LIST_LARMBTM,
	ANIM_SWIM_LIST_RARMTOP,
	ANIM_SWIM_LIST_RARMBTM,
	ANIM_SWIM_LIST_LLEGTOP,
	ANIM_SWIM_LIST_LLEGBTM,
	ANIM_SWIM_LIST_RLEGTOP,
	ANIM_SWIM_LIST_RLEGBTM,
	ANIM_SWIM_LIST_LWING,
	ANIM_SWIM_LIST_RWING,
	ANIM_SWIM_LIST_TAIL,

	// ���
	ANIM_FLY_LIST_BODY,
	ANIM_FLY_LIST_HEAD,
	ANIM_FLY_LIST_LARMTOP,
	ANIM_FLY_LIST_LARMBTM,
	ANIM_FLY_LIST_RARMTOP,
	ANIM_FLY_LIST_RARMBTM,
	ANIM_FLY_LIST_LLEGTOP,
	ANIM_FLY_LIST_LLEGBTM,
	ANIM_FLY_LIST_RLEGTOP,
	ANIM_FLY_LIST_RLEGBTM,
	ANIM_FLY_LIST_LWING,
	ANIM_FLY_LIST_RWING,
	ANIM_FLY_LIST_TAIL,

	// ����
	ANIM_WALK_LIST_BODY,
	ANIM_WALK_LIST_HEAD,
	ANIM_WALK_LIST_LARMTOP,
	ANIM_WALK_LIST_LARMBTM,
	ANIM_WALK_LIST_RARMTOP,
	ANIM_WALK_LIST_RARMBTM,
	ANIM_WALK_LIST_LLEGTOP,
	ANIM_WALK_LIST_LLEGBTM,
	ANIM_WALK_LIST_RLEGTOP,
	ANIM_WALK_LIST_RLEGBTM,
	ANIM_WALK_LIST_LWING,
	ANIM_WALK_LIST_RWING,
	ANIM_WALK_LIST_TAIL,

	// �W�����v�U��
	ANIM_JUMP_LIST_BODY,
	ANIM_JUMP_LIST_HEAD,
	ANIM_JUMP_LIST_LARMTOP,
	ANIM_JUMP_LIST_LARMBTM,
	ANIM_JUMP_LIST_RARMTOP,
	ANIM_JUMP_LIST_RARMBTM,
	ANIM_JUMP_LIST_LLEGTOP,
	ANIM_JUMP_LIST_LLEGBTM,
	ANIM_JUMP_LIST_RLEGTOP,
	ANIM_JUMP_LIST_RLEGBTM,
	ANIM_JUMP_LIST_LWING,
	ANIM_JUMP_LIST_RWING,
	ANIM_JUMP_LIST_TAIL,

	BOSSANIM_LIST_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void AnimFinBoss(int no, int state);
void SetBossTblAnim(int no, int state);
void LoadBossBlendAnimTbl(int no);
void BossAnimation(int i);
void BossBlendAnimation(int i);
void StartBossAnim(int no, int state);
void ChangeBossAttr(int no);
void MoveUnderGround(int no);
void MoveJumpOut(int no);
void MoveSwimming(int no);
void MoveFlying(int no);
void MoveWalking(int no);
void HomingPlayer(int no, BOOL dirOnly);
void BackingPosition(int no);
void SearchPlayer(int no, int attribute);
void ActionSand(int no, float len, int nextAct);
void ActionWater(int no, float len, int nextAct);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOSS			g_Boss[MAX_BOSS];								// �{�X�����X�^�[
static BOSS			g_Parts[MAX_BOSS][BOSSPARTS_MAX];				// �{�X�̃p�[�c�p
static BOSS_MODEL	g_Model[MAX_BOSS][BOSSPARTS_MAX];
static ID3D11ShaderResourceView* g_Texture[BOSSATTR_MAX] = { NULL };	// �e�N�X�`�����

static int			g_Boss_load = 0;
static float		g_actionCnt = 0;
static float		g_flyingCnt = 0;
static float		g_len;
static BOOL			g_disappearing;
static BOOL			g_Interval;
static BOOL			g_Gravity;
static BOOL			g_waterOut;										// ������オ�����ォ

static char* g_TextureName[BOSSATTR_MAX] = {
	"data/MODEL/boss/YellowSkin.png",
	"data/MODEL/boss/BlueSkin.png",
	// "data/MODEL/boss/RedSkin.png",
};

// �A�j���[�V�����̃f�B���N�g���̔z��
static char* g_AnimDir[] =
{
	// �ҋ@���
	"data/ANIMDATA/boss/idle/boss_body.csv",
	"data/ANIMDATA/boss/idle/boss_head.csv",
	"data/ANIMDATA/boss/idle/boss_armLtop.csv",
	"data/ANIMDATA/boss/idle/boss_armLbtm.csv",
	"data/ANIMDATA/boss/idle/boss_armRtop.csv",
	"data/ANIMDATA/boss/idle/boss_armRbtm.csv",
	"data/ANIMDATA/boss/idle/boss_legLtop.csv",
	"data/ANIMDATA/boss/idle/boss_legLbtm.csv",
	"data/ANIMDATA/boss/idle/boss_legRtop.csv",
	"data/ANIMDATA/boss/idle/boss_legRbtm.csv",
	"data/ANIMDATA/boss/idle/boss_wingL.csv",
	"data/ANIMDATA/boss/idle/boss_wingR.csv",
	"data/ANIMDATA/boss/idle/boss_tail.csv",

	// �@����
	"data/ANIMDATA/boss/dig/boss_body.csv",
	"data/ANIMDATA/boss/dig/boss_head.csv",
	"data/ANIMDATA/boss/dig/boss_armLtop.csv",
	"data/ANIMDATA/boss/dig/boss_armLbtm.csv",
	"data/ANIMDATA/boss/dig/boss_armRtop.csv",
	"data/ANIMDATA/boss/dig/boss_armRbtm.csv",
	"data/ANIMDATA/boss/dig/boss_legLtop.csv",
	"data/ANIMDATA/boss/dig/boss_legLbtm.csv",
	"data/ANIMDATA/boss/dig/boss_legRtop.csv",
	"data/ANIMDATA/boss/dig/boss_legRbtm.csv",
	"data/ANIMDATA/boss/dig/boss_wingL.csv",
	"data/ANIMDATA/boss/dig/boss_wingR.csv",
	"data/ANIMDATA/boss/dig/boss_tail.csv",

	// �n�ʂ����яo�����
	"data/ANIMDATA/boss/jumpOut/boss_body.csv",
	"data/ANIMDATA/boss/jumpOut/boss_head.csv",
	"data/ANIMDATA/boss/jumpOut/boss_armLtop.csv",
	"data/ANIMDATA/boss/jumpOut/boss_armLbtm.csv",
	"data/ANIMDATA/boss/jumpOut/boss_armRtop.csv",
	"data/ANIMDATA/boss/jumpOut/boss_armRbtm.csv",
	"data/ANIMDATA/boss/jumpOut/boss_legLtop.csv",
	"data/ANIMDATA/boss/jumpOut/boss_legLbtm.csv",
	"data/ANIMDATA/boss/jumpOut/boss_legRtop.csv",
	"data/ANIMDATA/boss/jumpOut/boss_legRbtm.csv",
	"data/ANIMDATA/boss/jumpOut/boss_wingL.csv",
	"data/ANIMDATA/boss/jumpOut/boss_wingR.csv",
	"data/ANIMDATA/boss/jumpOut/boss_tail.csv",

	// �����ω���
	"data/ANIMDATA/boss/change/boss_body.csv",
	"data/ANIMDATA/boss/change/boss_head.csv",
	"data/ANIMDATA/boss/change/boss_armLtop.csv",
	"data/ANIMDATA/boss/change/boss_armLbtm.csv",
	"data/ANIMDATA/boss/change/boss_armRtop.csv",
	"data/ANIMDATA/boss/change/boss_armRbtm.csv",
	"data/ANIMDATA/boss/change/boss_legLtop.csv",
	"data/ANIMDATA/boss/change/boss_legLbtm.csv",
	"data/ANIMDATA/boss/change/boss_legRtop.csv",
	"data/ANIMDATA/boss/change/boss_legRbtm.csv",
	"data/ANIMDATA/boss/change/boss_wingL.csv",
	"data/ANIMDATA/boss/change/boss_wingR.csv",
	"data/ANIMDATA/boss/change/boss_tail.csv",

	// �j��
	"data/ANIMDATA/boss/swim/boss_body.csv",
	"data/ANIMDATA/boss/swim/boss_head.csv",
	"data/ANIMDATA/boss/swim/boss_armLtop.csv",
	"data/ANIMDATA/boss/swim/boss_armLbtm.csv",
	"data/ANIMDATA/boss/swim/boss_armRtop.csv",
	"data/ANIMDATA/boss/swim/boss_armRbtm.csv",
	"data/ANIMDATA/boss/swim/boss_legLtop.csv",
	"data/ANIMDATA/boss/swim/boss_legLbtm.csv",
	"data/ANIMDATA/boss/swim/boss_legRtop.csv",
	"data/ANIMDATA/boss/swim/boss_legRbtm.csv",
	"data/ANIMDATA/boss/swim/boss_wingL.csv",
	"data/ANIMDATA/boss/swim/boss_wingR.csv",
	"data/ANIMDATA/boss/swim/boss_tail.csv",

	// ���
	"data/ANIMDATA/boss/fly/boss_body.csv",
	"data/ANIMDATA/boss/fly/boss_head.csv",
	"data/ANIMDATA/boss/fly/boss_armLtop.csv",
	"data/ANIMDATA/boss/fly/boss_armLbtm.csv",
	"data/ANIMDATA/boss/fly/boss_armRtop.csv",
	"data/ANIMDATA/boss/fly/boss_armRbtm.csv",
	"data/ANIMDATA/boss/fly/boss_legLtop.csv",
	"data/ANIMDATA/boss/fly/boss_legLbtm.csv",
	"data/ANIMDATA/boss/fly/boss_legRtop.csv",
	"data/ANIMDATA/boss/fly/boss_legRbtm.csv",
	"data/ANIMDATA/boss/fly/boss_wingL.csv",
	"data/ANIMDATA/boss/fly/boss_wingR.csv",
	"data/ANIMDATA/boss/fly/boss_tail.csv",

	// ����
	"data/ANIMDATA/boss/walk/boss_body.csv",
	"data/ANIMDATA/boss/walk/boss_head.csv",
	"data/ANIMDATA/boss/walk/boss_armLtop.csv",
	"data/ANIMDATA/boss/walk/boss_armLbtm.csv",
	"data/ANIMDATA/boss/walk/boss_armRtop.csv",
	"data/ANIMDATA/boss/walk/boss_armRbtm.csv",
	"data/ANIMDATA/boss/walk/boss_legLtop.csv",
	"data/ANIMDATA/boss/walk/boss_legLbtm.csv",
	"data/ANIMDATA/boss/walk/boss_legRtop.csv",
	"data/ANIMDATA/boss/walk/boss_legRbtm.csv",
	"data/ANIMDATA/boss/walk/boss_wingL.csv",
	"data/ANIMDATA/boss/walk/boss_wingR.csv",
	"data/ANIMDATA/boss/walk/boss_tail.csv",

	// �W�����v�U��
	"data/ANIMDATA/boss/jump/boss_body.csv",
	"data/ANIMDATA/boss/jump/boss_head.csv",
	"data/ANIMDATA/boss/jump/boss_armLtop.csv",
	"data/ANIMDATA/boss/jump/boss_armLbtm.csv",
	"data/ANIMDATA/boss/jump/boss_armRtop.csv",
	"data/ANIMDATA/boss/jump/boss_armRbtm.csv",
	"data/ANIMDATA/boss/jump/boss_legLtop.csv",
	"data/ANIMDATA/boss/jump/boss_legLbtm.csv",
	"data/ANIMDATA/boss/jump/boss_legRtop.csv",
	"data/ANIMDATA/boss/jump/boss_legRbtm.csv",
	"data/ANIMDATA/boss/jump/boss_wingL.csv",
	"data/ANIMDATA/boss/jump/boss_wingR.csv",
	"data/ANIMDATA/boss/jump/boss_tail.csv",
};

// ���f���f�[�^�̃f�B���N�g��
static char* g_ModelAdr[] =
{
	"data/MODEL/boss/body.obj",
	"data/MODEL/boss/head.obj",
	"data/MODEL/boss/armLtop.obj",
	"data/MODEL/boss/armLbtm.obj",
	"data/MODEL/boss/armRtop.obj",
	"data/MODEL/boss/armRbtm.obj",
	"data/MODEL/boss/legLtop.obj",
	"data/MODEL/boss/legLbtm.obj",
	"data/MODEL/boss/legRtop.obj",
	"data/MODEL/boss/legRbtm.obj",
	"data/MODEL/boss/wingL.obj",
	"data/MODEL/boss/wingR.obj",
	"data/MODEL/boss/tail.obj",
};

// �G�l�~�[�̊K�w�A�j���[�V�����f�[�^
// vector�^��
// �ҋ@���
static std::vector<INTERPOLATION_DATA> idle_body;
static std::vector<INTERPOLATION_DATA> idle_head;
static std::vector<INTERPOLATION_DATA> idle_lArmTop;
static std::vector<INTERPOLATION_DATA> idle_lArmBtm;
static std::vector<INTERPOLATION_DATA> idle_rArmTop;
static std::vector<INTERPOLATION_DATA> idle_rArmBtm;
static std::vector<INTERPOLATION_DATA> idle_lLegTop;
static std::vector<INTERPOLATION_DATA> idle_lLegBtm;
static std::vector<INTERPOLATION_DATA> idle_rLegTop;
static std::vector<INTERPOLATION_DATA> idle_rLegBtm;
static std::vector<INTERPOLATION_DATA> idle_lWing;
static std::vector<INTERPOLATION_DATA> idle_rWing;
static std::vector<INTERPOLATION_DATA> idle_tail;

// �@����
static std::vector<INTERPOLATION_DATA> dig_body;
static std::vector<INTERPOLATION_DATA> dig_head;
static std::vector<INTERPOLATION_DATA> dig_lArmTop;
static std::vector<INTERPOLATION_DATA> dig_lArmBtm;
static std::vector<INTERPOLATION_DATA> dig_rArmTop;
static std::vector<INTERPOLATION_DATA> dig_rArmBtm;
static std::vector<INTERPOLATION_DATA> dig_lLegTop;
static std::vector<INTERPOLATION_DATA> dig_lLegBtm;
static std::vector<INTERPOLATION_DATA> dig_rLegTop;
static std::vector<INTERPOLATION_DATA> dig_rLegBtm;
static std::vector<INTERPOLATION_DATA> dig_lWing;
static std::vector<INTERPOLATION_DATA> dig_rWing;
static std::vector<INTERPOLATION_DATA> dig_tail;

// �n�ʂ���̔�яo�����
static std::vector<INTERPOLATION_DATA> jumpOut_body;
static std::vector<INTERPOLATION_DATA> jumpOut_head;
static std::vector<INTERPOLATION_DATA> jumpOut_lArmTop;
static std::vector<INTERPOLATION_DATA> jumpOut_lArmBtm;
static std::vector<INTERPOLATION_DATA> jumpOut_rArmTop;
static std::vector<INTERPOLATION_DATA> jumpOut_rArmBtm;
static std::vector<INTERPOLATION_DATA> jumpOut_lLegTop;
static std::vector<INTERPOLATION_DATA> jumpOut_lLegBtm;
static std::vector<INTERPOLATION_DATA> jumpOut_rLegTop;
static std::vector<INTERPOLATION_DATA> jumpOut_rLegBtm;
static std::vector<INTERPOLATION_DATA> jumpOut_lWing;
static std::vector<INTERPOLATION_DATA> jumpOut_rWing;
static std::vector<INTERPOLATION_DATA> jumpOut_tail;

// �����ω�
static std::vector<INTERPOLATION_DATA> change_body;
static std::vector<INTERPOLATION_DATA> change_head;
static std::vector<INTERPOLATION_DATA> change_lArmTop;
static std::vector<INTERPOLATION_DATA> change_lArmBtm;
static std::vector<INTERPOLATION_DATA> change_rArmTop;
static std::vector<INTERPOLATION_DATA> change_rArmBtm;
static std::vector<INTERPOLATION_DATA> change_lLegTop;
static std::vector<INTERPOLATION_DATA> change_lLegBtm;
static std::vector<INTERPOLATION_DATA> change_rLegTop;
static std::vector<INTERPOLATION_DATA> change_rLegBtm;
static std::vector<INTERPOLATION_DATA> change_lWing;
static std::vector<INTERPOLATION_DATA> change_rWing;
static std::vector<INTERPOLATION_DATA> change_tail;

// �j��
static std::vector<INTERPOLATION_DATA> swim_body;
static std::vector<INTERPOLATION_DATA> swim_head;
static std::vector<INTERPOLATION_DATA> swim_lArmTop;
static std::vector<INTERPOLATION_DATA> swim_lArmBtm;
static std::vector<INTERPOLATION_DATA> swim_rArmTop;
static std::vector<INTERPOLATION_DATA> swim_rArmBtm;
static std::vector<INTERPOLATION_DATA> swim_lLegTop;
static std::vector<INTERPOLATION_DATA> swim_lLegBtm;
static std::vector<INTERPOLATION_DATA> swim_rLegTop;
static std::vector<INTERPOLATION_DATA> swim_rLegBtm;
static std::vector<INTERPOLATION_DATA> swim_lWing;
static std::vector<INTERPOLATION_DATA> swim_rWing;
static std::vector<INTERPOLATION_DATA> swim_tail;

// ���
static std::vector<INTERPOLATION_DATA> fly_body;
static std::vector<INTERPOLATION_DATA> fly_head;
static std::vector<INTERPOLATION_DATA> fly_lArmTop;
static std::vector<INTERPOLATION_DATA> fly_lArmBtm;
static std::vector<INTERPOLATION_DATA> fly_rArmTop;
static std::vector<INTERPOLATION_DATA> fly_rArmBtm;
static std::vector<INTERPOLATION_DATA> fly_lLegTop;
static std::vector<INTERPOLATION_DATA> fly_lLegBtm;
static std::vector<INTERPOLATION_DATA> fly_rLegTop;
static std::vector<INTERPOLATION_DATA> fly_rLegBtm;
static std::vector<INTERPOLATION_DATA> fly_lWing;
static std::vector<INTERPOLATION_DATA> fly_rWing;
static std::vector<INTERPOLATION_DATA> fly_tail;

// ����
static std::vector<INTERPOLATION_DATA> walk_body;
static std::vector<INTERPOLATION_DATA> walk_head;
static std::vector<INTERPOLATION_DATA> walk_lArmTop;
static std::vector<INTERPOLATION_DATA> walk_lArmBtm;
static std::vector<INTERPOLATION_DATA> walk_rArmTop;
static std::vector<INTERPOLATION_DATA> walk_rArmBtm;
static std::vector<INTERPOLATION_DATA> walk_lLegTop;
static std::vector<INTERPOLATION_DATA> walk_lLegBtm;
static std::vector<INTERPOLATION_DATA> walk_rLegTop;
static std::vector<INTERPOLATION_DATA> walk_rLegBtm;
static std::vector<INTERPOLATION_DATA> walk_lWing;
static std::vector<INTERPOLATION_DATA> walk_rWing;
static std::vector<INTERPOLATION_DATA> walk_tail;

// �W�����v�U��
static std::vector<INTERPOLATION_DATA> jump_body;
static std::vector<INTERPOLATION_DATA> jump_head;
static std::vector<INTERPOLATION_DATA> jump_lArmTop;
static std::vector<INTERPOLATION_DATA> jump_lArmBtm;
static std::vector<INTERPOLATION_DATA> jump_rArmTop;
static std::vector<INTERPOLATION_DATA> jump_rArmBtm;
static std::vector<INTERPOLATION_DATA> jump_lLegTop;
static std::vector<INTERPOLATION_DATA> jump_lLegBtm;
static std::vector<INTERPOLATION_DATA> jump_rLegTop;
static std::vector<INTERPOLATION_DATA> jump_rLegBtm;
static std::vector<INTERPOLATION_DATA> jump_lWing;
static std::vector<INTERPOLATION_DATA> jump_rWing;
static std::vector<INTERPOLATION_DATA> jump_tail;

static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// �ҋ@���
	&idle_body,
	&idle_head,
	&idle_lArmTop,
	&idle_lArmBtm,
	&idle_rArmTop,
	&idle_rArmBtm,
	&idle_lLegTop,
	&idle_lLegBtm,
	&idle_rLegTop,
	&idle_rLegBtm,
	&idle_lWing,
	&idle_rWing,
	&idle_tail,

	// �@����
	&dig_body,
	&dig_head,
	&dig_lArmTop,
	&dig_lArmBtm,
	&dig_rArmTop,
	&dig_rArmBtm,
	&dig_lLegTop,
	&dig_lLegBtm,
	&dig_rLegTop,
	&dig_rLegBtm,
	&dig_lWing,
	&dig_rWing,
	&dig_tail,

	// �n�ʂ���̔�яo�����
	&jumpOut_body,
	&jumpOut_head,
	&jumpOut_lArmTop,
	&jumpOut_lArmBtm,
	&jumpOut_rArmTop,
	&jumpOut_rArmBtm,
	&jumpOut_lLegTop,
	&jumpOut_lLegBtm,
	&jumpOut_rLegTop,
	&jumpOut_rLegBtm,
	&jumpOut_lWing,
	&jumpOut_rWing,
	&jumpOut_tail,

	// �����ω�
	&change_body,
	&change_head,
	&change_lArmTop,
	&change_lArmBtm,
	&change_rArmTop,
	&change_rArmBtm,
	&change_lLegTop,
	&change_lLegBtm,
	&change_rLegTop,
	&change_rLegBtm,
	&change_lWing,
	&change_rWing,
	&change_tail,

	// �j��
	&swim_body,
	&swim_head,
	&swim_lArmTop,
	&swim_lArmBtm,
	&swim_rArmTop,
	&swim_rArmBtm,
	&swim_lLegTop,
	&swim_lLegBtm,
	&swim_rLegTop,
	&swim_rLegBtm,
	&swim_lWing,
	&swim_rWing,
	&swim_tail,

	// ���
	&fly_body,
	&fly_head,
	&fly_lArmTop,
	&fly_lArmBtm,
	&fly_rArmTop,
	&fly_rArmBtm,
	&fly_lLegTop,
	&fly_lLegBtm,
	&fly_rLegTop,
	&fly_rLegBtm,
	&fly_lWing,
	&fly_rWing,
	&fly_tail,

	// ����
	&walk_body,
	&walk_head,
	&walk_lArmTop,
	&walk_lArmBtm,
	&walk_rArmTop,
	&walk_rArmBtm,
	&walk_lLegTop,
	&walk_lLegBtm,
	&walk_rLegTop,
	&walk_rLegBtm,
	&walk_lWing,
	&walk_rWing,
	&walk_tail,

	// �W�����v�U��
	&jump_body,
	&jump_head,
	&jump_lArmTop,
	&jump_lArmBtm,
	&jump_rArmTop,
	&jump_rArmBtm,
	&jump_lLegTop,
	&jump_lLegBtm,
	&jump_rLegTop,
	&jump_rLegBtm,
	&jump_lWing,
	&jump_rWing,
	&jump_tail,
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBoss(void)
{
	g_waterOut = FALSE;

	// �e�N�X�`������
	for (int i = 0; i < BOSSATTR_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// ���f���̓Ǎ�
	for (int i = 0; i < MAX_BOSS; i++)
	{
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			LoadModel(g_ModelAdr[j], &g_Model[i][j].model);
			g_Model[i][j].load = TRUE;
		}
	}

	// �A�j���[�V�����f�[�^���t�@�C������ǂݍ���
	for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}

	// �{�X�̏�����
	for (int i = 0; i < MAX_BOSS; i++)
	{
		g_Boss[i].pos = XMFLOAT3(0.0f, 0.0f, 100.0f);
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(0.022f, 0.022f, 0.022f);

		g_Boss[i].spd = 0.0f;						// �ړ��X�s�[�h�N���A
		g_Boss[i].hp = MAX_BOSSHP;

		g_Boss[i].use = TRUE;						// TRUE:�����Ă�
		g_Boss[i].underGround = FALSE;				// �����ĂȂ�

		g_Boss[i].state = BOSSSTATE_IDLE;
		g_Boss[i].setTbl = BOSSSTATE_IDLE;
		g_Boss[i].attribute = BOSSATTR_SAND;		// �ŏ��̑����͓y

		// �f�B�]����
		g_Boss[i].dissolve.Enable = TRUE;			// �f�B�]������L��
		g_Boss[i].dissolve.ColorType = g_Boss[i].attribute;
		g_Boss[i].dissolve.threshold = 0.0f;		// �f�B�]�����̂������l

		g_Boss[i].parent = NULL;					// �{�́i�e�j�Ȃ̂�NULL������


		// �K�w�A�j���[�V�����̏�����
		for (int j = 0; j < BOSSPARTS_MAX; j++)
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
			g_Parts[i][j].time = 0.0f;												// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[i][j].tblNo = j + ANIM_IDLE_LIST_BODY;							// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[i][j].tblMax = (int)g_TblAdr[g_Parts[i][j].tblNo]->size();		// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

			// �e�q�֌W
			switch (j)
			{
			case BOSSPARTS_BODY:
				g_Parts[i][j].parent = &g_Boss[i];
				break;

			case BOSSPARTS_LARMBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_LARMTOP];
				break;

			case BOSSPARTS_RARMBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_RARMTOP];
				break;

			case BOSSPARTS_LLEGBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_LLEGTOP];
				break;

			case BOSSPARTS_RLEGBTM:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_RLEGTOP];
				break;

			default:
				g_Parts[i][j].parent = &g_Parts[i][BOSSPARTS_BODY];
				break;
			}
		}

		// �N�H�[�^�j�I���̏�����
		XMStoreFloat4(&g_Boss[i].Quaternion, XMQuaternionIdentity());


	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBoss(void)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		// ���f���̉������
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			if (g_Model[i][j].load == FALSE) continue;

			UnloadModel(&g_Model[i][j].model);
			g_Model[i][j].load = FALSE;
		}

		// �A�j���[�V�����e�[�u�����t�@�C�����珉����
		for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
		{
			g_TblAdr[i]->clear();
		}

	}
	// �e�N�X�`���̉��
	for (int i = 0; i < BOSSATTR_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBoss(void)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		if (g_Boss[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			g_Boss[i].attack = FALSE;	// �����l�͍U�����ł͂Ȃ�
			g_Boss[i].oldPos = g_Boss[i].pos;			// �O��poos��ۑ����Ă���

			switch (g_Boss[i].state)					// ���݂̃X�e�[�g�ɂ���đ���𕪂���
			{
			case BOSSSTATE_IDLE:
				if (g_Boss[i].underGround)
				{
					MoveUnderGround(i);					// �n���̎��͐���s��
				}
				else if(!GetFieldChange())
				{
					if (g_actionCnt > COUNTMAX_IDLE) SearchPlayer(i, g_Boss[i].attribute);
				}
				break;

			case BOSSSTATE_JUMPOUT:
				MoveJumpOut(i);
				break;

			case BOSSSTATE_CHANGE:
				ChangeBossAttr(i);
				break;

			case BOSSSTATE_SWIM:
				MoveSwimming(i);
				break;

			case BOSSSTATE_FLY:
				g_Boss[i].spd = SPEED_UNDERGROUND / 2;
				MoveFlying(i);
				break;

			case BOSSSTATE_WALK:
				g_Boss[i].spd = SPEED_UNDERGROUND / 3;
				MoveWalking(i);
				break;

			default:
				break;
			}

			g_actionCnt++;
			// �A�j���[�V�������Z�b�g
			SetBossTblAnim(i, g_Boss[i].state);
			AnimFinBoss(i, g_Boss[i].state);

			// �u�����h�����ǂ����Ő��`��Ԃ��킯��
			(g_Interval) ? BossBlendAnimation(i) : BossAnimation(i);
			
			// �n�ʂƂ̔���n
			if(g_Boss[i].state != BOSSSTATE_SWIM)
			{
				// ���C�L���X�g���đ����̍��������߂�
				XMFLOAT3 HitPosition;		// ��_
				XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
				BOOL ans = RayHitField(g_Boss[i].pos, &HitPosition, &Normal);

				g_Gravity = TRUE;
				float distance = 0.0f;

				if (ans)
				{
					distance = g_Boss[i].pos.y - HitPosition.y;

					// �n�ʂɖ��܂�̂�h�~
					if (distance <= 10.0f)
					{
						g_Boss[i].pos.y = HitPosition.y + BOSS_OFFSET_Y;
						g_Gravity = FALSE;
					}
				}

				if ((g_Gravity == TRUE) && (g_Boss[i].state != BOSSSTATE_FLY))				// ��s���͏d�͖���
				{
					// �d�͂�t����
					g_Boss[i].pos.y -= BOSS_GRAVITY;
				}
			}

			// �t�B�[���h�O�ɂ͏o��Ȃ�
			if (((g_Boss[i].pos.x < GetFieldSize() / 2 * -1) || (g_Boss[i].pos.x > GetFieldSize() / 2)) ||
				((g_Boss[i].pos.z < GetFieldSize() / 2 * -1) || (g_Boss[i].pos.z > GetFieldSize() / 2)))
			{
				g_Boss[i].pos = g_Boss[i].oldPos;
			}

#ifdef _DEBUG
			PrintDebugProc("BOSSstate: %d BOSSAttack: %d\n", g_Boss[i].state, g_Boss[i].attack);
			PrintDebugProc("BOSSpos x:%f y:%f z:%f ����: %f actionCnt: %f flyingCnt: %f\n", g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z, g_len, g_actionCnt, g_flyingCnt);
#endif
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBoss(BOOL shadow)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		if (g_Boss[i].use)
		{

			SetDissolve(&g_Boss[i].dissolve);
			if(!shadow) SetAlphaTestEnable(g_Boss[i].dissolve.Enable);		// ���e�X�g��L����

			// �����̐F�Ƀe�N�X�`�������킹��
			GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[g_Boss[i].attribute]);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

			// �J�����O����
			SetCullingMode(CULL_MODE_NONE);

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Boss[i].scl.x, g_Boss[i].scl.y, g_Boss[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �N�H�[�^�j�I���𔽉f
			quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Boss[i].Quaternion));
			mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);

			// �K�w�A�j���[�V����
			for (int j = 0; j < BOSSPARTS_MAX; j++)
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
					// g_Enemy.mtxWorld���w���Ă���
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// �g���Ă���Ȃ珈������
				if (g_Parts[i][j].use == FALSE) continue;

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);

				// ���f���`��
				DrawModel(&g_Model[i][g_Parts[i][j].modelIdx].model);
			}

			// �J�����O�ݒ��߂�
			SetCullingMode(CULL_MODE_BACK);

			if(!shadow) SetAlphaTestEnable(FALSE);		// ���e�X�g�𖳌���
			SetDissolveEnable(FALSE);


		}
	}

}

//=============================================================================
// �{�X�̎擾
//=============================================================================
BOSS *GetBoss(void)
{
	return &g_Boss[0];
}

//=============================================================================
// �A�j���[�V�����֘A
//=============================================================================
// �A�j���[�V�������Z�b�g
void SetBossTblAnim(int no, int state)
{
	// ���̃X�e�[�g�ƖړI�̃X�e�[�g����v���ĂȂ����
	if (((g_Boss[no].setTbl) != (state)) && (!g_Interval))
	{
		// �u�����h�A�j���[�V�������s��
		LoadBossBlendAnimTbl(no);
		g_Interval = TRUE;
	}
}

// �A�j���[�V�������I��炷
void AnimFinBoss(int no, int state)
{
	// ��ԑJ��
	{
		BOOL ans = TRUE;

		// ���ׂẴp�[�c�̍ő�e�[�u������ -1 �ɂȂ��Ă���ꍇ(���A�j���[�V�������I�����Ă���ꍇ)�A��ԑJ�ڂ�����
		for (int i = 0; i < BOSSPARTS_MAX; i++)
		{
			if (g_Parts[no][i].tblMax != -1) ans = FALSE;
		}

		if (ans == TRUE)
		{
			// �u�����h�A�j���[�V�������I������Ƃ���Ȃ�
			if (g_Interval)
			{
				for (int i = 0; i < BOSSPARTS_MAX; i++)
				{
					if (g_Parts[no][i].use == FALSE) continue;

					g_Parts[no][i].time = 0.0f;
					g_Parts[no][i].tblNo = i + (BOSSPARTS_MAX * state);

					g_Parts[no][i].tblMax = (int)g_TblAdr[g_Parts[no][i].tblNo]->size();

				}
				g_Boss[no].setTbl = state;
				g_Interval = FALSE;							// �u�����h�A�j���[�V�������I��炷
			}
			else
			{
				switch (g_Boss[no].state)
				{
				case BOSSSTATE_DIG:
					g_Boss[no].underGround = TRUE;
					ResetBossAction();
					StartBossAnim(no, BOSSSTATE_IDLE);
					break;

				case BOSSSTATE_CHANGE:
					ResetField(g_Boss[no].attribute);		// �������ς��ƃt�B�[���h���ς��
					StartBossAnim(no, BOSSSTATE_IDLE);
					break;

				default:
					SearchPlayer(no, g_Boss[no].attribute);
					break;
				}
			}
		}
	}
}

// �A�j���J�n
void StartBossAnim(int no, int state)
{
	g_Boss[no].state = state;
	
	if (state == BOSSSTATE_CHANGE) g_disappearing = TRUE;
}

// �n����i��
void MoveUnderGround(int no)
{
	g_Boss[no].pos.y = -BOSS_HEIGHT;
	g_Boss[no].spd = SPEED_UNDERGROUND;
HomingPlayer(no, FALSE);
}

// �n�ʂ����яo��
void MoveJumpOut(int no)
{
	if (g_Parts[no][BOSSPARTS_BODY].time < 1)
	{
		g_Boss[no].pos.y += BOSS_UPPOWER;
		HomingPlayer(no, TRUE);
	}
}

// �j��
void MoveSwimming(int no)
{
	// �����͐����ɂ��Ă���
	g_Boss[no].pos.y = BOSS_SWIMY;

	// �ȉ~�̓����ŗ��̎�����j�����Ă���
	float longR = ELLIPSE_LONGR;					// �ȉ~�̒����̔��a
	float shortR = ELLIPSE_SHORTR;					// �ȉ~�̒Z���̔��a
	float centerX = 0.0f;							// �ȉ~�̒��S���W x
	float centerZ = 0.0f;							// �ȉ~�̒��S���W z
	float rot = XM_PI - 0.48f;						// �ȉ~���̂���]������

	// �p���x (1��]������̎���)
	float angularSpeed = XM_PI / SWIM_SPEED;		// 1�t���[���Ői�ރX�s�[�h

	// �ȉ~��̈ʒu���v�Z
	float angle = angularSpeed * g_actionCnt;
	float posX = (float)(centerX + (longR * cos(angle) * cos(rot)) - (shortR * sin(angle) * sin(rot)));
	float posZ = (float)(centerZ + (longR * cos(angle) * sin(rot)) + (shortR * sin(angle) * cos(rot)));

	// posX��posY���g���ăL�����N�^�[�̈ʒu���X�V
	g_Boss[no].pos.x = posX;
	g_Boss[no].pos.z = posZ;

	float dirX = (float)(shortR * cos(angle) * sin(rot) + longR * sin(angle) * cos(rot));
	float dirZ = (float)(longR * cos(angle) * cos(rot) - shortR * sin(angle) * sin(rot));

	g_Boss[no].rot.y = (float)(atan2(-dirZ, -dirX) - XM_PIDIV2);

	if (g_actionCnt > COUNTMAX_SWIM)
	{
		ResetBossAction();
		StartBossAnim(no, BOSSSTATE_WALK);
		g_waterOut = TRUE;
	}
}

// ���
void MoveFlying(int no)
{
	float diff;
	switch ((int)(g_flyingCnt))
	{
	case 0:
		diff = BOSS_HEIGHT - g_Boss[no].pos.y;
		g_Boss[no].pos.y += diff * 0.08f;

		// ��������������
		if (fabsf(diff) < 1.0f) g_flyingCnt++;
		break;

	case 1:
		BackingPosition(no);
		// �������������ʒu�ɖ߂�����
		if (fabsf(g_Boss[no].pos.x) < 1.5f)
		{
			g_Boss[no].rot.y = 0.0f;
			g_flyingCnt++;
		}
		break;

	case 2:
		diff = BOSS_OFFSET_Y - g_Boss[no].pos.y;
		g_Boss[no].pos.y += diff * 0.03f;

		// ��������������
		if (!g_Gravity)
		{
			StartBossAnim(no, BOSSSTATE_CHANGE);		// �X�^�[�g�ʒu�ɂ����̂ő����ω����n�߂�
			g_flyingCnt = 0.0f;
		}
		break;
	}
}

// ����
void MoveWalking(int no)
{
	g_Boss[no].pos.x -= sinf(g_Boss[no].rot.y) * g_Boss[no].spd;
	g_Boss[no].pos.z -= cosf(g_Boss[no].rot.y) * g_Boss[no].spd;

	if (g_Boss[no].attribute == BOSSATTR_SAND) g_Boss[no].rot.y += 0.005f;

	if (g_waterOut)
	{
		if (g_actionCnt > COUNTMAX_WALK2)
		{
			SearchPlayer(no, g_Boss[no].attribute);
			g_waterOut = FALSE;
		}
	}
	else if (g_actionCnt > COUNTMAX_WALK)
	{
		SearchPlayer(no, g_Boss[no].attribute);
	}
}

// �����`�F���W
void ChangeBossAttr(int no)
{
	g_Boss[no].pos.y = BOSS_OFFSET_Y;
	if (g_disappearing)
	{
		g_Boss[no].dissolve.threshold += 0.01f;

		if (g_Boss[no].dissolve.threshold >= 1.0f)
		{
			g_disappearing = FALSE;
			g_Boss[no].attribute ++;
			g_Boss[no].dissolve.ColorType = g_Boss[no].attribute;
		}
	}
	else
	{
		g_Boss[no].dissolve.threshold -= 0.01f;
		
		if (g_Boss[no].dissolve.threshold < 0.0f)
		{
			g_Boss[no].dissolve.threshold = 0.0f;
		}
	}
}

// �u�����h�A�j���[�V�����p�̃e�[�u���̒��g���쐬
void LoadBossBlendAnimTbl(int no)
{
	float frame = 0.0f;

	switch (g_Boss[no].state)
	{
	case BOSSSTATE_JUMP:
		frame = 20.0f;
		break;

	default:
		frame = 15.0f;
		break;
	}

	for (int i = 0; i < BOSSPARTS_MAX; i++)
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
		INTERPOLATION_DATA nextData = g_TblAdr[i + (BOSSPARTS_MAX * g_Boss[no].state)]->at(0);
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

// �p�[�c�̊K�w�A�j���[�V����
void BossAnimation(int i)
{
	for (int j = 0; j < BOSSPARTS_MAX; j++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[i][j].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i][j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
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

				// �J��Ԃ��Ȃ��A�j���[�V�����̏ꍇ
				switch (g_Boss[i].setTbl)
				{
				case BOSSSTATE_DIG:
				case BOSSSTATE_JUMPOUT:
				case BOSSSTATE_CHANGE:
				case BOSSSTATE_JUMP:
					g_Parts[i][j].tblMax = -1;
					break;
				}
			}
		}
	}
	switch (g_Boss[i].state)
	{
	case BOSSSTATE_JUMP:
		if (g_Parts[i][BOSSPARTS_BODY].time < 3) g_Boss[i].attack = TRUE;
		break;

	case BOSSSTATE_JUMPOUT:
		g_Boss[i].attack = TRUE;
		break;
	}
}

// �K�w�A�j���[�V�����̃u�����h
void BossBlendAnimation(int i)
{
	for (int j = 0; j < BOSSPARTS_MAX; j++)
	{
		// �g���Ă���Ȃ珈������
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

	if (g_Boss[i].state == BOSSSTATE_JUMP)
	{
		PLAYER* player = GetPlayer();

		XMVECTOR bpos = XMLoadFloat3(&g_Boss[i].pos);
		XMVECTOR vec = XMLoadFloat3(&player[0].pos) - bpos;
		float angle = atan2f(vec.m128_f32[2], vec.m128_f32[0]);
		float diff = ((angle + (3.14f * 2) / 4) * -1) - g_Boss[i].rot.y;
		g_Boss[i].rot.y += diff * 0.05f;
	}
	else if ((g_Boss[i].state == BOSSSTATE_WALK) && (g_waterOut))
	{
		XMFLOAT3 spos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMVECTOR bpos = XMLoadFloat3(&g_Boss[i].pos);
		XMVECTOR vec = XMLoadFloat3(&spos) - bpos;
		float angle = atan2f(vec.m128_f32[2], vec.m128_f32[0]);
		g_Boss[i].rot.y = (angle + (3.14f * 2) / 4) * -1;
	}
}

//=============================================================================
// �s���p�^�[���֘A
//=============================================================================
// �v���C���[���z�[�~���O����
void HomingPlayer(int no, BOOL dirOnly)
{
	// �z�[�~���O
	PLAYER* player = GetPlayer();

	XMVECTOR bpos = XMLoadFloat3(&g_Boss[no].pos);
	XMVECTOR vec = XMLoadFloat3(&player[0].pos) - bpos;

	float angle = atan2f(vec.m128_f32[2], vec.m128_f32[0]);

	if(g_actionCnt == 0) g_Boss[no].rot.y = (angle + (3.14f * 2) / 4) * -1;

	if (!dirOnly)
	{
		g_Boss[no].pos.x += cosf(angle) * g_Boss[no].spd;
		g_Boss[no].pos.z += sinf(angle) * g_Boss[no].spd;

		if (g_Boss[no].underGround)
		{
			if (g_actionCnt > TIME_UNDERGROUND)
			{
				StartBossAnim(no, BOSSSTATE_JUMPOUT);
				g_Boss[no].underGround = FALSE;
			}
		}
	}
}

// �X�^�[�g�ʒu�ɖ߂�
void BackingPosition(int no)
{
	XMFLOAT3 startPos = { 0.0f, BOSS_HEIGHT, 0.0f };
	XMVECTOR bpos = XMLoadFloat3(&g_Boss[no].pos);
	XMVECTOR vec = XMLoadFloat3(&startPos) - bpos;

	float angle = atan2f(vec.m128_f32[2], vec.m128_f32[0]);

	g_Boss[no].pos.x += cosf(angle) * g_Boss[no].spd;
	g_Boss[no].pos.z += sinf(angle) * g_Boss[no].spd;
	g_Boss[no].rot.y = (angle + (3.14f * 2) / 4) * -1;
}

// �v���C���[�Ƃ̋���
void SearchPlayer(int no, int attribute)
{
	if ((g_Boss[no].hp <= 0) || (g_Interval)) return;

	PLAYER* player = GetPlayer();
	int nextAction = rand() % 100;

	float dx = player->pos.x - g_Boss[no].pos.x;
	float dz = player->pos.z - g_Boss[no].pos.z;

	// �v���C���[�Ƃ̋���(2�搔)
	g_len = (float)sqrt(dx * dx + dz * dz);
	ResetBossAction();

	switch (attribute)
	{
	case BOSSATTR_SAND:					// �y����
		ActionSand(no, g_len, nextAction);
		break;

	case BOSSATTR_WATER:				// ������
		ActionWater(no, g_len, nextAction);
		break;
	}
}

// �����ł̍s��
void ActionSand(int no, float len, int nextAct)
{
	if (len < 400.0f)
	{
		if (nextAct <= 10)
		{
			StartBossAnim(no, BOSSSTATE_WALK);
		}
		else if (nextAct <= 30)
		{
			StartBossAnim(no, BOSSSTATE_JUMP);
		}
		else if (nextAct <= 35)
		{
			StartBossAnim(no, BOSSSTATE_DIG);
		}
		else
		{
			StartBossAnim(no, BOSSSTATE_IDLE);
		}
	}
	else if (g_len < 700.0f)
	{
		if (nextAct <= 30)
		{
			StartBossAnim(no, BOSSSTATE_WALK);
		}
		else if (nextAct <= 50)
		{
			StartBossAnim(no, BOSSSTATE_DIG);
		}
		else
		{
			StartBossAnim(no, BOSSSTATE_IDLE);
		}
	}
	else if (g_len < 800.0f)
	{
		if (nextAct <= 20)
		{
			StartBossAnim(no, BOSSSTATE_DIG);
		}
		else if(nextAct <= 50)
		{
			StartBossAnim(no, BOSSSTATE_IDLE);
		}
		else
		{
			StartBossAnim(no, BOSSSTATE_WALK);
		}
	}
}

// ���ӂł̍s��
void ActionWater(int no, float len, int nextAct)
{
	if (len < 400.0f)
	{
		if (nextAct <= 10)
		{
			StartBossAnim(no, BOSSSTATE_WALK);
		}
		else if (nextAct <= 30)
		{
			StartBossAnim(no, BOSSSTATE_JUMP);
		}
		else
		{
			StartBossAnim(no, BOSSSTATE_IDLE);
		}
	}
}

//=============================================================================
// �����蔻��֌W
//=============================================================================
//�q�p�[�c�̃��[���h���W
XMFLOAT3 ChildBossPartsPos(int no, int index)
{
	// �p�[�c�̎q�̕t�����̍��W�����߂Ă���
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[no][index].mtxWorld);

	// ���߂����W�����Ă�
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//�q�p�[�c�̃��[���h���W��Ԃ�
	return pos;
}

//=============================================================================
// �X�e�[�^�X�n
//=============================================================================
void DamageBoss(int no, int damage)
{
	// ���G���Ԃ���Ȃ����
	g_Boss[no].hp -= damage;

	// hp���Ȃ��Ȃ�����FALSE��
	if (g_Boss[no].hp <= 0.0f)
	{
		if (g_Boss[no].attribute != BOSSATTR_MAX - 1)
		{	// �Ō�̑�������Ȃ����
			g_Boss[no].state = BOSSSTATE_FLY;
			ResetBossAction();
		}
		else
		{
			g_Boss[no].use = FALSE;
			DECO* deco = GetDeco();
			deco[DECO_CLEAR].use = TRUE;
			SetSoundFade(FADE_OUT, SOUND_LABEL_SE_gameClear);
		}
	}
}

// �{�X�̃J�E���g�����Z�b�g
void ResetBossAction(void)
{
	g_actionCnt = 0.0f;
	g_flyingCnt = 0.0f;
}