#include "pch.h"
#include "FireBall.h"
#include "Transform.h"



void FireBall::Init()
{
}

void FireBall::Start()
{
}

void FireBall::Update()
{
	_currTime += Time::main->GetDeltaTime();

	if (_currTime >= _autoDestroyTime)
	{
		GetOwner()->SetDestroy();
	}


	GetOwner()->_transform->SetLocalPosition(GetOwner()->_transform->GetLocalPosition() + vec3(0,1,0)* _speed * Time::main->GetDeltaTime());
}

void FireBall::Update2()
{
}

void FireBall::Enable()
{
}

void FireBall::Disable()
{
}

void FireBall::RenderBegin()
{
}

void FireBall::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FireBall::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FireBall::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void FireBall::SetDestroy()
{
}

void FireBall::Destroy()
{
	switch (_color)
	{
	case yellow:
		ParticleManager::main->GenParticle(2.0f, 500, 8.0f, 12.0f, GetOwner()->_transform->GetWorldPosition(), vec3(0, 0, 0), ParticleMoveType::RadialSpread, ParticleColorType::Red, { 0,0,0,0 }
		, ResourceManager::main->Get<Texture>(L"success"));
		break;
	case red:
		ParticleManager::main->GenParticle(2.0f, 500, 8.0f, 12.0f, GetOwner()->_transform->GetWorldPosition(), vec3(0, 0, 0), ParticleMoveType::RadialSpread, ParticleColorType::Red, { 0,0,0,0 }
		, ResourceManager::main->Get<Texture>(L"success_red"));
		break;
	case green:
		ParticleManager::main->GenParticle(2.0f, 500, 8.0f, 12.0f, GetOwner()->_transform->GetWorldPosition(), vec3(0, 0, 0), ParticleMoveType::RadialSpread, ParticleColorType::Red, { 0,0,0,0 }
		, ResourceManager::main->Get<Texture>(L"success_green"));
		break;
	default:
		break;
	}

}
