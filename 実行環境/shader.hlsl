//=============================================================================
//
// shader.hlsl
// Author : GP11A132 15 �����ق̍�
//
//=============================================================================

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte���E�p
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// �����p�o�b�t�@
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// ���C�v
struct WIPE
{
    float4 Direction;
    float WipeSize;
    int Type;
    int Dummy2[2];
};

cbuffer WipeBuffer : register(b8)
{
    WIPE Wipe;
}

// �h��Ԃ�
struct FILL
{
    float4 FillColor;
    float Size;
    int Enable;
    int Dummy3[2];
};

cbuffer FillBuffer : register(b9)
{
    FILL Fill;
}

//���m�N��
struct MONOCHROME
{
    int Enable;
    int Dummy5[3];
};

cbuffer MonochromeBuffer : register(b10)
{
    MONOCHROME Monochrome;
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

// �e
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
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0,
						  out float4 outlightTransformedPos : POSITION1 )	// shadowMap�p
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);
    
	outDiffuse = inDiffuse;
	
	// shadowMap�p��lightViewProj�v�Z
    outlightTransformedPos = mul(outWorldPos, Shadow.LightViewProj);
}


//=============================================================================
// �V���h�E�}�b�v�v�Z
//=============================================================================
// �V���h�E�}�b�v�֘A�̃O���[�o���ϐ�
Texture2D g_TexShadow : register(t3);
SamplerState g_SSShadow : register(s1);

float CalculateSoftShadow(float4 position)
{
    float4 shadowPosition = position;
    shadowPosition.xyz /= shadowPosition.w; // ���K���X�N���[�����W�n�ɕϊ�

	// �[�x�l��1.0�𒴂���ꍇ�͉e�̊O��
    if (shadowPosition.z > 1.0f)
    {
        return 1.0f;
    }

	// ���C�g�r���[�X�N���[����Ԃ���UV���W��Ԃɕϊ�
    shadowPosition.x = 0.5f * shadowPosition.x + 0.5f;
    shadowPosition.y = -0.5f * shadowPosition.y + 0.5f;

    static const float bias = 0.002f;
	// PCF Sample with 8x8 kernel
    static const int SampleSize = 4;
    static float texelSize = 0.00008f;

    float shadow = 0.0f;
    for (int i = -SampleSize; i <= SampleSize; i++)
    {
        for (int j = -SampleSize; j <= SampleSize; j++)
        {
            float pcfDepth = g_TexShadow.SampleLevel(g_SSShadow, shadowPosition.xy + float2(i, j) * texelSize, 0).r;
            shadow += step(shadowPosition.z - bias, pcfDepth);
        }
    }

	// Number of iterations of inner + outer loop combined.
    return shadow / pow(SampleSize * 2 + 1, 2);
}

