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



float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}
float OcclusionFunction(float distZ)
{
	float occlusion = 0.0f;
    float gSurfaceEpsilon = 0.05;
    float gOcclusionFadeStart = 0.2;
    float gOcclusionFadeEnd = 0.2;
	if(distZ > gSurfaceEpsilon)
		occlusion = saturate( (gOcclusionFadeEnd - distZ) / (gOcclusionFadeEnd - gOcclusionFadeStart) );
	
	return occlusion;	
}

RWTexture2D<float4> resultTexture : register(u0);
RWTexture2D<float> ssaoResultTexture : register(u1);

Texture2D depthT : register(t0); 
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> PositionT : register(t2);
Texture2D<float4> NormalT : register(t3);
Texture2D<float4> MAOET : register(t4);


#define SAMPLE_COUNT 16
#define RADIUS 0.3
#define BIAS 0.05
#define INTENSITY 1.25
#define POWER 3

static const float3 samples[SAMPLE_COUNT] = 
{
    float3(0.135, 0.242, 0.968),
    float3(-0.545, 0.312, 0.778),
    float3(0.635, 0.133, 0.759),
    float3(-0.322, -0.455, 0.835),
    float3(-0.742, -0.031, 0.669),
    float3(0.160, -0.532, 0.832),
    float3(0.464, -0.540, 0.701),
    float3(-0.104, -0.840, 0.533),
    float3(-0.687, -0.566, 0.456),
    float3(0.445, 0.735, 0.512),
    float3(-0.248, 0.859, 0.452),
    float3(0.814, 0.415, 0.406),
    float3(0.846, -0.380, 0.371),
    float3(-0.894, 0.242, 0.375),
    float3(-0.352, -0.924, 0.147),
    float3(0.125, 0.210, 0.969)
};

