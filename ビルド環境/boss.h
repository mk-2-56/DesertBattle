//=============================================================================
//
// ボス処理 [boss.h]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#pragma once

#include <vector>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BOSS		(1)					// ボスの数
#define MAX_BOSSHP		(70)				// ボスのHP初期値

// アニメーションの状態
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

// ボスの属性
enum BOSSATTRIBUTE_LIST
{
	BOSSATTR_SAND = 0,
	BOSSATTR_WATER,
	// BOSSATTR_MAGMA,

	BOSSATTR_MAX,
};

// 子パーツのリスト
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
// 構造体定義
//*****************************************************************************
struct BOSS
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMFLOAT3			oldPos;				// 前フレームのpos

	float				spd;				// 移動スピード
	float				dir;				// 向き
	float				size;				// 当たり判定の大きさ
	int					state;
	int					hp;
	BOOL				attack;
	BOOL				use;
	BOOL				underGround;		// 地下にいるときか

	// 階層アニメーション用のメンバー変数
	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	int					setTbl;				// セットしているテーブル

	int					modelIdx;
	int					attribute;			// 現在の属性

	// 親は、NULL、子供は親のアドレスを入れる
	BOSS*				parent;				// 自分が親ならNULL、自分が子供なら親のplayerアドレス
	std::vector<INTERPOLATION_DATA> blendTbl;
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	DISSOLVE			dissolve;	// ディゾルヴ
};

struct BOSS_MODEL
{
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBoss(void);
void UninitBoss(void);
void UpdateBoss(void);
void DrawBoss(BOOL shadow = FALSE);

BOSS *GetBoss(void);
XMFLOAT3 ChildBossPartsPos(int no, int index);
void DamageBoss(int no, int damage);
void ResetBossAction(void);