#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldTangent : TANGENT;
};

struct FishInfo
{
    float fishWaveAmount;
    float fishSpeed;
    float boundsCenterZ;
    float boundsSizeZ;
};


StructuredBuffer<FishInfo> FIshInfos : register(t32);

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;
    
    FishInfo fishinfo = FIshInfos[offset[STRUCTURED_OFFSET(32)].r + id];

    float localZ = input.pos.z;
    float minZ = fishinfo.boundsCenterZ - fishinfo.boundsSizeZ;
    float maxZ = fishinfo.boundsCenterZ + fishinfo.boundsSizeZ;
    float weight = saturate((localZ - minZ) / (maxZ - minZ));

    float phaseOffset = localZ * 0.7f;

    float wave = sin(g_Time * fishinfo.fishSpeed + phaseOffset)
           * fishinfo.fishWaveAmount * weight;

    float3 animatedPos = input.pos;
    animatedPos.x += wave;

    float4 worldPos = mul(float4(animatedPos, 1.0f), l2wMatrix);
    output.pos = mul(worldPos, VPMatrix);
    output.worldPos = worldPos.xyz;

    output.uv = input.uv;
    output.worldNormal = mul(float4(input.normal, 0.0f), l2wMatrix).xyz;
    output.worldTangent = mul(float4(input.tangent, 0.0f), l2wMatrix).xyz;

    return output;
  
}
struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float4 maoe : SV_Target3;
};

PS_OUT PS_Main(VS_OUT input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0;
    
    output.position = float4(input.worldPos, 1.0f);
    float3 N = ComputeNormalMapping(input.worldNormal, input.worldTangent, _BumpMap.Sample(sampler_lerp, input.uv));
    output.color = _BaseMap.Sample(sampler_lerp, input.uv);
    output.normal = float4(N, 1.0f);
    

    return output;
}