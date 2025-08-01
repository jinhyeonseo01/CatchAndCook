#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"


#define PI 3.14159f


cbuffer popo : register(b7)
{
    float2 uv;
}


struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 worldNormal : NORMAL;
};

Texture2D g_tex_0 : register(t0);

VS_IN WaveGeneration(VS_IN input)
{
    const int waveCount = 3;
    float amplitudes[waveCount] = { 11.0f, 9.0f, 7.0f };
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f };
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f };

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)),
        normalize(float2(0.0f, 1.0f)),
        normalize(float2(-0.5f, 0.7f))
    };

    float3 modifiedPos = input.pos;
    float dHdX = 0.0f; 
    float dHdZ = 0.0f; 

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

        modifiedPos.y += amplitudes[i] * wave;

        // 편미분 계산
        float dWavedX = frequency * waveDerivative * direction.x;
        float dWavedZ = frequency * waveDerivative * direction.y;

        dHdX += amplitudes[i] * dWavedX;
        dHdZ += amplitudes[i] * dWavedZ;
    }

    // 법선 벡터 계산
    float3 normal = normalize(float3(-dHdX, 1.0f, -dHdZ));

    VS_IN result;
    result.pos = modifiedPos;
    result.normal = normal;

    return result;
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    VS_IN result = WaveGeneration(input);
 
    // 월드, 뷰, 프로젝션 변환
    float4 worldPos = mul(float4(result.pos, 1.0f), LocalToWorldMatrix);
    
    output.worldPos = worldPos.xyz;
    output.pos = mul(worldPos, VPMatrix);
    output.uv = input.uv;
    output.worldNormal = normalize(mul(float4(result.normal, 0.0f), LocalToWorldMatrix).xyz);
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 color;
    
    float4 worldPos = mul(input.pos, InvertVPMatrix);
    float3 WolrdNormal = input.worldNormal;
    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral,worldPos.xyz, WolrdNormal.xyz, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 2)
        {
            color += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, toEye);
        }
    }
    
    return float4(color, 1.0f);
}
