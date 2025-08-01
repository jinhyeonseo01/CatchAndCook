#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"


#define G_MaxTess 8
#define G_MinTess 1

#define PI 3.14159f
#define DIST_MAX 1000.0f
#define DIST_MIN 1.0f


cbuffer TerrainDetailsParam : register(b10)
{
	float3 fieldSize;
    float padding0;
	int detailsCount;
    float3 padding2;
	int blendCount;
    float3 padding3;
	float4 tileST[12];
    float4  textureActive[12];
	//Vector4 color;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

struct DS_OUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

struct HS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

Texture2D heightMap : register(t0);

Texture2D _detailMap0 :     register(t40);
Texture2D _detailMap1 :     register(t41);
Texture2D _detailMap2 :     register(t42);
Texture2D _detailMap3 :     register(t43);

Texture2D _detailMap4 :     register(t44);
Texture2D _detailMap5 :     register(t45);
Texture2D _detailMap6 :     register(t46);
Texture2D _detailMap7 :     register(t47);

Texture2D _detailMap8 :     register(t48);
Texture2D _detailMap9 :     register(t49);
Texture2D _detailMap10 :    register(t50);
Texture2D _detailMap11 :    register(t51);

Texture2D _normalMap0 :     register(t52);
Texture2D _normalMap1 :     register(t53);
Texture2D _normalMap2 :     register(t54);
Texture2D _normalMap3 :     register(t55);
                                      
Texture2D _normalMap4 :     register(t56);
Texture2D _normalMap5 :     register(t57);
Texture2D _normalMap6 :     register(t58);
Texture2D _normalMap7 :     register(t59);
                                      
Texture2D _normalMap8 :     register(t60);
Texture2D _normalMap9 :     register(t61);
Texture2D _normalMap10 :    register(t62);
Texture2D _normalMap11 :    register(t63);

Texture2D _maskMap0 :     register(t64);
Texture2D _maskMap1 :     register(t65);
Texture2D _maskMap2 :     register(t66);
Texture2D _maskMap3 :     register(t67);
           
Texture2D _maskMap4 :     register(t68);
Texture2D _maskMap5 :     register(t69);
Texture2D _maskMap6 :     register(t70);
Texture2D _maskMap7 :     register(t71);
                        
Texture2D _maskMap8 :     register(t72);
Texture2D _maskMap9 :     register(t73);
Texture2D _maskMap10 :    register(t74);
Texture2D _maskMap11 :    register(t75);


Texture2D _blendMap0 :      register(t76);
Texture2D _blendMap1 :      register(t77);
Texture2D _blendMap2 :      register(t78);



VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;

    output.pos = TransformLocalToWorld(float4(input.pos, 1.0f), l2wMatrix);
    output.normal = TransformNormalLocalToWorld(input.normal, w2lMatrix);

    output.uv = input.uv;
    output.uvTile = input.uv * fieldSize.xz;
    
    return output;
}

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

float CalcTessFactor(float3 p)
{
    float d = distance(p, cameraPos.xyz);
    float s = smoothstep(DIST_MIN, DIST_MAX, d);
    float tess = exp2(lerp(G_MaxTess, G_MinTess, s));
    return clamp(tess, 1.0f, 64.0f);
    
    //float d = distance(p, cameraPos.xyz);
    //float s = saturate((d - DIST_MAX) / (DIST_MAX - DIST_MIN));
    //return clamp(pow(2, lerp(G_MaxTess, G_MinTess, s)), 3.0f, 64.0f);
    
  
};

//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{

    PatchConstOutput pt;
    
    float3 e0 = 0.5f * (patch[0].pos.xyz + patch[2].pos.xyz);
    float3 e1 = 0.5f * (patch[0].pos.xyz + patch[1].pos.xyz);
    float3 e2 = 0.5f * (patch[1].pos.xyz + patch[3].pos.xyz);
    float3 e3 = 0.5f * (patch[2].pos.xyz + patch[3].pos.xyz);
    float3 c = 0.25f * (patch[0].pos.xyz + patch[1].pos.xyz + patch[2].pos.xyz + patch[3].pos.xyz);


    pt.edges[0] = CalcTessFactor(e0);
    pt.edges[1] = CalcTessFactor(e1);
    pt.edges[2] = CalcTessFactor(e2);
    pt.edges[3] = CalcTessFactor(e3);

    pt.inside[0] = CalcTessFactor(c);
    pt.inside[1] = pt.inside[0];
    
    return pt;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    //4번호출됨.
    HS_OUT hout;
    hout.pos = patch[vertexID].pos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    hout.uvTile = patch[vertexID].uvTile;
    
    return hout;
}


[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 location : SV_DomainLocation)
{
    DS_OUT dout;
    
  
    dout.pos = lerp(
		lerp(quad[0].pos, quad[1].pos, location.x),
		lerp(quad[2].pos, quad[3].pos, location.x),
		location.y);

    dout.uv = lerp(
		lerp(quad[0].uv, quad[1].uv, location.x),
		lerp(quad[2].uv, quad[3].uv, location.x),
		location.y);

    dout.normal = normalize(lerp(
		normalize(lerp(quad[0].normal, quad[1].normal, location.x)),
		normalize(lerp(quad[2].normal, quad[3].normal, location.x)),
		location.y));
    
    dout.uvTile = lerp(
		lerp(quad[0].uvTile, quad[1].uvTile, location.x),
		lerp(quad[2].uvTile, quad[3].uvTile, location.x),
		location.y);

   
    dout.pos.y += heightMap.SampleLevel(sampler_lerp_clamp, dout.uv, 0).r * fieldSize.y;
    dout.worldPos = dout.pos;
    dout.pos = mul(dout.pos, VPMatrix);
    
    return dout;
}


struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(DS_OUT input)
{

    PS_OUT output = (PS_OUT) 0;
    
    float4 finalColor = float4(0, 0, 0, 0);
    float4 blend;

    if (blendCount >= 1)
    {
        blend = _blendMap0.Sample(sampler_lerp, input.uv);

        if (blend.x != 0)
        {
            float2 tileUV0 = input.uvTile / tileST[0].xy + tileST[0].zw;
            float4 mask0 = (textureActive[0].g == 0) ? 1 : (_maskMap0.Sample(sampler_lerp, tileUV0));
            finalColor = _detailMap0.Sample(sampler_aniso8, tileUV0) * mask0.g * blend.x;
        }
        if (blend.y != 0)
        {
            float2 tileUV1 = input.uvTile / tileST[1].xy + tileST[1].zw;
            float4 mask1 = (textureActive[1].g == 0) ? 1 : (_maskMap1.Sample(sampler_lerp, tileUV1));
            finalColor += _detailMap1.Sample(sampler_aniso8, tileUV1) * mask1.g * blend.y;
        }
        if (blend.z != 0)
        {
            float2 tileUV2 = input.uvTile / tileST[2].xy + tileST[2].zw;
            float4 mask2 = (textureActive[2].g == 0) ? 1 : (_maskMap2.Sample(sampler_lerp, tileUV2));
            finalColor += _detailMap2.Sample(sampler_aniso8, tileUV2) * mask2.g * blend.z;
        }
        if (blend.w != 0)
        {
            float2 tileUV3 = input.uvTile / tileST[3].xy + tileST[3].zw;
            float4 mask3 = (textureActive[3].g == 0) ? 1 : (_maskMap3.Sample(sampler_lerp, tileUV3));
            finalColor += _detailMap3.Sample(sampler_aniso8, tileUV3) * mask3.g * blend.w;
        }
    }
    if (blendCount >= 2)
    {
        blend = _blendMap1.Sample(sampler_lerp, input.uv);

        if (blend.x != 0)
        {
            float2 tileUV0 = input.uvTile / tileST[4].xy + tileST[4].zw;
            float4 mask0 = (textureActive[4].g == 0) ? 1 : (_maskMap4.Sample(sampler_lerp, tileUV0));
            finalColor += _detailMap4.Sample(sampler_aniso8, tileUV0) * mask0.g * blend.x;
        }
        if (blend.y != 0)
        {
            float2 tileUV1 = input.uvTile / tileST[5].xy + tileST[5].zw;
            float4 mask1 = (textureActive[5].g == 0) ? 1 : (_maskMap5.Sample(sampler_lerp, tileUV1));
            finalColor += _detailMap5.Sample(sampler_aniso8, tileUV1) * mask1.g * blend.y;
        }
        if (blend.z != 0)
        {
            float2 tileUV2 = input.uvTile / tileST[6].xy + tileST[6].zw;
            float4 mask2 = (textureActive[6].g == 0) ? 1 : (_maskMap6.Sample(sampler_lerp, tileUV2));
            finalColor += _detailMap6.Sample(sampler_aniso8, tileUV2) * mask2.g * blend.z;
        }
        if (blend.w != 0)
        {
            float2 tileUV3 = input.uvTile / tileST[7].xy + tileST[7].zw;
            float4 mask3 = (textureActive[7].g == 0) ? 1 : (_maskMap7.Sample(sampler_lerp, tileUV3));
            finalColor += _detailMap7.Sample(sampler_aniso8, tileUV3) * mask3.g * blend.w;
        }
    }
    if (blendCount >= 3)
    {
        blend = _blendMap2.Sample(sampler_lerp, input.uv);


        if (blend.x != 0)
        {
            float2 tileUV0 = input.uvTile / tileST[8].xy + tileST[8].zw;
            float4 mask0 = (textureActive[8].g == 0) ? 1 : _maskMap8.Sample(sampler_lerp, tileUV0);
            finalColor += _detailMap8.Sample(sampler_aniso8, tileUV0) * mask0.g * blend.x;
        }

        if (blend.y != 0)
        {
            float2 tileUV1 = input.uvTile / tileST[9].xy + tileST[9].zw;
            float4 mask1 = (textureActive[9].g == 0) ? 1 : _maskMap9.Sample(sampler_lerp, tileUV1);
            finalColor += _detailMap9.Sample(sampler_aniso8, tileUV1) * mask1.g * blend.y;
        }
        if (blend.z != 0)
        {
            float2 tileUV2 = input.uvTile / tileST[10].xy + tileST[10].zw;
            float4 mask2 = (textureActive[10].g == 0) ? 1 : _maskMap10.Sample(sampler_lerp, tileUV2);
            finalColor += _detailMap10.Sample(sampler_aniso8, tileUV2) * mask2.g * blend.z;
        }
        if (blend.w != 0)
        {
            float2 tileUV3 = input.uvTile / tileST[11].xy + tileST[11].zw;
            float4 mask3 = (textureActive[11].g == 0) ? 1 : _maskMap11.Sample(sampler_lerp, tileUV3);
            finalColor += _detailMap11.Sample(sampler_aniso8, tileUV3) * mask3.g * blend.w;
        }
    }

   
    output.color = finalColor;
    output.normal = float4(normalize(input.normal), 1.0f);
    output.position = input.pos;
    
    return output;
}
