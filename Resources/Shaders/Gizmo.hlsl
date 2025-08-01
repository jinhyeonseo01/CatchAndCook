#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

struct VS_IN
{
    float4 color : FLOAT4_0;
    float3 pos : FLOAT3_0;
    float3 pos2 : FLOAT3_1;
    float  stroke: FLOAT_0;
};

struct GS_IN
{
    float4 color : FLOAT4_0;
    float3 pos   : FLOAT3_0;
    float3 pos2  : FLOAT3_1;
    float  stroke: FLOAT_0;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : FLOAT4_0;
};

GS_IN VS_Main(VS_IN input, uint index : SV_VertexID)
{
    GS_IN output = (GS_IN)0;
    //output.pos = float4(input.pos * index + input.pos2 * (1 - index), 1);
    //output.pos = mul(output.pos, VPMatrix);
    output.pos    = input.pos;
    output.pos2   = input.pos2;
    output.color = input.color;
    output.stroke = input.stroke;
    return output;
}

[maxvertexcount(4)]
void GS_Main(line GS_IN input[2], inout TriangleStream<VS_OUT> triStream)
{
    // 라인 두 점
    float3 p0 = input[0].pos;
    float3 p1 = input[0].pos2; 
    // (pos와 pos2가 2번째 정점에도 있을 수 있으나, 보통 동일한 라인)

    // 컬러/굵기는 일단 첫 번째 정점 기준(혹은 둘 다 평균 등을 써도 됨)
    float4 color  = input[0].color;
    float  stroke = input[0].stroke;

    // 라인의 중심점
    float3 center = 0.5f * (p0 + p1);
    // 라인 방향 (정규화)
    float3 dir    = normalize(p1 - p0);

    // 카메라 위치
    float3 camPos = cameraPos.xyz;

    // 라인의 두께가 "카메라를 향하도록(billboard)" 하기 위해
    // 카메라에서 라인 중심까지 벡터를 구하고, dir 과의 외적(cross)으로 '수직 방향'을 만든다.
    float3 toCam  = normalize(camPos - center);
    float3 side   = normalize(cross(toCam, dir));

    // side 방향으로 stroke 절반만큼 오프셋
    float3 offset = side * (stroke * 0.5f);

    // 네 꼭짓점(직사각형)
    float3 p0a = p0 + offset; 
    float3 p0b = p0 - offset;
    float3 p1a = p1 + offset;
    float3 p1b = p1 - offset;

    // 변환(월드 -> 클립), VPMatrix 사용
    float4 wvp0a = mul(float4(p0a, 1.0f), VPMatrix);
    float4 wvp0b = mul(float4(p0b, 1.0f), VPMatrix);
    float4 wvp1a = mul(float4(p1a, 1.0f), VPMatrix);
    float4 wvp1b = mul(float4(p1b, 1.0f), VPMatrix);

    // TriangleStream으로 4개의 정점을 순서대로 Append
    // -> TriStrip: (0->1->2->3) = 삼각형 2개
    VS_OUT outV;

    // 1
    outV.pos   = wvp0a;
    outV.color = color;
    triStream.Append(outV);

    // 2
    outV.pos   = wvp0b;
    triStream.Append(outV);

    // 3
    outV.pos   = wvp1a;
    triStream.Append(outV);

    // 4
    outV.pos   = wvp1b;
    triStream.Append(outV);

    // 라인별로 스트립을 끊어주려면(다음 라인을 새로 그릴 경우)
    triStream.RestartStrip();
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}
