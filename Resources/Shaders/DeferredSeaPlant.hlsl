#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"

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



static float amplitude = 4.7f;
static float frequency = 0.25f;

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;

    float4 worldPos = mul(float4(input.pos, 1.0f), l2wMatrix);

    float2 noiseCoord = worldPos.xz * 0.07f + g_Time * frequency;
    float2 dir = float2(1, 1) * simple_noise(noiseCoord) * amplitude;

    float swayFactor = sin(saturate(input.pos.y));
    dir *= swayFactor;

    float4 animatedWorldPos = worldPos + float4(dir.x, 0, dir.y, 0);
    output.worldPos = animatedWorldPos.xyz;
    output.pos = mul(animatedWorldPos, VPMatrix);

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
    output.color = _BaseMap.Sample(sampler_lerp, input.uv) * color;
    output.normal = float4(N, 1.0f);
    
    float3 diff = abs(output.color.rgb - ClipingColor.rgb);
    
    if (all(diff < 0.01f))
    {
        discard;
    }
    
    if (output.color.a == ClipingColor.a)
        discard;
    
    return output;
}