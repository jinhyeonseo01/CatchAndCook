#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"


struct ParticleData
{
    float3 color;
    int textureUse;

    float3 worldPos;
    float size;

    float3 dir;
    float velocity;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
    float size : SIZE;
    float TextureUse : USE;
};

struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
    float TextureUse : USE;
};

StructuredBuffer<ParticleData> ParticleDatas : register(t0);
Texture2D _BaseMap : register(t1);

VS_OUT VS_Main(uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    ParticleData data =ParticleDatas[id];
    output.pos = float4(data.worldPos, 1.0f);
    output.pos = mul(output.pos, ViewMatrix);
    output.color = data.color;
    output.size = data.size;
    output.TextureUse = data.textureUse;
    
    
    return output;
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f
    };


    float scale = input[0].size;

    output[0].pos = input[0].pos + float4(-scale, scale, 0.f, 0.f);
    output[1].pos = input[0].pos + float4(scale, scale, 0.f, 0.f);
    output[2].pos = input[0].pos + float4(scale, -scale, 0.f, 0.f);
    output[3].pos = input[0].pos + float4(-scale, -scale, 0.f, 0.f);

    output[0].pos = mul(output[0].pos, ProjectionMatrix);
    output[1].pos = mul(output[1].pos, ProjectionMatrix);
    output[2].pos = mul(output[2].pos, ProjectionMatrix);
    output[3].pos = mul(output[3].pos, ProjectionMatrix);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);
    
    output[0].color = input[0].color;
    output[1].color = input[0].color;
    output[2].color = input[0].color;
    output[3].color = input[0].color;
    
    output[0].TextureUse = input[0].TextureUse;
    output[1].TextureUse = input[0].TextureUse;
    output[2].TextureUse = input[0].TextureUse;
    output[3].TextureUse = input[0].TextureUse;

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
   
    if(input.TextureUse)
    {
        float4 color = _BaseMap.Sample(sampler_lerp, input.uv);
        
        if (color.a == 0)
            discard;
    
        return color;
    }

    
    return float4(input.color, 1.0f);

}