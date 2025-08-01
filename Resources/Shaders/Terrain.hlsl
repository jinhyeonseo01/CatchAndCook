#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "ShadowReceive_b6.hlsl"
#include "SeaGlobal.hlsl"

#define TessFactor 18
#define PI 3.14159f
#define DIST_MAX 100.0f
#define DIST_MIN 20.0f

cbuffer TerrainDetailsParam : register(b10)
{
    float3 fieldSize;
    float padding0;
    int detailsCount;
    float3 padding2;
    int blendCount;
    float3 padding3;
    float4 tileST[12];
    float4 textureActive[12];
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
    //float3 tangent : TANGENT;
};

struct HS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

Texture2D heightMap : register(t0);


Texture2D _detailMap0 : register(t40);
Texture2D _detailMap1 : register(t41);
Texture2D _detailMap2 : register(t42);
Texture2D _detailMap3 : register(t43);

Texture2D _detailMap4 : register(t44);
Texture2D _detailMap5 : register(t45);
Texture2D _detailMap6 : register(t46);
Texture2D _detailMap7 : register(t47);

Texture2D _detailMap8 : register(t48);
Texture2D _detailMap9 : register(t49);
Texture2D _detailMap10 : register(t50);
Texture2D _detailMap11 : register(t51);

Texture2D _normalMap0 : register(t52);
Texture2D _normalMap1 : register(t53);
Texture2D _normalMap2 : register(t54);
Texture2D _normalMap3 : register(t55);
                                      
Texture2D _normalMap4 : register(t56);
Texture2D _normalMap5 : register(t57);
Texture2D _normalMap6 : register(t58);
Texture2D _normalMap7 : register(t59);
                                      
Texture2D _normalMap8 : register(t60);
Texture2D _normalMap9 : register(t61);
Texture2D _normalMap10 : register(t62);
Texture2D _normalMap11 : register(t63);

Texture2D _maskMap0 : register(t64);
Texture2D _maskMap1 : register(t65);
Texture2D _maskMap2 : register(t66);
Texture2D _maskMap3 : register(t67);
           
Texture2D _maskMap4 : register(t68);
Texture2D _maskMap5 : register(t69);
Texture2D _maskMap6 : register(t70);
Texture2D _maskMap7 : register(t71);
                        
Texture2D _maskMap8 : register(t72);
Texture2D _maskMap9 : register(t73);
Texture2D _maskMap10 : register(t74);
Texture2D _maskMap11 : register(t75);


Texture2D _blendMap0 : register(t76);
Texture2D _blendMap1 : register(t77);
Texture2D _blendMap2 : register(t78);



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
    float edges[3] : SV_TessFactor;
    float inside[1] : SV_InsideTessFactor;
};



float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float minDist, float maxDist, float maxLv)
{
    
    //return 8;
    
    // 수직 거리 무시 (XZ 평면 기준 거리 계산)
    float distance = length(float2(patchPos.x, patchPos.z) - float2(cameraWorldPos.x, cameraWorldPos.z));

    // 거리를 [minDist, maxDist] 구간으로 정규화
    float t = saturate((distance - minDist) / (maxDist - minDist));
    float tSmooth = pow(t, 0.4f);

    // 거리가 가까울수록 maxLv, 멀수록 1.0
    float tessLevel = lerp(maxLv, 1.0f, tSmooth);

    // 결과를 보정 범위 안에 클램프
    return clamp(tessLevel, 1.0f, maxLv);
}

//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 3> patch, uint patchID : SV_PrimitiveID)
{

    PatchConstOutput pt;
    
    float3 edge0Pos = (patch[1].pos.xyz + patch[2].pos.xyz) / 2.f;
    float3 edge1Pos = (patch[2].pos.xyz + patch[0].pos.xyz) / 2.f;
    float3 edge2Pos = (patch[0].pos.xyz + patch[1].pos.xyz) / 2.f;

    float edge0TessLevel = CalculateTessLevel(cameraPos.xyz, edge0Pos, DIST_MIN, DIST_MAX, TessFactor);
    float edge1TessLevel = CalculateTessLevel(cameraPos.xyz, edge1Pos, DIST_MIN, DIST_MAX, TessFactor);
    float edge2TessLevel = CalculateTessLevel(cameraPos.xyz, edge2Pos, DIST_MIN, DIST_MAX, TessFactor);

    pt.edges[0] = edge0TessLevel;
    pt.edges[1] = edge1TessLevel;
    pt.edges[2] = edge2TessLevel;
    pt.inside[0] = edge2TessLevel;
    return pt;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64)]
