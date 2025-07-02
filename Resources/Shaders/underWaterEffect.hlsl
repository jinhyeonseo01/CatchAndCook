
#include "Light_b3.hlsl"

//#define SHADOW_ON

SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerComparisonState sampler_shadow : register(s5);
SamplerState sampler_lerp_clamp : register(s6);


cbuffer ShadowCasterParams : register(b9) 
{
    row_major matrix shadowViewMatrix[4];
	row_major matrix shadowProjectionMatrix[4];
	row_major matrix shadowVPMatrix[4];
	row_major matrix shadowInvertViewMatrix[4];
	row_major matrix shadowInvertProjectionMatrix[4];
	row_major matrix shadowInvertVPMatrix[4];

	unsigned int cascadeCount = 1;
	float3 padding_shadow0;
    float4 cascadeDistance;
};

Texture2D ShadowTexture[4] : register(t100);

static const int shadowTexelSize = 2048;

struct ShadowCascadeIndexParams
{
	unsigned int cascadeIndex;
	float3 padding2;
};



void ComputeCascadeShadowUVs(float3 worldPos, out float3 uvOut[4])
{
    float4 worldPosH = float4(worldPos, 1.0f);
    uvOut[0] = float3(0, 0, 1);
    uvOut[1] = float3(0, 0, 1);
    uvOut[2] = float3(0, 0, 1);
    uvOut[3] = float3(0, 0, 1);
    
    [unroll]
    for (uint i = 0; i < cascadeCount; ++i)
    {
        // 1) world → 라이트 클립 공간 (NDC)
        float4 clipPos = mul(worldPosH, shadowVPMatrix[i]);
        // - bias
        // 2) 퍼스펙티브 디바이드 (clip → ndc)
        float invW = 1.0f / clipPos.w;
        float3 ndc = clipPos.xyz * invW; // ndc.x,y ∈ [-1,1], ndc.z ∈ [0,1]

        // 3) ndc → 텍스처 UV (0..1) + Y축 뒤집기
        uvOut[i].x = ndc.x * 0.5f + 0.5f;
        uvOut[i].y = (-ndc.y) * 0.5f + 0.5f;
        uvOut[i].z = ndc.z;
    }
}

float ComputeCascadeShadowAtten(in float3 uvOut[4], float viewDepth)
{
    const float bias = 1.0f * (0.005 / shadowTexelSize);
    
    [unroll]
    for (uint i = 0; i < cascadeCount; ++i)
    {
        float3 uvw = uvOut[i];

        if (viewDepth <= cascadeDistance[i] && uvw.x >= 0.0f && uvw.y >= 0.0f && uvw.x <= 1.0f && uvw.y <= 1.0f)
        {
            return ShadowTexture[i].SampleCmpLevelZero(sampler_shadow, uvw.xy, uvw.z - bias);
        }
        
    }
    
    return 1.0f;
}

cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
};

cbuffer underWatgerParam : register(b1)
{
    float3 g_fogColor;
    float g_fog_power;

    float3 g_underWaterColor;
    float g_fogMin;

    float2 padding;
    int g_on; 
    float g_fogMax; 
};

cbuffer cameraParams : register(b2)
{
    row_major Matrix ViewMatrix;
    row_major Matrix ProjectionMatrix;
    row_major Matrix VPMatrix;
    row_major Matrix InvertViewMatrix;
    row_major Matrix InvertProjectionMatrix;
    row_major Matrix InvertVPMatrix;

    float4 cameraPos;
    float4 cameraLook;
    float4 cameraUp;
    float4 cameraFrustumData;
    float4 cameraScreenData;
};

RWTexture2D<float4> resultTexture : register(u0);
Texture2D depthT : register(t0);
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> PositionTexture : register(t2);
Texture2D<float4> NormalTexture : register(t3);
Texture2D<float4> MaoTexture : register(t4);

float3 ProjToView(float2 texCoord)
{
    float4 posProj;
    // texCoord는 픽셀 좌표이므로 정규화 좌표로 변환
    posProj.xy = (texCoord + 0.5f) / cameraScreenData.xy * 2.0f - 1.0f;
    posProj.y *= -1; 
    posProj.z = depthT.Load(int3(texCoord, 0));
    posProj.w = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    return posView.xyz/posView.w;
}

float CalculateFogFactor(float3 posView)
{
    float dist = length(posView.xyz);
    float distFog = saturate((dist - g_fogMin) / (g_fogMax - g_fogMin));
    float fogFactor = exp(-distFog * g_fog_power);
    return 1-fogFactor;
}



///////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    int2 tex = id.xy;
    float2 uv = (float2(tex) + 0.5) / cameraScreenData.xy;

    float3 albedo = RenderT.SampleLevel(sampler_lerp, uv, 0).xyz;
    float3 normal = normalize(NormalTexture.SampleLevel(sampler_point, uv, 0).xyz);
    float3 worldPos = PositionTexture.SampleLevel(sampler_point, uv, 0).xyz;
    float3 viewPos = ProjToView(tex);

    float fogFactor = CalculateFogFactor(viewPos);
    
    LightingResult light = ComputeLightColor(worldPos, normal);
    float distFogNorm = saturate((length(viewPos.xyz) - g_fogMin) / (g_fogMax - g_fogMin));
    float swAtten = lerp(light.subWaterAtten, 1.0f, distFogNorm);
    
#ifdef SHADOW_ON
    float3 uvz[4];
    ComputeCascadeShadowUVs(worldPos.xyz, uvz);
    light.atten *= ComputeCascadeShadowAtten(uvz, mul(float4(worldPos.xyz, 1), ViewMatrix).z);
#endif
    
    float3 underCol = lerp(g_underWaterColor * albedo, albedo, light.atten);

    float3 litColor = underCol + light.subColor;

    float adjustedFog = saturate(fogFactor * swAtten);
    float3 finalColor = lerp(litColor, g_fogColor, adjustedFog);
    
    resultTexture[tex] = float4(finalColor, 1.0f);
}