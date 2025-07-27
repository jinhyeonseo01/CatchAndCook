#include "pch.h"
#include "GUIDevelop.h"
#include "RectTransform.h"

#include "Canvas.h"
#include "Game.h"


COMPONENT(GUIDevelop)

GUIDevelop::~GUIDevelop()
{
}

bool GUIDevelop::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIDevelop::Init()
{
	Component::Init();
}

void GUIDevelop::Start()
{
	Component::Start();
}

void GUIDevelop::Update()
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
				Sound::main->PlayImmediate("j_button_click", 2.0f);
			}
		}
	}

}

void GUIDevelop::Update2()
{
	Component::Update2();
}

void GUIDevelop::Enable()
{
	Component::Enable();
}

void GUIDevelop::Disable()
{
	Component::Disable();
	Game::main->RemoveFunction(GetOwner());
}

void GUIDevelop::RenderBegin()
{
	Component::RenderBegin();
}

void GUIDevelop::RenderEnd()
{
	Component::RenderEnd();
}

void GUIDevelop::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIDevelop::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIDevelop::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIDevelop::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIDevelop::SetDestroy()
{
	Component::SetDestroy();
}

void GUIDevelop::Destroy()
{
	Component::Destroy();
}

void GUIDevelop::Reset()
{
	Component::Reset();
}
