//=============================================================================
//
// ボス処理 [boss.cpp]
// Author : GP11A 015 高橋ほの香
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
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define BOSS_HEIGHT			(170.0f)					// ボスの身長
#define BOSS_SWIMY			(-60.0f)					// 泳いでるときのy座標
#define SPEED_UNDERGROUND	(7.0f)						// 地中でのホーミングスピード
#define TIME_UNDERGROUND	(120.0f)					// 最大地中ホーミングタイム
#define BOSS_OFFSET_Y		(3.0f)						// エネミーの足元をあわせる
#define BOSS_GRAVITY		(10.0f)						// ボスにかかる重力の大きさ
#define BOSS_UPPOWER		(17.0f)						// 地面から飛び出す力
#define COUNTMAX_WALK		(60.0f)						// 歩きカウントMAX
#define COUNTMAX_WALK2		(160.0f)					// 歩きカウントMAX2
#define COUNTMAX_IDLE		(30.0f)						// 待機カウントMAX
#define COUNTMAX_SWIM		(1000.0f)					// 泳ぎカウントMAX

#define ELLIPSE_LONGR		(950.0f)					// 楕円の長辺
#define ELLIPSE_SHORTR		(550.0f)					// 楕円の長辺
#define SWIM_SPEED			(800.0f)					// 泳ぐスピード

// アニメーションのリスト
enum BOSSANIM_LIST
{
	// 待機状態
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

	// 掘り状態
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

	// 地面から飛び出す状態
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

	// 属性変化
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

	// 泳ぎ
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

	// 飛ぶ
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

	// 歩く
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

	// ジャンプ攻撃
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
// プロトタイプ宣言
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
// グローバル変数
//*****************************************************************************
static BOSS			g_Boss[MAX_BOSS];								// ボスモンスター
static BOSS			g_Parts[MAX_BOSS][BOSSPARTS_MAX];				// ボスのパーツ用
static BOSS_MODEL	g_Model[MAX_BOSS][BOSSPARTS_MAX];
static ID3D11ShaderResourceView* g_Texture[BOSSATTR_MAX] = { NULL };	// テクスチャ情報

static int			g_Boss_load = 0;
static float		g_actionCnt = 0;
static float		g_flyingCnt = 0;
static float		g_len;
static BOOL			g_disappearing;
static BOOL			g_Interval;
static BOOL			g_Gravity;
static BOOL			g_waterOut;										// 水から上がった後か

static char* g_TextureName[BOSSATTR_MAX] = {
	"data/MODEL/boss/YellowSkin.png",
	"data/MODEL/boss/BlueSkin.png",
	// "data/MODEL/boss/RedSkin.png",
};

// アニメーションのディレクトリの配列
static char* g_AnimDir[] =
{
	// 待機状態
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

	// 掘り状態
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

	// 地面から飛び出す状態
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

	// 属性変化中
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

	// 泳ぎ
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

	// 飛ぶ
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

	// 歩く
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

	// ジャンプ攻撃
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

// モデルデータのディレクトリ
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

// エネミーの階層アニメーションデータ
// vector型版
// 待機状態
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

// 掘り状態
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

// 地面からの飛び出し状態
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

// 属性変化
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

// 泳ぎ
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

// 飛ぶ
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

// 歩く
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

// ジャンプ攻撃
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
	// 待機状態
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

	// 掘り状態
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

	// 地面からの飛び出し状態
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

	// 属性変化
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

	// 泳ぎ
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

	// 飛ぶ
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

	// 歩く
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

	// ジャンプ攻撃
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
// 初期化処理
//=============================================================================
HRESULT InitBoss(void)
{
	g_waterOut = FALSE;

	// テクスチャ生成
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

	// モデルの読込
	for (int i = 0; i < MAX_BOSS; i++)
	{
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			LoadModel(g_ModelAdr[j], &g_Model[i][j].model);
			g_Model[i][j].load = TRUE;
		}
	}

	// アニメーションデータをファイルから読み込み
	for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}

