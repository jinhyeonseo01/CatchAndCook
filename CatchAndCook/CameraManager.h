#pragma once

class Camera;
enum class CameraType;

class CameraManager
{
public:

	static unique_ptr<CameraManager> main;

	shared_ptr<Camera> GetCamera(CameraType type);

	void AddCamera(CameraType type, shared_ptr<Camera> camera) { _cameras[type] = camera; }
	void RemoveCamera(CameraType type);
	void Setting(CameraType type);
	void Setting();
	void SetActiveCamera(CameraType type);
	unordered_map<CameraType, shared_ptr<Camera>>& GetCameraAlls() { return _cameras; }

	shared_ptr<Camera> GetActiveCamera() { return _activeCamera; }
	CameraType GetCameraType();

	const char* CameraTypeToString(CameraType type);

private:
	unordered_map<CameraType, shared_ptr<Camera>> _cameras;
	shared_ptr<Camera> _activeCamera{};

};

