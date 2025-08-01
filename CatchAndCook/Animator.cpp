#include "pch.h"
#include "Animator.h"

Animator::~Animator()
{
}

bool Animator::IsExecuteAble()
{
    return false;
}

void Animator::Init()
{
}

void Animator::Start()
{
}

void Animator::Update()
{
}

void Animator::Update2()
{
}

void Animator::Enable()
{
}

void Animator::Disable()
{
}

void Animator::RenderBegin()
{
}

void Animator::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Animator::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Animator::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void Animator::SetDestroy()
{
}

void Animator::Destroy()
{
}
