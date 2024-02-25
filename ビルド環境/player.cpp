//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP11A132 15 高橋ほの香 
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
// マクロ定義
//****************************************************************************
#define	VALUE_MOVE				(2.2f)							// 移動量
#define	VALUE_DASH				(3.6f)							// ダッシュ中の移動量
#define	COUNT_KNOCKBACK			(5.0f)							// 吹っ飛ばされるカウント

#define PLAYER_PARTS_MAX		(8)								// プレイヤーのパーツの数
#define	VALUE_MOVE_CAMERA		(2.0f)							// カメラの移動量
#define	VALUE_ROTATE_CAMERA		(XM_PI * 0.01f)					// カメラの回転量
#define PAD_LENGTH				(1000.0f)						// スティックの傾きの最大値
#define PLAYER_HPMAX			(100.0f)						// マックスのHP
#define PLAYER_ENERGYMAX		(100.0f)						// マックスの体力
#define PLAYER_DAMAGETIME		(30.0f)							// 無敵時間
#define PLAYER_ENERGYUP			(0.1f)							// エナジー回復量
#define PLAYER_DASHENERGY		(0.25f)							// ダッシュに使うエナジー
#define MIN_POSY_WATER			(10.0f)

// アニメーションのリスト
enum ANIM_IDLE_LIST
{
	// 待機状態
	ANIM_IDLE_LIST_JOINT1 = 0,
	ANIM_IDLE_LIST_JOINT2,
	ANIM_IDLE_LIST_JOINT3,
	ANIM_IDLE_LIST_JOINT4,
	ANIM_IDLE_LIST_JOINT5,
	ANIM_IDLE_LIST_SWORD,
	ANIM_IDLE_LIST_BODY,
	ANIM__LIST_SWORDTIP1,

	// アタック
	ANIM_ATTACK_LIST_JOINT1,
	ANIM_ATTACK_LIST_JOINT2,
	ANIM_ATTACK_LIST_JOINT3,
	ANIM_ATTACK_LIST_JOINT4,
	ANIM_ATTACK_LIST_JOINT5,
	ANIM_ATTACK_LIST_SWORD,
	ANIM_ATTACK_LIST_BODY,
	ANIM__LIST_SWORDTIP5,

