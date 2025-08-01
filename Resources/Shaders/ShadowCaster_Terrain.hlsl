
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
	float3 padding_csm2;
};


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
};

struct DS_OUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    //float3 tangent : TANGENT;
};

struct HS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

Texture2D heightMap : register(t0);


VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;

    output.pos = TransformLocalToWorld(float4(input.pos, 1.0f), l2wMatrix);
    output.uv = input.uv;
    
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
    
    return hout;
}



[domain("tri")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 3> quad, PatchConstOutput patchConst, float3 location : SV_DomainLocation)
{
    DS_OUT dout;

    float2 uv = quad[0].uv * location.x + quad[1].uv * location.y + quad[2].uv * location.z;
    float3 pos = quad[0].pos * location.x + quad[1].pos * location.y + quad[2].pos * location.z;
    float height = heightMap.SampleLevel(sampler_lerp_clamp, uv, 0).r;
    pos.y += height * fieldSize.y;


    dout.pos = mul(float4(pos, 1.0f), shadowVPMatrix[cascadeIndex]);
    dout.worldPos = float4(pos, 1.0f);
    dout.uv = uv;
    //dout.tangent = normalize(cross(float3(1,0,0), normal));
    return dout;
}


float PS_Main(DS_OUT input) : SV_Depth
{
    return input.pos.z;
}
