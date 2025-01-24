#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "GameObject.h"
#include "ICBufferInjector.h"
#include "IGuid.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Transform.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "TextManager.h"

void Game::Init(HWND hwnd)
{
	IGuid::StaticInit();
	Time::main = make_unique<Time>();
	Time::main->Init();
	Input::main = make_unique<Input>();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);

	ResourceManager::main = make_unique<ResourceManager>();
	ResourceManager::main->Init();
	SceneManager::main = make_unique<SceneManager>();
	CameraManager::main = make_unique<CameraManager>();

	InjectorManager::main = make_unique<InjectorManager>();
	InjectorManager::main->Init();
	InjectorManager::main->Register<TestSubMaterialParamInjector>(BufferType::MateriaSubParam);

	CameraManager::main->AddCamera(CameraType::ThirdPersonCamera, static_pointer_cast<Camera>(make_shared<ThirdPersonCamera>()));
	CameraManager::main->GetCamera(CameraType::ThirdPersonCamera)->SetCameraPos(vec3(0, 0, -5.0f));
	CameraManager::main->SetActiveCamera(CameraType::ThirdPersonCamera);

	TextManager::main = make_unique<TextManager>();
	TextManager::main->Init();

	auto scene = SceneManager::main->AddScene(SceneType::TestScene);
	SceneManager::main->ChangeScene(scene);

	ResourceManager::main->Load<Model>(L"testModel", L"../Resources/Models/Kindred/kindred_unity.fbx", VertexType::Vertex_Skinned);
	auto obj = ResourceManager::main->Get<Model>(L"testModel")->CreateGameObject(scene);

	vector<shared_ptr<MeshRenderer>> v;
	obj->GetComponentsWithChilds(v);

	for (auto& ele : v)
	{
		ele->SetDrawNormal(false);
	}

	obj->_transform->SetWorldPosition(vec3(0, 5.0f, -1));
}

void Game::PrevUpdate()
{
	if (Input::main->GetKeyDown(KeyCode::Esc))
	{
		Core::main->Fence();
		DestroyWindow(Core::main->GetHandle());
		Release();
		_quit = true;
		return;
	}

	if (Input::main->GetKeyDown(KeyCode::F9))
	{
		HWND hWnd = Core::main->GetHandle();
		if (!_fullScreen)
		{
			RECT rc;
			GetWindowRect(hWnd, &rc);

			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);

			SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(hWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			WINDOW_WIDTH = mi.rcMonitor.right - mi.rcMonitor.left;
			WINDOW_HEIGHT = mi.rcMonitor.bottom - mi.rcMonitor.top;

			Core::main->ResizeWindowSize();
		}
		else
		{
			SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			// 원래 크기로 윈도우 설정
			SetWindowPos(hWnd, HWND_TOP, 0, 0, 800, 600, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			RECT rect;
			if (GetClientRect(hWnd, &rect))
			{
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				WINDOW_WIDTH = width;
				WINDOW_HEIGHT = height;
			}
	
			Core::main->ResizeWindowSize();

		}
		_fullScreen = !_fullScreen;
		return;
	}
}

void Game::Run()
{

	Input::main->Update();
	Time::main->Update();
	PrevUpdate();
	if (_quit)
		return;

	CameraUpdate();

	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();
	currentScene->Update();
	
	Core::main->RenderBegin();
	currentScene->RenderBegin();
	currentScene->Rendering();
	currentScene->DebugRendering();
	currentScene->RenderEnd();
	Core::main->RenderEnd();
	currentScene->Finish();

}

void Game::Release()
{
	SceneManager::main.reset(nullptr);
	InjectorManager::main.reset(nullptr);
	CameraManager::main.reset(nullptr);
	InjectorManager::main.reset(nullptr);
	ResourceManager::main.reset(nullptr);
	Time::main.reset(nullptr);
	Input::main.reset(nullptr);
	IGuid::StaticRelease();
	Core::main.reset(nullptr);
}

void Game::CameraUpdate()
{
	shared_ptr<Camera> camera = CameraManager::main->GetActiveCamera();

	const float speed = 15.f;
	const float dt =Time::main->GetDeltaTime() *speed;

	if (Input::main->GetKey(KeyCode::W))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraLook();
		camera->SetCameraPos(direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::S))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraLook();
		camera->SetCameraPos(-direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::D))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraRight();
		camera->SetCameraPos(direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::A))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraRight();
		camera->SetCameraPos(-direction * dt + prevPos);
	}

	if (Input::main->GetMouse(KeyCode::LeftMouse))
	{
		static vec2 lastMousePos;
		vec2 currentMousePos = Input::main->GetMousePosition();

		//튀는현상 방어
		if (Input::main->GetMouseDown(KeyCode::LeftMouse))
		{
			lastMousePos = currentMousePos;
		}

		float speed = 0.3f;

		vec2 delta = (currentMousePos - lastMousePos)*speed;
		lastMousePos = currentMousePos;

		camera->SetCameraRotation(delta.x, delta.y, 0);
	}

	


}
