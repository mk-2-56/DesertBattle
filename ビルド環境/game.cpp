//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "model.h"

// システム系
#include "renderer.h"
#include "guide.h"
#include "debugproc.h"
#include "collision.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

// オブジェクト系
#include "player.h"
#include "meshfield.h"
#include "liquid.h"
#include "light.h"
#include "bg.h"
#include "boss.h"
#include "particle.h"
#include "effect.h"

// 2D系
#include "status.h"
#include "deco.h"
#include "map.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define		BOSS_CAMERAY			(270.0f)
#define		BOSS_ATY				(180.0f)
#define		BOSS_CAMERAZ			(-250.0f)
#define		FIELD_CAMERAY			(400.0f)
#define		FIELD_CAMERALEN			(1000.0f)
#define		FIELD_CAMERAROT			(-0.4f)
#define		PLAYER_CAMERAY			(45.0f)
#define		PLAYER_CAMERAZ			(10.0f)
#define		PLAYER_DAMAGE			(10)
#define		BOSS_DAMAGE				(10)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawShadow(void);
void DrawGame0(void);
void CheckHit(void);
void ChangeGameCamera(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF
static BOOL g_EnableSetShadowMap = TRUE;
static int g_Count;

#ifdef _DEBUG
static BOOL	g_Guide = FALSE;	// 衝突判定ガイドON/OFF
#endif

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 40, 40, 50.0f, 50.0f);
	// 液体(水やマグマ)の初期化
	InitLiquid(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 200, 200, 10.0f, 10.0f);

	// プレイヤーの初期化
	InitPlayer();

	// ボスの初期化
	InitBoss();

	// 背景の初期化
	InitBG();

	// マップアイコンの初期化
	InitMap();

	// ステータスの初期化
	InitStatus();

	// パーティクルの初期化
	InitParticle();

	// エフェクトの初期化
	InitEffect();

	// 装飾の初期化
	InitDeco();

	// 判定用
	InitGuide();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	UninitParticle();

	// エフェクトの終了処理
	UninitEffect();

	// ステータスの終了処理
	UninitStatus();

	// 背景の終了処理
	UninitBG();

	// 地面の終了処理
	UninitMeshField();

	// 液体の終了処理
	UninitLiquid();

	// ボスの終了処理
	UninitBoss();

	// プレイヤーの終了処理
	UninitPlayer();

	// マップアイコンの終了処理
	UninitMap();

	// 判定用
	UninitGuide();

	// 装飾
	UninitDeco();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
	BOSS* boss = GetBoss();

#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

	if (GetKeyboardTrigger(DIK_3))
	{
		g_Guide = g_Guide ? FALSE : TRUE;
	}
#endif

	if(g_bPause == FALSE)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// 液体処理の更新
	UpdateLiquid();

	// プレイヤーの更新処理
	if(boss->use) UpdatePlayer();

	// ボスの更新処理
	UpdateBoss();

	// 背景の更新処理
	UpdateBG();

	// パーティクルの更新処理
	UpdateParticle();

	// エフェクトの更新処理
	UpdateEffect();

	// 判定用
	UpdateGuide();

	// 当たり判定処理
	CheckHit();

	// マップアイコンの更新処理
	UpdateMap();

	// ステータスの更新処理
	UpdateStatus();

	// 装飾
	UpdateDeco();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame(void)
{

#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	XMFLOAT3 pos;

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);

		// shadowMap描画
		if (g_EnableSetShadowMap == TRUE)
		{
			StartShadowSetting();
			SetLightView();
			DrawShadow();
			EndShadowSetting();
		}

		// ゲームオブジェクトの描画
		ChangeGameCamera();
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// ボス視点
		pos = GetBoss()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetBoss()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}

// ゲームの基本オブジェクト描画
void DrawGame0(void)
{
	// 3Dの物を描画する処理

	// 地面の描画処理
	DrawMeshField();

	// 液体の描画処理
	DrawLiquid();

	// ボスの描画処理
	DrawBoss();

	// プレイヤーの描画処理
	DrawPlayer();

	// 背景の描画処理
	DrawBG();

	// パーティクルの描画処理
	DrawParticle();

	// エフェクトの描画処理
	DrawEffect();

#ifdef _DEBUG
	// 当たり判定可視化用
	if (g_Guide) DrawGuide();

#endif

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// ステータスの描画処理
	DrawStatus();

	// 装飾の描画処理
	DrawDeco();

	// マップの描画
	DrawMap();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}

