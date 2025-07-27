#include "pch.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TestScene.h"
#include "TestScene_jin.h"
#include "LightingTest.h"
#include "Scene_Sea01.h"
#include "UITestScene.h"
#include "CameraManager.h"
#include "Camera.h"
#include "MainMenuScene.h"
std::unique_ptr<SceneManager> SceneManager::main = nullptr;

shared_ptr<Scene> SceneManager::AddScene(SceneType type, bool initExecute)
{

	shared_ptr<Scene> scene;

	switch (type)
	{
	case::SceneType::MainMenu:
		scene = make_shared<MainMenuScene>();
		break;
	case::SceneType::TestScene:
		scene = make_shared<TestScene>();
		break;
	case::SceneType::TestScene2:
		scene = make_shared<TestScene_jin>();
		break;
	break;
	case::SceneType::LightingTest:
		scene = make_shared<LightingTest>();
		break;
	case::SceneType::Sea01:
		scene = make_shared<Scene_Sea01>();
		break;
	case ::SceneType::UITest:
		scene = make_shared<UITestScene>();
		break;
	default:
		break;
	}

	scene->InitGuid();
	scene->_type = type;
	_sceneTable[type] = scene;

	_currentScene = scene;

	if(initExecute)
		scene->Init();

	return scene;
}

void SceneManager::ChangeScene(const shared_ptr<Scene>& prevScene, const shared_ptr<Scene>& nextScene, bool initExecute, bool removeExecute)
{
	if (prevScene)
		Scene::_prevSceneType = prevScene->_type;

	auto currentScene = prevScene;
	std::vector<std::shared_ptr<GameObject>> dontObj;


	Core::main->FenceCurrentFrame();
	Sound::main->StopAll();

	switch (nextScene->GetSceneType())
	{
	case SceneType::MainMenu:
		Sound::main->Play("main_1", 0.44f);
		CameraManager::main->Setting(CameraType::ComponentCamera);
		break;
	case SceneType::TestScene2:
		Sound::main->Play("main_2", 0.225f);
		CameraManager::main->Setting(CameraType::ComponentCamera);
		break;
	case SceneType::Sea01:
		Sound::main->Play("underwater", 0.06f);
		Sound::main->Play("sea", 0.14f);
		CameraManager::main->Setting(CameraType::SeaCamera);
		break;
	default:
		break;
	}

	if (currentScene != nullptr)
	{
		for (auto& obj : prevScene->_dont_destroy_gameObjects)
		{
			if (!obj->IsDestroy())
				obj->GetChildsAll(dontObj);
		}
	}

	for (auto& obj : dontObj)
	{
		nextScene->AddGameObject(obj);
		if(currentScene != nullptr)
			currentScene->RemoveGameObject(obj);
	}
	if (currentScene != nullptr)
	{
		for (auto& obj : prevScene->_dont_destroy_gameObjects)
		{
			nextScene->_dont_destroy_gameObjects.push_back(obj);
		}
		currentScene->_dont_destroy_gameObjects.clear();
		if (removeExecute)
			currentScene->Release();
	}


	_currentScene = nextScene;

	//
	for (auto& obj : nextScene->_gameObjects)
		for (auto& component : obj->GetComponentAll())
			component->ChangeScene(currentScene, nextScene);

	if (initExecute)
		_currentScene->Init();

	ComputeManager::main->SetChangeSceneState(ChangeSceneState::FadeIn,1.0f);

}

std::shared_ptr<Scene> SceneManager::FindScene(SceneType type)
{
	return _sceneTable[type];
}

void SceneManager::Reload()
{
	//SceneManager::GetCurrentScene()->_type
	auto prevScene = GetCurrentScene();
	auto nextScene = AddScene(prevScene->_type, false);
	ChangeScene(prevScene, nextScene, true, true);
}
