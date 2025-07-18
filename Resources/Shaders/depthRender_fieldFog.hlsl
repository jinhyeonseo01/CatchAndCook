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

cbuffer FogParam : register(b1)
{
   
    float3 g_fogColor;
    float power;

    float g_fogMin;
    float g_fogMax;
    int g_depthRendering;
    float padding;
    
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
Texture2D depthT : register(t0); 
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> PositionT : register(t2);
Texture2D<float4> ColorGrading : register(t3);

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    float2 uv = (float2(texCoord) + 0.5f) / float2(cameraScreenData.x, cameraScreenData.y);
    float3 color2 = RenderT[texCoord.xy].xyz;

    float4 posProj = float4(0, 0, 0, 0);
    posProj.xy = (texCoord.xy + 0.5f) / float2(cameraScreenData.x, cameraScreenData.y) * 2.0f - 1.0f;
    posProj.y *= -1;
    posProj.z = depthT[texCoord.xy];
    posProj.w = 1.0f;

    if (posProj.z == 1 || ((color2.x + color2.y + color2.z) >= 2.9))
    {
        resultTexture[texCoord.xy] = float4(color2, 1);
		return;
    }
    
    //if (RenderT[texCoord.xy].a < 0.1f)
    //{
    //    resultTexture[texCoord.xy] = float4(color2, 1);
    //    return;
    //}
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    float3 actualPosView = posView.xyz / posView.w;
    

    float dist = length(actualPosView);    
    float distFog = saturate((dist - g_fogMin) / (g_fogMax - g_fogMin));
    
    float fogFactor = clamp(0, 1, exp(-distFog * power));
    fogFactor = smoothstep(0.0, 1.0, fogFactor);
    
    float3 color = lerp(g_fogColor, color2, fogFactor);
    
    resultTexture[texCoord.xy] = float4(color.xyz, 1.0f);
    return;


}


