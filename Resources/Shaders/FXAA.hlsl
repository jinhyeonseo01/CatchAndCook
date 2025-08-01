

cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
};

cbuffer FXAAParams : register(b1)
{
    float  uQualitySubpix;            // 0.75 ~ 1.0 (�����ȼ� ���� ����)
    float  uQualityEdge;              // 2.0 ~ 8.0   (���� ����)
    float  uQualityEdgeThreshold;     // 0.125 ~ 0.2 (���� �ּ� ��� ��)
    float  uQualityEdgeThresholdMin;  // 0.0625~0.1  (���� �ּ� ���밪)
};

Texture2D<float4>    inputTexture   : register(t0);   // ���� �н� �÷�
SamplerState         sampler_lerp   : register(s0);   // ���� ����, Ŭ����
RWTexture2D<float4>  resultTexture  : register(u0);   // FXAA ���

[numthreads(16,16,1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 pix = dispatchThreadID.xy;
    // ȭ�� ���� üũ
    if (pix.x >= g_window_size.x || pix.y >= g_window_size.y)
        return;

    // 1) ȭ�� UV �� �ȼ��� ������
    float2 uv      = (float2(pix) + 0.5) / g_window_size;
    float2 rcpFrame = (1.0 / g_window_size);
    float2 rcpFrameEdgeRange = rcpFrame * 2;

    // 2) �߾� �ȼ� ��
    float3 rgbM = inputTexture.SampleLevel(sampler_lerp, uv, 0).rgb;
    float  lumaM = dot(rgbM, float3(0.299, 0.587, 0.114));

    // 3) ��/��/��/�� 4�ȼ� ��
    float lumaN = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2( 0, -rcpFrameEdgeRange.y), 0).rgb, float3(0.299,0.587,0.114));
    float lumaS = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2( 0, +rcpFrameEdgeRange.y), 0).rgb, float3(0.299,0.587,0.114));
    float lumaW = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2(-rcpFrameEdgeRange.x,  0), 0).rgb, float3(0.299,0.587,0.114));
    float lumaE = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2(+rcpFrameEdgeRange.x,  0), 0).rgb, float3(0.299,0.587,0.114));

    // 4) �ּ�/�ִ� ��
    float lumaMin = min(lumaM, min(min(lumaN,lumaS), min(lumaW,lumaE)));
    float lumaMax = max(lumaM, max(max(lumaN,lumaS), max(lumaW,lumaE)));

    // 5) ���� ���� (threshold �����̸� �н�)
    float range = lumaMax - lumaMin;
    if (range < max(uQualityEdgeThresholdMin, lumaMax * uQualityEdgeThreshold))
    {
        resultTexture[pix] = float4(rgbM, 1);
        return;
    }

    // 6) �밢�� 4�ȼ� ��
    float lumaNW = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2(-rcpFrameEdgeRange.x, -rcpFrameEdgeRange.y), 0).rgb, float3(0.299,0.587,0.114));
    float lumaNE = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2(+rcpFrameEdgeRange.x, -rcpFrameEdgeRange.y), 0).rgb, float3(0.299,0.587,0.114));
    float lumaSW = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2(-rcpFrameEdgeRange.x, +rcpFrameEdgeRange.y), 0).rgb, float3(0.299,0.587,0.114));
    float lumaSE = dot(inputTexture.SampleLevel(sampler_lerp, uv + float2(+rcpFrameEdgeRange.x, +rcpFrameEdgeRange.y), 0).rgb, float3(0.299,0.587,0.114));

    // 7) ���� ���� ���
    float2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    // 8) ���� ���� �� Normalize
    float dirReduce  = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * uQualitySubpix),
                           uQualityEdgeThresholdMin);
    float rcpDirMin  = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = saturate(dir * rcpDirMin) * uQualityEdge;

    // 9) ���� ���� ������
    float2 offset1 = dir * (1.0/3.0 - 0.5);
    float2 offset2 = dir * (2.0/3.0 - 0.5);

    // 10) �� ���ø� ��� ���
    float3 rgb1 = inputTexture.SampleLevel(sampler_lerp, uv + offset1 * rcpFrame, 0).rgb;
    float3 rgb2 = inputTexture.SampleLevel(sampler_lerp, uv + offset2 * rcpFrame, 0).rgb;
    float3 fxaa  = 0.5 * (rgb1 + rgb2);

    // 11) ���
    resultTexture[pix] = float4(fxaa, 1);
}