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
#include "ProgressCycleComponent.h"
#include "Transform.h"
COMPONENT(InteractiveComponent)

void InteractiveComponent::Init()
{

}

void InteractiveComponent::Start()
{

	{
		auto player = SceneManager::main->GetCurrentScene()->Find(L"seaPlayer");
		_seaPlayerController = player->GetComponent< SeaPlayerController>();
	}

	{
		//ProgressBar
		shared_ptr<GameObject> progressBar = SceneManager::main->GetCurrentScene()->Find(L"progressBar_Circle");

		if (progressBar)
		{
			_progressBar = progressBar;
		}

		else
		{

			_progressBar = SceneManager::main->GetCurrentScene()->CreateGameObject(L"progressBar_Circle");
			auto spriteComponet = _progressBar->AddComponent<Sprite>();

			spriteComponet->SetSize(vec2(0.4f, 0.05f));
			spriteComponet->SetLocalPos(vec3(0.3f, 0.2f, 0.0f));

			auto renderer = _progressBar->AddComponent<MeshRenderer>();
			shared_ptr<Material> material = make_shared<Material>();
			material->SetBlendFactor({ 1.0f,1.0f,1.0f,1.0f });
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteProgressCycle"));
			material->SetPass(RENDER_PASS::UI);
			renderer->AddMaterials({ material });
			_progressBar->SetActiveSelf(false);
			_progressBar->AddComponent<ProgressCycleComponent>();
		};
	}

	{
		shared_ptr<GameObject> interactiveBox = SceneManager::main->GetCurrentScene()->Find(L"interactiveBox");

		if (interactiveBox)
		{
			_interactiveBox = interactiveBox;
		}

		else
		{

			_interactiveBox = SceneManager::main->GetCurrentScene()->CreateGameObject(L"interactiveBox");
			auto spriteComponet = _interactiveBox->AddComponent<Sprite>();
			spriteComponet->SetTexture(ResourceManager::main->Get<Texture>(L"interactive"));
			spriteComponet->SetSize(vec2(0.5f, 0.6f));
			spriteComponet->SetLocalPos(vec3(0.25f, 0.1f, 0.01f));
			spriteComponet->SetClipingColor(vec4(0, 0, 0, 0));

			auto renderer = _interactiveBox->AddComponent<MeshRenderer>();
			shared_ptr<Material> material = make_shared<Material>();
			material->SetBlendFactor({ 0.9f,0.9f,0.9f,0.9f });
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
			material->SetPass(RENDER_PASS::UI);
			renderer->AddMaterials({ material });
			_interactiveBox->SetActiveSelf(false);
		};
	}


	{
		//TextMessage
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
			sprite->SetLocalPos(vec3(0.45f, 0.7f, 0.0f));
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
	}

	{
		//exitMessage Message
		shared_ptr<GameObject> exitMessage = SceneManager::main->GetCurrentScene()->Find(L"exitMiniGame" );

		if (exitMessage)
		{
			_exitMessage = exitMessage;
		}

		else
		{
			shared_ptr<GameObject> exitMessage = SceneManager::main->GetCurrentScene()->CreateGameObject(L"exitMiniGame:");
			auto renderer = exitMessage->AddComponent<MeshRenderer>();
			auto sprite = exitMessage->AddComponent<Sprite>();
			sprite->SetLocalPos(vec3(0.52f, 0.4f, 0.0f));
			sprite->SetSize(vec2(0.10f, 0.15f));
			sprite->SetTexture(ResourceManager::main->Load<Texture>(L"exitTexture",L"../Resources/Textures/exit.png"));
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
			material->SetPass(RENDER_PASS::UI);
			renderer->AddMaterials({ material });
			exitMessage->SetActiveSelf(false);
			_exitMessage = exitMessage;
		}
	}


	{
		//stop Message
		shared_ptr<GameObject> stopMessage = SceneManager::main->GetCurrentScene()->Find(L"stop");

		if (stopMessage)
		{
			_stopMessage = stopMessage;
		}

		else
		{
			shared_ptr<GameObject> stopMessage = SceneManager::main->GetCurrentScene()->CreateGameObject(L"stop");
			auto renderer = stopMessage->AddComponent<MeshRenderer>();
			auto sprite = stopMessage->AddComponent<Sprite>();
			sprite->SetLocalPos(vec3(0.38f, 0.4f, 0.0f));
			sprite->SetSize(vec2(0.10f, 0.15f));
			sprite->SetTexture(ResourceManager::main->Load<Texture>(L"stopTexture", L"../Resources/Textures/stop.png"));
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
			material->SetPass(RENDER_PASS::UI);
			renderer->AddMaterials({ material });
			stopMessage->SetActiveSelf(false);
			_stopMessage = stopMessage;
		}
	}



}

