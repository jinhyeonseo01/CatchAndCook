
#ifndef INCLUDE_SHADOWRECEIVE_SETTING
#define INCLUDE_SHADOWRECEIVE_SETTING

#include "Camera_b2.hlsl"
#include "Skinned_t32.hlsl"


cbuffer ShadowCasterParams : register(b6) 
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

static const int shadowTexelSize = 2048;

struct ShadowCascadeIndexParams
{
	unsigned int cascadeIndex;
	float3 padding2;
};

void ComputeCascadeShadowUVs(float3 worldPos, out float3 uvOut[4])
{
    float4 worldPosH = float4(worldPos, 1.0f);
    uvOut[0] = float3(0,0,1);
    uvOut[1] = float3(0,0,1);
    uvOut[2] = float3(0,0,1);
    uvOut[3] = float3(0,0,1);

    [unroll]
    for (uint i = 0; i < cascadeCount; ++i)
    {
        // 1) world → 라이트 클립 공간 (NDC)
        float4 clipPos = mul(worldPosH, shadowVPMatrix[i]);
        // - bias
        // 2) 퍼스펙티브 디바이드 (clip → ndc)
        float invW = 1.0f / clipPos.w;
        float3 ndc  = clipPos.xyz * invW;    // ndc.x,y ∈ [-1,1], ndc.z ∈ [0,1]

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
        float2 uv  = uvw.xy;

        if (viewDepth <= cascadeDistance[i] && uv.x >= 0.0f && uv.y >= 0.0f && uv.x <= 1.0f && uv.y <= 1.0f)
        {
            return ShadowTexture[i].SampleCmpLevelZero(sampler_shadow, uv, uvw.z - bias);
        }
        
    }
    
    return 1.0f;
}
float ComputeCascadeShadowAttenCustomBias(in float3 uvOut[4], float viewDepth, float customBias)
{
    [unroll]
    for (uint i = 0; i < cascadeCount; ++i)
    {
        float3 uvw = uvOut[i];
        float2 uv  = uvw.xy;

        if (viewDepth <= cascadeDistance[i] && uv.x >= 0.0f && uv.y >= 0.0f && uv.x <= 1.0f && uv.y <= 1.0f)
        {
            return ShadowTexture[i].SampleCmpLevelZero(sampler_shadow, uv, uvw.z - customBias);
        }
    }
    return 1.0f;
}

#endif