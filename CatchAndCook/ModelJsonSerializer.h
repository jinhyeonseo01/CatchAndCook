#pragma once

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <memory>
#include <string>
#include "simple_mesh_LH.h"
#include "simple_mesh_ext.h"
#include "SimpleMath_LH.inl"
#include <vector>

using nlohmann::json;


// ----------------------------------------------------------------------------
// 헬퍼: 내부 객체 참조를 GUID(id) 문자열로 맵핑하기 위한 컨텍스트
// ----------------------------------------------------------------------------


inline void to_json(json& j, const Vector2& v) { j = { v.x, v.y }; }
inline void from_json(const json& j, Vector2& v) { v.x = j[0]; v.y = j[1]; }
inline void to_json(json& j, const Vector3& v) { j = { v.x, v.y, v.z }; }
inline void from_json(const json& j, Vector3& v) { v.x = j[0]; v.y = j[1]; v.z = j[2]; }
inline void to_json(json& j, const Vector4& v) { j = { v.x, v.y, v.z, v.w }; }
inline void from_json(const json& j, Vector4& v) { v.x = j[0]; v.y = j[1]; v.z = j[2]; v.w = j[3]; }
inline void to_json(json& j, const Quaternion& q) { j = { q.x, q.y, q.z, q.w }; }
inline void from_json(const json& j, Quaternion& q) { q.x = j[0]; q.y = j[1]; q.z = j[2]; q.w = j[3]; }




inline void to_json(json& j, const Matrix& m)
{
    // row‑major 4x4 → 16 요소 평탄화
    float buf[16];
    memcpy(&buf, m.m, sizeof(float[16]));
    j = json::array();
    for (int i = 0; i < 16; ++i) j.push_back(buf[i]);
}
inline void from_json(const json& j, Matrix& m)
{
    float buf[16];
    for (int i = 0; i < 16; ++i) buf[i] = j[i];
    memcpy(m.m, &buf, sizeof(float[16]));
}


class ModelSerialize
{
	
};

// ----------------------------------------------------------------------------