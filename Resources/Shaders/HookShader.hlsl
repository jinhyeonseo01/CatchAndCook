#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"

Texture2D _BaseMap : register(t0);



struct VS_OUT
{
    float4 pos1 : POSITION1;
    float4 pos2 : POSITION2;
    float2 uv : TEXCOORD0;
};

struct GS_OUT
{
    float4 pos : SV_POSITION; 

};

cbuffer PosParam : register(b8)
{
    float3 pos1;
    float padding;
    float3 pos2;
    float padding2;
}

VS_OUT VS_Main()
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos1 = float4(pos1, 1.0f);
    output.pos2 = float4(pos2, 1.0f);

    return output;
}

[maxvertexcount(36)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    float3 p0 = pos1;
    float3 p1 = pos2;

    float height = 0.5f; 

    float3 dir = normalize(p1 - p0);

    // ī�޶� ��ġ
    float3 camPos = cameraPos.xyz;

    // ī�޶� ����� dir�� �������� side ���� (�� ����)
    float3 toCam = normalize(camPos - 0.5f * (p0 + p1));
    float3 side = normalize(cross(toCam, dir));

    // dir, side�� �����ϴ� ���� (���� ����)
    float3 up = normalize(cross(dir, side));

    float halfStroke = height * 0.5f;
    float halfHeight = height * 0.5f;

    // 8���� ������ (p0 �ֺ� 4��, p1 �ֺ� 4��)
    float3 v[8];

    // p0 ��
    v[0] = p0 - side * halfStroke - up * halfHeight; // �Ʒ� ����
    v[1] = p0 + side * halfStroke - up * halfHeight; // �Ʒ� ������
    v[2] = p0 + side * halfStroke + up * halfHeight; // �� ������
    v[3] = p0 - side * halfStroke + up * halfHeight; // �� ����

    // p1 ��
    v[4] = p1 - side * halfStroke - up * halfHeight;
    v[5] = p1 + side * halfStroke - up * halfHeight;
    v[6] = p1 + side * halfStroke + up * halfHeight;
    v[7] = p1 - side * halfStroke + up * halfHeight;

    // �ε��� (12���� �ﰢ������ ����ü ����)
    int idx[36] =
    {
        0, 1, 2, 0, 2, 3, // p0 ��
        5, 4, 7, 5, 7, 6, // p1 ��
        4, 0, 3, 4, 3, 7, // ���� ��
        1, 5, 6, 1, 6, 2, // ������ ��
        3, 2, 6, 3, 6, 7, // ���� ��
        4, 5, 1, 4, 1, 0 // �Ʒ� ��
    };


    for (int i = 0; i < 36; ++i)
    {
        GS_OUT output;
        output.pos = mul(float4(v[idx[i]], 1.0f), VPMatrix);
        outputStream.Append(output);
    }

    outputStream.RestartStrip();
}
float4 PS_Main(GS_OUT input) : SV_Target
{
    return float4(1, 1, 1, 1);
}