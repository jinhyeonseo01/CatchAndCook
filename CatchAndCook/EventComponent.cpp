#include "pch.h"
#include "EventComponent.h"
#include "Collider.h"

COMPONENT(EventComponent)

void EventComponent::Init()
{



}

void EventComponent::Start()
{


}

void EventComponent::Update()
{

}

void EventComponent::Update2()
{
	if (_Oncollision)
	{
		for (auto& func : _OnUpdate)
		{
			func(_otherCashing);
		}
	}
}

void EventComponent::Enable()
{
}

void EventComponent::Disable()
{

}

void EventComponent::RenderBegin()
{

}

void EventComponent::RenderEnd()
{
}

void EventComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (_oneTimeEventCall && _Oncollision)
		return;

	if (HasTag(bindTag, other->GetOwner()->GetTag()) == false)
		return;

	_Oncollision = true;
	_otherCashing = other;

	for (auto& func : _OnCollisionBegins)
	{
		func(other);
	}

}

void EventComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (_oneTimeEventCall && _Oncollision==false)
		return;

	if (HasTag(bindTag, other->GetOwner()->GetTag()) == false)
		return;

	_Oncollision = false;
	_otherCashing = nullptr;

	for (auto& func : _OnCollisionEnds)
	{
		func(other);
	}

}

void EventComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void EventComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void EventComponent::SetDestroy()
{
}

void EventComponent::Destroy()
{
}
