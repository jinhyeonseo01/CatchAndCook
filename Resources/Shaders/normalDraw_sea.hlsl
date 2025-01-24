#include "GLOBAL.hlsl"

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

#define PI 3.14159f

cbuffer test : register(b1)
{
    row_major matrix WorldMat;
}

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

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VS_IN WaveGeneration(VS_IN input)
{
    const int waveCount = 3; // 파동의 개수
    float amplitudes[waveCount] = { 9.0f, 6.0f, 4.0f }; // 각 파동의 진폭 (높이를 줄여 자연스럽게)
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f }; // 각 파동의 파장 (더 넓은 범위)
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f }; // 각 파동의 속도 (속도 조정)

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)), // 주 방향 (우측 하단 방향으로 진행)
        normalize(float2(0.0f, 1.0f)), // 수직 방향 (위쪽 방향)
        normalize(float2(-0.5f, 0.7f)) // 대각선 방향 (좌측 위쪽 방향)
    };

    // 초기 위치
    float3 modifiedPos = input.pos;
    float3 modifiedNormal = float3(0.0f, 0.0f, 0.0f); // 초기화 변경

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

        modifiedPos.xz += amplitudes[i] * direction * waveDerivative;
        modifiedPos.y += amplitudes[i] * wave;

        float3 tangentX = float3(direction.x, waveDerivative * direction.x, 0.0f);
        float3 tangentZ = float3(0.0f, waveDerivative * direction.y, direction.y);
        
        modifiedNormal += cross(tangentX, tangentZ);
    }

    modifiedNormal = normalize(modifiedNormal);

    VS_IN result;
    result.pos = modifiedPos;
    result.normal = modifiedNormal;

    return result;
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    VS_IN result = WaveGeneration(input);

    output.pos = float4(result.pos, 1.0f);
    output.normal = result.normal;
    
    return output;
}

[maxvertexcount(2)]
void GS_Main(point VS_OUT input[1], inout LineStream<GS_OUT> outputStream)
{
    GS_OUT output;
    
    float4 worldPos = mul(float4(input[0].pos.xyz, 1.0f), WorldMat);
    float4 normalWorld = mul(float4(input[0].normal, 0.0f), WorldMat);
    
    normalWorld = normalize(normalWorld);
    output.pos = mul(worldPos, VPMatrix);
    output.color = float4(1, 0, 0, 0);
    outputStream.Append(output);
    
    output.pos = mul(worldPos + normalWorld * 5.0f, VPMatrix);
    output.color = float4(0, 1, 0, 0);
    outputStream.Append(output);

}


float4 PS_Main(GS_OUT input) : SV_Target
{
    return input.color;
}