void InteractiveComponent::Update()
{
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

		if (auto text = _text.lock())
		{
			text->SetActiveSelf(true);

		}

		 SetState(InteractiveState::ONCOLLISION);
	}
}

void InteractiveComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (collider->IsTrigger() && other->GetOwner()->HasTag(GameObjectTag::Player))
	{
	
		if (auto text = _text.lock())
		{
			text->SetActiveSelf(false);

		}
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

void InteractiveComponent::Reset()
{
	GetOwner()->GetRoot()->SetActiveSelf(true);
	SetState(InteractiveState::NONE);
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
		if (ActionFunc::OnClickAction(KeyCode::LeftMouse, _exitMessage->GetComponent<Sprite>().get()))
		{
			Sound::main->Play("j_button_click", 0.3f, true);
			SetState(InteractiveState::ONCOLLISION);
		}

		if (ActionFunc::OnClickAction(KeyCode::LeftMouse, _stopMessage->GetComponent<Sprite>().get()))
		{
			Sound::main->Play("j_button_click", 0.3f, true);

			if (_progressBar->GetComponent<ProgressCycleComponent>()->isInAnswer())
			{
				SetState(InteractiveState::SUCCESS);
			}
			else
			{
				SetState(InteractiveState::FAIL);
			}
		}
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
	{
		MiniGameTotalOnOff(false);
		_seaPlayerController->SetMoveLock(false);
	}
		break;
	case InteractiveState::ONCOLLISION:
	{
		MiniGameTotalOnOff(false);
		_seaPlayerController->SetMoveLock(false);
	}
		break;
	case InteractiveState::GAMEON:
	{
		MiniGameTotalOnOff(true);
		_seaPlayerController->SetMoveLock(true);
		_seaPlayerController->SetState(SeaPlayerState::Idle);
	}
		break;
	case InteractiveState::SUCCESS:
	{
		Sound::main->Play("success", 0.1f, true);

		static vec3 offset = vec3(0, 20.0f, 0);

		ParticleManager::main->GenParticle(8.0f, 500, 6.0f, 30.0f, GetOwner()->_transform->GetWorldPosition() + offset, vec3(0, 0, 0), ParticleMoveType::RadialSpread, ParticleColorType::Red, { 0,0,0,0 }
		, ResourceManager::main->Get<Texture>(L"success"));

		InGameGlobal::main->AddItem(GetOwner()->GetRoot()->GetName());

		MiniGameTotalOnOff(false);
		_seaPlayerController->SetMoveLock(false);
		GetOwner()->GetRoot()->SetActiveSelf(false);
		break;
	}
	case InteractiveState::FAIL:
		Sound::main->Play("fail",0.1f, true);
		MiniGameTotalOnOff(false);
		_seaPlayerController->SetMoveLock(false);
		SetState(InteractiveState::ONCOLLISION);
		break;
	default:
		break;
	}
}

void InteractiveComponent::MiniGameTotalOnOff(bool onOff)
{
	_interactiveBox->SetActiveSelf(onOff);
	_exitMessage->SetActiveSelf(onOff);
	_stopMessage->SetActiveSelf(onOff);
	_progressBar->SetActiveSelf(onOff);

	if (onOff == true)
	{
		_progressBar->GetComponent<ProgressCycleComponent>()->Reset();
	}

	//if (onOff==false)
	//{
	//	::SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	//}

	Input::main->SetMouseLock(!onOff);

}
