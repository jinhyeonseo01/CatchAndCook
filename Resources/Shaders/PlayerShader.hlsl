
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_t32.hlsl"
#include "ShadowReceive_b6.hlsl"
#include "ObjectSetting_t31.hlsl"

#include "ForwardFunction.hlsl"


cbuffer DefaultMaterialParam : register(b7)
{
    float4 color = float4(1, 1, 1, 1);
    float4 _baseMapST = float4(1, 1, 1, 1);
};

cbuffer PlayerMaterialParam : register(b8)
{
    float4 temp = float4(1, 1, 1, 1);
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
    uint id : InstanceID;

    float4 position : SV_Position;
    float4 positionCS : PositionCS;
    float4 positionWS : PositionWS;
    float3 normalOS : NormalOS;
    float3 normalWS : NormalWS;
    float3 tangentOS : TangentOS;
    float3 tangentWS : TangentWS;
    float2 uv : TEXCOORD0;

    float4 positionVS : PositionVS;
    float3 shadowCoord0 : SHADOW0;
    float3 shadowCoord1 : SHADOW1;
    float3 shadowCoord2 : SHADOW2;
    float3 shadowCoord3 : SHADOW3;
};

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);
Texture2D _RampShadowMap : register(t2);
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

    float3 uvz[4];
	ComputeCascadeShadowUVs(output.positionWS, uvz);

    output.positionVS = TransformWorldToView(output.positionWS);
    output.shadowCoord0 = uvz[0];
    output.shadowCoord1 = uvz[1];
    output.shadowCoord2 = uvz[2];
    output.shadowCoord3 = uvz[3];

    return output;
}




float4 PS_Main(VS_OUT input) : SV_Target
{
    uint id = (uint)input.id;
    float3 N = ComputeNormalMapping(input.normalWS, input.tangentWS, _BumpMap.Sample(sampler_lerp, input.uv));

    ObjectSettingParam objectData = ObjectSettingDatas[offset[STRUCTURED_OFFSET(31)].r + id];

    LightingResult lightColor = ComputeLightColorForward(input.positionWS.xyz, N);
    
    float4 BaseColor = _BaseMap.Sample(sampler_lerp, input.uv * _baseMapST.xy + _baseMapST.zw) * color;
    float4 ShadowColor = _BakedGIMap.Sample(sampler_lerp_clamp, saturate(dot(float3(0, 1, 0), N) * 0.5 + 0.5));

    float3 uvz[4];
    uvz[0] = input.shadowCoord0;
    uvz[1] = input.shadowCoord1;
    uvz[2] = input.shadowCoord2;
    uvz[3] = input.shadowCoord3;
    lightColor.atten = lerp(min(lightColor.atten, 0.5), lightColor.atten, ComputeCascadeShadowAtten(uvz, input.positionVS.z).x);

    //float3 finalColor = (lerp(ShadowColor * BaseColor, BaseColor, lightColor.atten) + float4(lightColor.subColor, 0)).xyz;
    float subIntensity =  lerp(1, 0.3, clamp(0, 1, lightColor.intensity));
    float3 finalColor = (BaseColor * _RampShadowMap.Sample(sampler_lerp_clamp, float2(lightColor.atten, 0)) + float4(lightColor.subColor * subIntensity, 0)).xyz;

    //_RampShadowMap

    float3 viewDir = normalize(cameraPos.xyz - input.positionWS.xyz);


    //Sobel Spec
    float N2L = saturate(dot(lightColor.direction, N));
    float sobelW = 6;
    float sobel = clamp(0, 1, Sobel(input.positionCS, sobelW / input.positionCS.w));
    sobel = lerp(0, sobel, (1 - saturate(dot(viewDir, N))) * N2L) * 0.6;

    // Rim
    float MinusN2L = saturate(dot(-lightColor.direction, N));
    float fresnel = (1 - saturate(dot(viewDir, N)));
    float3 rim = pow(fresnel, exp2(2)) * MinusN2L * float3(0.6, 0.6, 0.8);

    //outline
    float outline = step(0.1f, Sobel(input.positionCS, 0.85f / input.positionCS.w));
    float rimRatio = 1 - saturate(dot(viewDir, N));
    float3 outlineColor = (finalColor / float3((outline * 1 + 1), (outline * 1.5 + 1), (outline * 1.1 + 1)));
    finalColor = lerp(finalColor, outlineColor, outline);


    // ==========================
    //      Object Setting
    // ==========================
    float3 hitFresnel = objectData.o_hit * objectData.o_hitValue * objectData.o_hitColor.xyz * fresnel;
    float3 selectLine = objectData.o_select * clamp(0, 1, Sobel(input.positionCS, 2)) * objectData.o_selectColor.xyz;


    if (BaseColor.a <= 0.1)
		discard;
    //return float4((input.positionWS.xyz - worldPosition) * 0.001f, 1);

    return float4(finalColor, 1) + float4(rim + sobel + hitFresnel + selectLine, 0); // * dot(N, viewDir)
}
