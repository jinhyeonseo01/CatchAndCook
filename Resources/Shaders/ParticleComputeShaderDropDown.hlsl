#include "Global_b0.hlsl"


struct ParticleData
{
    float3 color;
    float lifetime;

    float3 worldPos;
    float size;

    float3 dir;
    float velocity;
};


cbuffer ParicleHelperParams : register(b4)
{
    int g_paricleCount;
    int g_textureUse;
    float2 g_paddingvv;
    float4 g_clipingColor;
    
    float changeDirTime;
    float pp1;
    float pp2;
    float pp3;
};

RWStructuredBuffer<ParticleData> ParticleDatas : register(u0);

[numthreads(256, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
   
    if (threadIndex.x >= g_paricleCount)
        return;
   
    ParticleData data = ParticleDatas[threadIndex.x];
    
    data.worldPos = data.worldPos +  data.velocity * data.dir * dt;
    
    data.lifetime += dt;
    
    if (data.lifetime >= 0)
    {
        data.lifetime += dt;
    }

    if (data.lifetime > changeDirTime)
    {
        data.dir.y *= -1;
        normalize(data.dir);
        data.velocity *= 1.2f;
        data.lifetime = -1.0f;
    }
    
    
    ParticleDatas[threadIndex.x] = data;
}