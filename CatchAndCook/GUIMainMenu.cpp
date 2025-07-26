#include "pch.h"
#include "GUIMainMenu.h"

#include "Game.h"
#include "RectTransform.h"


COMPONENT(GUIMainMenu);

GUIMainMenu::~GUIMainMenu()
{
}

bool GUIMainMenu::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIMainMenu::Init()
{
	Component::Init();
}

void GUIMainMenu::Start()
{
	Component::Start();

	int i = 0;
	while (auto obj = GetOwner()->GetScene()->Find(L"Button_" + to_wstring(i)))
	{
		buttons.push_back(obj);
		++i;
	}
}

void GUIMainMenu::Update()
{
	Component::Update();

	for (int i = 0; i < buttons.size(); i++)
	{
		auto rect = buttons[i]->GetComponent<RectTransform>();

		if (Input::main->GetMouseDown(KeyCode::LeftMouse))
		{
			if (rect->IsBoundScreenPos(Input::main->GetMousePosition()))
			{
				switch (i)
				{
				case 0:
				{
					ComputeManager::main->StartChangeScene(1.0f);
					changeToggle = true;
					break;
				}
				case 1:
				{

					break;
				}
				case 2:
				{

					break;
				}
				case 3:
				{

					break;
				}
				case 4:
				{
					Game::main->Quit();
					break;
				}
				}
			}
		}
	}

	if (changeToggle && ComputeManager::main->IsChangeEffectEnd())
	{
		Scene::_changeScene = true;
		changeToggle = false;
	}
}

void GUIMainMenu::Update2()
{
	Component::Update2();
}

void GUIMainMenu::Enable()
{
	Component::Enable();
}

void GUIMainMenu::Disable()
{
	Component::Disable();
}

void GUIMainMenu::RenderBegin()
{
	Component::RenderBegin();
}

void GUIMainMenu::RenderEnd()
{
	Component::RenderEnd();
}

void GUIMainMenu::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIMainMenu::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIMainMenu::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIMainMenu::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIMainMenu::SetDestroy()
{
	Component::SetDestroy();
}

void GUIMainMenu::Destroy()
{
	Component::Destroy();
}

void GUIMainMenu::Reset()
{
	Component::Reset();
}
