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
#include "ColliderManager.h"
#include "Gizmo.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "TextManager.h"
#include "LightManager.h"
#include "TerrainManager.h"
#include "InstancingManager.h"
#include "Profiler.h"
#include "ComputeManager.h"
#include "ImguiManager.h"
#include "NavMeshManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "GUIInventory.h"
#include "InGameGlobal.h"
#include "GUIItemBox.h"
#include "GUIMainMenu.h"
#include "Terrain.h"
#include "PathStamp.h"
#include "ShadowManager.h"
#include "ParticleManager.h"
#include "SoundManager.h"

std::shared_ptr<Game> Game::main = nullptr;

void Game::Init(HWND hwnd)
{
	IGuid::StaticInit();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);

	Time::main = make_unique<Time>();
	Time::main->Init();

	Input::main = make_unique<Input>();

	//Profiler::main = make_unique<Profiler>();
	//Profiler::main->Init(_hwnd, _hInstance);

#ifdef IMGUI_ON
	ImguiManager::main = make_unique<ImguiManager>();
	ImguiManager::main->Init();
#endif // IMGUI_ON

	ResourceManager::main = make_unique<ResourceManager>();
	ResourceManager::main->Init();



	NavMeshManager::main = make_unique<NavMeshManager>();
	NavMeshManager::main->Init();

	TerrainManager::main = make_unique<TerrainManager>();

	SceneManager::main = make_unique<SceneManager>();
	ColliderManager::main = make_unique<ColliderManager>();
	CameraManager::main = make_unique<CameraManager>();

	InjectorManager::main = make_unique<InjectorManager>();
	InjectorManager::main->Init();

	TextManager::main = make_unique<TextManager>();
	TextManager::main->Init();

	ShadowManager::main = make_unique<ShadowManager>();
	ShadowManager::main->Init();

	ComputeManager::main = make_unique<ComputeManager>();
	ComputeManager::main->Init();

	Gizmo::main = std::make_unique<Gizmo>();
	Gizmo::main->Init();
	CameraManager::main->AddCamera(CameraType::DebugCamera, make_shared<DebugCamera>());
	CameraManager::main->AddCamera(CameraType::SeaCamera, make_shared<SeaCamera>());
	CameraManager::main->AddCamera(CameraType::BoatCamera, make_shared<BoatCamera>());

	LightManager::main = make_unique<LightManager>();
	LightManager::main->Init();

	InstancingManager::main = make_unique<InstancingManager>();

	InGameGlobal::main = make_unique<InGameGlobal>();
	InGameGlobal::main->Init();

	PathStamp::main = make_unique<PathStamp>();
	PathStamp::main->Init();

	ParticleManager::main = make_unique<ParticleManager>();
	ParticleManager::main->Init();

	Sound::main = make_unique<Sound>();
	Sound::main->Init();

	SceneManager::main->AddScene(SceneType::MainMenu, true);
	SceneManager::main->AddScene(SceneType::TestScene2, true);
	SceneManager::main->AddScene(SceneType::Sea01, true);
	SceneManager::main->ChangeScene(nullptr, SceneManager::main->FindScene(SceneType::MainMenu), false, false);
};

void Game::PrevUpdate()
{
	//제거
	if (Input::main->GetKey(KeyCode::K))
	{
		InGameGlobal::main->gold += 1000;
	}

	if (Input::main->GetKeyDown(KeyCode::Esc) || _quit)
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
	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();

	Profiler::Set("CPU_Time");

	Input::main->Update();
	Time::main->Update();
	Sound::main->Update();


	PrevUpdate();

	if (_quit)
		return;

	CameraUpdate();

	
	Profiler::Set("CoreRenderBegin");
	Core::main->RenderBegin();
	Profiler::Fin();

	{
		Profiler::Set("Logic_Total");
			currentScene->Update();
			currentScene->RenderBegin();
				Profiler::Set("Logic_Light");
					LightManager::main->SetData();
				Profiler::Fin();
		Profiler::Fin();
	}

	
	Profiler::Set("Rendering_PASS", BlockTag::CPU);
		currentScene->Rendering();
		currentScene->DebugRendering();
		currentScene->RenderEnd();
	Profiler::Fin();
	Profiler::Fin();

	Profiler::Set("GPU_Time", BlockTag::GPU);
		Core::main->RenderEnd();
	Profiler::Fin();


	currentScene->Finish();
	Profiler::main->Reset();
}

void Game::Release()
{
	GUIInventory::main = nullptr;
	GUIItemBox::main = nullptr;
	GUIMainMenu::main = nullptr;

	ColliderManager::main.reset(nullptr);
	SceneManager::main.reset(nullptr);
	Gizmo::main.reset(nullptr);
	InjectorManager::main.reset(nullptr);
	CameraManager::main.reset(nullptr);
	ResourceManager::main.reset(nullptr);
	Time::main.reset(nullptr);
	Input::main.reset(nullptr);
	Core::main.reset(nullptr);
	IGuid::StaticRelease();

	main = nullptr;
}

void Game::CameraUpdate()
{
	if (CameraManager::main->GetActiveCamera()->GetCameraType() != CameraType::DebugCamera)
		return;

	shared_ptr<Camera> camera = CameraManager::main->GetCamera(CameraType::DebugCamera);

	static float speed = 500.0f;
	const float dt =Time::main->GetDeltaTime() *speed;

	if (Input::main->GetKey(KeyCode::Minus))
	{

		speed -= 10.0f * dt;

		if(speed<=0)
		{
			speed = 0;
		}
		
	}

	if (Input::main->GetKey(KeyCode::Plus))
	{
		if (speed <= 1000)
		{
			speed += 10.0f * dt;
		}
		else
		{
			speed = 1000;
		}
	}



	if (Input::main->GetKey(KeyCode::UpArrow))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraLook();
		camera->SetCameraPos(direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::DownArrow))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraLook();
		camera->SetCameraPos(-direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::RightArrow))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraRight();
		camera->SetCameraPos(direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::LeftArrow))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraRight();
		camera->SetCameraPos(-direction * dt + prevPos);
	}
	if(Input::main->GetKey(KeyCode::Space))
	{
		auto prevPos = camera->GetCameraPos();
		camera->SetCameraPos(Vector3::Up * dt + prevPos);
	}

	if(Input::main->GetKey(KeyCode::Shift))
	{
		auto prevPos = camera->GetCameraPos();
		camera->SetCameraPos(Vector3::Down * dt + prevPos);
	}

	if (Input::main->GetMouse(KeyCode::RightMouse))
	{
		static vec2 lastMousePos;
		static vec2 sumDelta =vec2::Zero;
		vec2 currentMousePos = Input::main->GetMousePosition();

		//Ƣ������ ���
		if (Input::main->GetMouseDown(KeyCode::RightMouse))
		{
			lastMousePos = currentMousePos;
		}

		float speed = 0.3f;

		vec2 delta = (currentMousePos - lastMousePos)*speed;
		lastMousePos = currentMousePos;

		sumDelta += delta;

		camera->SetCameraRotation(sumDelta.x, sumDelta.y, 0);
		
	}

	
	


}

void Game::SetHandle(HWND hwnd, HINSTANCE hInst)
{
	_hwnd = hwnd;
	_hInstance = hInst;
}
