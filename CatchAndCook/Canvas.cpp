#include "pch.h"
#include "Canvas.h"

#include "Camera.h"
#include "CameraManager.h"

Canvas::~Canvas()
{
}

bool Canvas::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void Canvas::Init()
{
	Component::Init();
}

void Canvas::Start()
{
	Component::Start();
}

void Canvas::Update()
{
	Component::Update();
}

void Canvas::Update2()
{
	Component::Update2();
}

void Canvas::Enable()
{
	Component::Enable();
}

void Canvas::Disable()
{
	Component::Disable();
}

void Canvas::RenderBegin()
{
	Component::RenderBegin();
}

void Canvas::RenderEnd()
{
	Component::RenderEnd();
}

void Canvas::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void Canvas::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void Canvas::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void Canvas::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void Canvas::SetDestroy()
{
	Component::SetDestroy();
}

void Canvas::Destroy()
{
	Component::Destroy();
}

Matrix Canvas::GetOverlayMatrix()
{
	float nears = CameraManager::main->GetActiveCamera()->GetCameraParams().cameraFrustumData.z;
	Matrix overlayMatrix = Matrix::CreateTranslation(Vector3(0, 0, nears))
		* Matrix::CreateTranslation(Vector3(-resolution.x / 2, -resolution.y / 2, 0))
		* Matrix::CreateScale(Vector3(2 / resolution.x, 2 / resolution.y, 1));
	return overlayMatrix;
}
