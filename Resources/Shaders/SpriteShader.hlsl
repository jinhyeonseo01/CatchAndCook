#include "Global_b0.hlsl"

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
 
    float2 tex_scale = (g_texSampleSize / g_origintexSize);
    float2 tex_offset = (g_texSamplePos / g_origintexSize);
    
    output.uv = input.uv * tex_scale + tex_offset;
    
    output.color = input.color;
    
    return output;
}


float4 PS_Main(VS_OUT input) : SV_TARGET
{
  
    float4 texColor = _BaseMap.Sample(sampler_lerp, input.uv);
    
    if (length(texColor.a - clipingColor.a) < 0.001)
    {
        discard;
    }
    
    return texColor * input.color * g_alpha;
}
