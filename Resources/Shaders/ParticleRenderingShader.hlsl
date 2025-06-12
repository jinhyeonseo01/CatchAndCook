#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"


struct ParticleData
{
    float3 color;
    int life;

    float3 worldPos;
    float p1;

    float3 dir;
    float velocity;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
};

struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
};

StructuredBuffer<ParticleData> ParticleDatas : register(t0);

VS_OUT VS_Main(uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    ParticleData data =ParticleDatas[id];
    output.pos = float4(data.worldPos, 1.0f);
    output.pos = mul(output.pos, ViewMatrix);
    output.color = data.color;
    
    return output;
}

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f
    };


    float scale = 5.0f;

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
    return float4(input.color, 1.0f);

}