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
	int i = 0;
	while (auto obj = GetOwner()->GetChildByName(L"Button_" + to_wstring(i)))
	{
		buttons.push_back(obj);
		++i;
	}
	//Button_0
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
			if (exitObj->GetComponent<RectTransform>()->IsBoundCanvasPos(mousePos))
			{
				GetOwner()->SetActiveSelf(false);
				Sound::main->PlayImmediate("j_button_click", 2.0f);
			}
		}

		for (int i = 0; i < buttons.size(); i++)
		{
			auto rect = buttons[i]->GetComponent<RectTransform>();

			if (rect->IsBoundCanvasPos(mousePos))
			{
				selectedIndex = i;
				Sound::main->PlayImmediate("j_button_swap", 0.1f);
				break;
			}
		}

		if (auto sleepObj = GetOwner()->GetChildByName(L"Button_Bad"))
		{
			if (sleepObj->GetComponent<RectTransform>()->IsBoundCanvasPos(mousePos))
			{
				GetOwner()->SetActiveSelf(false);
				InGameGlobal::main->skyTime = ((((int)std::round(InGameGlobal::main->skyTime) / 4) + 1) * 4) + (selectedIndex - 1);
				ComputeManager::main->SetChangeSceneState(ChangeSceneState::FadeOutIn, 1 / 2.0f);
				selectedIndex = 0;
				Sound::main->PlayImmediate("j_button_click", 2.0f);
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
