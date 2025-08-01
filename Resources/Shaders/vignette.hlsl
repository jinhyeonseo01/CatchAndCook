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

cbuffer VignetteParam : register(b1)
{
	float power;
	float3 vignetteColor;
    float startRound;
    float Range;
    float2 padding;
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

RWTexture2D<float4> resultTexture : register(u0);
Texture2D<float4> RenderT : register(t1);

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    float2 uv = (float2(texCoord) + 0.5f) / float2(cameraScreenData.x, cameraScreenData.y);
    float3 color = RenderT[texCoord.xy].xyz;

    uv = uv * 2 - 1;
    float d = length(uv);
    float invMask = smoothstep(startRound, startRound + Range, d);

    resultTexture[texCoord.xy] = float4(color * (1 - invMask) + (1 - color) * vignetteColor * invMask, 1.0f); // color.xyz *
    return;
}


