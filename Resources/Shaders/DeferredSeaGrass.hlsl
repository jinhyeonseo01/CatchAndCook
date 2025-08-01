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

cbuffer SeaGrassParam:register(b8)
{
    float amplitude;
    float frequency;
    float boundsCenterY;
    float boundsSizeY;
    float PushPower;
    float padding1;
    float padding2;
    float padding3;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;

    MATRIX_DEFINE(instance_trs, 0);
    MATRIX_DEFINE(instance_invert_trs, 1);
    float3 instance_worldPos : FLOAT3_0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldTangent : TANGENT;

};

static float PLAYER_RADIUS = 40.0f; 

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
     
    float3 playerPos = g_eyeWorld;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r];
    row_major float4x4 l2wMatrix = mul(data.localToWorld, MATRIX(input.instance_trs));
 
    float4 worldPos = mul(float4(input.pos, 1.0f), l2wMatrix);
    float2 noiseCoord = worldPos.xz * 0.07f + g_Time * frequency;
    float2 dir = float2(1, 1) * simple_noise(noiseCoord) * amplitude;

    float3 toGrass = worldPos.xyz - playerPos; 
    float distSq3 = dot(toGrass, toGrass); 
    float falloff = saturate(1.0f - distSq3 / (PLAYER_RADIUS * PLAYER_RADIUS));
    float invSqrt = rsqrt(distSq3 + 1e-6f); 
    float2 normXZ = toGrass.xz * invSqrt;

    dir += normXZ * falloff * PushPower;
    
    float minY = boundsCenterY - boundsSizeY;
    float maxY = boundsCenterY + boundsSizeY;
    float influence = smoothstep(minY, maxY, input.pos.y);

    dir *= influence;

    float3 animatedWorldPos = worldPos.xyz + float3(dir.x, 0, dir.y);
    output.worldPos = animatedWorldPos;
    output.pos = mul(float4(output.worldPos, 1.0f), VPMatrix);

    output.uv = input.uv;
    output.worldNormal = mul(float4(input.normal, 0.0f), l2wMatrix).xyz;
    output.worldTangent = mul(float4(input.tangent, 0.0f), l2wMatrix).xyz;

    return output;
};
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
    output.color = _BaseMap.Sample(sampler_lerp, input.uv)*color;
    //output.color = float4(input.inf, input.inf, input.inf, input.inf);
    output.normal = float4(N, 1.0f);
    
    //float3 diff = abs(output.color.rgb - ClipingColor.rgb);
    
    //if (all(diff < 0.01f))
    //{
    //    discard;
    //}
    
    if (output.color.a == ClipingColor.a)
        discard;
    
    return output;
}