	// ボスの初期化
	for (int i = 0; i < MAX_BOSS; i++)
	{
		g_Boss[i].pos = XMFLOAT3(0.0f, 0.0f, 100.0f);
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(0.022f, 0.022f, 0.022f);

		g_Boss[i].spd = 0.0f;						// 移動スピードクリア
		g_Boss[i].hp = MAX_BOSSHP;

		g_Boss[i].use = TRUE;						// TRUE:生きてる
		g_Boss[i].underGround = FALSE;				// 潜ってない

		g_Boss[i].state = BOSSSTATE_IDLE;
		g_Boss[i].setTbl = BOSSSTATE_IDLE;
		g_Boss[i].attribute = BOSSATTR_SAND;		// 最初の属性は土

		// ディゾルヴ
		g_Boss[i].dissolve.Enable = TRUE;			// ディゾルヴを有効
		g_Boss[i].dissolve.ColorType = g_Boss[i].attribute;
		g_Boss[i].dissolve.threshold = 0.0f;		// ディゾルヴのしきい値

		g_Boss[i].parent = NULL;					// 本体（親）なのでNULLを入れる


		// 階層アニメーションの初期化
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			g_Parts[i][j].use = TRUE;

			// 位置・回転・スケールの初期設定
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Parts[i][j].modelIdx = j;

			// モデルのディフューズを保存しておく。色変え対応の為。
			GetModelDiffuse(&g_Model[i][g_Parts[i][j].modelIdx].model, &g_Parts[i][j].diffuse[0]);

			// 階層アニメーション用のメンバー変数の初期化
			g_Parts[i][j].time = 0.0f;												// 線形補間用のタイマーをクリア
			g_Parts[i][j].tblNo = j + ANIM_IDLE_LIST_BODY;							// 再生する行動データテーブルNoをセット
			g_Parts[i][j].tblMax = (int)g_TblAdr[g_Parts[i][j].tblNo]->size();		// 再生する行動データテーブルのレコード数をセット

			// 親子関係
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

		// クォータニオンの初期化
		XMStoreFloat4(&g_Boss[i].Quaternion, XMQuaternionIdentity());


	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBoss(void)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		// モデルの解放処理
		for (int j = 0; j < BOSSPARTS_MAX; j++)
		{
			if (g_Model[i][j].load == FALSE) continue;

			UnloadModel(&g_Model[i][j].model);
			g_Model[i][j].load = FALSE;
		}

		// アニメーションテーブルをファイルから初期化
		for (int i = 0; i < BOSSANIM_LIST_MAX; i++)
		{
			g_TblAdr[i]->clear();
		}

	}
	// テクスチャの解放
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
// 更新処理
//=============================================================================
void UpdateBoss(void)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		if (g_Boss[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
			g_Boss[i].attack = FALSE;	// 初期値は攻撃中ではない
			g_Boss[i].oldPos = g_Boss[i].pos;			// 前のpoosを保存しておく

			switch (g_Boss[i].state)					// 現在のステートによって操作を分ける
			{
			case BOSSSTATE_IDLE:
				if (g_Boss[i].underGround)
				{
					MoveUnderGround(i);					// 地中の時は潜り行動
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
			// アニメーションをセット
			SetBossTblAnim(i, g_Boss[i].state);
			AnimFinBoss(i, g_Boss[i].state);

			// ブレンド中かどうかで線形補間をわける
			(g_Interval) ? BossBlendAnimation(i) : BossAnimation(i);
			
			// 地面との判定系
			if(g_Boss[i].state != BOSSSTATE_SWIM)
			{
				// レイキャストして足元の高さを求める
				XMFLOAT3 HitPosition;		// 交点
				XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
				BOOL ans = RayHitField(g_Boss[i].pos, &HitPosition, &Normal);

				g_Gravity = TRUE;
				float distance = 0.0f;

				if (ans)
				{
					distance = g_Boss[i].pos.y - HitPosition.y;

					// 地面に埋まるのを防止
					if (distance <= 10.0f)
					{
						g_Boss[i].pos.y = HitPosition.y + BOSS_OFFSET_Y;
						g_Gravity = FALSE;
					}
				}

				if ((g_Gravity == TRUE) && (g_Boss[i].state != BOSSSTATE_FLY))				// 飛行中は重力無視
				{
					// 重力を付ける
					g_Boss[i].pos.y -= BOSS_GRAVITY;
				}
			}

			// フィールド外には出れない
			if (((g_Boss[i].pos.x < GetFieldSize() / 2 * -1) || (g_Boss[i].pos.x > GetFieldSize() / 2)) ||
				((g_Boss[i].pos.z < GetFieldSize() / 2 * -1) || (g_Boss[i].pos.z > GetFieldSize() / 2)))
			{
				g_Boss[i].pos = g_Boss[i].oldPos;
			}

#ifdef _DEBUG
			PrintDebugProc("BOSSstate: %d BOSSAttack: %d\n", g_Boss[i].state, g_Boss[i].attack);
			PrintDebugProc("BOSSpos x:%f y:%f z:%f 距離: %f actionCnt: %f flyingCnt: %f\n", g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z, g_len, g_actionCnt, g_flyingCnt);
#endif
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBoss(BOOL shadow)
{
	for (int i = 0; i < MAX_BOSS; i++)
	{
		if (g_Boss[i].use)
		{

			SetDissolve(&g_Boss[i].dissolve);
			if(!shadow) SetAlphaTestEnable(g_Boss[i].dissolve.Enable);		// αテストを有効に

			// 属性の色にテクスチャを合わせる
			GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[g_Boss[i].attribute]);

			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

			// カリング無効
			SetCullingMode(CULL_MODE_NONE);

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Boss[i].scl.x, g_Boss[i].scl.y, g_Boss[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// クォータニオンを反映
			quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Boss[i].Quaternion));
			mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);

			// 階層アニメーション
			for (int j = 0; j < BOSSPARTS_MAX; j++)
			{
				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();

				// スケールを反映
				mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);

				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// 回転を反映
				mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
					// ↑
					// g_Enemy.mtxWorldを指している
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// 使われているなら処理する
				if (g_Parts[i][j].use == FALSE) continue;

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);

				// モデル描画
				DrawModel(&g_Model[i][g_Parts[i][j].modelIdx].model);
			}

			// カリング設定を戻す
			SetCullingMode(CULL_MODE_BACK);

			if(!shadow) SetAlphaTestEnable(FALSE);		// αテストを無効に
			SetDissolveEnable(FALSE);


		}
	}

}

