#include "Global_b0.hlsl"


struct ParticleData
{
    float3 color;
    int life;

    float3 worldPos;
    float p1;

    float3 dir;
    float velocity;
};

RWStructuredBuffer<ParticleData> ParticleDatas : register(u0);

[numthreads(256, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    ParticleData data = ParticleDatas[threadIndex.x];
    
    data.worldPos = data.worldPos +  data.velocity * data.dir;
    
    data.color = float3(1, 1, 1);
    
    ParticleDatas[threadIndex.x] = data;
}