
#ifndef INCLUDE_CAMERA
#define INCLUDE_CAMERA

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
    float4 cameraFrustumData; // _fov , _params.cameraScreenData.x / _params.cameraScreenData.y, _near, _far
    float4 cameraScreenData;
};

float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}

float3 NdcToViewPosition(float3 positionCS)
{
    float4 clip = float4(positionCS, 1.0);
    float4 viewH = mul(clip, InvertProjectionMatrix);
    return viewH.xyz / viewH.w;
}
float NdcToWorldPosition(float3 positionCS)
{
    float4 worldH = mul(float4(NdcToViewPosition(positionCS), 1.0), InvertViewMatrix);
    return worldH.xyz;  // worldH.w´Â Ç×»ó 1.0
}

#endif
