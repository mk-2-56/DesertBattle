//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#pragma once
#include "model.h"

#include <vector>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER				(1)					// プレイヤーの数
#define PLAYER_OFFSET_Y			(7.0f)				// プレイヤーの足元をあわせる
#define PLAYER_FIRST_X			(100.0f)				// プレイヤーの初期座標X
#define PLAYER_FIRST_Z			(-200.0f)			// プレイヤーの初期座標Z


// アニメーションの状態
enum STATE_LIST
{
	STATE_IDLE = 0,
	STATE_ATK,
	STATE_SLICE,

	STATE_MAX,
};

// 攻撃の状態
enum ATKSTATE_LIST
{
	ATKSTATE_NONE = 0,
	ATKSTATE_NOHIT,
	ATKSTATE_HIT,

	ATKSTATE_MAX,
};

// 子パーツのリスト
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
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMFLOAT3			oldPos;				// 前フレームのpos

	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス

	float				spd;				// 移動スピード
	float				dir;				// 向き
	int					state;				// ステート(状態）
	int					attack;				// 攻撃状態
	BOOL				use;

	// 階層アニメーション用のメンバー変数
	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数
	int					setTbl;				// セットしているテーブル

	int					modelIdx;

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER*				parent;				// 自分が親ならNULL、自分が子供なら親のplayerアドレス
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	std::vector<INTERPOLATION_DATA> blendTbl;

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	// ステータス
	float				hp;					// HP
	float				energy;				// エネルギー
	float				damageRot;			// 吹っ飛ぶ方向
};

struct PLAYER_MODEL
{
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
PLAYER* GetPlayer(void);
BOOL GetPushed(void);

// キーボード入力
BOOL AnyKeyPress(void);
// スティック入力
BOOL AnyStickPress(void);

// 当たり判定関連
XMFLOAT3 ChildPlayerPartsPos(int no, int index);
void SetPlayerPush(float rot, float speed);

// ステータス関連
float GetPlayerHP(int no);
float GetPlayerEnergy(int no);
void DamagePlayer(int no, int damage);