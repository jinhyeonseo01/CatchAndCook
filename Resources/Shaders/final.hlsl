#include "Global_b0.hlsl"
#include "Light_b3.hlsl"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.f);
    output.uv = input.uv;
    
    return output;
}

Texture2D _BakedGIMap : register(t8);

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 worldPos = PositionTexture.Sample(sampler_point, input.uv);
    float3 WolrdNormal = normalize(NormalTexture.Sample(sampler_point, input.uv).xyz);
    float4 AlbedoColor = AlbedoTexture.Sample(sampler_lerp, input.uv);

    //float3 lightColor = ComputeLightColor(worldPos.xyz, WolrdNormal.xyz);
    
    return /*float4(lightColor, 1.0f)*/  AlbedoColor;
};