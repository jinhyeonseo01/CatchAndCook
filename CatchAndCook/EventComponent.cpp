#include "pch.h"
#include "EventComponent.h"


COMPONENT(EventComponent)

void EventComponent::Init()
{
	cout << "히히" << endl;
}

void EventComponent::Start()
{


}

void EventComponent::Update()
{
	for (auto& func : _OnUpdateEvents)
	{
		func();
	}
}

void EventComponent::Update2()
{
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


	_Oncollision = true;

	for (auto& func : _OnCollisionBegins)
	{
		func();
	}

}

void EventComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

	if (_oneTimeEventCall && _Oncollision==false)
		return;

	_Oncollision = false;

	for (auto& func : _OnCollisionEnds)
	{
		func();
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
