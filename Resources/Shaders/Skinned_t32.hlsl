
#ifndef INCLUDE_SKINNED
#define INCLUDE_SKINNED

#include "Global_b0.hlsl"

#define BoneMatrixSize 256

cbuffer BoneParam : register(b5)
{
    row_major matrix BoneMatrix[BoneMatrixSize];
    row_major matrix InvertBoneMatrix[BoneMatrixSize];
}


StructuredBuffer<matrix> BoneDatas : register(t32);

//float4 CalculateBone(float4 localPosition, float4 bIds, float4 bWs)
//{
//    return (mul(localPosition, BoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
//         + mul(localPosition, BoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
//         + mul(localPosition, BoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
//         + mul(localPosition, BoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w))) / (bWs.x + bWs.y + bWs.z + bWs.w);
//}
float4 CalculateBone(float4 lp, float4 bIds, float4 bWs, uint id)
{
    //BoneDatas data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    //BoneDatas[BoneMatrixSize * 2 * id + 0 + BoneMatrixSize]
    // 1) ID → uint 인덱스 (음수는 0으로 클램프)
    uint i0 = (uint)max(bIds.x, 0.0);
    uint i1 = (uint)max(bIds.y, 0.0);
    uint i2 = (uint)max(bIds.z, 0.0);
    uint i3 = (uint)max(bIds.w, 0.0);

    // 2) 가중치 합과 역수 (한 번만)
    float w0 = bWs.x, w1 = bWs.y, w2 = bWs.z, w3 = bWs.w;
    float sumW = w0 + w1 + w2 + w3;
    float invW = 1.0 / sumW;

    // 3) 매트릭스 로드 후 곱하고 가중치 곱셈
    //    mul(row‑vec, row_major matrix) 를 이용
    //float4 p = mul(lp, BoneMatrix[i0]) * w0;
    //       p += mul(lp, BoneMatrix[i1]) * w1;
    //       p += mul(lp, BoneMatrix[i2]) * w2;
    //       p += mul(lp, BoneMatrix[i3]) * w3;
    uint boneDataOffset = uint(offset[STRUCTURED_OFFSET(32)].r);
    float4 p = mul (BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i0],lp) * w0; //offset[STRUCTURED_OFFSET(32)].r
           p += mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i1],lp) * w1;
           p += mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i2],lp) * w2;
           p += mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i3],lp) * w3;

    // 4) 정규화
    return p * invW;
}

//float4 CalculateBoneInvert(float4 worldPosition, float4 bIds, float4 bWs)
//{
//    return mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
//         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
//         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
//         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w)) / (bWs.x + bWs.y + bWs.z + bWs.w);
//}

float4 CalculateBoneInvert(float4 wp, float4 bIds, float4 bWs, uint id)
{
    uint i0 = (uint)max(bIds.x, 0.0);
    uint i1 = (uint)max(bIds.y, 0.0);
    uint i2 = (uint)max(bIds.z, 0.0);
    uint i3 = (uint)max(bIds.w, 0.0);

    float w0 = bWs.x, w1 = bWs.y, w2 = bWs.z, w3 = bWs.w;
    float sumW = w0 + w1 + w2 + w3;
    float invW = 1.0 / sumW;

    uint boneDataOffset = uint(offset[STRUCTURED_OFFSET(32)].r);
    float4 p =  mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i0 + BoneMatrixSize],wp) * w0;
           p += mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i1 + BoneMatrixSize],wp) * w1;
           p += mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i2 + BoneMatrixSize],wp) * w2;
           p += mul(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i3 + BoneMatrixSize],wp) * w3;

    return p * invW;
}

//float3 CalculateBoneNormal(float3 localNormal, float4 bIds, float4 bWs)
//{
//    float4 normal = float4(0, 0, 0, 0);

//    // InvertBoneMatrix는 단순 inverse이므로, 노말 변환에 사용하기 위해 transpose를 취한다.
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.x)])) * (bWs.x * step(-0.5, bIds.x));
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.y)])) * (bWs.y * step(-0.5, bIds.y));
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.z)])) * (bWs.z * step(-0.5, bIds.z));
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.w)])) * (bWs.w * step(-0.5, bIds.w));

//    return normalize(normal.xyz);
//}
float3 CalculateBoneNormal(float3 ln, float4 bIds, float4 bWs, uint id)
{
    uint i0 = (uint)max(bIds.x, 0.0);
    uint i1 = (uint)max(bIds.y, 0.0);
    uint i2 = (uint)max(bIds.z, 0.0);
    uint i3 = (uint)max(bIds.w, 0.0);

    float w0 = bWs.x, w1 = bWs.y, w2 = bWs.z, w3 = bWs.w;

    // inverse‑transpose: transpose(InvertBoneMatrix) 의 상위 3×3 부분만 사용
    uint boneDataOffset = uint(offset[STRUCTURED_OFFSET(32)].r);
    float3 n = mul((float3x3)transpose(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i0 + BoneMatrixSize]),ln) * w0;
          n += mul((float3x3)transpose(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i1 + BoneMatrixSize]),ln) * w1;
          n += mul((float3x3)transpose(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i2 + BoneMatrixSize]),ln) * w2;
          n += mul((float3x3)transpose(BoneDatas[boneDataOffset + BoneMatrixSize * 2 * id + i3 + BoneMatrixSize]),ln) * w3;

    return normalize(n);
}

#endif