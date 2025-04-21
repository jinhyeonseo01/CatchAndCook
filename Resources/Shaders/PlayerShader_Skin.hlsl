
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_b5.hlsl"

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

cbuffer PlayerSkinMaterialParam : register(b9)
{
    float SSSDistorion = 0.3f;
	float SSSScale = 0.3f;
    float SSSPow = 2.5f;
    float padding_p0;
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

    float4 positionOS : PositionOS;
};

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);
Texture2D _Thinkness : register(t2);
Texture2D _RampSkinMap : register(t3);

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


    output.positionOS = float4(input.pos, 1.0f);
    output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix);
    //output.positionWS.w = 1;
    output.positionCS = TransformWorldToClip(output.positionWS);
    output.position = output.positionCS;

    output.normalWS = TransformNormalLocalToWorld(input.normal, boneIds, boneWs, w2lMatrix);
    output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs, w2lMatrix);

    output.uv = input.uv;

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
    //float3 finalColor = (lerp(ShadowColor * BaseColor, BaseColor, lightColor.atten) + float4(lightColor.subColor, 0)).xyz;
    //ShadowColor
    float subIntensity =  lerp(1, 0.3, clamp(0, 1, lightColor.intensity));
    float3 finalColor = (BaseColor * _RampSkinMap.Sample(sampler_lerp_clamp, float2(lightColor.atten, 0)) + float4(lightColor.subColor * subIntensity, 0)).xyz;
    

    float SSSDistorion = 0.3f;
	float SSSScale = 0.3f;
    float SSSPow = 2.5f;

    // SSS ��Ʈ.
    float3 viewDir = normalize(cameraPos - input.positionWS.xyz);
    float3 H = normalize(lightColor.direction + N * SSSDistorion);
    float SSS = pow(saturate(dot(viewDir, -H)), SSSPow) * _Thinkness.Sample(sampler_lerp, input.uv).r;
    //float3 SSSFinal = lerp(float3(1,1,1), float3(1, 0, 0) * SSSScale, SSS);
    float3 SSSFinal =  float3(1, 0.2, 0.1) * SSSScale * SSS;


    //Sobel Spec
    float N2L = saturate(dot(lightColor.direction, N));
    float sobelW = 5;
    float sobel = clamp(0, 1, Sobel(input.positionCS, sobelW / input.positionCS.w));
    sobel = lerp(0, sobel, (1 - saturate(dot(viewDir, N))) * N2L) * 0.5;

    // Rim
    float MinusN2L = saturate(dot(-lightColor.direction, N));
    float fresnel = (1 - saturate(dot(viewDir, N)));
    float3 rim = pow(fresnel, exp2(2)) * MinusN2L * float3(0.6, 0.6, 0.8);

	//outline
    float outline = step(0.1f, Sobel(input.positionCS, 0.6f / input.positionCS.w)); //  / (outline * 2 + 1)
    float3 outlineColor = (finalColor / float3((outline * 1 + 1), (outline * 1.5 + 1), (outline * 1.1 + 1)));
    finalColor = lerp(finalColor, outlineColor, outline);


    // ==========================
    //      Object Setting
    // ==========================
    float3 hitFresnel = objectData.o_hit * objectData.o_hitValue * objectData.o_hitColor.xyz * fresnel;
    float3 selectLine = objectData.o_select * clamp(0, 1, Sobel(input.positionCS, 2)) * objectData.o_selectColor.xyz;




    if (BaseColor.a <= 0.1f)
		discard;

    return float4(finalColor, 1) + float4(SSSFinal + rim + sobel + hitFresnel + selectLine, 0);
}