	// 薙ぎ払い
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
// プロトタイプ宣言
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
// グローバル変数
//*****************************************************************************
struct PUSH
{
	BOOL			pushed;
	float			spd;
	float			dir;
	int				time;
};

static PUSH			g_Push;									// 敵に押されてるか
static BOOL			g_Interval;								// アニメーションのブレンド中か
static BOOL			g_Dash;									// ダッシュ中か
static BOOL			g_Damaged = FALSE;						// 無敵時間か
static int			g_DamageCnt;
static PLAYER		g_Player[MAX_PLAYER];					// プレイヤー
static PLAYER		g_Parts[MAX_PLAYER][PLAYER_PARTS_MAX];	// プレイヤーのパーツ用
static PLAYER_MODEL g_Model[MAX_PLAYER][PLAYER_PARTS_MAX];
static LIGHT		g_Light;

// アニメーションのディレクトリの配列
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

// モデルデータのディレクトリ
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

// プレイヤーの階層アニメーションデータ
// vector型版
// 待機状態
static std::vector<INTERPOLATION_DATA> idle_joint1;
static std::vector<INTERPOLATION_DATA> idle_joint2;
static std::vector<INTERPOLATION_DATA> idle_joint3;
static std::vector<INTERPOLATION_DATA> idle_joint4;
static std::vector<INTERPOLATION_DATA> idle_joint5;
static std::vector<INTERPOLATION_DATA> idle_sword;
static std::vector<INTERPOLATION_DATA> idle_body;
// 剣先
static std::vector<INTERPOLATION_DATA> always_swordTip;

// アタック
static std::vector<INTERPOLATION_DATA> attack_joint1;
static std::vector<INTERPOLATION_DATA> attack_joint2;
static std::vector<INTERPOLATION_DATA> attack_joint3;
static std::vector<INTERPOLATION_DATA> attack_joint4;
static std::vector<INTERPOLATION_DATA> attack_joint5;
static std::vector<INTERPOLATION_DATA> attack_sword;
static std::vector<INTERPOLATION_DATA> attack_body;

// 薙ぎ払い
static std::vector<INTERPOLATION_DATA> slice_joint1;
static std::vector<INTERPOLATION_DATA> slice_joint2;
static std::vector<INTERPOLATION_DATA> slice_joint3;
static std::vector<INTERPOLATION_DATA> slice_joint4;
static std::vector<INTERPOLATION_DATA> slice_joint5;
static std::vector<INTERPOLATION_DATA> slice_sword;
static std::vector<INTERPOLATION_DATA> slice_body;

static std::vector<INTERPOLATION_DATA>* g_TblAdr[] =
{
	// 待機状態
	&idle_joint1,
	&idle_joint2,
	&idle_joint3,
	&idle_joint4,
	&idle_joint5,
	&idle_sword,
	&idle_body,
	&always_swordTip,

	// アタック
	&attack_joint1,
	&attack_joint2,
	&attack_joint3,
	&attack_joint4,
	&attack_joint5,
	&attack_sword,
	&attack_body,
	&always_swordTip,

	// 薙ぎ払い
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
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Push.pushed = FALSE;

	// モデルの読込
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			LoadModel(g_ModelAdr[j], &g_Model[i][j].model);
			g_Model[i][j].load = TRUE;
		}
	}

	// アニメーションデータをファイルから読み込み
	for (int i = 0; i < ANIM_LIST_MAX; i++)
	{
		LoadAnimDataCSV(g_AnimDir[i], *g_TblAdr[i]);
	}

	// プレイヤーの初期化
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		g_Player[i].pos = XMFLOAT3(0.0f, PLAYER_OFFSET_Y, PLAYER_FIRST_Z);
		g_Player[i].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Player[i].spd = 0.0f;						// 移動スピードクリア
		g_Player[i].use = TRUE;						// TRUE:生きてる
		g_Player[i].attack = ATKSTATE_NONE;			// アタック中か

		g_Player[i].state = STATE_IDLE;
		g_Player[i].setTbl = STATE_IDLE;

		g_Player[i].hp = PLAYER_HPMAX;
		g_Player[i].energy = PLAYER_ENERGYMAX;

		g_Player[i].parent = NULL;					// 本体（親）なのでNULLを入れる


		// 階層アニメーションの初期化
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
			g_Parts[i][j].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[i][j].tblNo = j + ANIM_IDLE_LIST_JOINT1;			// 再生する行動データテーブルNoをセット
			g_Parts[i][j].tblMax = (int)g_TblAdr[g_Parts[i][j].tblNo]->size();			// 再生する行動データテーブルのレコード数をセット


			// 親子関係
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
		// クォータニオンの初期化
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		// モデルの解放処理
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			if (g_Model[i][j].load == FALSE) continue;

			UnloadModel(&g_Model[i][j].model);
			g_Parts[i][j].blendTbl.clear();
			g_Model[i][j].load = FALSE;
		}
	}

	// アニメーションテーブルをファイルから初期化
	for (int i = 0; i < ANIM_LIST_MAX; i++)
	{
		g_TblAdr[i]->clear();
	}

	
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use)
		{
			g_Dash = FALSE;								// ダッシュは初期値はFALSE

			InputMoveCamera();							// 右スティックでカメラ操作

			switch (g_Player[i].state)					// 現在のステートによって操作を分ける
			{
			case STATE_IDLE:
				if (!g_Push.pushed) InputMovePlayer(i);
				InputAttackPlayer(i);
				InputSlicePlayer(i);
				break;

			default:
				break;
			}

			// アニメーションをセット
			AnimFinPlayer(i, g_Player[i].state);
			SetPlayerTblAnim(i, g_Player[i].state);

			// ブレンド中かどうかで線形補間をわける
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

			// 慣性をつけて減速させる
			g_Player[i].spd *= 0.7f;
			g_Push.spd *= 0.7f;


			// レイキャストして足元の高さを求める
			XMFLOAT3 HitPosition;		// 交点
			XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル
			BOOL ans = RayHitField(g_Player[i].pos, &HitPosition, &Normal);

			BOOL gravity = TRUE;
			float distance = 0.0f;

			if (ans)
			{
				g_Player[i].pos.y = HitPosition.y + PLAYER_OFFSET_Y;
				distance = g_Player[i].pos.y - HitPosition.y;

				// 地面に埋まるのを防止
				if ((distance <= 10.0f) && (distance >= -100.0f))
				{
					gravity = FALSE;
				}

				// 地面に埋まってるなら出す
				if (distance < 0.0f)
				{
					g_Player[i].pos.y -= distance - PLAYER_OFFSET_Y;
				}
			}

			if (gravity == TRUE)
			{
				// 重力を付ける
				g_Player[i].pos.y -= 1.0f;
			}
			//else
			//{
			//	g_Player.pos.y = PLAYER_OFFSET_Y;
			//	Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			//}


			// 姿勢制御

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


			// エナジー系
			{
				// ダッシュ中はエナジーが減る
				if (g_Dash) g_Player[i].energy -= PLAYER_DASHENERGY;

				// 自然回復
				if (g_Player[i].energy < PLAYER_ENERGYMAX) g_Player[i].energy += PLAYER_ENERGYUP;
			}

			// ダメージ系
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

			// 動いた後の当たり判定チェック系(プレイヤー側で必要なもの)
			{
				// 水フィールドの時は水に入れない
				if ((GetFieldType() == FIELD_WATER) && (g_Player[i].pos.y <= MIN_POSY_WATER)) g_Player[i].pos = g_Player[i].oldPos;

				// 水フィールド以外では単純にマップの外に出れない
				else if (((g_Player[i].pos.x < GetFieldSize() / 2 * -1) || (g_Player[i].pos.x > GetFieldSize() / 2)) ||
					((g_Player[i].pos.z < GetFieldSize() / 2 * -1) || (g_Player[i].pos.z > GetFieldSize() / 2)))
				{
					g_Player[i].pos = g_Player[i].oldPos;
				}
			}

#ifdef _DEBUG
			// デバッグ表示
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
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (g_Player[i].use)
		{
			XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

			// カリング無効
			SetCullingMode(CULL_MODE_NONE);

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// クォータニオンを反映
			quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
			mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);

			// 階層アニメーション
			for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
					// g_Player.mtxWorldを指している
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// 使われているなら処理する
				if (g_Parts[i][j].use == FALSE) continue;

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);

				// モデル描画
				if (j != PLAYER_PARTS_MAX - 1) DrawModel(&g_Model[i][g_Parts[i][j].modelIdx].model);
			}

			// カリング設定を戻す
			SetCullingMode(CULL_MODE_BACK);

		}
	}
}


