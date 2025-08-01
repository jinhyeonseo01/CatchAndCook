
#ifndef INCLUDE_OBJECT_SETTING
#define INCLUDE_OBJECT_SETTING

#include "Camera_b2.hlsl"
#include "Skinned_t32.hlsl"
#include "Global_b0.hlsl"


struct ObjectSettingParam
{
    uint o_select;
    float4 o_selectColor;

	uint o_hit;
    float o_hitValue;

    float4 o_hitColor;
};

StructuredBuffer<ObjectSettingParam> ObjectSettingDatas : register(t31);

#endif