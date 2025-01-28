#include "GLOBAL.hlsl"
Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

#define PI 3.14159f

cbuffer test : register(b1)
{
    row_major matrix WorldMat;
}

cbuffer cameraParams : register(b2)
{
    row_major Matrix ViewMatrix;
    row_major Matrix ProjectionMatrix;
    row_major Matrix VPMatrix;
    row_major Matrix InvertViewMatrix;
    row_major Matrix InvertProjectionMatrix;
    row_major Matrix InvertVPMatrix;

    float4 cameraPos;
    float4 cameraLook;
    float4 cameraUp;
    float4 cameraFrustumData;
    float4 cameraScreenData;
};

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : POSITION;
};

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

VS_IN WaveGeneration(VS_IN input)
{
    const int waveCount = 3; // �ĵ��� ����
    float amplitudes[waveCount] = { 9.0f, 6.0f, 4.0f }; // �� �ĵ��� ���� (���̸� �ٿ� �ڿ�������)
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f }; // �� �ĵ��� ���� (�� ���� ����)
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f }; // �� �ĵ��� �ӵ� (�ӵ� ����)

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)), // �� ���� (���� �ϴ� �������� ����)
        normalize(float2(0.0f, 1.0f)), // ���� ���� (���� ����)
        normalize(float2(-0.5f, 0.7f)) // �밢�� ���� (���� ���� ����)
    };

    // �ʱ� ��ġ
    float3 modifiedPos = input.pos;
    float3 modifiedNormal = float3(0.0f, 0.0f, 0.0f); // �ʱ�ȭ ����

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

    // xz �� y ���� ���� ����
        modifiedPos.xz += amplitudes[i] * direction * waveDerivative;
        modifiedPos.y += amplitudes[i] * wave;

    // ���� ���� ���
        float3 tangentX = float3(direction.x, waveDerivative * direction.x, 0.0f);
        float3 tangentZ = float3(0.0f, waveDerivative * direction.y, direction.y);

    // ���� �⿩�� �ջ�
        modifiedNormal += cross(tangentX, tangentZ);
    }

    modifiedNormal = normalize(modifiedNormal); // ����ȭ

    VS_IN result;
    result.pos = modifiedPos;

    return result;
}

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;
    
    VS_IN result = WaveGeneration(vin);
    
    vout.pos = float4(result.pos.xyz, 1.0f);

    return vout;
}

struct HS_OUT
{
    float3 pos : POSITION;
};


PatchConstOutput MyPatchConstantFunc(InputPatch<VS_OUT, 4> patch,
                                     uint patchID : SV_PrimitiveID)
{
    PatchConstOutput pt;
    
    pt.edges[0] =8;
    pt.edges[1] = 8;
    pt.edges[2] = 8;
    pt.edges[3] =8;
    pt.inside[0] = 8;
    pt.inside[1] =8;
	
    return pt;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("MyPatchConstantFunc")]
[maxtessfactor(64.0f)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> p,
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
    HS_OUT hout;
	
    hout.pos = p[i].pos.xyz;

    return hout;
}

struct DS_OUT
{
    float4 pos : SV_POSITION;
};

[domain("quad")]
DS_OUT DS_Main(PatchConstOutput patchConst,
             float2 uv : SV_DomainLocation,
             const OutputPatch<HS_OUT, 4> quad)
{
    DS_OUT dout;

	// Bilinear interpolation.
    float3 v1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 v2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(v1, v2, uv.y);
    
    dout.pos = float4(p, 1.0);
    dout.pos = mul(dout.pos, VPMatrix);

    return dout;
}

float4 PS_Main(DS_OUT pin) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