//=============================================================================
// ボスの取得
//=============================================================================
BOSS *GetBoss(void)
{
	return &g_Boss[0];
}

//=============================================================================
// アニメーション関連
//=============================================================================
// アニメーションをセット
void SetBossTblAnim(int no, int state)
{
	// 今のステートと目的のステートが一致してなければ
	if (((g_Boss[no].setTbl) != (state)) && (!g_Interval))
	{
		// ブレンドアニメーションを行う
		LoadBossBlendAnimTbl(no);
		g_Interval = TRUE;
	}
}

// アニメーションを終わらす
void AnimFinBoss(int no, int state)
{
	// 状態遷移
	{
		BOOL ans = TRUE;

		// すべてのパーツの最大テーブル数が -1 になっている場合(≒アニメーションが終了している場合)、状態遷移させる
		for (int i = 0; i < BOSSPARTS_MAX; i++)
		{
			if (g_Parts[no][i].tblMax != -1) ans = FALSE;
		}

		if (ans == TRUE)
		{
			// ブレンドアニメーションが終わったところなら
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
				g_Interval = FALSE;							// ブレンドアニメーションを終わらす
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
					ResetField(g_Boss[no].attribute);		// 属性が変わるとフィールドが変わる
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

// アニメ開始
void StartBossAnim(int no, int state)
{
	g_Boss[no].state = state;
	
	if (state == BOSSSTATE_CHANGE) g_disappearing = TRUE;
}

// 地中を進む
void MoveUnderGround(int no)
{
	g_Boss[no].pos.y = -BOSS_HEIGHT;
	g_Boss[no].spd = SPEED_UNDERGROUND;
HomingPlayer(no, FALSE);
}

// 地面から飛び出す
void MoveJumpOut(int no)
{
	if (g_Parts[no][BOSSPARTS_BODY].time < 1)
	{
		g_Boss[no].pos.y += BOSS_UPPOWER;
		HomingPlayer(no, TRUE);
	}
}

// 泳ぐ
void MoveSwimming(int no)
{
	// 高さは水中にしておく
	g_Boss[no].pos.y = BOSS_SWIMY;

	// 楕円の動きで陸の周りを泳がせておく
	float longR = ELLIPSE_LONGR;					// 楕円の長軸の半径
	float shortR = ELLIPSE_SHORTR;					// 楕円の短軸の半径
	float centerX = 0.0f;							// 楕円の中心座標 x
	float centerZ = 0.0f;							// 楕円の中心座標 z
	float rot = XM_PI - 0.48f;						// 楕円自体を回転させる

	// 角速度 (1回転あたりの時間)
	float angularSpeed = XM_PI / SWIM_SPEED;		// 1フレームで進むスピード

	// 楕円上の位置を計算
	float angle = angularSpeed * g_actionCnt;
	float posX = (float)(centerX + (longR * cos(angle) * cos(rot)) - (shortR * sin(angle) * sin(rot)));
	float posZ = (float)(centerZ + (longR * cos(angle) * sin(rot)) + (shortR * sin(angle) * cos(rot)));

	// posXとposYを使ってキャラクターの位置を更新
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

// 飛ぶ
void MoveFlying(int no)
{
	float diff;
	switch ((int)(g_flyingCnt))
	{
	case 0:
		diff = BOSS_HEIGHT - g_Boss[no].pos.y;
		g_Boss[no].pos.y += diff * 0.08f;

		// 差分が減ったら
		if (fabsf(diff) < 1.0f) g_flyingCnt++;
		break;

	case 1:
		BackingPosition(no);
		// だいたい初期位置に戻ったら
		if (fabsf(g_Boss[no].pos.x) < 1.5f)
		{
			g_Boss[no].rot.y = 0.0f;
			g_flyingCnt++;
		}
		break;

	case 2:
		diff = BOSS_OFFSET_Y - g_Boss[no].pos.y;
		g_Boss[no].pos.y += diff * 0.03f;

		// 差分が減ったら
		if (!g_Gravity)
		{
			StartBossAnim(no, BOSSSTATE_CHANGE);		// スタート位置についたので属性変化を始める
			g_flyingCnt = 0.0f;
		}
		break;
	}
}

// 歩く
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

// 属性チェンジ
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

// ブレンドアニメーション用のテーブルの中身を作成
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
		// テーブルの中身をクリア
		g_Parts[no][i].blendTbl.clear();

		g_Parts[no][i].time = 0.0f;
		g_Parts[no][i].tblNo = i;

		// 現在のパーツ情報をテーブルに追加
		g_Parts[no][i].blendTbl.push_back(
			{
			XMFLOAT3(g_Parts[no][i].pos.x, g_Parts[no][i].pos.y, g_Parts[no][i].pos.z),
			XMFLOAT3(g_Parts[no][i].rot.x, g_Parts[no][i].rot.y, g_Parts[no][i].rot.z),
			XMFLOAT3(g_Parts[no][i].scl.x, g_Parts[no][i].scl.y, g_Parts[no][i].scl.z),
			frame
			}
		);

		// 次のアニメーションのパーツの最初の情報をテーブルに追加
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

// パーツの階層アニメーション
void BossAnimation(int i)
{
	for (int j = 0; j < BOSSPARTS_MAX; j++)
	{
		// 使われているなら処理する
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i][j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i][j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			std::vector<INTERPOLATION_DATA> tbl = *g_TblAdr[g_Parts[i][j].tblNo];	// 行動テーブルのアドレスを取得(vector型で)


			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i][j].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i][j].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i][j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる

				// 繰り返さないアニメーションの場合
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

// 階層アニメーションのブレンド
void BossBlendAnimation(int i)
{
	for (int j = 0; j < BOSSPARTS_MAX; j++)
	{
		// 使われているなら処理する
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i][j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i][j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;				// 移動先テーブルの番号を求めている
			std::vector<INTERPOLATION_DATA> tbl = g_Parts[i][j].blendTbl;	// 行動テーブルのアドレスを取得(vector型で)


			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i][j].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i][j].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i][j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
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
// 行動パターン関連
//=============================================================================
// プレイヤーをホーミングする
void HomingPlayer(int no, BOOL dirOnly)
{
	// ホーミング
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

// スタート位置に戻る
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

// プレイヤーとの距離
void SearchPlayer(int no, int attribute)
{
	if ((g_Boss[no].hp <= 0) || (g_Interval)) return;

	PLAYER* player = GetPlayer();
	int nextAction = rand() % 100;

	float dx = player->pos.x - g_Boss[no].pos.x;
	float dz = player->pos.z - g_Boss[no].pos.z;

	// プレイヤーとの距離(2乗数)
	g_len = (float)sqrt(dx * dx + dz * dz);
	ResetBossAction();

	switch (attribute)
	{
	case BOSSATTR_SAND:					// 土属性
		ActionSand(no, g_len, nextAction);
		break;

	case BOSSATTR_WATER:				// 水属性
		ActionWater(no, g_len, nextAction);
		break;
	}
}

// 砂漠での行動
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

// 水辺での行動
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
// 当たり判定関係
//=============================================================================
//子パーツのワールド座標
XMFLOAT3 ChildBossPartsPos(int no, int index)
{
	// パーツの子の付け根の座標を求めている
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[no][index].mtxWorld);

	// 求めた座標を入れてる
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//子パーツのワールド座標を返す
	return pos;
}

//=============================================================================
// ステータス系
//=============================================================================
void DamageBoss(int no, int damage)
{
	// 無敵時間じゃなければ
	g_Boss[no].hp -= damage;

	// hpがなくなったらFALSEに
	if (g_Boss[no].hp <= 0.0f)
	{
		if (g_Boss[no].attribute != BOSSATTR_MAX - 1)
		{	// 最後の属性じゃなければ
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

// ボスのカウントをリセット
void ResetBossAction(void)
{
	g_actionCnt = 0.0f;
	g_flyingCnt = 0.0f;
}