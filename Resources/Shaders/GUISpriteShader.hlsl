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
    float4 GUISprite_border;
    float2 GUISprite_targetSize;
    float2 padding_ASD;
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


float Remap(float p,
            float dst0, float dst1, float dst2, float dst3,
            float src0, float src1, float src2, float src3)
{
    if (p <= dst1) {           // 보더1
        float t = (p - dst0) / max(dst1 - dst0, 1e-6);
        return lerp(src0, src1, t);
    }
    else if (p < dst2) {       // 중앙
        float t = (p - dst1) / max(dst2 - dst1, 1e-6);
        return lerp(src1, src2, t);
    }
    else {                     // 보더2
        float t = (p - dst2) / max(dst3 - dst2, 1e-6);
        return lerp(src2, src3, t);
    }
}

float2 Calc9SliceUV(float2 pos01)   // pos01 = Quad 내부 0‒1
{
    // ── 출력(타깃) 쪽 보더 두께 → 정규화 ──
    float2 dstSize = max(GUISprite_targetSize, float2(1.0, 1.0));
    float4 bPx     = GUISprite_border;

    float dstL = bPx.x / dstSize.x;
    float dstR = bPx.z / dstSize.x;
    float dstB = bPx.y / dstSize.y;
    float dstT = bPx.w / dstSize.y;

    float xDst[4] = { 0.0, dstL, 1.0 - dstR, 1.0 };
    float yDst[4] = { 0.0, dstB, 1.0 - dstT, 1.0 };

    // ── 스프라이트(소스) 쪽 보더 두께 → 정규화 ──
    // scale = rectSize / texSize   →  rectSize = scale * texSize
    float2 rectSizeNorm = GUISprite_offsetSize.xy;
    float srcL = bPx.x / (rectSizeNorm.x * dstSize.x);  // rectWidth = dstSize.x * scale.x
    float srcR = bPx.z / (rectSizeNorm.x * dstSize.x);
    float srcB = bPx.y / (rectSizeNorm.y * dstSize.y);
    float srcT = bPx.w / (rectSizeNorm.y * dstSize.y);

    float xSrc[4] = { 0.0, srcL, 1.0 - srcR, 1.0 };
    float ySrc[4] = { 0.0, srcB, 1.0 - srcT, 1.0 };

    // ── 매핑 ──
    float u = Remap(pos01.x, xDst[0], xDst[1], xDst[2], xDst[3],
                              xSrc[0], xSrc[1], xSrc[2], xSrc[3]);

    float v = Remap(pos01.y, yDst[0], yDst[1], yDst[2], yDst[3],
                              ySrc[0], ySrc[1], ySrc[2], ySrc[3]);

    // ── 아틀라스 변환 ──
    return float2(u, v) * GUISprite_offsetSize.xy +
                        GUISprite_offsetSize.zw;
}


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
    //output.uv = input.uv * GUISprite_offsetSize.xy + GUISprite_offsetSize.zw;
    output.uv = Calc9SliceUV(input.uv);
    
    output.color = float4(0,0,0,0);
    
    return output;
}


float4 PS_Main(VS_OUT input) : SV_TARGET
{
  
    float4 texColor = _BaseMap.Sample(sampler_lerp, input.uv);
    
    //return texColor * input.color;
    return pow(texColor, 0.55);
    //return float4(0,0,0,1);
}

