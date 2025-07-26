#include "pch.h"
#include "GUIMainUI.h"



COMPONENT(GUIMainUI)

GUIMainUI::~GUIMainUI()
{
}

bool GUIMainUI::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIMainUI::Init()
{
	Component::Init();
}

void GUIMainUI::Start()
{
	Component::Start();
}

void GUIMainUI::Update()
{
	Component::Update();
}

void GUIMainUI::Update2()
{
	Component::Update2();
}

void GUIMainUI::Enable()
{
	Component::Enable();
}

void GUIMainUI::Disable()
{
	Component::Disable();
}

void GUIMainUI::RenderBegin()
{
	Component::RenderBegin();
}

void GUIMainUI::RenderEnd()
{
	Component::RenderEnd();
}

void GUIMainUI::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIMainUI::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIMainUI::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIMainUI::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);

	if (nextScene->_type == SceneType::MainMenu)
	{
		GetOwner()->SetActiveSelf(false);
	}
	else
		GetOwner()->SetActiveSelf(true);
}

void GUIMainUI::SetDestroy()
{
	Component::SetDestroy();
}

void GUIMainUI::Destroy()
{
	Component::Destroy();
}

void GUIMainUI::Reset()
{
	Component::Reset();
}
