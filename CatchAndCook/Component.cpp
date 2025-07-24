#include "pch.h"
#include "Component.h"
#include "Game.h"
#include "GameObject.h"

Component::Component()
{
}

Component::~Component()
{
	
}

bool Component::operator<(const Component& other) const
{
	return this->_order < other._order;
}

bool Component::IsExecuteAble()
{
	return GetOwner()->IsExecuteAble() && IDelayDestroy::IsExecuteAble();
}

void Component::Init()
{
}

void Component::Start()
{
}

void Component::Update()
{
}

void Component::Update2()
{
}

void Component::Enable()
{

}

void Component::Disable()
{
}

void Component::Destroy()
{

}

void Component::Reset()
{

}

void Component::RenderBegin()
{

}

void Component::RenderEnd()
{

}

void Component::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void Component::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void Component::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{

}

void Component::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{

}


void Component::SetDestroy()
{
	IDelayDestroy::SetDestroy();
	GameObject::AddDestroyComponent(GetCast<Component>());
}


