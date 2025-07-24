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
        pos.y = WaterController::GetWaveHeight(pos,_seaParam);
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




