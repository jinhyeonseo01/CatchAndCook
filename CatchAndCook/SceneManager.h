#pragma once
#include "Scene.h"
#include "SceneType.h"

class SceneManager
{

public:
	static std::unique_ptr<SceneManager> main;
	std::shared_ptr<Scene> GetCurrentScene() { return _currentScene;};
	shared_ptr<Scene> AddScene(SceneType type, bool initExecute = true);

	template <class T,class = std::enable_if_t<std::is_base_of_v<Scene,T>>>
	shared_ptr<T> AddScene(std::shared_ptr<T> scene)
	{
		if(_sceneTable.find(scene->_type) != _sceneTable.end())
			assert(false && "Scene name already exists!");

		_sceneTable[scene->_type] = scene;

		if(_currentScene == nullptr)
			_currentScene = scene;
		return scene;
	};

	template <class T,class = std::enable_if_t<std::is_base_of_v<Scene,T>>>
	shared_ptr<T> AddScene(SceneType type, bool initExecute = true)
	{
		if(_sceneTable.find(type) != _sceneTable.end())
			assert(false && "Scene name already exists!");

		shared_ptr<Scene> scene;
		scene = make_shared<T>();
		scene->InitGuid();
		scene->_type = type;
		_sceneTable[type] = scene;

		if(_currentScene == nullptr)
			_currentScene = scene;

		if(initExecute)
			scene->Init();
		return scene;
	};

	void ChangeScene(const shared_ptr<Scene>& prevScene, const shared_ptr<Scene>& nextScene, bool initExecute = true, bool removeExecute = true);
	std::shared_ptr<Scene> FindScene(SceneType type);
	template <class T,class = std::enable_if_t<std::is_base_of_v<Scene,T>>>
	std::shared_ptr<T> FindScene()
	{
		for(auto& scene : _sceneTable)
			if(auto targetScene = std::dynamic_pointer_cast<T>(scene.second); targetScene != nullptr)
				return targetScene;
		return nullptr;
	};
	void Reload();

	friend class Scene;
private:
	std::unordered_map<SceneType, std::shared_ptr<Scene>> _sceneTable;
	std::shared_ptr<Scene> _currentScene = nullptr;
};
