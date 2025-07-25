#include "pch.h"
#include "AddDontDestroy.h"

COMPONENT(AddDontDestroy)
AddDontDestroy::~AddDontDestroy()
{
}

bool AddDontDestroy::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void AddDontDestroy::Init()
{
	Component::Init();
}

void AddDontDestroy::Start()
{
	Component::Start();
	GetOwner()->GetScene()->SetDontDestroy(GetOwner());
	std::cout << "1234\n";
}

void AddDontDestroy::Update()
{
	Component::Update();
}

void AddDontDestroy::Update2()
{
	Component::Update2();
}

void AddDontDestroy::Enable()
{
	Component::Enable();
}

void AddDontDestroy::Disable()
{
	Component::Disable();
}

void AddDontDestroy::RenderBegin()
{
	Component::RenderBegin();
}

void AddDontDestroy::RenderEnd()
{
	Component::RenderEnd();
}

void AddDontDestroy::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void AddDontDestroy::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void AddDontDestroy::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void AddDontDestroy::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void AddDontDestroy::SetDestroy()
{
	Component::SetDestroy();
}

void AddDontDestroy::Destroy()
{
	Component::Destroy();
}

void AddDontDestroy::Reset()
{
	Component::Reset();
}
