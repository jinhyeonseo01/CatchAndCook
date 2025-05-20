#include "pch.h"
#include "WeaponSystem.h"
#include "Collider.h"

WeaponSystem::WeaponSystem()
{
}

WeaponSystem::~WeaponSystem()
{
}

bool WeaponSystem::IsExecuteAble()
{
    return false;
}

void WeaponSystem::Init()
{
}

void WeaponSystem::Start()
{
}

void WeaponSystem::Update()
{
}

void WeaponSystem::Update2()
{
}

void WeaponSystem::Enable()
{
}

void WeaponSystem::Disable()
{
}

void WeaponSystem::RenderBegin()
{
}

void WeaponSystem::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    std::wcout << collider->GetOwner()->GetRoot()->GetName();

}

void WeaponSystem::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void WeaponSystem::SetDestroy()
{
}

void WeaponSystem::Destroy()
{
}