// shadowMap
// 影を描画したいオブジェクト
void DrawShadow(void)
{
	// プレイヤーの描画処理
	DrawPlayer();
	// ボスの描画処理
	DrawBoss(TRUE);
}

//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	PLAYER* player = GetPlayer();
	BOSS *boss = GetBoss();
	GUIDE* guide = GetGuide();


	// 敵からプレイヤーキャラ
	for (int i = 0; i < MAX_BOSS; i++)
	{
		//敵の有効フラグをチェックする
		if (boss[i].use == FALSE) continue;

		for (int j = 0; j < MAX_PLAYER; j++)
		{
			if (player[j].use == FALSE) continue;

			for (int k = GUIDE_BOSSHEAD; k < MAX_GUIDE; k++)
			{
				// ボスからぶつかられたらプレイヤーは押される
				BOOL ans = CollisionBC(guide[k].pos, guide[GUIDE_PLAYER].pos, guide[k].size, guide[GUIDE_PLAYER].size);

				if (ans)
				{
					if (!GetPushed()) SetPlayerPush(player[j].rot.y * -1, 4.0f);

					if (boss[i].attack)
					{
						DamagePlayer(j, PLAYER_DAMAGE);
						boss[i].attack = FALSE;
					}
					break;
				}	
			}
		}
	}

	// プレイヤーから敵
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		//プレイヤーの有効フラグをチェックする
		if (player[i].use == FALSE) continue;

		for (int j = 0; j < MAX_BOSS; j++)
		{
			if (boss[j].use == FALSE) continue;

			//BCの当たり判定
			for (int k = GUIDE_BOSSHEAD; k < MAX_GUIDE; k++)
			{
				if (player[i].attack != ATKSTATE_NOHIT) continue;
				if (CollisionBC(guide[GUIDE_SWORD].pos, guide[k].pos, guide[GUIDE_SWORD].size, guide[k].size))
				{	// 剣が当たったら
					SetEffect(guide[GUIDE_SWORD].pos, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
					DamageBoss(j, BOSS_DAMAGE);
					PlaySound(SOUND_LABEL_SE_slash01);
					player[i].attack = ATKSTATE_HIT;			// ヒットした

					break;
				}
			}
		}
	}
}

//=============================================================================
// カメラ制御
//=============================================================================
void ChangeGameCamera(void)
{
	PLAYER* player = GetPlayer();
	BOSS* boss = GetBoss();
	CAMERA* cam = GetCamera();

	// ワールドマトリックスの初期化
	XMMATRIX  mtxRot, mtxTranslate, mtxWorld;
	mtxWorld = XMMatrixIdentity();
	// ベクトル
	XMFLOAT3 vector;

	if (boss->state == BOSSSTATE_CHANGE)
	{	// 属性変化中はイベントシーン
		cam->pos.y = BOSS_CAMERAY;
		cam->rot.y = XM_PIDIV4;

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(0, BOSS_ATY, BOSS_CAMERAZ);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		vector.x += boss->pos.x;
		vector.y += boss->pos.y;
		vector.z += boss->pos.z;

		// イベント中はプレイヤーは初期位置
		player->pos.x = PLAYER_FIRST_X;
		player->pos.z = PLAYER_FIRST_Z;
	}
	else if (GetFieldChange() == TRUE)
	{
		// フィールドの変化中のカメラ
		g_Count++;
		float shakeX = (g_Count % 4 == 0) ? 10.0f : 0.0f;
		float shakeZ = (g_Count % 4 == 2) ? 10.0f : 0.0f;
		cam->pos.y = FIELD_CAMERAY;
		cam->len = FIELD_CAMERALEN;
		cam->rot.y = FIELD_CAMERAROT;

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(shakeX, shakeZ, 0);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);


		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		// イベント中はプレイヤーは初期位置
		player->pos.x = PLAYER_FIRST_X;
		player->pos.z = PLAYER_FIRST_Z;
	}
	else
	{
		// 通常時のカメラ
		mtxTranslate = XMMatrixTranslation(0, PLAYER_CAMERAY, PLAYER_CAMERAZ);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		mtxRot = XMMatrixRotationRollPitchYaw(cam->rot.x, cam->rot.y, cam->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, mtxWorld);

		vector.x = result._41;
		vector.y = result._42;
		vector.z = result._43;

		vector.x += player->pos.x;
		vector.y += player->pos.y;
		vector.z += player->pos.z;
	}

	SetCameraAT(vector);
	SetCamera();
}
