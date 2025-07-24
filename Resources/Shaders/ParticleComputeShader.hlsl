#include "Global_b0.hlsl"


struct ParticleData
{
    float3 color;
    int textureUse;

    float3 worldPos;
    float p1;

    float3 dir;
    float velocity;
};


cbuffer ParicleHelperParams : register(b4)
{
    int g_paricleCount;
    int g_textureUse;
    float2 g_paddingvv;
    float4 g_clipingColor;
};

RWStructuredBuffer<ParticleData> ParticleDatas : register(u0);

[numthreads(256, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
   
    if (threadIndex.x >= g_paricleCount)
        return;
   
    ParticleData data = ParticleDatas[threadIndex.x];
    
    data.worldPos = data.worldPos +  data.velocity * data.dir * dt;
    
    ParticleDatas[threadIndex.x] = data;
}