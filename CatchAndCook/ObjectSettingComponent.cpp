#include "pch.h"
#include "ObjectSettingComponent.h"

#include "InGameMainField.h"


COMPONENT(ObjectSettingComponent)

ObjectSettingComponent::~ObjectSettingComponent()
{
}

bool ObjectSettingComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void ObjectSettingComponent::SetData(StructuredBuffer* buffer, Material* material)
{
	buffer->AddData(_objectSettingParam);
}

void ObjectSettingComponent::RenderEnd()
{
	Component::RenderEnd();
	_objectSettingParam.o_select = false;
}


void ObjectSettingComponent::Init()
{
	Component::Init();
}

void ObjectSettingComponent::Start()
{
	Component::Start();
	InGameMainField::GetMain()->AddObjectSetting(GetCast<ObjectSettingComponent>());
}

void ObjectSettingComponent::Update()
{
	Component::Update();
}

void ObjectSettingComponent::Update2()
{
	Component::Update2();
}

void ObjectSettingComponent::Enable()
{
	Component::Enable();
}

void ObjectSettingComponent::Disable()
{
	Component::Disable();
}

void ObjectSettingComponent::RenderBegin()
{
	Component::RenderBegin();
}

void ObjectSettingComponent::CollisionBegin(const std::shared_ptr<Collider>& collider,
	const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void ObjectSettingComponent::CollisionEnd(const std::shared_ptr<Collider>& collider,
	const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void ObjectSettingComponent::ChangeParent(const std::shared_ptr<GameObject>& prev,
	const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void ObjectSettingComponent::SetDestroy()
{
	Component::SetDestroy();
}

void ObjectSettingComponent::Destroy()
{
	Component::Destroy();
	if (InGameMainField::GetMain())
		InGameMainField::GetMain()->RemoveObjectSetting(GetCast<ObjectSettingComponent>());
}

