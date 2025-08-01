#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "SeaGlobal.hlsl"


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
    float2 uv : TEXCOORD;
    
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldTangent : TANGENT;
    
};

VS_OUT VS_Main(VS_IN input , uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;
    
    output.pos = mul(float4(input.pos, 1.0f), l2wMatrix);
    output.worldPos = output.pos.xyz;
    
    float4 clipPos = mul(output.pos, VPMatrix);
    output.pos = clipPos;
    
    output.uv = input.uv;
    
    output.worldNormal = mul(float4(input.normal, 0.0f), l2wMatrix);
    output.worldTangent = mul(float4(input.tangent, 0.0f), l2wMatrix);
    
    return output;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
  
    float4 output = _BaseMap.Sample(sampler_lerp, input.uv) * color;
 
    
    if (length(output.rgb - ClipingColor.rgb) < 0.001)
    {
        discard;
    }

    
    if (length(output.a - ClipingColor.a) < 0.001)
    {
        discard;
    }

    
    return output;
}