#include "pch.h"
#include "CameraManager.h"
#include "Camera.h"
unique_ptr<CameraManager> CameraManager::main = nullptr;

shared_ptr<Camera> CameraManager::GetCamera(CameraType type)
{
	return _cameras[type];
}

void CameraManager::RemoveCamera(CameraType type)
{
	_cameras.erase(type);
}

void CameraManager::Setting(CameraType type)
{
	if (_cameras.find(type) == _cameras.end())
	{
		return;
	}

	_activeCamera = _cameras[type];
	_activeCamera->Update();
	_activeCamera->PushData();
	_activeCamera->SetData();
}

void CameraManager::Setting()
{
	_activeCamera->Update();
	_activeCamera->PushData();
	_activeCamera->SetData();
}

void CameraManager::SetActiveCamera(CameraType type)
{
	_activeCamera = _cameras[type];
}

CameraType CameraManager::GetCameraType()
{
	return _activeCamera->GetCameraType();
}

const char* CameraManager::CameraTypeToString(CameraType type)
{
	switch (type)
	{
	case CameraType::DebugCamera:    return "DebugCamera";
	case CameraType::SeaCamera:      return "SeaCamera";
	case CameraType::BoatCamera:     return "BoatCamera";
	case CameraType::ComponentCamera:return "ComponentCamera";
	default:                        return "Unknown";
	}
}