float CalculateHardShadow(float4 position)
{
    float4 shadowPosition = position;
    shadowPosition.xyz /= shadowPosition.w;				// ���K���X�N���[�����W�n�ɕϊ�

	// �[�x�l��1.0�𒴂���ꍇ�͉e�̊O��
    if (shadowPosition.z > 1.0f)
    {
        return 1.0f;
    }

	// ���C�g�r���[�X�N���[����Ԃ���UV���W��Ԃɕϊ�
    shadowPosition.x = 0.5f * shadowPosition.x + 0.5f;
    shadowPosition.y = -0.5f * shadowPosition.y + 0.5f;

    float shadow = 1.0f;
    static const float bias = 0.002f;
    float depthFromLight = g_TexShadow.SampleLevel(g_SSShadow, shadowPosition.xy, 0).r;
    return step(shadowPosition.z, depthFromLight + bias);
}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture0 : register( t0 );
Texture2D		g_Texture1 : register( t1 );
Texture2D		g_Fractalnoise : register( t2 );
SamplerState	g_SamplerState : register( s0 );

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,
						 in  float4 inLightTransformedPos : POSITION1, // shadowMap�p

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
        if ((dissolve.Enable == 1) && (Monochrome.Enable != 1))
        {
			// �{�X���f���̐F��ς���
            color = g_Texture1.Sample(g_SamplerState, inTexCoord); // 2���ڂ̐F���擾
        }
        else
        {
            color = g_Texture0.Sample(g_SamplerState, inTexCoord);
        }

        color *= inDiffuse;
	}
    else if (Material.noTexSampling == 2)
    {
		// �g��������
        float4 color1;
        float4 color2;

        float w;
		
        color1 = g_Texture0.Sample(g_SamplerState, inTexCoord);
        color2 = g_Texture1.Sample(g_SamplerState, inTexCoord);

		// saturate��0�ȉ��͂O�ɁA�P�ȏ�͂P��
        w = saturate((inWorldPos.y + 8) / 10);

		// w�O�Ȃ甼������
        color = (1 - w) * color1 + w * color2;
        color *= inDiffuse;
    }
    else
    {
        color = inDiffuse;
    }

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		// ���C�g�����Ă���Ȃ�shadow������
        float shadowColor = 1.0f;
        switch (Shadow.Type)
        {
            case 1:	// �n�[�h�V���h�E
                shadowColor = CalculateHardShadow(inLightTransformedPos);
                break;
            case 2: // �\�t�g�V���h�E
                shadowColor = CalculateSoftShadow(inLightTransformedPos);
                break;
        }
		
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
					
					// shadow�̐F��t�^
                    tempColor *= shadowColor;
                }
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	//�t�H�O
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	//�����
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.3f)
		{
			outDiffuse.rb  = 1.0f;
			outDiffuse.g = 0.0f;			
		}
	}
	
	// ���C�v
    if (Wipe.Type == 1)													// �e�N�X�`����؂�ւ��郏�C�v
    {
        float4 nextColor;
        nextColor = g_Texture1.Sample(g_SamplerState, inTexCoord);		// 2���ڂ̐F���擾
			
        float vec = dot(Wipe.Direction.xz, inWorldPos.xz);
        if ((vec - Wipe.WipeSize) < -1500)
        {
            outDiffuse = nextColor;
        }
    }
	if (Wipe.Type == 2)													// �߂��郏�C�v
    {
        float vec = dot(Wipe.Direction.xy, inPosition.xy);
        clip(vec - Wipe.WipeSize);

    }
    if (Wipe.Type == 3)													// �~�`�̃��C�v
    {
        float2 posFromCenter = inPosition.xy - float2(480, 270);		
        clip(length(posFromCenter) - Wipe.WipeSize);

    }
    if (Wipe.Type == 4)													// �X�g���C�v�̃��C�v
    {
        float cut = fmod(inPosition.x, 64.0f);
	//float cut = (int)fmod(inPosition.x, 64.0f);
        clip(cut - Wipe.WipeSize);
    }
	
	// �h��Ԃ�
    if (Fill.Enable == 1)
    {	
        if(inPosition.x < Fill.Size)
        {
            outDiffuse = Fill.FillColor;
        }
    }
	
	// ���m�N��
    if (Monochrome.Enable == 1)
    {
        float r = 0.299;
        float g = 0.587;
        float b = 0.114;
		
        float4 gray = color.r * r + color.g * g + color.b * b;
        color.rgb = gray.rgb;

        outDiffuse = color;
    }
	
	//�@�f�B�]����
    if (dissolve.Enable == 1)
    {		
		// 1�s�N�Z���̎w�肳�ꂽ�ꏊ�̐F���擾����
        float4 noise = g_Fractalnoise.Sample(g_SamplerState, inTexCoord);

        if (noise.r < dissolve.threshold)
        {
            outDiffuse.a = 0.0f;
        }
        else if (noise.r < dissolve.threshold + 0.1f)
        {
            float temp = (noise.r - dissolve.threshold) * 10.0f;
			
            switch (dissolve.ColorType)
            {// �{�X�̑����ɍ��킹�ăf�B�]���u�̐F��ς���
                case 0:
                    outDiffuse.rbg = temp;
                    outDiffuse.g = 0.8f;
                    break;
                
                case 1:
                    outDiffuse.rbg = temp;
                    outDiffuse.b = 0.8f;
                    break;
                
                case 2:
                    outDiffuse.rbg = temp;
                    outDiffuse.r = 0.8f;
                    break;
            }
        }
    }
}
