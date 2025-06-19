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


}

void WaterHeight::Update()
{
    if(auto transform = GetOwner()->_transform)
    {
        vec3 pos = transform->GetLocalPosition();
        pos.y = GetWaveHeight(pos.x,pos.z, Time::main->GetTime()) + _offset;
        transform->SetLocalPosition(pos);
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

float WaterHeight::GetWaveHeight(float x,float z,float time)
{ 

    float height = 0.0f;

    for(int i = 0; i < WaterController::_seaParam.wave_count; i++)
    {
        float frequency = 2 * PI / WaterController::_seaParam.waves[i].wavelength;
        float phase = WaterController::_seaParam.waves[i].speed * time;
        vec2 direction = WaterController::_seaParam.waves[i].direction;

        float dotProduct = x * direction.x + z * direction.y;
        float wave = sin(dotProduct * frequency + phase);

        height += WaterController::_seaParam.waves[i].amplitude * wave;
    }

    return height;
};
