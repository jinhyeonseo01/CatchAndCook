#include "pch.h"
#include "EventComponent.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "TextManager.h"
COMPONENT(EventComponent)

void EventComponent::Init()
{



}

void EventComponent::Start()
{

	


}

void EventComponent::Update()
{
	if (_Oncollision)
	{
		for (auto& func : _OnUpdateBlock)
		{
			func(_otherCashing);
		}
	}


	for (auto& func : _OnUpdateAlways)
	{
		func(_otherCashing);
	};

};

void EventComponent::Update2()
{
	
}

void EventComponent::Enable()
{
}

void EventComponent::Disable()
{

}

void EventComponent::RenderBegin()
{

}

void EventComponent::RenderEnd()
{
}

void EventComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (_oneTimeEventCall && _Oncollision)
		return;

	if (HasTag(bindTag, other->GetOwner()->GetTag()) == false)
		return;

	_Oncollision = true;
	_otherCashing = other;

	for (auto& func : _OnCollisionBegins)
	{
		func(other);
	}

}

void EventComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (_oneTimeEventCall && _Oncollision==false)
		return;

	if (HasTag(bindTag, other->GetOwner()->GetTag()) == false)
		return;

	_Oncollision = false;
	_otherCashing = nullptr;

	for (auto& func : _OnCollisionEnds)
	{
		func(other);
	}

}

void EventComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void EventComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void EventComponent::SetDestroy()
{
}

void EventComponent::Destroy()
{
}

void EventComponent::ShowEventMessage(bool show)
{
	if (_messageObject.lock() == nullptr)
		return;

	_messageObject.lock()->SetActiveSelf(show);
}

void EventComponent::SetBindMessage(const wstring& message, const vec3& pos, const vec2& size, bool active)
{
	{
		shared_ptr<GameObject> text = SceneManager::main->GetCurrentScene()->CreateGameObject(L"EvenetMessage");
		auto& renderer = text->AddComponent<MeshRenderer>();
		auto& sprite = text->AddComponent<TextSprite>();
		

		sprite->SetLocalPos(pos);
		sprite->SetSize(size);
		sprite->SetText(message);

		sprite->CreateObject(550, 256, L"Arial", FontColor::WHITE, 60);
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);
		renderer->AddMaterials({ material });
		text->SetActiveSelf(active);

		_messageObject = text;
	};

}
