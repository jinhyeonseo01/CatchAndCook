#include "pch.h"
#include "GUIMenuToggle.h"
#include "RectTransform.h"


COMPONENT(GUIMenuToggle)
GUIMenuToggle::~GUIMenuToggle()
{
}

bool GUIMenuToggle::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIMenuToggle::Init()
{
	Component::Init();
}

void GUIMenuToggle::Start()
{
	Component::Start();
}

void GUIMenuToggle::Update()
{
	Component::Update();
	auto rect = GetOwner()->GetComponent<RectTransform>();

	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		if (rect->IsBoundScreenPos(Input::main->GetMousePosition()))
		{
			auto menu = GetOwner()->GetScene()->Find(L"GUIMenu");
			Sound::main->PlayImmediate("j_button_click", 2.0f);
			menu->SetActiveSelf(!menu->GetActiveSelf());
		}
	}
}

void GUIMenuToggle::Update2()
{
	Component::Update2();
}

void GUIMenuToggle::Enable()
{
	Component::Enable();
}

void GUIMenuToggle::Disable()
{
	Component::Disable();
}

void GUIMenuToggle::RenderBegin()
{
	Component::RenderBegin();
}

void GUIMenuToggle::RenderEnd()
{
	Component::RenderEnd();
}

void GUIMenuToggle::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIMenuToggle::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIMenuToggle::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIMenuToggle::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIMenuToggle::SetDestroy()
{
	Component::SetDestroy();
}

void GUIMenuToggle::Destroy()
{
	Component::Destroy();
}

void GUIMenuToggle::Reset()
{
	Component::Reset();
}
