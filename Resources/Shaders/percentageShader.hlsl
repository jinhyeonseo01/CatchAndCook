#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "SeaGlobal.hlsl"

Texture2D _BaseMap : register(t0);

struct hpstruct
{
    float g_percentage;
    float pp;
    float pp2;
    float pp3;
};

StructuredBuffer<hpstruct> HPData : register(t33);

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
    float percentage : Percentage;
    
    float localX : TEXCOORD1; 
    
};

VS_OUT VS_Main(VS_IN input , uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    hpstruct hpdata = HPData[offset[STRUCTURED_OFFSET(33)].r + id];
    
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;
    
    output.pos = mul(float4(input.pos, 1.0f), l2wMatrix);
    output.worldPos = output.pos.xyz;
    
    output.percentage = hpdata.g_percentage;
    
    float4 clipPos = mul(output.pos, VPMatrix);
    output.pos = clipPos;
    
    output.uv = input.uv;
    
    output.worldNormal = mul(float4(input.normal, 0.0f), l2wMatrix);
    output.worldTangent = mul(float4(input.tangent, 0.0f), l2wMatrix);
    
    output.localX = input.pos.x; 
    
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;

};
PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    output.position = float4(input.worldPos, 1.0f);
    output.normal = float4(input.worldNormal, 1.0f);

    float percentage = saturate(input.percentage / 100.0f);

    float x01 = saturate((input.localX + 1.0f) * 0.5f);

    if (x01 >= (1.0f - percentage))
    {
        output.color = float4(0, 1.0f, 0.0f, 1.0f); 
    }
    else
    {
        output.color = float4(0.3f, 0.3f, 0.3f, 1.0f);
    }

    return output;
}