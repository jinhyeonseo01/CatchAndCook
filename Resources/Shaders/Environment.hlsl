
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"
#include "ShadowReceive_b6.hlsl"


cbuffer EnvMaterialParam : register(b7)
{
    float4 color = float4(1, 1, 1, 1);
    float4 _baseMapST = float4(1, 1, 1, 1);
    float4 emissionColor = float4(0, 0, 0, 0);
    float emission = 0;
	float _smoothness = 0.2;
	float _metallic = 0.1;
	float padding_env0;
};

//

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
    float4 positionVS : PositionVS;
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
    output.positionVS = TransformWorldToView(output.positionWS);
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

    float3 N = ComputeNormalMapping(input.normalWS, input.tangentWS, _BumpMap.Sample(sampler_lerp, input.uv));
    float4 BaseColor = _BaseMap.Sample(sampler_lerp, input.uv * _baseMapST.xy + _baseMapST.zw) * color;

    output.position = input.positionWS;
    output.color = BaseColor;
    output.normal = float4(N, 0.0f);
    output.maoe = float4(_metallic, _smoothness, 0, emission);

    if (emission == 1)
		output.color = emissionColor;
    #ifdef _NO_AO
		output.maoe.z = -1;
	#endif

    if (output.color.a <= 0.5f || Dither(input.position.xyz, input.positionVS.xyz))
        discard;

    return output;
}
