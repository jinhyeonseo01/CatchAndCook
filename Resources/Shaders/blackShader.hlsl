RWTexture2D<float4> resultTexture : register(u0); // 결과 텍스처
Texture2D<float4> inputTexture : register(t0); // 입력 텍스처
Texture2D<float4> MAOTexture : register(t1); // 입력 텍스처


static float Luminance[3] = { 0.2126f, 0.7152f, 0.0722f };//float3(0.2126, 0.7152, 0.0722);
static float th = 0.98f;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    
    float3 color = inputTexture[texCoord];
    
    float relativeLuminance = (color.r * Luminance[0]) + (color.g * Luminance[1]) + (color.b * Luminance[2]);

	float bloomFactor = saturate((relativeLuminance - th) / (1.0 - th)); // 스무스 하게 처리

	color *= bloomFactor;

    if (MAOTexture[texCoord].a > 0.1f)
		color = inputTexture[texCoord];
    
    // 결과 텍스처에 쓰기
    resultTexture[texCoord] = float4(color, 1.0f);
}