#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "ShadowReceive_b6.hlsl"

//[���̴�����][�ø�ó��]

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

Texture2D _BakedGIMap : register(t8);
Texture2D _BakedGIMap2 : register(t9);


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.f);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 worldPos = PositionTexture.Sample(sampler_point, input.uv);
    float3 worldNormal = normalize(NormalTexture.Sample(sampler_point, input.uv).xyz);
    float4 albedoColor = AlbedoTexture.Sample(sampler_lerp, input.uv);
    float4 MAOEColor = MAOTexture.Sample(sampler_point, input.uv);

    LightingResult lightColor = ComputeLightColor(worldPos.xyz, worldNormal.xyz);
    float4 ShadowColor = _BakedGIMap.Sample(sampler_lerp_clamp, saturate(dot(float3(0, 1, 0), worldNormal) * 0.5 + 0.5));
    float4 ShadowColor2 = _BakedGIMap2.Sample(sampler_lerp_clamp, saturate(dot(float3(0, 1, 0), worldNormal) * 0.5 + 0.5));
    ShadowColor = lerp(ShadowColor, ShadowColor2, fmod(g_skyBlendTime, 1));

    float3 uvz[4];
	ComputeCascadeShadowUVs(worldPos.xyz, uvz);
    lightColor.atten *= ComputeCascadeShadowAtten(uvz, mul(float4(worldPos.xyz, 1), ViewMatrix).z);

    float subIntensity =  lerp(1, 0.3, clamp(0, 1, lightColor.intensity));
    float3 finalColor = (lerp(ShadowColor * albedoColor, albedoColor, lightColor.atten) + float4(lightColor.subColor * subIntensity, 0)).xyz;

    float3 viewDir = normalize(cameraPos.xyz - worldPos.xyz);

    // Rim
    float N2L = saturate(dot(lightColor.direction, worldNormal));
    float MinusN2L = saturate(dot(-lightColor.direction, worldNormal));
    float fresnel = (1 - saturate(dot(viewDir, worldNormal)));
    float3 rim = pow(fresnel, exp2(2)) * pow(MinusN2L, 3) * float3(0.6, 0.6, 0.8) * 0.5;

    float smoothness = MAOEColor.g;
    float roughness = 1 - smoothness;

    float3 V = viewDir;
    float3 L = lightColor.direction;
    float3 H = normalize(V + L);
    float NdotH = pow(saturate(dot(worldNormal, H)), exp2(smoothness * 9));
    float invDenom = 1.0 / max(1 - roughness, 0.001);
    float spec = saturate((NdotH - roughness) * invDenom) * smoothness * lightColor.intensity;
    finalColor = lerp(finalColor, pow(finalColor, 2), MAOEColor.r);


	// 최종 스펙큘러

    if (MAOEColor.a == 1)
		finalColor = albedoColor;
    
    return float4(finalColor + rim + spec, 1.0f);
};
