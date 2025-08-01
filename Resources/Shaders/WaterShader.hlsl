
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"
#include "ForwardFunction.hlsl"


cbuffer DefaultMaterialParam : register(b7)
{
    float4 color = float4(1, 1, 1, 1);
    float4 _baseMapST = float4(1, 1, 1, 1);
};

cbuffer WaterParam : register(b6)
{
    float3 waterScale;
    float padding_water;
};


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    
    #ifdef SKINNED
    float2 uv1 : TEXCOORD1;
    float2 uv2 : TEXCOORD2;
    float4 color : COLOR;
    float4 boneIds : BONEIDs;
    float4 boneWs : BONEWs;
	#endif

};

struct VS_OUT
{
    float id : InstanceID;

    float4 position : SV_Position;
    float4 positionCS : PositionCS;
    float4 positionWS : PositionWS;
    float3 normalOS : NormalOS;
    float3 normalWS : NormalWS;
    float3 tangentOS : TangentOS;
    float3 tangentWS : TangentWS;
    float2 uv : TEXCOORD0;
    
};

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);
Texture2D _Ramp_Water : register(t2);
Texture2D _ColorTexture : register(t3);
Texture2D _WaterNormal : register(t4);

Texture2D _BakedGIMap : register(t8);

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform transformData = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    //LightForwardParams lightingData = ForwardLightDatas[offset[STRUCTURED_OFFSET(31)].r + id];

    row_major float4x4 l2wMatrix = transformData.localToWorld;
    row_major float4x4 w2lMatrix = transformData.worldToLocal;
    float4 boneIds = 0;
    float4 boneWs = 0;
    
    
#ifdef SKINNED
	    boneIds = input.boneIds;
	    boneWs = input.boneWs;
#endif
    
    output.normalOS = input.normal;
    output.tangentOS = input.tangent;


    output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix, id);
    output.positionCS = TransformWorldToClip(output.positionWS);
    output.position = output.positionCS;

    output.normalWS = TransformNormalLocalToWorld(input.normal, boneIds, boneWs, w2lMatrix, id);
    output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs, w2lMatrix, id);

    output.uv = input.uv;

    return output;
}




#define FOAM_NOISE_FREQ   2.0
#define FOAM_EDGE_SCALE   2.0             // (v0-v1) 스케일
#define FOAM_EDGE_BIAS    0.10
#define NORMAL_STRENGTH   0.02
#define NORMAL_SCROLL1    0.05
#define NORMAL_SCROLL2    0.06

float4 PS_Main (VS_OUT IN) : SV_Target
{
    const float2  projUV     = IN.positionCS.xy / IN.positionCS.w;
    const float2  screenUV   = projUV * float2( 0.5, -0.5 ) + 0.5;

    const float  v0 = NdcDepthToViewDepth(DepthTexture.SampleLevel(sampler_point, screenUV, 0).r);
    const float  v1 = NdcDepthToViewDepth(IN.positionCS.z / IN.positionCS.w);
    const float  edgeTerm = saturate((v0 - v1) * (1.0 / FOAM_EDGE_SCALE));


    const float2 scrollT = g_Time.xx * float2(NORMAL_SCROLL1, -NORMAL_SCROLL2);
    const float2 nmUV    = IN.uv * waterScale.xz * 1.2 + scrollT;
    float2 nSample = _WaterNormal.Sample(sampler_lerp, nmUV).rg * 2.0 - 1.0;

    // 간단한 회전으로 두 장 섞는 효과
    const float2 rotated  = float2(nSample.x - nSample.y, nSample.x + nSample.y);
    const float2 ripple   = rotated * NORMAL_STRENGTH;


    float foamNoise = simple_noise(IN.positionWS.xz * FOAM_NOISE_FREQ + (-1).xx * g_Time * 0.2) * 0.5 + 0.5;
    float foam      = 1.0 - step(saturate(1.0 - edgeTerm - FOAM_EDGE_BIAS), foamNoise);

    // 화면 색 & 수심 Ramp
    const float2 distortUV = screenUV + ripple;
    const float  v2        = NdcDepthToViewDepth(DepthTexture.SampleLevel(sampler_point, distortUV, 0).r);

    float depthRamp = saturate((IN.positionWS.y - PositionTexture.SampleLevel(sampler_point, screenUV, 0).y) / 6.0);
    float4 sceneCol = _ColorTexture.Sample(sampler_point, (v2 - v1 < 0) ? screenUV : distortUV);

    float3 rampCol  = _Ramp_Water.Sample(sampler_lerp_clamp, float2(1.0 - depthRamp, 0)).rgb;
    float3 waterCol = lerp(sceneCol.rgb * rampCol, rampCol, lerp(0.15, 0.90, depthRamp));

    // 조명 & 최종 합성
    //float3 N = ComputeNormalMapping(IN.normalWS, IN.tangentWS, _BumpMap.Sample(sampler_lerp, IN.uv));
    LightingResult L = ComputeLightColorForward(IN.positionWS.xyz, IN.normalWS);

    float3 finalCol = lerp(waterCol * lerp(0.6, 1.0, L.intensity), (foam * 0.97).xxx * lerp(0.8, 1.0, L.intensity), foam);
    return float4(finalCol, 1.0);
}