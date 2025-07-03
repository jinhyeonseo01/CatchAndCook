#include "pch.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
AnimationListComponent::~AnimationListComponent()
{
}

bool AnimationListComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void AnimationListComponent::Init()
{
	Component::Init();

	for (auto& [key, value] : _animationKeys)
	{
		auto animation = ResourceManager::main->Get<Animation>(to_wstring(value));
		if (animation != nullptr)
			_animations[key] = animation;
	}
}

void AnimationListComponent::Start()
{
	Component::Start();

	if (auto hieracy = GetOwner()->GetComponent<SkinnedHierarchy>())
	{
		if (!_animations.empty())
		{
			auto& firstAnimation = _animations.begin()->second;

			if (firstAnimation)
			{
				hieracy->SetAnimation(firstAnimation);
			}
		}
		else
		{
			cout << "호잇" << endl;
		}
	}

}

void AnimationListComponent::Update()
{
	Component::Update();
}

void AnimationListComponent::Update2()
{
	Component::Update2();
}

void AnimationListComponent::Enable()
{
	Component::Enable();
}

void AnimationListComponent::Disable()
{
	Component::Disable();
}

void AnimationListComponent::RenderBegin()
{
	Component::RenderBegin();
}

void AnimationListComponent::CollisionBegin(const std::shared_ptr<Collider>& collider,
	const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void AnimationListComponent::CollisionEnd(const std::shared_ptr<Collider>& collider,
	const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void AnimationListComponent::ChangeParent(const std::shared_ptr<GameObject>& prev,
	const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void AnimationListComponent::SetDestroy()
{
	Component::SetDestroy();
}

void AnimationListComponent::Destroy()
{
	Component::Destroy();
}
