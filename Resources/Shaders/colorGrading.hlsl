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

RWTexture2D<float4> resultTexture : register(u0);
Texture2D<float4> RenderT : register(t1);


static const float3 LumaCoeffs = float3(0.2126, 0.7152, 0.0722);

static const float3 lift     = float3(-0.02,  0.01,  0.04) * 1.4;
static const float3 gamma    = float3(1.0f,  1.0f,  1.0f);
static const float3 gain     = float3( 1 + 0.1 * 0.9,  1 + 0.02 * 0.9,  1 -0.1 * 0.8);


float3 ApplyLiftGammaGain(float3 c)
{
    float3 invGamma = float3(1.0f/gamma.x, 1.0f/gamma.y, 1.0f/gamma.z);
    // 1) Lift: shadow 보정
    c = c + lift * (1.0f - c);

    // 2) Gain: highlights 보정
    c = c * gain;

    // 3) Gamma: mid‑tones 보정 (pow 적용)
    c = pow(c, invGamma);

    return c;
}

float3 AdjustSaturation(float3 col, float sat)
{
    // 1) 명도(luminance) 계산
    float lum = dot(col, LumaCoeffs);
    return lerp(lum.xxx, col, sat);
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    float2 uv = (float2(texCoord) + 0.5f) / float2(cameraScreenData.x, cameraScreenData.y);
    float3 color = RenderT[texCoord.xy].xyz;

    color = ApplyLiftGammaGain(color);
    color = AdjustSaturation(color, 1.075f);

    resultTexture[texCoord.xy] = float4(color, 1); // color.xyz *
    return;
}


