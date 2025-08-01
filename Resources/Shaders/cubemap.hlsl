#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

struct VS_IN
{
    float3 localPos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD;
};

TextureCube _BaseMap : register(t0);

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    output.localPos = input.localPos;
    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    
    row_major float4x4 l2wMatrix = data.localToWorld;
    
    output.localPos = mul(float4(output.localPos, 0.0f), l2wMatrix);
    float4 clipSpacePos = mul(float4(input.localPos, 0), VPMatrix);
    output.pos = clipSpacePos.xyww;
    
    return output;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    return _BaseMap.Sample(sampler_lerp, input.localPos);
}