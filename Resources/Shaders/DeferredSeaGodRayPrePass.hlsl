
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"


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
};

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;

    float4 worldPos = mul(float4(input.pos, 1.0f), l2wMatrix);
    output.pos = mul(worldPos, VPMatrix);

    return output;
  
}

struct PS_OUT
{
    float4 mao : SV_Target3;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output;
    output.mao = float4(1, 1, 1, 1);
    return output;
}