float Hash(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float GetRandomRotation(float2 texCoord)
{
    return Hash(texCoord) * 2.0 * 3.14159265;
}

[numthreads(16, 16, 1)]
void CS_Blur(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    const int BLUR_RADIUS = 4;

    if (texCoord.x >= (uint)cameraScreenData.x || texCoord.y >= (uint)cameraScreenData.y)
        return;

    float horizontalBlur = 0.0;
    float weight = 0.0;

    [unroll]
    for (int x = -BLUR_RADIUS; x <= BLUR_RADIUS; x++)
    {
        int2 sampleCoord = texCoord + int2(x, 0);
        if (sampleCoord.x < 0 || sampleCoord.x >= (int)cameraScreenData.x)
            continue;
        
        float sampleAO = ssaoResultTexture[sampleCoord];
        horizontalBlur += sampleAO;
        weight += 1.0;
    }
    horizontalBlur /= max(weight, 1.0);

    float verticalBlur = 0.0;
    weight = 0.0;

    [unroll]
    for (int y = -BLUR_RADIUS; y <= BLUR_RADIUS; y++)
    {
        int2 sampleCoord = texCoord + int2(0, y);
        if (sampleCoord.y < 0 || sampleCoord.y >= (int)cameraScreenData.y)
            continue;
        
        float sampleAO = ssaoResultTexture[sampleCoord];
        verticalBlur += sampleAO;
        weight += 1.0;
    }
    verticalBlur /= max(weight, 1.0);

    float diagonalBlur = 0.0;
    weight = 0.0;

    //대각선
    [unroll]
    for (int d = -3; d <= 3; d++)
    {
        {
            int2 sampleCoord = texCoord + int2(d, d);
            if (sampleCoord.x >= 0 && sampleCoord.x < (int)cameraScreenData.x &&
                sampleCoord.y >= 0 && sampleCoord.y < (int)cameraScreenData.y)
            {
                float sampleAO = ssaoResultTexture[sampleCoord];
                diagonalBlur += sampleAO;
                weight += 1.0;
            }
        }
        {
            int2 sampleCoord = texCoord + int2(-d, d);
            if (sampleCoord.x >= 0 && sampleCoord.x < (int)cameraScreenData.x &&
                sampleCoord.y >= 0 && sampleCoord.y < (int)cameraScreenData.y)
            {
                float sampleAO = ssaoResultTexture[sampleCoord];
                diagonalBlur += sampleAO;
                weight += 1.0;
            }
        }
    }

    diagonalBlur /= max(weight, 1.0);

    float aoBlurred = (horizontalBlur + verticalBlur + diagonalBlur) / 3.0;
    float aoFinal = ssaoResultTexture[texCoord] = lerp(ssaoResultTexture[texCoord], aoBlurred, 0.8);

    float4 originalColor = RenderT.Load(int3(texCoord, 0));
    float4 finalColor = lerp(originalColor*float4(0.1,0.07,0.12,1), originalColor, saturate(aoFinal));

    resultTexture[texCoord] = finalColor;
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;

    if (texCoord.x >= (uint)cameraScreenData.x || texCoord.y >= (uint)cameraScreenData.y) {
        return;
    }

    const float MaxDepth = 150;
    
    float2 uv = (float2(texCoord) + 0.5) / cameraScreenData.xy;

    float3 worldPos = PositionT.Load(int3(texCoord, 0)).xyz;
    float3 worldNormal = normalize(NormalT.Load(int3(texCoord, 0)).xyz);
    float depth = depthT.Load(int3(texCoord, 0));

    if (depth >= 0.9999 || MAOET.Load(int3(texCoord, 0)).z == -1) { // 뎁스 없음.
        resultTexture[texCoord] = RenderT.Load(int3(texCoord, 0));
        ssaoResultTexture[texCoord] = 1;
        return;
    }

    float3 viewPos = mul(float4(worldPos, 1.0), ViewMatrix).xyz;
    float3 viewNormal = normalize(mul(float4(worldNormal, 0.0), ViewMatrix).xyz);

    if (viewPos.z > MaxDepth)
    {
	    resultTexture[texCoord] = RenderT.Load(int3(texCoord, 0));
        ssaoResultTexture[texCoord] = 1;
        return;
    }


    float randomAngle = GetRandomRotation(texCoord);
    float cosAngle = cos(randomAngle);
    float sinAngle = sin(randomAngle);

    float occlusion = 0.0;

    [unroll]
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        float3 sampleDir = samples[i];

        float2 rotatedOffset = float2( // 삼각 덧샘공식으로 랜덤 벡터에 샘플 더해
            sampleDir.x * cosAngle - sampleDir.y * sinAngle,
            sampleDir.x * sinAngle + sampleDir.y * cosAngle
        );

        float3 offset = float3(rotatedOffset, sampleDir.z);
        if (dot(offset, viewNormal) < 0.0)
            offset = -offset;

        float3 samplePos = viewPos + offset * RADIUS * saturate((MaxDepth - viewPos.z) / MaxDepth);

        float4 projectedPos = mul(float4(samplePos, 1.0), ProjectionMatrix);
        projectedPos.xy /= projectedPos.w;
        projectedPos.y = -projectedPos.y;
        float2 sampleUV = projectedPos.xy * 0.5 + 0.5;

        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            continue;

        int2 sampleTexCoord = int2(sampleUV * cameraScreenData.xy); //샘플 위치의 depth 값을 가져오기
        float sampleDepth = depthT.Load(int3(sampleTexCoord, 0));
        float sampleViewZ = NdcDepthToViewDepth(sampleDepth);

        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(viewPos.z - sampleViewZ));
        occlusion += (sampleViewZ <= (samplePos.z - BIAS) ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion * saturate((MaxDepth - viewPos.z) / MaxDepth) / SAMPLE_COUNT) * INTENSITY;

    //float4 originalColor = RenderT.Load(int3(texCoord, 0));
    //float4 finalColor = originalColor * occlusion;

    ssaoResultTexture[texCoord] = occlusion;
    GroupMemoryBarrierWithGroupSync(); // 싱크 걸고 블러링

    CS_Blur(dispatchThreadID);
}