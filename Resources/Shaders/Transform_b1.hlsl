
#ifndef INCLUDE_TRANSFORM
#define INCLUDE_TRANSFORM

#include "Camera_b2.hlsl"
#include "Skinned_t32.hlsl"
#include "Global_b0.hlsl"

cbuffer Transform : register(b1)
{
    row_major matrix LocalToWorldMatrix;
	row_major matrix WorldToLocalMatrix;
    float3 worldPosition;
    float padding1234;
}

struct Instance_Transform
{
    row_major matrix localToWorld;
    row_major matrix worldToLocal;
    float4 worldPosition;
};

StructuredBuffer<Instance_Transform> TransformDatas : register(t30);


float3 NormalUnpack(float3 normalSample, float power)
{
	//if (dot(normalSample, normalSample) > 2.98)
	//	return float3(0, 0, 1);
    
    if (any(normalSample.rgb) == false)
        return float3(0, 0, 1);
	
	float3 norm = normalSample * 2.0 - 1.0;
	norm.xy *= power;
	return  normalize(norm);
}

float4 TransformSpaceToTangent(float4 localPos, float3 normalS, float3 tangentS)
{
    float3 N = normalS;
    float3 T = normalize(tangentS - dot(tangentS, N) * N);
    float3 B = cross(T, N);

	float4x4 localToTangent = float4x4(
         float4(T.x, B.x, N.x, 0),
         float4(T.y, B.y, N.y, 0),
         float4(T.z, B.z, N.z, 0),
         float4(0,   0,   0,   1)
    );
	return mul(localPos, localToTangent);
}

//[]->[]
float4 TransformTangentToSpace(float4 tangentPos, float3 normalS, float3 tangentS)
{
    // 정규화된 tangentS 및 normal 계산
    float3 N = normalize(normalS);
    float3 T = normalize(tangentS - dot(tangentS, N) * N);
    float3 B = cross(T, N);

    float4x4 tangentToLocal = float4x4(
         float4(T, 0),
         float4(B, 0),
         float4(N, 0),
         float4(0, 0, 0, 1)
    );
    
    return mul(tangentPos, tangentToLocal);
}

float3 TransformNormalTangentToSpace(float3 n, float3 normalS, float3 tangentS)
{
    float3 N = normalS;
    float3 T = normalize(tangentS - dot(tangentS, N) * N);
    float3 B = cross(T, N);
	B = cross(B, T);

    float3 worldNormal = n.x * T + n.y * B + n.z * N;
    return normalize(worldNormal);
}

//[]->[]
float3 TransformNormalTangentToSpace(float4 tangentPos, float3 normalS, float3 tangentS)
{
    // 정규화된 tangentS 및 normal 계산
    float3 N = normalize(normalS);
    float3 T = normalize(tangentS - dot(tangentS, N) * N);
    float3 B = cross(T, N);

    float4x4 tangentToLocal = float4x4(
         float4(T, 0),
         float4(B, 0),
         float4(N, 0),
         float4(0, 0, 0, 1)
    );
    
    return mul(tangentPos, tangentToLocal);
}

float4 TransformLocalToWorld(float4 localPos)
{
	return mul(localPos, LocalToWorldMatrix);
}

float4 TransformLocalToWorld(float4 localPos, float4 boneIds, float4 boneWs, uint id)
{
	#ifdef SKINNED
	return CalculateBone(localPos, boneIds, boneWs, id);
	#endif
	return TransformLocalToWorld(localPos);
}

float3 TransformNormalLocalToWorld(float3 normal)
{
	float4x4 worldIT = transpose(WorldToLocalMatrix);
	return normalize(mul(normal, (float3x3)worldIT));
}

float3 TransformNormalLocalToWorld(float3 normal, float4 boneIds, float4 boneWs, uint id)
{
	#ifdef SKINNED
	return CalculateBoneNormal(normal, boneIds, boneWs, id);
	#endif
	return TransformNormalLocalToWorld(normal);
}


float4 TransformWorldToLocal(float4 worldPos)
{
	return mul(worldPos, WorldToLocalMatrix);
}

float4 TransformWorldToLocal(float4 worldPos, float4 boneIds, float4 boneWs, uint id)
{
	#ifdef SKINNED
	return CalculateBoneInvert(worldPos, boneIds, boneWs, id);
	#endif
	return TransformWorldToLocal(worldPos);
}

// 직접 행렬을 지정하는것 --
float4 TransformLocalToWorld(float4 localPos, float4x4 l2w)
{
	return mul(localPos, l2w);
}

float4 TransformLocalToWorld(float4 localPos, float4 boneIds, float4 boneWs, float4x4 l2w, uint id)
{
	#ifdef SKINNED
	return CalculateBone(localPos, boneIds, boneWs, id);
	#endif
	
	return TransformLocalToWorld(localPos, l2w);
}

float3 TransformNormalLocalToWorld(float3 normal, float4x4 w2l)
{
	float4x4 worldIT = transpose(w2l);
	return normalize(mul(normal, (float3x3)worldIT));
}

float3 TransformNormalLocalToWorld(float3 normal, float4 boneIds, float4 boneWs, float4x4 w2l, uint id)
{
	#ifdef SKINNED
	return CalculateBoneNormal(normal, boneIds, boneWs, id);
	#endif
	return TransformNormalLocalToWorld(normal, w2l);
}

float4 TransformWorldToLocal(float4 worldPos, float4x4 w2l)
{
	return mul(worldPos, w2l);
}

float4 TransformWorldToLocal(float4 worldPos, float4 boneIds, float4 boneWs, float4x4 w2l, uint id)
{
	#ifdef SKINNED
	return CalculateBoneInvert(worldPos, boneIds, boneWs, id);
	#endif
	return TransformWorldToLocal(worldPos, w2l);
}


float3 ComputeNormalMapping(float3 normal, float3 tangent, float4 normalMap)
{
	// [0,255] 범위에서 [0,1]로 변환
    float4 map = normalMap;
    
    if (dot(map, map) > 2.98f)
        return normalize(normal);

    float3 N = normalize(normal); // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(T, N)); // y
    
    float3x3 TBN = float3x3(T, B, N); // TS -> WS

	// [0,1] 범위에서 [-1,1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    tangentSpaceNormal.z = 1;
    tangentSpaceNormal = normalize(tangentSpaceNormal);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);
    
    normal = normalize(worldNormal);
    
    
    return normal;
}



//--------------------------------------------------------------------------------------------

float4 TransformWorldToView(float4 worldPos)
{
	return mul(worldPos, ViewMatrix);
}

float4 TransformViewToWorld(float4 viewPos)
{
	return mul(viewPos, InvertViewMatrix);
}

float4 TransformWorldToClip(float4 worldPos)
{
	return mul(worldPos, VPMatrix);
}
float4 TransformClipToWorld(float4 clipPos)
{
	return mul(clipPos, InvertVPMatrix);
}
float4 TransformViewToClip(float4 viewPos)
{
	return mul(viewPos, ProjectionMatrix);
}
float4 TransformClipToView(float4 clipPos)
{
	return mul(clipPos, InvertProjectionMatrix);
}
float2 TransformClipToScreen(float4 clipPos)
{
    float2 uv = (clipPos.xy / clipPos.w) * 0.5 + 0.5;
    uv.y = 1.0 - uv.y;
	return uv;
}

#endif