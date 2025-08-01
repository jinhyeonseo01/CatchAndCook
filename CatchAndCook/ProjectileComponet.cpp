#include "pch.h"
#include "ProjectileComponet.h"
#include "Transform.h"
#include "Weapon.h"
void ProjectileComponet::Init()
{
	
}

void ProjectileComponet::Start()
{
}

void ProjectileComponet::Update()
{
	auto& pos = GetOwner()->_transform->GetLocalPosition();
	GetOwner()->_transform->SetLocalPosition(pos + _dir * _speed * Time::main->GetDeltaTime());
}

void ProjectileComponet::Update2()
{
	_currTime += Time::main->GetDeltaTime();

	if (_autoDestroyTime <= _currTime)
	{
		_currTime = 0;
		Weapon::RecycleBullet(GetOwner());
	}
}

void ProjectileComponet::Enable()
{
}

void ProjectileComponet::Disable()
{
}

void ProjectileComponet::RenderBegin()
{
}

void ProjectileComponet::RenderEnd()
{
}

void ProjectileComponet::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ProjectileComponet::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void ProjectileComponet::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void ProjectileComponet::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void ProjectileComponet::SetDestroy()
{
}

void ProjectileComponet::Destroy()
{
}



void ProjectileComponet::SetDir(const vec3& dir)
{
	_dir = dir;

	GetOwner()->_transform->SetForward(dir);
}
