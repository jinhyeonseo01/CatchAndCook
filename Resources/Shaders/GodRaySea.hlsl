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

cbuffer RayData : register(b1)
{
    float4 lightWorldPos; 
    float2 lightScreenUV; 
    int   sampleCount;    
    float decay;          
    float exposure;       

    float2 padding_ray;
};

float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}

RWTexture2D<float4> resultTexture : register(u0);

Texture2D depthT : register(t0); 
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> EmmT : register(t2);
// RayData 

[numthreads(16,16,1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{

    static const int count = 50;
    int2 texCoord = dispatchThreadID.xy;

    float2 uv = (float2(texCoord) + 0.5) / g_window_size;

    float2 lightUV = lightScreenUV;

    float2 dir = uv - lightUV;
    dir /= length(dir);
    float2 step = dir / count;
    step *= 0.125;

    // 4) 누적 변수
    float illuminationDecay = 10.0;
    float weight           = 1.0 / count;
    float3 godRayColor     = 0;

    float2 currentUV = uv;
    [unroll]
    for (int i = 0; i < count; ++i)
    {
        currentUV -= step;
        if (currentUV.x < 0 || currentUV.x > 1 || currentUV.y < 0 || currentUV.y > 1)
            break;
        float3 occ = EmmT.SampleLevel(sampler_lerp, currentUV, 0);
        godRayColor += occ * illuminationDecay * weight;
        illuminationDecay *= decay;
    }

    godRayColor *= exposure;

    float fade = smoothstep(0.0, 5.0, NdcDepthToViewDepth(depthT.SampleLevel(sampler_lerp, uv, 0).r));
    float3 sceneCol = RenderT.SampleLevel(sampler_lerp, uv, 0).rgb;
    resultTexture[texCoord] = float4(sceneCol + godRayColor , 1);

}