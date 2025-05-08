#include "Global_b0.hlsl"

RWTexture2D<float4> resultTexture : register(u0); // ��� �ؽ�ó
Texture2D depthT : register(t1); // ��� �ؽ�ó
Texture2D<float4> inputTexture : register(t0); // �Է� �ؽ�ó


cbuffer cameraParams : register(b2)
{
    row_major Matrix ViewMatrix;
    row_major Matrix ProjectionMatrix;
    row_major Matrix VPMatrix;
    row_major Matrix InvertViewMatrix;
    row_major Matrix InvertProjectionMatrix;
    row_major Matrix InvertVPMatrix;

    float4 cameraPos;
    float4 cameraLook;
    float4 cameraUp;
    float4 cameraFrustumData;
    float4 cameraScreenData;
};

cbuffer DepthParam : register(b1)
{
    float g_fogMin;
    float g_fogMax;
    float2 padding_dof;
}

float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}


static const float weights[19] = {
    0.00147945f,
    0.00380424f,
    0.00875346f,
    0.01802341f,
    0.03320773f,
    0.05475029f,
    0.08077532f,
    0.10663900f,
    0.12597909f,
    0.13317600f, // center
    0.12597909f,
    0.10663900f,
    0.08077532f,
    0.05475029f,
    0.03320773f,
    0.01802341f,
    0.00875346f,
    0.00380424f,
    0.00147945f
};

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
    int2 loadCoord = tileStart + int2(groupThreadID.xy);
    loadCoord.x = clamp(loadCoord.x, 0, g_window_size.x - 1);
    loadCoord.y = clamp(loadCoord.y, 0, g_window_size.y - 1);

    
    // ������ ���� ������ �� �ȼ����� ������ �ȼ��� �Ź� Load �ϴ��� ��������. �ȼ� �� * 19. �ʹ� ����.
    // �׷��� �ش� �׷� �� �ȼ����� ���� ĳ���ؼ� �ѹ��� Load�ϰ� �����ϰ� �ٲ�.

    int localIndex = groupThreadID.x + groupThreadID.y * threadGroupSizeX;
    sharedTile[localIndex] = inputTexture.Load(int3(loadCoord, 0)).rgb;
    

    GroupMemoryBarrierWithGroupSync(); // �׷쳻 ������鿡�� �̸� Load �س��� ��ũ �ɱ�.

    float depthZ = depthT[texCoord.xy];
    float viewZ = NdcDepthToViewDepth(depthZ);
    float distFog = saturate((viewZ - g_fogMin) / (g_fogMax - g_fogMin));
    float fogFactor = pow(1 - distFog, 1);


    float3 blurColor = float3(0, 0, 0);

    [unroll]
    for (int i = -blurRadius; i <= blurRadius; i++)
    {
        int2 sampleCoord = texCoord + int2(i, 0);
        sampleCoord.x = clamp(sampleCoord.x, 0, g_window_size.x - 1);
        sampleCoord.y = clamp(sampleCoord.y, 0, g_window_size.y - 1);
         
        float3 color;
        if (sampleCoord.x >= tileStart.x && sampleCoord.x < tileEnd.x && //ĳ�̱����̸� ĳ�� ���
			sampleCoord.y >= tileStart.y && sampleCoord.y < tileEnd.y)
        {
            int2 localCoord = sampleCoord - tileStart;
            int sharedIndex = localCoord.x + localCoord.y * threadGroupSizeX;
            color = sharedTile[sharedIndex];
        }
        else
            color = inputTexture.Load(int3(sampleCoord, 0)).rgb; // �ƴϸ� ���� Load
        
        blurColor += weights[i + blurRadius] * color;
    }
    

    resultTexture[texCoord] = float4(lerp(blurColor, sharedTile[localIndex], fogFactor), 1.0f);
}