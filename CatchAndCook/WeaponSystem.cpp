#include "pch.h"
#include "WeaponSystem.h"
#include "Collider.h"
#include "Transform.h"
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

    if (GetOwner()->GetRenderer())
    {
        GetOwner()->GetRenderer()->AddCbufferSetter(static_pointer_cast<WeaponSystem>(shared_from_this()));
    }
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
    if (GetOwner()->GetRenderer())
    {
        GetOwner()->GetRenderer()->RemoveCbufferSetter(static_pointer_cast<WeaponSystem>(shared_from_this()));
    }
}

void WeaponSystem::Destroy()
{
}

void WeaponSystem::SetData(Material* material)
{
    
    _hookPos.pos1 = _weaponSlot->_transform->GetWorldPosition();
    _hookPos.pos2 = GetOwner()->_transform->GetWorldPosition();

    auto cbuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::HookData)->Alloc(1);
    memcpy(cbuffer->ptr, &_hookPos, sizeof(_hookPos));
    Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(8, cbuffer->GPUAdress);

}
