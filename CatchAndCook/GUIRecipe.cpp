#include "pch.h"
#include "GUIRecipe.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "Game.h"

COMPONENT(GUIRecipe);

GUIRecipe::~GUIRecipe()
{
}

bool GUIRecipe::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIRecipe::Init()
{
	Component::Init();
}

void GUIRecipe::Start()
{
	Component::Start();
}

void GUIRecipe::Update()
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

void GUIRecipe::Update2()
{
	Component::Update2();
}

void GUIRecipe::Enable()
{
	Component::Enable();
}

void GUIRecipe::Disable()
{
	Component::Disable();
	Game::main->RemoveFunction(GetOwner());
	Input::main->SetMouseLock(true);
}

void GUIRecipe::RenderBegin()
{
	Component::RenderBegin();
}

void GUIRecipe::RenderEnd()
{
	Component::RenderEnd();
}

void GUIRecipe::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIRecipe::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIRecipe::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIRecipe::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIRecipe::SetDestroy()
{
	Component::SetDestroy();
}

void GUIRecipe::Destroy()
{
	Component::Destroy();
}

void GUIRecipe::Reset()
{
	Component::Reset();
}
