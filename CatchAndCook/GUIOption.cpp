#include "pch.h"
#include "GUIOption.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "Game.h"

COMPONENT(GUIOption)
GUIOption::~GUIOption()
{
}

bool GUIOption::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIOption::Init()
{
	Component::Init();
}

void GUIOption::Start()
{
	Component::Start();
}

void GUIOption::Update()
{
	Component::Update();

	Game::main->AddFunctionBack(GetOwner(), [=]()
		{
			GetOwner()->SetActiveSelf(false);
		});


	Vector2 mousePos;

	if (auto canvas = GetOwner()->GetComponentWithParents<Canvas>())
		if (canvas->type == CanvasType::Overlay)
			mousePos = canvas->GetScreenToCanvasPos(Input::main->GetMousePosition());

	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{

		if (auto exitObj = GetOwner()->GetChildByName(L"Exit"))
		{
			if (exitObj->GetComponent<RectTransform>()->IsBoundScreenPos(Input::main->GetMousePosition()))
			{
				GetOwner()->SetActiveSelf(false);
			}
		}
	}

}

void GUIOption::Update2()
{
	Component::Update2();
}

void GUIOption::Enable()
{
	Component::Enable();
}

void GUIOption::Disable()
{
	Component::Disable();
	Game::main->RemoveFunction(GetOwner());
	Input::main->SetMouseLock(true);
}

void GUIOption::RenderBegin()
{
	Component::RenderBegin();
}

void GUIOption::RenderEnd()
{
	Component::RenderEnd();
}

void GUIOption::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIOption::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIOption::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIOption::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIOption::SetDestroy()
{
	Component::SetDestroy();
}

void GUIOption::Destroy()
{
	Component::Destroy();

}

void GUIOption::Reset()
{
	Component::Reset();
}
