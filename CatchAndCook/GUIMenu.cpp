#include "pch.h"
#include "GUIMenu.h"

#include "Game.h"
#include "GUIOption.h"
#include "RectTransform.h"


COMPONENT(GUIMenu)
GUIMenu::~GUIMenu()
{
}

bool GUIMenu::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIMenu::Init()
{
	Component::Init();
}

void GUIMenu::Start()
{
	Component::Start();

	int i = 0;
	while (auto obj = GetOwner()->GetChildByName(L"Button_" + to_wstring(i))) {
		buttons.push_back(obj);
		++i;
	}
}

void GUIMenu::Update()
{
	Component::Update();

	Game::main->AddFunction(GetOwner(), [=]()
		{
			GetOwner()->SetActiveSelf(false);
		});

	for (int i=0;i< buttons.size();i++)
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
					GetOwner()->SetActiveSelf(false);
					break;
					}
				case 1:
				{
					GetOwner()->GetScene()->Find(L"GUIOption")->SetActiveSelf(true);
					GetOwner()->SetActiveSelf(false);
					break;
				}
				case 2:
				{
					ComputeManager::main->SetChangeSceneState(ChangeSceneState::FadeOut, 1.0f);
					changeToggle = true;
					//메인메뉴
					break;
				}
				case 3:
					{
						Game::main->Quit();
						break;
					}
				}
				break;
			}
		}
	}

	if (changeToggle && ComputeManager::main->GetChangeSceneState()== ChangeSceneState::FadeOutEnd)
	{
		changeToggle = false;
		Scene::_changeScene = true;
		Scene::_changeSceneType = SceneType::MainMenu;

		auto menu = GetOwner()->GetScene()->Find(L"GUIMenu");
		menu->SetActiveSelf(!menu->GetActiveSelf());

	}
}

void GUIMenu::Update2()
{
	Component::Update2();
}

void GUIMenu::Enable()
{
	Component::Enable();
}

void GUIMenu::Disable()
{
	Component::Disable();
	Game::main->RemoveFunction(GetOwner());
	Input::main->SetMouseLock(true);
}

void GUIMenu::RenderBegin()
{
	Component::RenderBegin();
}

void GUIMenu::RenderEnd()
{
	Component::RenderEnd();
}

void GUIMenu::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIMenu::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIMenu::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIMenu::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIMenu::SetDestroy()
{
	Component::SetDestroy();
}

void GUIMenu::Destroy()
{
	Component::Destroy();
}

void GUIMenu::Reset()
{
	Component::Reset();
}
