
#ifndef INCLUDE_GLOBAL
#define INCLUDE_GLOBAL

cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
};

SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_shadow : register(s5);



float4x4 Inverse(float4x4 m)
{
    // 16개의 요소를 로컬 변수에 캐싱 (메모리 접근 비용 최소화)
    float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
    float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
    float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
    float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

    // 첫 두 열에 대한 2x2 소행렬들의 행렬식 (여인수의 일부)
    float a0 = m00 * m11 - m01 * m10;
    float a1 = m00 * m12 - m02 * m10;
    float a2 = m00 * m13 - m03 * m10;
    float a3 = m01 * m12 - m02 * m11;
    float a4 = m01 * m13 - m03 * m11;
    float a5 = m02 * m13 - m03 * m12;

    // 마지막 두 열에 대한 2x2 소행렬들의 행렬식
    float b0 = m20 * m31 - m21 * m30;
    float b1 = m20 * m32 - m22 * m30;
    float b2 = m20 * m33 - m23 * m30;
    float b3 = m21 * m32 - m22 * m31;
    float b4 = m21 * m33 - m23 * m31;
    float b5 = m22 * m33 - m23 * m32;

    // 전체 행렬식 계산
    float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
    if (abs(det) < 1e-6)
        return m; // 특이행렬(또는 거의 0)에선 원본 반환

    float invDet = 1.0 / det;

    float4x4 inv;

    // 각 성분을 인라인 계산 (불필요한 인덱스 접근 제거)
    inv[0][0] = ( m11 * b5 - m12 * b4 + m13 * b3) * invDet;
    inv[0][1] = (-m01 * b5 + m02 * b4 - m03 * b3) * invDet;
    inv[0][2] = ( m31 * a5 - m32 * a4 + m33 * a3) * invDet;
    inv[0][3] = (-m21 * a5 + m22 * a4 - m23 * a3) * invDet;

    inv[1][0] = (-m10 * b5 + m12 * b2 - m13 * b1) * invDet;
    inv[1][1] = ( m00 * b5 - m02 * b2 + m03 * b1) * invDet;
    inv[1][2] = (-m30 * a5 + m32 * a2 - m33 * a1) * invDet;
    inv[1][3] = ( m20 * a5 - m22 * a2 + m23 * a1) * invDet;

    inv[2][0] = ( m10 * b4 - m11 * b2 + m13 * b0) * invDet;
    inv[2][1] = (-m00 * b4 + m01 * b2 - m03 * b0) * invDet;
    inv[2][2] = ( m30 * a4 - m31 * a2 + m33 * a0) * invDet;
    inv[2][3] = (-m20 * a4 + m21 * a2 - m23 * a0) * invDet;

    inv[3][0] = (-m10 * b3 + m11 * b1 - m12 * b0) * invDet;
    inv[3][1] = ( m00 * b3 - m01 * b1 + m02 * b0) * invDet;
    inv[3][2] = (-m30 * a3 + m31 * a1 - m32 * a0) * invDet;
    inv[3][3] = ( m20 * a3 - m21 * a1 + m22 * a0) * invDet;

    return inv;
}

#define MATRIX_DEFINE(mat, num) \
    float4 mat##_r0 : MATRIX_##num##_0; \
    float4 mat##_r1 : MATRIX_##num##_1; \
    float4 mat##_r2 : MATRIX_##num##_2; \
    float4 mat##_r3 : MATRIX_##num##_3

#define MATRIX(mat) float4x4( mat##_r0, mat##_r1, mat##_r2, mat##_r3 )

#endif
