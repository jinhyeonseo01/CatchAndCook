RWTexture2D<float4> resultTexture : register(u0); 

Texture2D<float4> blurrTexture : register(t0); 
Texture2D<float4> originTexture : register(t1); 

static float weight = 2.0f;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    float3 color;
 
    color = clamp(blurrTexture[texCoord] * weight + originTexture[texCoord], 0, 1.0f);
  
    resultTexture[texCoord] = float4(color, 1.0f);
}