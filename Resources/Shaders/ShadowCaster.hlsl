
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"
#include "ShadowReceive_b6.hlsl"

//cbuffer DefaultMaterialParam : register(b7)
//{
//    float4 color = float4(1, 1, 1, 1);
//    float4 _baseMapST = float4(1, 1, 1, 1);
//};

cbuffer ShadowCascadeIndexParams : register(b7) {
	unsigned int cascadeIndex = 1;
	float3 padding2;
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
    float2 uv : TEXCOORD0;
};

Texture2D _BaseMap : register(t0);

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
    row_major float4x4 trs  = MATRIX(input.instance_trs);
    row_major float4x4 itrs = MATRIX(input.instance_invert_trs);
	l2wMatrix = mul(data.localToWorld, trs);
	w2lMatrix = mul(itrs, data.worldToLocal);
#endif
    
    
#ifdef SKINNED
	    boneIds = input.boneIds;
	    boneWs = input.boneWs;
#endif


    //output.position = mul(TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix, id), shadowVPMatrix[0]);
    output.position = mul(TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix, id), shadowVPMatrix[cascadeIndex]);
    //output.position.w = 1;
    //output.position = mul(output.position, shadowProjectionMatrix[0]);
    //output.position = mul(TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix, id), VPMatrix);

    output.uv = input.uv;

    return output;
}


//[earlydepthstencil]
float PS_Main(VS_OUT input) : SV_Depth
{
    if (_BaseMap.SampleLevel(sampler_lerp, input.uv, 0).a < 0.1)
        discard;
    
    return input.position.z;
}
