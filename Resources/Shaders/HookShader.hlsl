#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"

Texture2D globalT18 : register(t18);

struct VS_OUT
{
    float4 pos1 : POSITION1;
    float4 pos2 : POSITION2;
    float2 uv : TEXCOORD0;
};

struct GS_OUT
{
    float4 pos : SV_POSITION; 
    float2 uv : TEXCOORD0;
};

cbuffer PosParam : register(b8)
{
    float3 pos1;
    float padding;
    float3 pos2;
    float padding2;
}

VS_OUT VS_Main()
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos1 = float4(pos1, 1.0f);
    output.pos2 = float4(pos2, 1.0f);

    return output;
}

[maxvertexcount(36)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    float3 p1 = input[0].pos1;
    float3 p2 = input[0].pos2;

    float side = 0.1f;
    float halfSide = side * 0.5f;

    float3 dir = normalize(p2 - p1);
    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, dir));
    float3 realUp = normalize(cross(dir, right));

    float3 v[8];
    v[0] = p1 - right * halfSide - realUp * halfSide;
    v[1] = p1 + right * halfSide - realUp * halfSide;
    v[2] = p1 + right * halfSide + realUp * halfSide;
    v[3] = p1 - right * halfSide + realUp * halfSide;
    v[4] = p2 - right * halfSide - realUp * halfSide;
    v[5] = p2 + right * halfSide - realUp * halfSide;
    v[6] = p2 + right * halfSide + realUp * halfSide;
    v[7] = p2 - right * halfSide + realUp * halfSide;

    int idx[36] =
    {
        0, 1, 2, 0, 2, 3, // front
        5, 4, 7, 5, 7, 6, // back
        4, 0, 3, 4, 3, 7, // left
        1, 5, 6, 1, 6, 2, // right
        3, 2, 6, 3, 6, 7, // top
        4, 5, 1, 4, 1, 0 // bottom
    };

    float2 uvMap[4] =
    {
        float2(0, 0), // bottom-left
        float2(1, 0), // bottom-right
        float2(1, 1), // top-right
        float2(0, 1) // top-left
    };

    // 면별 정점의 uv 순서 (6면 * 6정점 = 36개)
    float2 uvList[36] =
    {
        // front
        uvMap[0], uvMap[1], uvMap[2],
        uvMap[0], uvMap[2], uvMap[3],

        // back
        uvMap[1], uvMap[0], uvMap[3],
        uvMap[1], uvMap[3], uvMap[2],

        // left
        uvMap[0], uvMap[1], uvMap[2],
        uvMap[0], uvMap[2], uvMap[3],

        // right
        uvMap[0], uvMap[1], uvMap[2],
        uvMap[0], uvMap[2], uvMap[3],

        // top
        uvMap[0], uvMap[1], uvMap[2],
        uvMap[0], uvMap[2], uvMap[3],

        // bottom
        uvMap[0], uvMap[1], uvMap[2],
        uvMap[0], uvMap[2], uvMap[3]
    };

    for (int i = 0; i < 36; ++i)
    {
        GS_OUT output;
        output.pos = mul(float4(v[idx[i]], 1.0f), VPMatrix);
        output.uv = uvList[i];
        outputStream.Append(output);
    }
}


float4 PS_Main(GS_OUT input) : SV_Target
{
    return float4(0.3f, 0, 0, 0);
}