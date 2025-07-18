#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"

#define MAX_WAVE 10
struct Wave
{
    float amplitude;
    float wavelength;
    float speed;
    float steepness;
    
    float2 direction;
    float2 padding;
};

cbuffer SeaParam : register(b7)
{
    float4 g_seaBaseColor;
    float4 g_seaShallowColor;
    
    float g_blendingFact;
    float3 g_sea_diffuse;

    float g_specularPower;
    float3 g_sea_light_direction;
    
    float g_envPower;
    int g_wave_count;
    
    float2 sea_padding;

    Wave waves[MAX_WAVE];
}


Texture2DArray _bumpMap : register(t1);
Texture2D _bumpMap2 : register(t2);
Texture2D _dudv : register(t3);
TextureCube _cubemap_skyTexture : register(t4);
TextureCube _cubemap_skyNTexture : register(t5);
TextureCube _cubemap_skyETexture : register(t6);




#define G_MaxTess 4
#define G_MinTess 1

#define PI 3.14159f
#define DIST_MAX 900.0f
#define DIST_MIN 10.0f


///////////////////////////////////////////////////////////////////////////
// 1) 정점 셰이더 관련 구조체
///////////////////////////////////////////////////////////////////////////

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

///////////////////////////////////////////////////////////////////////////
// 2) Hull/Domain 셰이더에 전달될 구조체
///////////////////////////////////////////////////////////////////////////

struct HS_OUT
{
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct DS_OUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

///////////////////////////////////////////////////////////////////////////
// 3) Gerstner Wave 함수 
///////////////////////////////////////////////////////////////////////////
void WaveGeneration(inout float3 worldPos, inout float3 worldNormal)
{
    int waveCount = g_wave_count;

    float3 modifiedPos = worldPos;

    float waveY = 0;
    for (int i = 0; i < waveCount; i++)
    {
        Wave wave = waves[i];

        float frequency = 2.0f * PI / wave.wavelength;
        float phase = wave.speed * g_Time;
        float2 dir = normalize(wave.direction);
        float steep = wave.steepness;

        float dotVal = dot(dir, worldPos.xz);

        float waveSin = sin(dotVal * frequency + phase);
        float waveCos = cos(dotVal * frequency + phase);
        
        modifiedPos.x += steep * wave.amplitude * dir.x * waveCos;
        modifiedPos.z += steep * wave.amplitude * dir.y * waveCos;
        waveY += wave.amplitude * waveSin;
    }
    
    modifiedPos.y += waveY;

    // 수정된 위치 반영 및 기본 법선 할당
    worldPos = modifiedPos;
    worldNormal = float3(0, 1, 0);
}


///////////////////////////////////////////////////////////////////////////
// 4) Vertex Shader
///////////////////////////////////////////////////////////////////////////
VS_OUT VS_Main(VS_IN input, uint instanceID : SV_InstanceID)
{
    VS_OUT output;

    // 인스턴스 변환 행렬 가져오기 (프로젝트 환경에 맞춰 사용)
    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + instanceID];
    row_major float4x4 l2wMatrix = data.localToWorld;

    // 정점 -> 월드
    float4 worldPos = mul(float4(input.pos, 1.0f), l2wMatrix);
    float3 worldNormal = mul(float4(input.normal, 0.0f), l2wMatrix);

    output.worldPos = worldPos;
    output.uv = input.uv;
    output.normal = input.normal;
    return output;
}

///////////////////////////////////////////////////////////////////////////
// 5) Hull Shader (테셀레이션 제어를 위한 상수 셰이더)
///////////////////////////////////////////////////////////////////////////
struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

float CalcTessFactor(float3 p)
{
    float d = distance(p, cameraPos.xyz);
    float s = smoothstep(DIST_MIN, DIST_MAX, d);
    float tess = exp2(lerp(G_MaxTess, G_MinTess, s));
    return clamp(tess, 1.0f, 64.0f);
}


PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchConstOutput pc;
    
    float3 e0 = 0.5f * (patch[0].worldPos.xyz + patch[2].worldPos.xyz);
    float3 e1 = 0.5f * (patch[0].worldPos.xyz + patch[1].worldPos.xyz);
    float3 e2 = 0.5f * (patch[1].worldPos.xyz + patch[3].worldPos.xyz);
    float3 e3 = 0.5f * (patch[2].worldPos.xyz + patch[3].worldPos.xyz);
    float3 c = 0.25f * (patch[0].worldPos.xyz + patch[1].worldPos.xyz + patch[2].worldPos.xyz + patch[3].worldPos.xyz);

    pc.edges[0] = CalcTessFactor(e0);
    pc.edges[1] = CalcTessFactor(e1);
    pc.edges[2] = CalcTessFactor(e2);
    pc.edges[3] = CalcTessFactor(e3);

    pc.inside[0] = CalcTessFactor(c);
    pc.inside[1] = pc.inside[0];
    
    return pc;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUT hout;
    hout.worldPos = patch[vertexID].worldPos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    return hout;
}

///////////////////////////////////////////////////////////////////////////
// 6) Domain Shader
///////////////////////////////////////////////////////////////////////////
[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 uvDomain : SV_DomainLocation)
{
    DS_OUT dout;
    
    float4 p0 = lerp(quad[0].worldPos, quad[1].worldPos, uvDomain.x);
    float4 p1 = lerp(quad[2].worldPos, quad[3].worldPos, uvDomain.x);
    dout.worldPos = lerp(p0, p1, uvDomain.y);

    float2 t0 = lerp(quad[0].uv, quad[1].uv, uvDomain.x);
    float2 t1 = lerp(quad[2].uv, quad[3].uv, uvDomain.x);
    dout.uv = lerp(t0, t1, uvDomain.y);

    float3 n0 = lerp(quad[0].normal, quad[1].normal, uvDomain.x);
    float3 n1 = lerp(quad[2].normal, quad[3].normal, uvDomain.x);
    dout.normal = lerp(n0, n1, uvDomain.y);

    WaveGeneration(dout.worldPos.xyz, dout.normal.xyz);

   
    dout.clipPos = mul(dout.worldPos, VPMatrix);
    return dout;
}