//=============================================================================
// プレイヤー情報を取得
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
// キーボードの入力判定
//=============================================================================
BOOL AnyKeyPress(void)
{
	// どれかが押されたら
	return (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_DOWN));
}


//=============================================================================
// スティックの入力判定
//=============================================================================
BOOL AnyStickPress(void)
{
	// どれかが押されたら
	return ((IsButtonPressed(0, BUTTON_LEFT)) || (IsButtonPressed(0, BUTTON_RIGHT)) || (IsButtonPressed(0, BUTTON_UP)) || (IsButtonPressed(0, BUTTON_DOWN)));
}


//=============================================================================
// プレイヤーの行動
//=============================================================================
void InputMovePlayer(int no)
{
	DIJOYSTATE2* dijs = GetButtonAmount();
	CAMERA* cam = GetCamera();

	// 移動処理
	// ゲームパッドまたはキーボードで何らかの入力があれば
	if ((AnyKeyPress()) || (AnyStickPress()))
	{
		// x,y軸の方向を決める用
		float dx = 0.0f;
		float dz = 0.0f;

		// キーボードの処理だったら
		if (AnyKeyPress())
		{
			if (GetKeyboardPress(DIK_LEFT)) dx = -1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) dx = 1.0f;
			if (GetKeyboardPress(DIK_UP)) dz = 1.0f;
			if (GetKeyboardPress(DIK_DOWN)) dz = -1.0f;
			if ((GetKeyboardPress(DIK_SPACE)) && (g_Player[no].energy >= PLAYER_DASHENERGY)) g_Dash = TRUE;			// ダッシュ中
		}

		// ゲームパッドの処理だったら
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
			if ((IsButtonPressed(0, BUTTON_R) && (g_Player[no].energy >= PLAYER_DASHENERGY))) g_Dash = TRUE;			// ダッシュ中
		}

		g_Player[no].spd = (g_Dash) ? VALUE_DASH : VALUE_MOVE;

		// x軸,z軸にかかった力を進む方向の角度にする
		g_Player[no].dir = atan2f(dx * -1, dz * -1);

		MovePlayer(no);										// 入力をもとに移動処理
	}
}

// 右スティックでカメラ操作
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

