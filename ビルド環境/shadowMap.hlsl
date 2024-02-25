//=============================================================================
//
// shadowMap�p����V�F�[�_�[
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************
cbuffer WorldBuffer : register(b0)
{
    matrix World;
}

// �f�B�]����
struct DISSOLVE
{
    int Enable;
    int ColorType;
    float threshold;
    int Dummy6[1];
};

cbuffer DissolveBuffer : register(b11)
{
    DISSOLVE dissolve;
};

// �V���h�E�}�b�v
struct SHADOW
{
    matrix LightViewProj;
    int Type;
    int Dummy7[3];
};

cbuffer ShadowBuffer : register(b12)
{
    SHADOW Shadow;
};

//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void VSShadow(  in  float4 inPosition	: POSITION0,
				in  float4 inNormal		: NORMAL0,
				in  float4 inDiffuse	: COLOR0,
				in  float2 inTexcoord	: TEXCOORD0,
			  
				out float4 outPosition	: SV_POSITION,
				out float2 outTexcoord	: TEXCOORD0)
{
    matrix wvp;
    wvp = mul(World, Shadow.LightViewProj);
	outPosition = mul(inPosition, wvp);
	outTexcoord = inTexcoord;
}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture0 : register( t0 );
Texture2D       g_Fractalnoise : register(t2);
SamplerState	g_SamplerState : register( s0 );

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PSShadow(  in float4 inPosition : SV_POSITION,
				in float2 inTexcoord : TEXCOORD0)
{
    float4 diffuse = g_Texture0.Sample(g_SamplerState, inTexcoord);
    
    //�@�f�B�]����
    if (dissolve.Enable == 1)
    {
		// 1�s�N�Z���̎w�肳�ꂽ�ꏊ�̐F���擾����
        float4 noise = g_Fractalnoise.Sample(g_SamplerState, inTexcoord);

        if (noise.r < dissolve.threshold)
        {
            diffuse.a = 0.0f;
        }
    }
    
    // �����ȃs�N�Z����j��
    clip(diffuse.a - 0.01f);
}