RWTexture2D<float4> resultTexture : register(u0); // ��� �ؽ�ó
Texture2D<float4> inputTexture : register(t0); // �Է� �ؽ�ó
Texture2D<float4> MAOTexture : register(t1); // �Է� �ؽ�ó


static float Luminance[3] = { 0.2126f, 0.7152f, 0.0722f };//float3(0.2126, 0.7152, 0.0722);
static float th = 0.5f;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    
    float3 color = inputTexture[texCoord];
    
    float relativeLuminance = (color.r * Luminance[0]) + (color.g * Luminance[1]) + (color.b * Luminance[2]);

	float bloomFactor = saturate((relativeLuminance - th) / (1.0 - th)); // ������ �ϰ� ó��

	color *= bloomFactor * 1.2;

    
    // ��� �ؽ�ó�� ����
    resultTexture[texCoord] = float4(color, 1.0f);
}