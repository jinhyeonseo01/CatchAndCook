#include "pch.h"
#include "itemBoxComponent.h"
#include "Transform.h"
#include "InGameGlobal.h"
void itemBoxComponent::Init()
{
}

void itemBoxComponent::Start()
{
    _orginY = GetOwner()->_transform->GetLocalPosition().y;
}

void itemBoxComponent::Update()
{

    float time = Time::main->GetTime();
    float speed = 1.0f;
    float amplitude = 5.0f;

    auto position = GetOwner()->_transform->GetLocalPosition();
    position.y = _orginY + sin(time * speed) * amplitude;
    GetOwner()->_transform->SetLocalPosition(position);

    _rotate = fmod(_rotate + Time::main->GetDeltaTime() * 45.0f, 360.0f);
    GetOwner()->_transform->SetLocalRotation(vec3(0, D2R * _rotate, 0));

}

void itemBoxComponent::Update2()
{
}

void itemBoxComponent::Enable()
{
}

void itemBoxComponent::Disable()
{
}

void itemBoxComponent::RenderBegin()
{
}

void itemBoxComponent::RenderEnd()
{
}

void itemBoxComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    Sound::main->Play("itemGet2",0.3f);
    InGameGlobal::main->AddItem(GetOwner()->GetName());
	GetOwner()->SetDestroy();
}

void itemBoxComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void itemBoxComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void itemBoxComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void itemBoxComponent::SetDestroy()
{
}

void itemBoxComponent::Destroy()
{
}

void itemBoxComponent::Reset()
{
    GetOwner()->SetDestroy();

}
