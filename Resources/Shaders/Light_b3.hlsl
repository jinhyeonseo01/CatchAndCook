#ifndef _LIGHTING_HLSL_
#define _LIGHTING_HLSL_

struct LightMateiral
{
    float3 ambient;
    float shininess;
    
    float3 diffuse;
    int lightType;
    
    float3 specular;
    float dummy;
};

struct Light
{
    LightMateiral mateiral;
    
    float3 strength;
    float fallOffStart;
    
    float3 direction;
    float fallOffEnd;
    
    float3 position;
    float spotPower;

    float dummy2;
	float spotAngle;
    float innerSpotAngle;
    float intensity;

    int onOff;
    float3 dummy1;
};


cbuffer LightHelperParams : register(b3)
{
    float3 g_eyeWorld;
    int g_lightCount;
    Light mainLight;
    
    float rimPower;
    float rimStrength;
    
    float pp1;
    float pp2;
};

StructuredBuffer<Light> g_lights : register(t16);

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    return 1.0 - clamp((d - falloffStart) / (falloffEnd - falloffStart), 0, 1);
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, LightMateiral mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = max(dot(halfway, normal), 0.0f);

    // ����ŧ�� �ݻ� (�⺻ ��� ����)
    float3 specular = mat.specular * pow(hdotn, mat.shininess);

    // �� ������ diffuse���� ���ϰ�, specular�� ���� �߰�
    float3 diffuse = mat.diffuse * lightStrength;
    
    return (mat.ambient + diffuse) + specular;
}

float3 Phong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, LightMateiral mat)
{
    float3 reflectVec = reflect(-lightVec, normal);
    float rdotv = max(dot(reflectVec, normalize(toEye)), 0.0f);
    float3 specular = mat.specular * pow(rdotv, mat.shininess);
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

struct LightingResult
{
	float3 color;
    float atten;
    float intensity;
    float3 direction;

    float3 subColor;
    float subAtten;
    float subWaterAtten;

};

void ComputeDirectionalLight(Light L, LightMateiral mat, float3 worldPos, float3 normal, float3 toEye, inout LightingResult lightingResult)
{
    //float3 lightVec = normalize(L.position - worldPos);
    float3 lightVec = normalize(-L.direction);
    float ndotl = max(dot(normal, lightVec), 0.0f);
    //float3 LightStrength = L.strength * L.intensity * ndotl;

    lightingResult.direction = lightVec;
    lightingResult.atten = ndotl * clamp(0, 1, L.intensity);
    lightingResult.color = L.strength* L.intensity;
    lightingResult.intensity = L.intensity;
}


void ComputePointLight(Light L, LightMateiral mat, float3 pos, float3 normal, float3 toEye, inout LightingResult lightingResult)
{
    float3 lightVec = L.position - pos;
    float d = length(lightVec);
    if (d > L.fallOffEnd )
        return;

    lightVec /= d;
    float ndotl = saturate(dot(normal, lightVec));
    float3 LightStrength = L.strength * ndotl * sqrt(L.intensity) / d; // * ndotl   *  sqrt(L.intensity / (d * d))
    float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
    LightStrength *= att;
    lightingResult.subColor += LightStrength;
    lightingResult.subAtten += att * ndotl;
    lightingResult.subWaterAtten *= (1 - att);
    //return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
    //return mat.diffuse * LightStrength;
    
}

void ComputeSpotLight(Light L, LightMateiral mat, float3 pos, float3 normal, float3 toEye, inout LightingResult lightingResult)
{
    float3 lightVec = L.position - pos;

    float d = length(lightVec);

    if (dot(lightVec, lightVec) > L.fallOffEnd * L.fallOffEnd)
        return;

    lightVec /= d;
    
    float ndotl = saturate(dot(normal, lightVec));
    
    float3 LightStrength = L.strength * ndotl * sqrt(L.intensity) / d; // * ndotl   *  sqrt(L.intensity / (d * d))
    
    float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
    LightStrength *= att;
    
    //float spotFactor = pow(saturate(dot(-lightVec, L.direction)), L.spotPower);
    //LightStrength *= spotFactor;

    float cosThreshold = cos(L.spotAngle/2);
    float cosInnerThreshold = cos(L.innerSpotAngle/2);
	float spotFactor = saturate((dot(-lightVec, L.direction) - cosThreshold) / (cosInnerThreshold - cosThreshold) );
    LightStrength *= spotFactor;

    lightingResult.subColor += LightStrength;
    lightingResult.subAtten += spotFactor * ndotl * att;
    lightingResult.subWaterAtten = max(lightingResult.subWaterAtten, att);
    //return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
    //return mat.diffuse * LightStrength;
    
}

LightingResult ComputeLightColor(float3 worldPos, float3 WorldNomral)
{
   
    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    LightingResult result = (LightingResult) 0;
    result.subWaterAtten = 1.0f;
    //[unroll]
    [loop]
    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].onOff == 1)
        {
            if (g_lights[i].mateiral.lightType == 0)
            {
                ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, worldPos, WorldNomral, toEye, result);
            }
            else if (g_lights[i].mateiral.lightType == 1)
            {
                ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
            }
            else if (g_lights[i].mateiral.lightType == 2)
            {
                ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
            }
        }
    };
    
    return result;

};



#endif

