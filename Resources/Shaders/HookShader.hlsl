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

[maxvertexcount(4)]
void GS_Main(line VS_OUT input[2], uint primID : SV_PrimitiveID,
                              inout TriangleStream<GS_OUT> outputStream)
{
    float hw = 0.5 * 300.0f;
    
    GS_OUT output;
    
    output.pos = float4(-hw, -hw, 0.0, 1.0);
    output.pos = mul(output.pos, VPMatrix);
    outputStream.Append(output);

    output.pos = float4(-hw, hw, 0.0f, 1.0f);
    output.pos = mul(output.pos, VPMatrix);
    outputStream.Append(output);
    
    
    output.pos = float4(hw, -hw, 0.0f, 1.0f);
    output.pos = mul(output.pos, VPMatrix);
    outputStream.Append(output);
    
    output.pos = float4(hw, hw, 0.0f, 1.0f);
    output.pos = mul(output.pos, VPMatrix);
    outputStream.Append(output);

    // outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    return float4(1, 1, 1, 1);
}