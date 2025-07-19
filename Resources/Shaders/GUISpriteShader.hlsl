#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"
#include "Transform_b1.hlsl"

cbuffer RectTransformParam : register(b7)
{
    row_major matrix RectLocalToWorldMatrix;
	row_major matrix RectWorldToLocalMatrix;
    float3 RectWorldPosition;
    float padding_Rect_1;
    row_major matrix RectNormalizeToLocal;
    row_major matrix RectOverlayMatrix;
};

cbuffer GUISpriteParam : register(b6)
{
    float4 GUISprite_offsetSize;
    
};


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 color : TANGENT;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D _BaseMap: register(t0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    //float4 pos = TransformLocalToWorld(float4(input.pos.xyz, 1), mul(RectNormalizeToLocal, RectLocalToWorldMatrix));
    //float4 pos = TransformLocalToWorld(float4(input.pos.xyz, 1), RectLocalToWorldMatrix);
    //localToWorld
    //output.pos = float4(pos.xy * float2(2, -2) + float2(-1, 1), g_pos.z, 1);
#ifdef Overlay
    float4 pos = TransformLocalToWorld(float4(input.pos.xyz, 1), mul(RectNormalizeToLocal, RectLocalToWorldMatrix));
    output.pos = mul(pos, RectOverlayMatrix);
#else
    float4 pos = TransformLocalToWorld(float4(input.pos.xyz, 1), mul(RectNormalizeToLocal, RectLocalToWorldMatrix));
    output.pos = TransformWorldToClip(pos);
#endif

    
    //output.uv = input.uv * tex_scale + tex_offset;
    output.uv = input.uv * GUISprite_offsetSize.xy + GUISprite_offsetSize.zw;
    
    output.color = float4(0,0,0,0);
    
    return output;
}


float4 PS_Main(VS_OUT input) : SV_TARGET
{
  
    float4 texColor = _BaseMap.Sample(sampler_lerp, input.uv);
    
    //return texColor * input.color;
    return texColor;
    //return float4(0,0,0,1);
}
