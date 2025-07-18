#include "pch.h"
#include "WaterHeight.h"
#include "Transform.h"
#include "WaterController.h"
COMPONENT(WaterHeight)

WaterHeight::~WaterHeight()
{
}

bool WaterHeight::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void WaterHeight::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);
}

void WaterHeight::Init()
{
}

void WaterHeight::Start()
{
    _seaParam = &SceneManager::main->GetCurrentScene()->Find(L"sea")->GetComponent<WaterController>()->_seaParam;
}

void WaterHeight::Update()
{
    if(auto transform = GetOwner()->_transform)
    {
        vec3 pos = transform->GetWorldPosition();
        pos.y = GetWaveHeight(pos);
        transform->SetWorldPosition(pos);
    }
}

void WaterHeight::Update2()
{
}

void WaterHeight::Enable()
{
}

void WaterHeight::Disable()
{
}

void WaterHeight::RenderBegin()
{
}

void WaterHeight::CollisionBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other)
{

}

void WaterHeight::SetDestroy()
{
}

void WaterHeight::Destroy()
{
}

float WaterHeight::GetWaveHeight(const vec3& worldPos)
{
    float height = 43.5f;

    for (int i = 0; i < _seaParam->wave_count; i++)
    {
        float frequency = 2 * PI / _seaParam->waves[i].wavelength;
        float phase = _seaParam->waves[i].speed * Time::main->GetTime();
        vec2 direction = _seaParam->waves[i].direction;
        direction.Normalize();

        float dotProduct = worldPos.x * direction.x + worldPos.z * direction.y;
        float wave = sin(dotProduct * frequency + phase);

        height += _seaParam->waves[i].amplitude * wave;
    }

    return height;

}

//float WaterHeight::GetWaveHeight(float x,float z,float time)
//{ 
//
//    void WaveGeneration(inout float3 worldPos, inout float3 worldNormal)
//    {
//        int waveCount = g_wave_count;
//
//        float3 modifiedPos = worldPos;
//
//        float waveY = 0;
//        for (int i = 0; i < waveCount; i++)
//        {
//            Wave wave = waves[i];
//
//            float frequency = 2.0f * PI / wave.wavelength;
//            float phase = wave.speed * g_Time;
//            float2 dir = normalize(wave.direction);
//            float steep = wave.steepness;
//
//            float dotVal = dot(dir, worldPos.xz);
//
//            float waveSin = sin(dotVal * frequency + phase);
//            float waveCos = cos(dotVal * frequency + phase);
//
//            modifiedPos.x += steep * wave.amplitude * dir.x * waveCos;
//            modifiedPos.z += steep * wave.amplitude * dir.y * waveCos;
//            waveY += wave.amplitude * waveSin;
//        }
//
//        modifiedPos.y += waveY;
//
//        // 수정된 위치 반영 및 기본 법선 할당
//        worldPos = modifiedPos;
//        worldNormal = float3(0, 1, 0);
//    }
//
//
//
//   
//};
