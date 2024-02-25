//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================
#pragma once


//*********************************************************
// マクロ定義
//*********************************************************
#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//ライト無し
	LIGHT_TYPE_DIRECTIONAL,	//ディレクショナルライト
	LIGHT_TYPE_POINT,		//ポイントライト

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_SUBTRACT,	//減算ブレンド

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)

	CULL_MODE_NUM
};

// ワイプ
enum WIPE_MODE
{
	WIPE_MODE_NONE,
	WIPE_MODE_TEXWIPE,		// テクスチャの切り替えワイプ
	WIPE_MODE_FLIP,			//フリップのワイプ(めくる)
	WIPE_MODE_CIRCLE,		//円形のワイプ
	WIPE_MODE_STRIPE,		//ストライプのワイプ

	WIPE_MODE_NUM,
};

// 影
enum SHADOW_TYPE
{
	SHADOW_TYPE_NONE,		// 影なし
	SHADOW_TYPE_HARD,		// ハードシャドウ
	SHADOW_TYPE_SOFT,		// ソフトシャドウ

	SHADOW_TYPE_NUM,
};

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
    XMFLOAT3	Position;
    XMFLOAT3	Normal;
    XMFLOAT4	Diffuse;
    XMFLOAT2	TexCoord;
	float		randHeight;
};

// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ライト構造体
struct LIGHT {
	XMFLOAT3	Direction;	// ライトの方向
	XMFLOAT3	Position;	// ライトの位置
	XMFLOAT4	Diffuse;	// 拡散光の色
	XMFLOAT4	Ambient;	// 環境光の色
	float		Attenuation;// 減衰率
	int			Type;		// ライト種別・有効フラグ
	int			Enable;		// ライト種別・有効フラグ
};

// フォグ構造体
struct FOG {
	float		FogStart;	// フォグの開始距離
	float		FogEnd;		// フォグの最大距離
	XMFLOAT4	FogColor;	// フォグの色
};

// 塗りつぶし構造体
struct FILL {
	XMFLOAT4	FillColor;	// 塗りつぶしの色
	float		Size;		// 塗りつぶす幅
};

// ディゾルヴ構造体
struct DISSOLVE {
	int		Enable;
	int		ColorType;
	float	threshold;
};

// シャドウ構造体
struct SHADOW {
	XMMATRIX	LightViewProj;
	int			Type;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
IDXGISwapChain* GetSwapChain(void);
ID3D11DeviceContext *GetDeviceContext( void );
ID3D11RenderTargetView* GetRenderTargerView(void);
ID3D11DepthStencilView* GetDepthStencilView(void);

void SetDepthEnable( BOOL Enable );
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);
void SetAlphaTestEnable(BOOL flag);

void SetWorldViewProjection2D( void );
void SetWorldMatrix( XMMATRIX *WorldMatrix );
void SetViewMatrix( XMMATRIX *ViewMatrix );
void SetProjectionMatrix( XMMATRIX *ProjectionMatrix );

void SetMaterial( MATERIAL material );

void SetLightEnable(BOOL flag);
void SetLight(int index, LIGHT* light);

void SetFogEnable(BOOL flag);
void SetFog(FOG* fog);

void DebugTextOut(char* text, int x, int y);

void SetFuchi(int flag);
void SetShaderCamera(XMFLOAT3 pos);

void SetClearColor(float* color4);
void SetWipe(int type, XMFLOAT4 dir, float size);

void SetFillEnable(BOOL flag);
void SetFillChange(FILL* pFill);

void SetMonochromeEnable(BOOL flag);

void SetDissolveEnable(BOOL flag);
void SetDissolve(DISSOLVE* dissolve);

// shadowmap関連
void StartShadowSetting(void);
void EndShadowSetting(void);
void SetShadow(SHADOW* shadow);