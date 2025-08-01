#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

#ifndef SEA_GLOBAL
#define SEA_GLOBAL


Texture2D globalT17 : register(t17);

float4 ComputeCaustics(float2 uv, float scale, float3 worldPos)
{
    float power = smoothstep(-800.0f, 50.0f, worldPos.y);
    float2 waveOffset = float2(
        sin(g_Time * 0.4f + uv.y * 10.0f),
        cos(g_Time * 0.6f + uv.x * 10.0f)
    ) * 0.05f;

    float2 causticUV = uv * scale + waveOffset;
    
    float3 caustic = globalT17.Sample(sampler_lerp, causticUV).rgb;

    return float4(caustic * 0.3f * power, 0.0f);
}

float3 ReconstructWorldPos(int2 texCoord, float depth)
{
    float2 uv = float2(texCoord) / float2(cameraScreenData.x, cameraScreenData.y);
    float4 ndc = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, depth, 1.0);

    float4 viewPos = mul(InvertProjectionMatrix, ndc);
    viewPos /= viewPos.w; 

    float4 worldPos = mul(InvertViewMatrix, float4(viewPos.xyz, 1.0));
    return worldPos.xyz;
}

float ConvertNdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}


#endif