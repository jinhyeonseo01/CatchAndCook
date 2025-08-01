RWTexture2D<float4> resultTexture : register(u0); // 결과 텍스처
Texture2D<float4> inputTexture : register(t0); // 입력 텍스처
Texture2D<float4> MAOTexture : register(t1); // 입력 텍스처
Texture2D depthT : register(t2); 

SamplerState sampler_lerp : register(s0);

static float Luminance[3] = { 0.2126f, 0.7152f, 0.0722f };//float3(0.2126, 0.7152, 0.0722);
static float th = 0.5f;

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

float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;
    float2 uv = (float2(texCoord) + 0.5) / g_window_size;
    
    float3 color = inputTexture[texCoord];
    
    float relativeLuminance = (color.r * Luminance[0]) + (color.g * Luminance[1]) + (color.b * Luminance[2]);

	float bloomFactor = saturate((relativeLuminance - th) / (1.0 - th)); // 스무스 하게 처리

    float fade = smoothstep(0.0, 5.0, NdcDepthToViewDepth(depthT.SampleLevel(sampler_lerp, uv, 0).r));
	color *= bloomFactor * 1.2 * fade;

    
    // 결과 텍스처에 쓰기
    resultTexture[texCoord] = float4(color, 1.0f);
}