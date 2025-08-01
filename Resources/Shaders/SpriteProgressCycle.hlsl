#include "Global_b0.hlsl"


cbuffer PROGRESS_PARAM : register(b4)
{
    float answer;
    float answerRange;
    float currentUV;
    float answerCorrectRange;
};


cbuffer SPRITE_WORLD_PARAM : register(b5)
{
    float3 g_pos;
    float g_alpha;
    
    float2 g_scale;
    float2 padding;
    
    float4 clipingColor;
};

cbuffer SPRITE_TEXTURE_PARAM : register(b6)
{
    float2 g_origintexSize;
    float2 g_texSamplePos;
    float2 g_texSampleSize;
    float2 padding2;
    
};


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D _BaseMap: register(t0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float2 pos = input.pos.xy * g_scale + g_pos.xy;
    output.pos = float4(pos.xy * float2(2, -2) + float2(-1, 1), g_pos.z, 1);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}


float4 PS_Main(VS_OUT input) : SV_TARGET
{
    float uvX = input.uv.x;


    float isInRange = step(answer - answerRange * 0.5f, uvX) * step(uvX, answer + answerRange * 0.5f);
    float isCurrent = step(currentUV - answerCorrectRange * 0.5f , uvX) * step(uvX, currentUV + answerCorrectRange * 0.5f);

    float3 gray = float3(0.3, 0.3, 0.3);
    float3 orange = float3(1.0, 0.5, 0.0);
    float3 green = float3(0.0, 1.0, 0.0);

    float3 baseColor = lerp(gray, orange, isInRange);
    float3 finalColor = lerp(baseColor, green, isCurrent);

    return float4(finalColor, 1.0f);
    
    
}