// 入力をもとに移動させる
void MovePlayer(int no)
{
	// Key入力があったら移動処理する
	CAMERA* cam = GetCamera();
	g_Player[no].oldPos = g_Player[no].pos;

	// 押した方向にプレイヤーを移動させる
	if (g_Player[no].spd > 0.0f)
	{
		g_Player[no].rot.y = g_Player[no].dir + cam->rot.y;

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player[no].pos.x -= sinf(g_Player[no].rot.y) * g_Player[no].spd;
		g_Player[no].pos.z -= cosf(g_Player[no].rot.y) * g_Player[no].spd;

		// ボスとの当たり判定
		BOSS* boss = GetBoss();
		GUIDE* guide = GetGuide();
		for (int j = 0; j < MAX_BOSS; j++)
		{
			//有効フラグをチェックする
			if (boss[j].use == FALSE) continue;

			for (int k = GUIDE_BOSSHEAD; k < MAX_GUIDE; k++)
			{
				// ボスからぶつかられたらプレイヤーは押される
				BOOL ans = CollisionBC(guide[k].pos, guide[GUIDE_PLAYER].pos, guide[k].size, guide[GUIDE_PLAYER].size);
				if (ans)
				{
					g_Player[no].pos = g_Player[no].oldPos;		// 元の場所に戻す
				}
			}
		}
	}
}

// 攻撃する
void InputAttackPlayer(int no)
{
	if ((GetKeyboardTrigger(DIK_G)) || (IsButtonPressed(0, BUTTON_Y)))
	{
		g_Player[no].state = STATE_ATK;
	}
}

// 薙ぎ払い攻撃
void InputSlicePlayer(int no)
{
	if ((GetKeyboardTrigger(DIK_F)) || (IsButtonPressed(0, BUTTON_B)))
	{
		g_Player[no].state = STATE_SLICE;
	}
}

// アニメーションを終わらす
void AnimFinPlayer(int no, int state)
{
	// 状態遷移
	{
		BOOL ans = TRUE;

		// すべてのパーツの最大テーブル数が -1 になっている場合(≒アニメーションが終了している場合)、状態遷移させる
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			if (g_Parts[no][i].tblMax != -1) ans = FALSE;
		}

		if (ans == TRUE)
		{
			// ブレンドアニメーションが終わったところなら
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
				g_Interval = FALSE;			// ブレンドアニメーションを終わらす
			}
			else
			{
				g_Player[no].state = STATE_IDLE;
			}
		}
	}
}

// アニメーションをセット
void SetPlayerTblAnim(int no, int state)
{
	// 今のステートと目的のステートが一致してなければ
	if (((g_Player[no].setTbl) != (state)) && (!g_Interval))
	{
		// ブレンドアニメーションを行う
		LoadBlendAnimTbl(no);
		g_Interval = TRUE;
		g_Player[no].attack = ATKSTATE_NONE;
	}
}

// ブレンドアニメーション用のテーブルの中身を作成
void LoadBlendAnimTbl(int no)
{
	float frame = 10.0f;

	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
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
	// 階層アニメーション
	for (int j = 0; j < PLAYER_PARTS_MAX; j++)
	{
		// 通常の線形補間
		if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i][j].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i][j].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;				// 移動先テーブルの番号を求めている
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

	if (g_Player[i].attack != ATKSTATE_HIT)					// 攻撃がまだ当たってなければ
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
	// 階層アニメーション
	for (int j = 0; j < PLAYER_PARTS_MAX; j++)
	{
		// アニメーションのブレンド用線形補間
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
}

//=============================================================================
// 当たり判定関係
//=============================================================================
//子パーツのワールド座標
XMFLOAT3 ChildPlayerPartsPos(int no, int index)
{
	// パーツの子の付け根の座標を求めている
	XMMATRIX mtx = XMLoadFloat4x4(&g_Parts[no][index].mtxWorld);

	// 求めた座標を入れてる
	XMFLOAT3 pos = XMFLOAT3(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);

	//子パーツのワールド座標を返す
	return pos;
}

// ボスに当たった時
void PushedPlayer(int no)
{
	// 押される方向に向かって移動する
	g_Player[no].pos.x += sinf(g_Player[no].rot.y) * g_Push.spd;
	g_Player[no].pos.z += cosf(g_Player[no].rot.y) * g_Push.spd;
}

//=============================================================================
// プレイヤーのステータス関係
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
	// 無敵時間じゃなければ
	if (!g_Damaged)
	{
		g_Player[no].hp -= damage;
		g_Damaged = TRUE;

		// hpがなくなったらFALSEに
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