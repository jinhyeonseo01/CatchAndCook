#include "pch.h"
#include "GUIBad.h"

#include "Canvas.h"
#include "Game.h"
#include "RectTransform.h"


COMPONENT(GUIBad)

GUIBad::~GUIBad()
{
}

bool GUIBad::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIBad::Init()
{
	Component::Init();
}

void GUIBad::Start()
{
	Component::Start();
}

void GUIBad::Update()
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

void GUIBad::Update2()
{
	Component::Update2();
}

void GUIBad::Enable()
{
	Component::Enable();
}

void GUIBad::Disable()
{
	Component::Disable();
	Game::main->RemoveFunction(GetOwner());
	Input::main->SetMouseLock(true);
}

void GUIBad::RenderBegin()
{
	Component::RenderBegin();
}

void GUIBad::RenderEnd()
{
	Component::RenderEnd();
}

void GUIBad::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIBad::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIBad::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIBad::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIBad::SetDestroy()
{
	Component::SetDestroy();
}

void GUIBad::Destroy()
{
	Component::Destroy();
}

void GUIBad::Reset()
{
	Component::Reset();
}
