#include "pch.h"
#include "BoatController.h"

BoatController::~BoatController()
{
}

bool BoatController::IsExecuteAble()
{
	return true;
}

void BoatController::Init()
{
}

void BoatController::Start()
{
}

void BoatController::Update()
{
}

void BoatController::Update2()
{
}

void BoatController::Enable()
{
}

void BoatController::Disable()
{
}

void BoatController::RenderBegin()
{
}

void BoatController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BoatController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BoatController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void BoatController::SetDestroy()
{
}

void BoatController::Destroy()
{
}
