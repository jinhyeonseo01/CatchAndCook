#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"

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
    float4 pos : SV_Position;
    float3 worldPos : PositionWS;
    float3 positionVS : PositionVS;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D _BaseMap : register(t0);

VS_OUT VS_Main(VS_IN input , uint id : SV_InstanceID)
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
    
    float3 normalOS = input.normal;

#ifdef INSTANCED
	float4 positionWS = TransformLocalToWorld(float4(input.pos, 1.0f),  boneIds, boneWs, l2wMatrix, id);
#else
    float4 positionWS = TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix, id);
#endif

	output.worldPos = positionWS;
    output.pos = TransformWorldToClip(positionWS);
    output.positionVS = TransformWorldToView(positionWS);

#ifdef INSTANCED
	output.worldNormal = TransformNormalLocalToWorld(normalOS, boneIds, boneWs, w2lMatrix, id);
#else
    output.worldNormal = TransformNormalLocalToWorld(normalOS, boneIds, boneWs, w2lMatrix, id);
#endif

	output.uv = input.uv;
    
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
    output.normal = float4(input.worldNormal, 1.0f);
    output.color = float4(1.0f, 1.0f, 1.0f, 0.0f);

    if(_BaseMap.Sample(sampler_point, input.uv).w <= 0.1f)
		discard;

    return output;
}