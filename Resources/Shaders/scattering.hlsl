#include "Light_b3.hlsl"

SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_shadow : register(s5);
SamplerState sampler_lerp_clamp : register(s6);

cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
};

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


cbuffer ScatterParams : register(b5)
{
    float phaseG;
    float absorption;
    float2 Padding;

    float density;
    float3 scatterColor;


    float3 direction;
    float paddding54;
};

Texture2D<float4> FoggedScene : register(t0);
Texture2D<float> DepthTexture : register(t1);
RWTexture2D<float4> Output : register(u0);

float3 ReconstructViewPos(int2 pixel)
{
    float4 posProj;
    posProj.xy = (pixel + 0.5f) / cameraScreenData.xy * 2.0f - 1.0f;
    posProj.y *= -1;
    posProj.z = DepthTexture.Load(int3(pixel, 0));
    posProj.w = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    return posView.xyz / posView.w;
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    int2 pixel = id.xy;
    float3 baseColor = FoggedScene[pixel].rgb;

    float3 viewPos = ReconstructViewPos(pixel);
    float3 worldPos = mul(float4(viewPos, 1.0f), InvertViewMatrix).xyz;

    //viewDir: 픽셀에서 카메라(또는 관찰자) 방향으로 향하는 단위 벡터  
    //lightDir: 픽셀에서 빛(광원) 방향으로 향하는 단위 벡터
    
    float3 lightToPixel = normalize(mainLight.position - worldPos); // 픽셀에서 광원으로 나가는방향
    float3 ndir = normalize(direction);
    float3 lightDir = normalize(-ndir); //픽셀에서 광원으로 향하는방향

    float cosTheta = dot(lightToPixel, lightDir);
    float g2 = phaseG * phaseG;
    float denom = max(1 + g2 - 2 * phaseG * cosTheta, 1e-3);
    float phase = (1 - g2) / (4 * 3.14159 * pow(denom, 1.5));

    float lightDepth = length(worldPos - mainLight.position);
    float atten = 1.0 / (1.0 + log(1.0 + absorption * lightDepth));

    float3 scatter = scatterColor * phase * atten * density;
    float3 finalColor = baseColor + scatter;
    Output[pixel] = float4(finalColor, 1);
}




//[numthreads(16, 16, 1)]
//void CS_Main(uint3 id : SV_DispatchThreadID)
//{
//    int2 pixel = id.xy;
//    float3 baseColor = FoggedScene[pixel].rgb;

//    float3 viewPos = ReconstructViewPos(pixel);
//    float3 worldPos = mul(float4(viewPos, 1.0f), InvertViewMatrix).xyz;

//    //viewDir: 픽셀에서 카메라(또는 관찰자) 방향으로 향하는 단위 벡터  
//    //lightDir: 픽셀에서 빛(광원) 방향으로 향하는 단위 벡터
//    //light -  > direction = vec3(-0.024f, 1.0f, 0.899f);
//    //light -  > position = vec3(769.f, 1282.f, 750.0f);
    
//    float3 lightToPixel = normalize(mainLight.position - worldPos); // 픽셀에서 광원으로 나가는방향
//    float3 lightDir = normalize(-mainLight.direction); //픽셀에서 광원으로 향하는방향

//    float cosTheta = dot(lightToPixel, lightDir);
//    float g2 = phaseG * phaseG;
//    float denom = max(1 + g2 - 2 * phaseG * cosTheta, 1e-3);
//    float phase = (1 - g2) / (4 * 3.14159 * pow(denom, 1.5));

//    float lightDepth = length(worldPos - mainLight.position);
//    float atten = 1.0 / (1.0 + log(1.0 + absorption * lightDepth));

//    float3 scatter = scatterColor * phase * atten * density;
//    float3 finalColor = baseColor + scatter;
//    Output[pixel] = float4(finalColor, 1);
//}