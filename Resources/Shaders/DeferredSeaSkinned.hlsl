#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);

cbuffer SeaDefaultMaterialParam : register(b7)
{
    float4 color;
    float4 ClipingColor;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    //float2 uv1 : TEXCOORD1;
    //float2 uv2 : TEXCOORD2;
    float4 color : COLOR;
    float4 boneIds : BONEIDs;
    float4 boneWs : BONEWs;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldTangent : TANGENT;
};

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;

    float4 boneIds = input.boneIds;
    float4 boneWs = input.boneWs;
   
    float4 worldPos = CalculateBone((float4(input.pos, 1.0f)), boneIds, boneWs, id);
    output.worldPos = worldPos.xyz;
    output.pos = TransformWorldToClip(worldPos);

    output.worldNormal = CalculateBoneNormal(input.normal, boneIds, boneWs, id);
    output.worldTangent = CalculateBoneNormal(input.tangent, boneIds, boneWs, id);

    output.uv = input.uv;

    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    //float4 maoe : SV_Target3; 
};

PS_OUT PS_Main(VS_OUT input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0;

    output.position = float4(input.worldPos, 1.0f);

    float3 normalMapped = ComputeNormalMapping(input.worldNormal, input.worldTangent, _BumpMap.Sample(sampler_lerp, input.uv));
    output.normal = float4(normalMapped, 1.0f);

    output.color = _BaseMap.Sample(sampler_lerp, input.uv) * color;

    //output.maoe = float4(0, 0, 0, 0);

    return output;
}
