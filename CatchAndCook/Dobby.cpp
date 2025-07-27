#include "pch.h"
#include "Dobby.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
#include "FireWorkManager.h"
#include "Animation.h"
COMPONENT(Dobby)

Dobby::~Dobby()
{
}

bool Dobby::IsExecuteAble()
{
	return false;
}

void Dobby::Init()
{
	
}

void Dobby::Start()
{
	_animations = GetOwner()->GetComponentWithChilds<AnimationListComponent>()->GetAnimations();
	_skined = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();
	FireWorkManager::main->PushDobby(GetCast<Dobby>());
	if (_animations.find("magic") != _animations.end())
	{
		_animations["magic"]->_isLoop = false;
	}

	if (_animations.find("idle") != _animations.end())
	{
		_skined->Play(_animations["idle"], 0.5f);
	}

}

void Dobby::Update()
{
	if (_skined->_isPlaying==false)
	{
		_skined->Play(_animations["idle"], 0.5f);
	}
}

void Dobby::Update2()
{
}

void Dobby::Enable()
{
}

void Dobby::Disable()
{
}

void Dobby::RenderBegin()
{
}

void Dobby::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Dobby::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Dobby::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void Dobby::SetDestroy()
{
}

void Dobby::Destroy()
{

}

void Dobby::SetAnimation(const std::string& name)
{
	if (_animations.find(name) != _animations.end())
	{
		_skined->Play(_animations[name], 0.5f);
	}
}