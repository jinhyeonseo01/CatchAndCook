#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

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

Texture2D _BaseMap : register(t0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    float4 worldPos = TransformLocalToWorld(float4(input.pos, 1));
    output.pos =  TransformWorldToClip(worldPos);
    output.uv = input.uv;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = _BaseMap.Sample(sampler_lerp, input.uv);
    return _BaseMap.Sample(sampler_lerp, input.uv);
}
