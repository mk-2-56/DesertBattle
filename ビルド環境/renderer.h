//=============================================================================
//
// �����_�����O���� [renderer.h]
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================
#pragma once


//*********************************************************
// �}�N����`
//*********************************************************
#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//���C�g����
	LIGHT_TYPE_DIRECTIONAL,	//�f�B���N�V���i�����C�g
	LIGHT_TYPE_POINT,		//�|�C���g���C�g

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//�u�����h����
	BLEND_MODE_ALPHABLEND,	//���u�����h
	BLEND_MODE_ADD,			//���Z�u�����h
	BLEND_MODE_SUBTRACT,	//���Z�u�����h

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//�J�����O����
	CULL_MODE_FRONT,		//�\�̃|���S����`�悵�Ȃ�(CW)
	CULL_MODE_BACK,			//���̃|���S����`�悵�Ȃ�(CCW)

	CULL_MODE_NUM
};

// ���C�v
enum WIPE_MODE
{
	WIPE_MODE_NONE,
	WIPE_MODE_TEXWIPE,		// �e�N�X�`���̐؂�ւ����C�v
	WIPE_MODE_FLIP,			//�t���b�v�̃��C�v(�߂���)
	WIPE_MODE_CIRCLE,		//�~�`�̃��C�v
	WIPE_MODE_STRIPE,		//�X�g���C�v�̃��C�v

	WIPE_MODE_NUM,
};

// �e
enum SHADOW_TYPE
{
	SHADOW_TYPE_NONE,		// �e�Ȃ�
	SHADOW_TYPE_HARD,		// �n�[�h�V���h�E
	SHADOW_TYPE_SOFT,		// �\�t�g�V���h�E

	SHADOW_TYPE_NUM,
};

//*********************************************************
// �\����
//*********************************************************

// ���_�\����
struct VERTEX_3D
{
    XMFLOAT3	Position;
    XMFLOAT3	Normal;
    XMFLOAT4	Diffuse;
    XMFLOAT2	TexCoord;
	float		randHeight;
};

// �}�e���A���\����
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ���C�g�\����
struct LIGHT {
	XMFLOAT3	Direction;	// ���C�g�̕���
	XMFLOAT3	Position;	// ���C�g�̈ʒu
	XMFLOAT4	Diffuse;	// �g�U���̐F
	XMFLOAT4	Ambient;	// �����̐F
	float		Attenuation;// ������
	int			Type;		// ���C�g��ʁE�L���t���O
	int			Enable;		// ���C�g��ʁE�L���t���O
};

// �t�H�O�\����
struct FOG {
	float		FogStart;	// �t�H�O�̊J�n����
	float		FogEnd;		// �t�H�O�̍ő勗��
	XMFLOAT4	FogColor;	// �t�H�O�̐F
};

// �h��Ԃ��\����
struct FILL {
	XMFLOAT4	FillColor;	// �h��Ԃ��̐F
	float		Size;		// �h��Ԃ���
};

// �f�B�]�����\����
struct DISSOLVE {
	int		Enable;
	int		ColorType;
	float	threshold;
};

// �V���h�E�\����
struct SHADOW {
	XMMATRIX	LightViewProj;
	int			Type;
};


//*****************************************************************************
// �v���g�^�C�v�錾
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

// shadowmap�֘A
void StartShadowSetting(void);
void EndShadowSetting(void);
void SetShadow(SHADOW* shadow);