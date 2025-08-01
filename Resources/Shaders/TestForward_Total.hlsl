
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"

cbuffer DefaultMaterialParam : register(b7)
{
    float4 color = float4(1, 1, 1, 1);
    float4 _baseMapST = float4(1, 1, 1, 1);
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
    
    #ifdef INSTANCED
    MATRIX_DEFINE(instance_trs, 0);
    MATRIX_DEFINE(instance_invert_trs, 1);
    float3 instance_worldPos : FLOAT3_0;
	#endif
};

struct VS_OUT
{
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
Texture2D _BakedGIMap : register(t8);
Texture2D _BakedGIMap2 : register(t9);

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;
    float4 boneIds = 0;
    float4 boneWs = 0;
    
   
#ifdef INSTANCED
	data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r];
	l2wMatrix = mul(data.localToWorld, MATRIX(input.instance_trs));
	w2lMatrix = mul(MATRIX(input.instance_invert_trs), data.worldToLocal);
#endif
    
    
#ifdef SKINNED
	    boneIds = input.boneIds;
	    boneWs = input.boneWs;
#endif
    
    output.normalOS = input.normal;
    output.tangentOS = input.tangent;

#ifdef INSTANCED
		output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f),  boneIds, boneWs, l2wMatrix, id);
#else
    output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix, id);
#endif
    output.positionCS = TransformWorldToClip(output.positionWS);

    output.position = output.positionCS;

#ifdef INSTANCED
		output.normalWS = TransformNormalLocalToWorld(output.normalOS, boneIds, boneWs, w2lMatrix, id);
		output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs, w2lMatrix, id);
#else
    output.normalWS = TransformNormalLocalToWorld(input.normal, boneIds, boneWs, w2lMatrix, id);
    output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs, w2lMatrix, id);
#endif


    output.uv = input.uv;

    return output;
}



[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 N = ComputeNormalMapping(input.normalWS, input.tangentWS, _BumpMap.Sample(sampler_lerp, input.uv));
    
    LightingResult lightColor = ComputeLightColor(input.positionWS.xyz, N);
    
    float4 BaseColor = _BaseMap.Sample(sampler_lerp, input.uv * _baseMapST.xy + _baseMapST.zw) * color;
    float4 ShadowColor = _BakedGIMap.Sample(sampler_lerp_clamp, saturate(dot(float3(0, 1, 0), N) * 0.5 + 0.5));
    float4 ShadowColor2 = _BakedGIMap2.Sample(sampler_lerp_clamp, saturate(dot(float3(0, 1, 0), N) * 0.5 + 0.5));
    ShadowColor = lerp(ShadowColor, ShadowColor2, fmod(g_skyBlendTime, 1));

    float3 finalColor = (lerp(ShadowColor * BaseColor, BaseColor, lightColor.atten) + float4(lightColor.subColor, 0)).xyz;

	return float4(finalColor, 1);
}