HS_OUT HS_Main(InputPatch<VS_OUT, 3> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    //4번호출됨.
    HS_OUT hout;
    hout.pos = patch[vertexID].pos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    hout.uvTile = patch[vertexID].uvTile;
    
    return hout;
}



[domain("tri")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 3> quad, PatchConstOutput patchConst, float3 location : SV_DomainLocation)
{
    DS_OUT dout;

    float2 uv = quad[0].uv * location.x + quad[1].uv * location.y + quad[2].uv * location.z;
    float2 uvTile = quad[0].uvTile * location.x + quad[1].uvTile * location.y + quad[2].uvTile * location.z;
    float3 pos = quad[0].pos * location.x + quad[1].pos * location.y + quad[2].pos * location.z;
    float height = heightMap.SampleLevel(sampler_lerp_clamp, uv, 0).r;
    pos.y += height * fieldSize.y;

    //float texelSize = 1.0f / 4096.0f;
    float texelSize = (1.0f / 4096.0f); 

    float heightL = heightMap.SampleLevel(sampler_lerp_clamp, uv + float2(-texelSize, 0), 0).r;
    float heightR = heightMap.SampleLevel(sampler_lerp_clamp, uv + float2(texelSize, 0), 0).r;
    float heightD = heightMap.SampleLevel(sampler_lerp_clamp, uv + float2(0, -texelSize), 0).r;
    float heightU = heightMap.SampleLevel(sampler_lerp_clamp, uv + float2(0, texelSize), 0).r;

    float3 dx = float3(2*texelSize * fieldSize.x, (heightR - heightL) * fieldSize.y, 0);
    float3 dz = float3(0, (heightD - heightU) * fieldSize.y,2* texelSize * fieldSize.z);

    float3 normal = normalize(cross(dz, dx));

    dout.pos = mul(float4(pos, 1.0f), VPMatrix);
    dout.worldPos = float4(pos, 1.0f);
    dout.uv = uv;
    dout.uvTile = uvTile;
    dout.normal = normal;


    //dout.tangent = normalize(cross(float3(1,0,0), normal));
    return dout;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float4 maoe : SV_Target3;
};

[earlydepthstencil]
PS_OUT PS_Main(DS_OUT input)
{

    PS_OUT output = (PS_OUT) 0;
    
    float4 finalColor = float4(0, 0, 0, 0);
    float4 blend;

    Texture2D blendMaps[3]  = { _blendMap0,  _blendMap1,  _blendMap2  };
    Texture2D detailMaps[12]= {
        _detailMap0,  _detailMap1,  _detailMap2,  _detailMap3,
        _detailMap4,  _detailMap5,  _detailMap6,  _detailMap7,
        _detailMap8,  _detailMap9,  _detailMap10, _detailMap11
    };
    Texture2D maskMaps[12] = {
        _maskMap0,  _maskMap1,  _maskMap2,  _maskMap3,
        _maskMap4,  _maskMap5,  _maskMap6,  _maskMap7,
        _maskMap8,  _maskMap9,  _maskMap10, _maskMap11
    };
    
    float2 uv     = input.uv;
    float2 uvTile = input.uvTile;

    [unroll]
    for (int bm = 0; bm < blendCount; ++bm)
    {
        float4 blend = blendMaps[bm].Sample(sampler_lerp, uv);

        [unroll]
        for (int ch = 0; ch < 4; ++ch)
        {
            float w = blend[ch];
            if (w > 0)
            {
                int idx = bm * 4 + ch;

                // UV 계산
                float4 st = tileST[idx];
                float2 tileUV = uvTile / st.xy + st.zw;
                float mask = (textureActive[idx].g == 0)
                           ? 1.0
                           : maskMaps[idx].Sample(sampler_lerp, tileUV).g;
                finalColor += detailMaps[idx]
                                .Sample(sampler_aniso8, tileUV)
                              * mask * w;
            }
        }
    }

    output.color = finalColor;
    output.normal = float4(normalize(input.normal), 1.0f);
    
    if(g_castic)
    {        
        output.color += ComputeCaustics(uv, 8, input.worldPos.xyz);
    }
    
    output.position = input.worldPos;
    return output;
}
