#ifndef _FORWARDFUNC_HLSL_
#define _FORWARDFUNC_HLSL_

#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"

void ComputeForwardDirectionalLight(Light L, LightMateiral mat, float3 worldPos, float3 normal, float3 toEye, inout LightingResult lightingResult)
{
    //float3 lightVec = normalize(L.position - worldPos);
    float3 lightVec = normalize(-L.direction);
    float ndotl = dot(normal, lightVec);
    ndotl = saturate(ndotl * 0.5 + 0.5);
    //ndotl = 1-pow(1-ndotl, 20);
    //float3 LightStrength = L.strength * L.intensity * ndotl;

    lightingResult.direction = lightVec;
    lightingResult.atten = ndotl * clamp(0, 1, L.intensity);
    lightingResult.color = L.strength* L.intensity;
    lightingResult.intensity = L.intensity;
}

LightingResult ComputeLightColorForward(float3 worldPos ,float3 WorldNomral)
{
    float3 lightColor = float3(0, 0, 0);

    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    LightingResult result = (LightingResult)0;

    //[unroll]
    [loop]
    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].onOff == 1)
        {
	        if (g_lights[i].mateiral.lightType == 0)
	        {
	            ComputeForwardDirectionalLight(g_lights[i], g_lights[i].mateiral, worldPos, WorldNomral, toEye, result);
	        }
	        else if (g_lights[i].mateiral.lightType == 1)
	        {
	            ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
	        }
	        else if (g_lights[i].mateiral.lightType == 2)
	        {
	            ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
	        }
		}
    }
    
    return result;
}


float Sobel(float4 positionCS, float scale)
{
    //9면 샘플링
    /*
    float2 uv = positionCS.xy / positionCS.w;
    uv.y *= -1;
    uv = uv * 0.5 + 0.5;

	float2 texelSize = scale * 0.001;

	float d0 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2(-texelSize.x, -texelSize.y)).r);
	float d1 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2( 0.0f,       -texelSize.y)).r);
	float d2 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2( texelSize.x, -texelSize.y)).r);
	float d3 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2(-texelSize.x,  0.0f       )).r);
	float d5 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2( texelSize.x,  0.0f       )).r);
	float d6 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2(-texelSize.x,  texelSize.y )).r);
	float d7 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2( 0.0f,        texelSize.y )).r);
	float d8 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2( texelSize.x,  texelSize.y )).r);

    float gx = (d2 + 2.0f * d5 + d8) - (d0 + 2.0f * d3 + d6);
    float gy = (d0 + 2.0f * d1 + d2) - (d6 + 2.0f * d7 + d8);

    float edgeStrength = length(float2(gx, gy));

    return edgeStrength; //  > threshold ? 1.0f : 0.0f
    */

    // 4대각 샘플링
    float2 uv = positionCS.xy / positionCS.w;
    uv = uv * float2(0.5, -0.5) + 0.5;
    float2 ts = scale * 0.001;

    // 4개 샘플
    float v0 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + ts).r);
    float v1 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv - ts).r);
    float v2 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2(ts.x, -ts.y)).r);
    float v3 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp_clamp, uv + float2(-ts.x, ts.y)).r);

    // 가중치 없이 간단 비교
    float gx = v0 - v1;
    float gy = v2 - v3;
    return abs(gx) + abs(gy);
}

#endif
