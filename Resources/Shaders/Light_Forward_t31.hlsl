#ifndef _LIGHTING_FORWARD_HLSL_
#define _LIGHTING_FORWARD_HLSL_

#include "Light_b3.hlsl"

#define MAX_LIGHTS_FORWARD 5 

struct LightForwardParams
{
    int g_lightCount;
    float3 padding;
    Light g_lights[MAX_LIGHTS_FORWARD];
};

StructuredBuffer<LightForwardParams> ForwardLightDatas : register(t31);


#endif

