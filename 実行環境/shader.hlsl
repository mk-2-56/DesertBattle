//=============================================================================
//
// shader.hlsl
// Author : GP11A132 15 高橋ほの香
//
//=============================================================================

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
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

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
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
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// ワイプ
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

// 塗りつぶし
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

//モノクロ
struct MONOCHROME
{
    int Enable;
    int Dummy5[3];
};

cbuffer MonochromeBuffer : register(b10)
{
    MONOCHROME Monochrome;
}

// ディゾルヴ
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

// 影
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
// 頂点シェーダ
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
						  out float4 outlightTransformedPos : POSITION1 )	// shadowMap用
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);
    
	outDiffuse = inDiffuse;
	
	// shadowMap用のlightViewProj計算
    outlightTransformedPos = mul(outWorldPos, Shadow.LightViewProj);
}


//=============================================================================
// シャドウマップ計算
//=============================================================================
// シャドウマップ関連のグローバル変数
Texture2D g_TexShadow : register(t3);
SamplerState g_SSShadow : register(s1);

float CalculateSoftShadow(float4 position)
{
    float4 shadowPosition = position;
    shadowPosition.xyz /= shadowPosition.w; // 正規化スクリーン座標系に変換

	// 深度値が1.0を超える場合は影の外側
    if (shadowPosition.z > 1.0f)
    {
        return 1.0f;
    }

	// ライトビュースクリーン空間からUV座標空間に変換
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
    shadowPosition.xyz /= shadowPosition.w;				// 正規化スクリーン座標系に変換

	// 深度値が1.0を超える場合は影の外側
    if (shadowPosition.z > 1.0f)
    {
        return 1.0f;
    }

	// ライトビュースクリーン空間からUV座標空間に変換
    shadowPosition.x = 0.5f * shadowPosition.x + 0.5f;
    shadowPosition.y = -0.5f * shadowPosition.y + 0.5f;

    float shadow = 1.0f;
    static const float bias = 0.002f;
    float depthFromLight = g_TexShadow.SampleLevel(g_SSShadow, shadowPosition.xy, 0).r;
    return step(shadowPosition.z, depthFromLight + bias);
}

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture0 : register( t0 );
Texture2D		g_Texture1 : register( t1 );
Texture2D		g_Fractalnoise : register( t2 );
SamplerState	g_SamplerState : register( s0 );

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,
						 in  float4 inLightTransformedPos : POSITION1, // shadowMap用

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
        if ((dissolve.Enable == 1) && (Monochrome.Enable != 1))
        {
			// ボスモデルの色を変える
            color = g_Texture1.Sample(g_SamplerState, inTexCoord); // 2枚目の色を取得
        }
        else
        {
            color = g_Texture0.Sample(g_SamplerState, inTexCoord);
        }

        color *= inDiffuse;
	}
    else if (Material.noTexSampling == 2)
    {
		// 波動方程式
        float4 color1;
        float4 color2;

        float w;
		
        color1 = g_Texture0.Sample(g_SamplerState, inTexCoord);
        color2 = g_Texture1.Sample(g_SamplerState, inTexCoord);

		// saturate→0以下は０に、１以上は１に
        w = saturate((inWorldPos.y + 8) / 10);

		// w０なら半分ずつ
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
		// ライトがついているならshadowをつける
        float shadowColor = 1.0f;
        switch (Shadow.Type)
        {
            case 1:	// ハードシャドウ
                shadowColor = CalculateHardShadow(inLightTransformedPos);
                break;
            case 2: // ソフトシャドウ
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
					
					// shadowの色を付与
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

	//フォグ
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

	//縁取り
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
	
	// ワイプ
    if (Wipe.Type == 1)													// テクスチャを切り替えるワイプ
    {
        float4 nextColor;
        nextColor = g_Texture1.Sample(g_SamplerState, inTexCoord);		// 2枚目の色を取得
			
        float vec = dot(Wipe.Direction.xz, inWorldPos.xz);
        if ((vec - Wipe.WipeSize) < -1500)
        {
            outDiffuse = nextColor;
        }
    }
	if (Wipe.Type == 2)													// めくるワイプ
    {
        float vec = dot(Wipe.Direction.xy, inPosition.xy);
        clip(vec - Wipe.WipeSize);

    }
    if (Wipe.Type == 3)													// 円形のワイプ
    {
        float2 posFromCenter = inPosition.xy - float2(480, 270);		
        clip(length(posFromCenter) - Wipe.WipeSize);

    }
    if (Wipe.Type == 4)													// ストライプのワイプ
    {
        float cut = fmod(inPosition.x, 64.0f);
	//float cut = (int)fmod(inPosition.x, 64.0f);
        clip(cut - Wipe.WipeSize);
    }
	
	// 塗りつぶし
    if (Fill.Enable == 1)
    {	
        if(inPosition.x < Fill.Size)
        {
            outDiffuse = Fill.FillColor;
        }
    }
	
	// モノクロ
    if (Monochrome.Enable == 1)
    {
        float r = 0.299;
        float g = 0.587;
        float b = 0.114;
		
        float4 gray = color.r * r + color.g * g + color.b * b;
        color.rgb = gray.rgb;

        outDiffuse = color;
    }
	
	//　ディゾルヴ
    if (dissolve.Enable == 1)
    {		
		// 1ピクセルの指定された場所の色を取得する
        float4 noise = g_Fractalnoise.Sample(g_SamplerState, inTexCoord);

        if (noise.r < dissolve.threshold)
        {
            outDiffuse.a = 0.0f;
        }
        else if (noise.r < dissolve.threshold + 0.1f)
        {
            float temp = (noise.r - dissolve.threshold) * 10.0f;
			
            switch (dissolve.ColorType)
            {// ボスの属性に合わせてディゾルブの色を変える
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
