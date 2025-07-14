#include "pch.h"
#include "InteractiveComponent.h"
#include "EventComponent.h"
#include "Sprite.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "TextManager.h"
#include "Scene_Sea01.h"
#include "SeaPlayerController.h"
#include "SpriteAction.h"
COMPONENT(InteractiveComponent)

void InteractiveComponent::Init()
{
}

void InteractiveComponent::Start()
{
	auto& rootname = GetOwner()->GetRoot()->GetName();

	shared_ptr<GameObject> object = SceneManager::main->GetCurrentScene()->Find(L"textMessage:" + rootname);

	if (object)
	{
		_text = object;
	}

	else
	{
		shared_ptr<GameObject> text = SceneManager::main->GetCurrentScene()->CreateGameObject(L"textMessage:" + rootname);
		auto renderer = text->AddComponent<MeshRenderer>();
		auto sprite = text->AddComponent<TextSprite>();
		sprite->SetLocalPos(vec3(0.45f, 0.9f, 0.1f));
		sprite->SetSize(vec2(0.3f, 0.3f));
		sprite->SetText(GetOwner()->GetRoot()->GetName());
		sprite->CreateObject(550, 256, L"Arial", FontColor::WHITE, 60);
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);
		renderer->AddMaterials({ material });

		text->SetActiveSelf(false);
		_text = text;
	}


	{
		 auto player  =SceneManager::main->GetCurrentScene()->Find(L"seaPlayer");
		 _seaPlayerController = player->GetComponent< SeaPlayerController>();
	}


	
}

void InteractiveComponent::Update()
{
	if (_state == InteractiveState::ONCOLLISION)
	{
		if (Input::main->GetKeyDown(KeyCode::F))
		{
			SetState(InteractiveState::GAMEON);
		}
	}

	UpdateState();
}

void InteractiveComponent::Update2()
{
}

void InteractiveComponent::Enable()
{
}

void InteractiveComponent::Disable()
{
}

void InteractiveComponent::RenderBegin()
{
}

void InteractiveComponent::RenderEnd()
{
}

void InteractiveComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (collider->IsTrigger() && other->GetOwner()->HasTag(GameObjectTag::Player))
	{

		 SetState(InteractiveState::ONCOLLISION);
	}
}

void InteractiveComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (collider->IsTrigger() && other->GetOwner()->HasTag(GameObjectTag::Player))
	{
	
		SetState(InteractiveState::NONE);
		
	}
}

void InteractiveComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void InteractiveComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void InteractiveComponent::SetDestroy()
{
}

void InteractiveComponent::Destroy()
{
}

void InteractiveComponent::UpdateState()
{
	switch (_state)
	{
	case InteractiveState::NONE:
		break;
	case InteractiveState::ONCOLLISION:
		if (Input::main->GetKeyDown(KeyCode::F))
		{
			SetState(InteractiveState::GAMEON);
		}
		break;
	case InteractiveState::GAMEON:
		break;
	case InteractiveState::SUCCESS:
		break;
	case InteractiveState::FAIL:
		break;
	default:
		break;
	}

}

void InteractiveComponent::SetState(InteractiveState state)
{
	if (_state == state)
		return;

	_state = state;

	switch (_state)
	{
	case InteractiveState::NONE:
		if (auto text = _text.lock())
		{
			text->SetActiveSelf(false);
		}

		_seaPlayerController->SetState(SeaPlayerState::Idle);
		break;
	case InteractiveState::ONCOLLISION:
		if (auto text = _text.lock())
		{
			text->SetActiveSelf(true);
		}

		break;
	case InteractiveState::GAMEON:
	{
		if (auto text = _text.lock())
		{
			text->SetActiveSelf(false);
		}
		auto scene = SceneManager::main->GetCurrentScene();
		auto seaScene = dynamic_pointer_cast<Scene_Sea01>(scene);
		if (seaScene)
		{
			seaScene->GetInteractiveBox()->SetActiveSelf(true);
		}
		_seaPlayerController->SetState(SeaPlayerState::MiniGame);
	}
		break;
	case InteractiveState::SUCCESS:
		if (auto text = _text.lock())
		{
			text->SetActiveSelf(false);
		}
		_seaPlayerController->SetState(SeaPlayerState::Idle);
		break;
	case InteractiveState::FAIL:
		if (auto text = _text.lock())
		{
			text->SetActiveSelf(false);
		}
		_seaPlayerController->SetState(SeaPlayerState::Idle);
		break;
	default:
		break;
	}
}
