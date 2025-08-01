#pragma once

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Ray = DirectX::SimpleMath::Ray;
using Quaternion = DirectX::SimpleMath::Quaternion;


enum class VertexProp
{
    pos = 0,
    normal,
    tangent,
    bitangent,
    color,
    uv,
    uv0,
    uv1,
    uv2,
    uv3,
    uv4,
    uv5,
    uv6,
    uv7,
    bone_ids,
    bone_id0,
    bone_id1,
    bone_id2,
    bone_id3,
    bone_weights,
    bone_w0,
    bone_w1,
    bone_w2,
    bone_w3,


#pragma region Instence
    int_0,
    int_1,
    int_2,
    int_3,
    int_4,
    int_5,
    int_6,
    int_7,
    int_8,
    int_9,
    int_10,
    int_11,
    int_12,
    int_13,
    int_14,
    int_15,
    int_16,
    int_17,
    int_18,
    int_19,
    int_20,
    int_21,
    int_22,
    int_23,
    int_24,
    int_25,
    int_26,
    int_27,
    int_28,
    int_29,
    int_30,
    int_31,

    float_0,
    float_1,
    float_2,
    float_3,
    float_4,
    float_5,
    float_6,
    float_7,
    float_8,
    float_9,
    float_10,
    float_11,
    float_12,
    float_13,
    float_14,
    float_15,
    float_16,
    float_17,
    float_18,
    float_19,
    float_20,
    float_21,
    float_22,
    float_23,
    float_24,
    float_25,
    float_26,
    float_27,
    float_28,
    float_29,
    float_30,
    float_31,

    vec2_0,
    vec2_1,
    vec2_2,
    vec2_3,
    vec2_4,
    vec2_5,
    vec2_6,
    vec2_7,
    vec2_8,
    vec2_9,
    vec2_10,
    vec2_11,
    vec2_12,
    vec2_13,
    vec2_14,
    vec2_15,
    vec2_16,
    vec2_17,
    vec2_18,
    vec2_19,
    vec2_20,
    vec2_21,
    vec2_22,
    vec2_23,
    vec2_24,
    vec2_25,
    vec2_26,
    vec2_27,
    vec2_28,
    vec2_29,
    vec2_30,
    vec2_31,

    vec3_0,
    vec3_1,
    vec3_2,
    vec3_3,
    vec3_4,
    vec3_5,
    vec3_6,
    vec3_7,
    vec3_8,
    vec3_9,
    vec3_10,
    vec3_11,
    vec3_12,
    vec3_13,
    vec3_14,
    vec3_15,
    vec3_16,
    vec3_17,
    vec3_18,
    vec3_19,
    vec3_20,
    vec3_21,
    vec3_22,
    vec3_23,
    vec3_24,
    vec3_25,
    vec3_26,
    vec3_27,
    vec3_28,
    vec3_29,
    vec3_30,
    vec3_31,

    vec4_1,
    vec4_0,
    vec4_2,
    vec4_3,
    vec4_4,
    vec4_5,
    vec4_6,
    vec4_7,
    vec4_8,
    vec4_9,
    vec4_10,
    vec4_11,
    vec4_12,
    vec4_13,
    vec4_14,
    vec4_15,
    vec4_16,
    vec4_17,
    vec4_18,
    vec4_19,
    vec4_20,
    vec4_21,
    vec4_22,
    vec4_23,
    vec4_24,
    vec4_25,
    vec4_26,
    vec4_27,
    vec4_28,
    vec4_29,
    vec4_30,
    vec4_31,

    matrix_0,
    matrix_1,
    matrix_2,
    matrix_3,
    matrix_4,
    matrix_5,
    matrix_6,
    matrix_7,
    matrix_8,
    matrix_9,
    matrix_10,
    matrix_11,
    matrix_12,
    matrix_13,
    matrix_14,
    matrix_15,
    matrix_16,
    matrix_17,
    matrix_18,
    matrix_19,
    matrix_20,
    matrix_21,
    matrix_22,
    matrix_23,
    matrix_24,
    matrix_25,
    matrix_26,
    matrix_27,
    matrix_28,
    matrix_29,
    matrix_30,
    matrix_31,
#pragma endregion
    COUNT
};

