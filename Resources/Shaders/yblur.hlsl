#include "Global_b0.hlsl"

RWTexture2D<float4> resultTexture : register(u0); // 결과 텍스처
Texture2D<float4> inputTexture : register(t0); // 입력 텍스처

static const float weights[19] =
{
    0.00148f, // x = -9
    0.00381f, // x = -8
    0.00876f, // x = -7
    0.01802f, // x = -6
    0.03320f, // x = -5
    0.05475f, // x = -4
    0.08080f, // x = -3
    0.10660f, // x = -2
    0.12600f, // x = -1
    0.13320f, // x =  0 (중앙)
    0.12600f, // x =  1
    0.10660f, // x =  2
    0.08080f, // x =  3
    0.05475f, // x =  4
    0.03320f, // x =  5
    0.01802f, // x =  6
    0.00876f, // x =  7
    0.00381f, // x =  8
    0.00148f  // x =  9
};

//static const float weights[5] =
//{
//    0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f
//};

static const int blurRadius = 9;

static const int threadGroupSizeX = 16;
static const int threadGroupSizeY = 16;
groupshared float3 sharedTile[threadGroupSizeX * threadGroupSizeY];

[numthreads(threadGroupSizeX, threadGroupSizeY, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
    int2 texCoord = threadIndex.xy;
    int2 tileStart = int2(groupID.x * threadGroupSizeX, groupID.y * threadGroupSizeY);
    int2 tileEnd   = tileStart + int2(threadGroupSizeX, threadGroupSizeY);

    // 성능이 느린 이유각 각 픽셀마다 인접한 픽셀을 매번 Load 하던게 문제였음. 픽셀 수 * 19. 너무 많아.
    // 그래서 해당 그룹 내 픽셀들은 전부 캐싱해서 한번만 Load하고 공유하게 바꿈.

    int localIndex = groupThreadID.x + groupThreadID.y * threadGroupSizeX;
    int2 loadCoord = tileStart + int2(groupThreadID.xy);
    loadCoord.x = clamp(loadCoord.x, 0, g_window_size.x - 1);
    loadCoord.y = clamp(loadCoord.y, 0, g_window_size.y - 1);
    sharedTile[localIndex] = inputTexture.Load(int3(loadCoord, 0)).rgb;
    

    GroupMemoryBarrierWithGroupSync(); // 그룹내 스레드들에서 Load 해놓고 싱크.



    float3 blurColor = float3(0, 0, 0);

    [unroll]
    for (int i = -blurRadius; i <= blurRadius; i++)
    {
        int2 sampleCoord = texCoord + int2(0, i);
        sampleCoord.x = clamp(sampleCoord.x, 0, g_window_size.x - 1);
        sampleCoord.y = clamp(sampleCoord.y, 0, g_window_size.y - 1);
         
        float3 color;
        if (sampleCoord.x >= tileStart.x && sampleCoord.x < tileEnd.x && //캐싱구간이면 캐싱 사용
			sampleCoord.y >= tileStart.y && sampleCoord.y < tileEnd.y)
        {
            int2 localCoord = sampleCoord - tileStart;
            int sharedIndex = localCoord.x + localCoord.y * threadGroupSizeX;
            color = sharedTile[sharedIndex];
        }
        else
            color = inputTexture.Load(int3(sampleCoord, 0)).rgb; // 아니면 직접 Load
        
        blurColor += weights[i + blurRadius] * color;
    }
    

    resultTexture[texCoord] = float4(blurColor, 1.0f);
}