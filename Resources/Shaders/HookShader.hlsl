#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"

Texture2D _BaseMap : register(t0);

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    
    return output;
}

[maxvertexcount(36)]
void GS_Main(line VS_OUT input[2], inout TriangleStream<GS_OUT> triStream)
{
    float3 p0 = input[0].pos.xyz;
    float3 p1 = input[1].pos.xyz;

    float3 center = (p0 + p1) * 0.5f;
    float3 halfSize = abs(p1 - p0) * 0.5f;

    float3 corners[8] =
    {
        center + float3(-halfSize.x, -halfSize.y, -halfSize.z), // 0
        center + float3(halfSize.x, -halfSize.y, -halfSize.z), // 1
        center + float3(halfSize.x, halfSize.y, -halfSize.z), // 2
        center + float3(-halfSize.x, halfSize.y, -halfSize.z), // 3
        center + float3(-halfSize.x, -halfSize.y, halfSize.z), // 4
        center + float3(halfSize.x, -halfSize.y, halfSize.z), // 5
        center + float3(halfSize.x, halfSize.y, halfSize.z), // 6
        center + float3(-halfSize.x, halfSize.y, halfSize.z) // 7
    };

    int cubeTriangles[36] =
    {
        0, 1, 2, 0, 2, 3, // 앞
        5, 4, 7, 5, 7, 6, // 뒤
        4, 0, 3, 4, 3, 7, // 좌
        1, 5, 6, 1, 6, 2, // 우
        3, 2, 6, 3, 6, 7, // 위
        4, 5, 1, 4, 1, 0 // 아래
    };

    for (int i = 0; i < 36; ++i)
    {
        GS_OUT output = (GS_OUT) 0;
        float4 worldPos = float4(corners[cubeTriangles[i]], 1.0f);
        output.pos = mul(worldPos, VPMatrix);
        output.uv = float2(0, 0);
        triStream.Append(output);
    }
}


float4 PS_Main(GS_OUT input) : SV_Target
{
    return float4(_BaseMap.Sample(sampler_lerp, input.uv).xyz, 1.0f);
}