const std::array<std::string, static_cast<size_t>(VertexProp::COUNT)> PropNameStrings = {
    "POSITION",
    "NORMAL",
    "TANGENT",
    "BITANGENT",
    "COLOR",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "BONEIDs",
    "BONEID",
    "BONEID",
    "BONEID",
    "BONEID",
    "BONEWs",
    "BONEW",
    "BONEW",
    "BONEW",
    "BONEW",


    #pragma region Instence
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    "INT_",
    
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    "FLOAT_",
    
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
    "FLOAT2_",
        
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
    "FLOAT3_",
        
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",
    "FLOAT4_",

    "MATRIX_0_",
    "MATRIX_1_",
    "MATRIX_2_",
    "MATRIX_3_",
    "MATRIX_4_",
    "MATRIX_5_",
    "MATRIX_6_",
    "MATRIX_7_",
    "MATRIX_8_",
    "MATRIX_9_",
    "MATRIX_10_",
    "MATRIX_11_",
    "MATRIX_12_",
    "MATRIX_13_",
    "MATRIX_14_",
    "MATRIX_15_",
    "MATRIX_16_",
    "MATRIX_17_",
    "MATRIX_18_",
    "MATRIX_19_",
    "MATRIX_20_",
    "MATRIX_21_",
    "MATRIX_22_",
    "MATRIX_23_",
    "MATRIX_24_",
    "MATRIX_25_",
    "MATRIX_26_",
    "MATRIX_27_",
    "MATRIX_28_",
    "MATRIX_29_",
    "MATRIX_30_",
    "MATRIX_31_",
    #pragma endregion
};

struct VertexPropInfo
{
    int size = 0;
    int byteSize = 0;
    int offset = 0;
    int byteOffset = 0;
    int index = 0;
    bool isFloat = true;
    VertexProp prop;
};

class SelectorInfo
{
public:
    int totalSize = 0;
    int totalByteSize = 0;
    int propCount = 0;
    std::vector<VertexPropInfo> propInfos;
    std::vector<VertexProp> props;

    static SelectorInfo GetInfo(const std::vector<VertexProp>& props);
    static D3D12_INPUT_ELEMENT_DESC GetDesc(const VertexPropInfo& propInfo);
    std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexDescs() const;
    std::vector<D3D12_INPUT_ELEMENT_DESC> GetInstanceDescs() const;
    std::vector<D3D12_INPUT_ELEMENT_DESC> GetRawDescs() const;
};

enum class VertexType
{
    Vertex_Skinned,
    Vertex_Static
};

struct Vertex_Skinned
{
    vec3 position = vec3(0, 0, 0);
    vec3 normal = vec3(0, 0, 1);
    vec3 tangent = vec3(0, 1, 0);
    std::array<vec2,3> uvs;
    vec4 color = vec4(1,1,1,1);
    vec4 boneId = vec4(-1, -1, -1, -1);
    vec4 boneWeight = vec4(0, 0, 0, 0);
};

struct Vertex_GeoMetry
{
    vec3 position = vec3(0, 0, 0);
    vec2 uv;
    vec3 normal = vec3(0, 0, 1);
};
struct Vertex_Static
{
    vec3 position = vec3(0,0,0);
    vec3 normal = vec3(0,0,1);
    vec3 tangent = vec3(0,1,0);
    vec2 uv;
};


struct Vertex_Color
{
    vec3 position = vec3(0, 0, 0);
    vec3 normal;
    vec2 uv;
    vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};


struct Instance_Gizmo
{
    vec4 color = vec4(1.0f,1.0f,1.0f,1.0f);
    vec3 position = vec3(0,0,0);
    vec3 position2 = vec3(0,0,0);
    float strock = 2;
};

struct Instance_GizmoText
{
    vec4 color = vec4(1.0f,1.0f,1.0f,1.0f);
};

struct Instance_Transform
{
	Matrix localToWorld = Matrix::Identity;
    Matrix worldToLocal = Matrix::Identity;
    vec4 worldPosition;
};

//struct Vertex_Full
//{
//    Vector3 position = Vector3(0, 0, 0);
//    Vector3 normal = Vector3(0, 0, 1);
//    Vector3 tangent = Vector3(0, 1, 0);
//    Vector3 bitangent = Vector3(0, 0, 0);
//    Vector4 color = Vector4(1, 1, 0, 1);
//    std::array<Vector3, 8> uvs;
//    Vector4 boneId = Vector4(-1, -1, -1, -1);
//    Vector4 boneWeight = Vector4(0, 0, 0, 0);
//};