///////////////////////////////////////////////////////////////////////////
// 7) Pixel Shader
///////////////////////////////////////////////////////////////////////////
float4 PS_Main(DS_OUT input) : SV_Target0
{
    ///////////////////////////////////////////////////////////////////////////
    // 1. 프레임 인덱스 및 애니메이션 인자 계산
    ///////////////////////////////////////////////////////////////////////////
    float frameIndex = fmod(g_Time * 4.0f, 120.0f);
    float i0 = floor(frameIndex);
    float i1 = (i0 + 1 >= 120.0f) ? 0.0f : i0 + 1.0f;
    float alpha = frac(frameIndex);

    ///////////////////////////////////////////////////////////////////////////
    // 2. UV 좌표 조정 및 노멀 맵 샘플링 (bump map 애니메이션)
    ///////////////////////////////////////////////////////////////////////////
    float4 normalA = _bumpMap.Sample(sampler_lerp, float3(input.uv * 32.0f, i0));
    float4 normalB = _bumpMap.Sample(sampler_lerp, float3(input.uv * 32.0f, i1));
    float4 normalLerp = lerp(normalA, normalB, alpha);

    ///////////////////////////////////////////////////////////////////////////
    // 3. 노멀 맵 변형 적용
    ///////////////////////////////////////////////////////////////////////////
    float3 baseNormal = normalize(input.normal);
    float3 viewDir = normalize(g_eyeWorld - input.worldPos.xyz);

    float3 N1 = ComputeNormalMapping(baseNormal, float3(1, 0, 0), normalLerp);
    float3 N2 = ComputeNormalMapping(baseNormal, float3(1, 0, 0), _bumpMap2.Sample(sampler_lerp, input.uv * 32.0f + g_Time * 0.01f));
    float3 N3 = normalize(N1 + N2);

    ///////////////////////////////////////////////////////////////////////////
    // 4. 기본 라이팅 계산 (Diffuse & Specular)
    ///////////////////////////////////////////////////////////////////////////
    float3 lightVec = normalize(-g_sea_light_direction);
    float ndotl = max(dot(N3, lightVec), 0.0f);
    float3 diffuse = ndotl * g_sea_diffuse * 1.3f;

    // 기본 반사 벡터 및 스페큘러 계산
    float3 R = reflect(-lightVec, N3);
    float rdotv = max(dot(R, normalize(viewDir)), 0.0f);
    float3 specular = pow(rdotv, g_specularPower);

    ///////////////////////////////////////////////////////////////////////////
    // 5. DuDv 맵을 이용한 노멀 흔들림 및 환경 반사 처리
    ///////////////////////////////////////////////////////////////////////////
    // DuDv 맵을 샘플링하여 왜곡 값 계산 (-1 ~ 1 범위)
    float2 distortion = _dudv.Sample(sampler_lerp, input.uv + g_Time * 0.005f).rg;
    distortion = distortion * 2.0f - 1.0f;

    const float distortionStrength = 0.03f;
    // 기준 노멀에 왜곡을 가해 흔들림 효과 적용 (약간 위쪽으로 치우치도록 0.8 사용)
    float3 perturbedNormal = normalize(
        float3(0.0f, 0.8f, 0.0f) +
        float3(distortion.x * distortionStrength,
               distortion.x * distortionStrength,
               distortion.y * distortionStrength)
    );

    // 시야에 대한 반사 벡터 재계산 및 큐브 맵 좌표 회전
    float3 R2 = reflect(-viewDir, perturbedNormal);
    float3 rotatedR2 = float3(R2.z, R2.y, -R2.x);
    
    float blend = fmod(g_skyBlendTime, 4);
    
    float3 envReflection;
    
    if (blend >=0 && blend <1)
    {
    
        float3 envReflection1 = _cubemap_skyTexture.Sample(sampler_lerp, rotatedR2).rgb;
        float3 envReflection2 = _cubemap_skyETexture.Sample(sampler_lerp, rotatedR2).rgb;
        envReflection = lerp(envReflection1, envReflection2, blend);
    }
    
    else if(blend >=1 && blend < 2)
    {
        float3 envReflection1 = _cubemap_skyETexture.Sample(sampler_lerp, rotatedR2).rgb;
        float3 envReflection2 = _cubemap_skyNTexture.Sample(sampler_lerp, rotatedR2).rgb;
        envReflection = lerp(envReflection1, envReflection2, blend-1);
    }
    else if (blend >= 2 && blend < 3)
    {
        float3 envReflection1 = _cubemap_skyNTexture.Sample(sampler_lerp, rotatedR2).rgb;
        float3 envReflection2 = _cubemap_skyTexture.Sample(sampler_lerp, rotatedR2).rgb;
        envReflection = lerp(envReflection1, envReflection2, blend-2);
    }
    else
    {
        envReflection = _cubemap_skyTexture.Sample(sampler_lerp, rotatedR2).rgb;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // 6. 깊이 기반 색상 블렌딩 및 Fresnel 효과
    ///////////////////////////////////////////////////////////////////////////
    float shallowFactor = pow(saturate(input.worldPos.y * g_blendingFact * 0.001f), 0.5f);
    
    const float F0 = 0.02f;
    float fresnel = F0 + (1.0f - F0) * pow(1.0f - saturate(dot(viewDir, N3)), 5.0f);

    ///////////////////////////////////////////////////////////////////////////
    // 7. 최종 해양 색상 계산 및 출력
    ///////////////////////////////////////////////////////////////////////////
    float3 sea_color = (g_seaBaseColor.rgb * diffuse) + (g_seaShallowColor.rgb * shallowFactor) + envReflection * fresnel * g_envPower + specular;
    
    return float4(sea_color, 1.0f);
}