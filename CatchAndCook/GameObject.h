#pragma once

#include "GameObjectSetting.h"
#include "IDelayDestroy.h"
#include "IGuid.h"
#include "SceneManager.h"

class Scene;
class Component;
class Transform;
class Collider;
class RendererBase;


class GameObject : public IGuid, public IDelayDestroy
{
public:
	GameObject();
	GameObject(std::wstring name);
	virtual ~GameObject() override;

	void Init();
	void Start(); 
	void Update(); 
	void Update2(); 
	void Enable(); 
	void Disable(); 
	void Destroy(); 
	void RenderBegin();
	void RenderEnd();

	void Reset();

	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	

public:
	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		component->InitGuid();
		component->SetOwner(GetCast<GameObject>());
		_components.push_back(component);
		component->Init();
		if (GetActive()) component->Enable();
		SceneManager::main->GetCurrentScene()->AddStartQueue(GetCast<GameObject>());
		return component;
	}


	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	void AddComponent(shared_ptr<T> component)
	{
		if (component == nullptr)
			return;
		std::shared_ptr<Component> parentComponent = std::static_pointer_cast<Component>(component);
		auto iter = std::find(_components.begin(), _components.end(), parentComponent);
		if (iter != _components.end())
			return;
		if (component->GetOwner() != nullptr)
			component->GetOwner()->DisconnectComponent(component);

		component->SetOwner(GetCast<GameObject>());
		_components.push_back(parentComponent);
		component->Init();
		if (GetActive()) component->Enable();
		SceneManager::main->GetCurrentScene()->AddStartQueue(GetCast<GameObject>());
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	bool DisconnectComponent(const std::shared_ptr<T>& component)
	{
		if (component == nullptr)
			return false;
		std::shared_ptr<Component> parentComponent = std::static_pointer_cast<Component>(component);
		auto iter = std::find(_components.begin(), _components.end(), parentComponent);
		if (iter != _components.end())
		{
			_components.erase(iter);
			return true;
		}
		return false;
	};
	bool DisconnectAtComponent(int index)
	{
		_components.erase(_components.begin() + index);
		return true;
	};

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	bool RemoveComponent(const std::shared_ptr<T>& component)
	{
		return false;
	};
	bool RemoveAtComponent(int index)
	{
		return false;
	};

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::shared_ptr<T> GetComponent()
	{
		auto iter = std::find_if(_components.begin(), _components.end(), 
			[&](const std::shared_ptr<Component>& component) {
				return std::dynamic_pointer_cast<T>(component) != nullptr;
			});
		if (iter == _components.end())
			return nullptr;
		return std::static_pointer_cast<T>(*iter);
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	int GetComponents(std::vector<std::shared_ptr<T>>& vec)
	{
		int prevCount = vec.size();
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(downCast);
		}
		return vec.size() - prevCount;
	}
	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::vector<std::shared_ptr<T>> GetComponents()
	{
		std::vector<std::shared_ptr<T>> vec;
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(downCast);
		}
		return vec;
	}


	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	int GetComponentsWithChilds(std::vector<std::shared_ptr<T>>& vec)
	{
		int count = vec.size();
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(downCast);
		}

		count = vec.size() - count;
		for (auto& child : _childs)
			if (!child.expired())
				count += child.lock()->GetComponentsWithChilds<T>(vec);

		return count;
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::shared_ptr<T> GetComponentWithChilds()
	{
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				return downCast;
		}
		for (auto& child : _childs)
			if (!child.expired())
			{
				std::shared_ptr<T> result = child.lock()->GetComponentWithChilds<T>();
				if (result != nullptr)
					return result;
			}
		return nullptr;
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::vector<std::shared_ptr<T>> GetComponentsWithChilds()
	{
		std::vector<std::shared_ptr<T>> vec;
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(downCast);
		}

		for (auto& child : _childs)
			if (!child.expired())
				child.lock()->GetComponentsWithChilds<T>(vec);

		return vec;
	}

	template <class T,class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	int GetComponentsWithParents(std::vector<std::shared_ptr<T>>& vec)
	{
		int count = vec.size();

		for(auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if(downCast != nullptr)
				vec.push_back(downCast);
		}

		count = vec.size() - count;

		auto parent = GetParent();
		if(parent != nullptr)
			count += parent->GetComponentsWithParents(vec);

		return count;
	}

	template <class T, class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::vector<std::shared_ptr<T>> GetComponentsWithParents()
	{
		std::vector<std::shared_ptr<T>> vec;
		for (auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if (downCast != nullptr)
				vec.push_back(downCast);
		}

		auto parent = GetParent();
		if (parent != nullptr)
			parent->GetComponentsWithParents(vec);

		return vec;
	}
	template <class T,class = std::enable_if_t<std::is_base_of_v<Component,T>>>
	std::shared_ptr<T> GetComponentWithParents()
	{
		for(auto& component : _components)
		{
			std::shared_ptr<T> downCast = std::dynamic_pointer_cast<T>(component);
			if(downCast != nullptr)
				return downCast;
		}

		auto parent = GetParent();
		if(parent != nullptr)
			if(auto result = parent->GetComponentWithParents<T>())
				return result;
		return nullptr;
	}

	template <class T>
	void ForHierarchyAll(const T& func)
	{
		func(GetCast<GameObject>());
		for(auto& child : _childs)
			if(!child.expired())
				child.lock()->ForHierarchyAll(func);
	}

	template <class T, class Y>
	void ForHierarchyBeginEndAll(const T& func, const Y& func2)
	{
		auto obj = GetCast<GameObject>();
		auto result = func(obj);
		if (result) {
			for (auto& child : _childs)
				if (!child.expired())
					child.lock()->ForHierarchyBeginEndAll(func, func2);
		}
		func2(result, obj);
	}


private:
	bool AddChild(const std::shared_ptr<GameObject>& obj);
	bool RemoveChild(const std::shared_ptr<GameObject>& obj);
	bool RemoveAtChild(int index);
	void SetRootParent(const std::shared_ptr<GameObject>& rootParent);

public:
	bool SetParent(const std::shared_ptr<GameObject>& nextParentObj);
	bool IsInParent(const std::shared_ptr<GameObject>& obj);

	std::shared_ptr<GameObject> GetChild(int index);
	std::shared_ptr<GameObject> GetChildByName(const std::wstring& name);

	int GetChildAll(std::vector<std::shared_ptr<GameObject>>& vec);
	int GetChildsByName(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec);
	int GetChildsAll(std::vector<std::shared_ptr<GameObject>>& vec);
	int GetChildsAllByName(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec);


private:
	bool _active_self = true; // 
	bool _active_total_prev = false;
	bool _active_total = true;

public:
	bool GetActive(); //_active_total
	bool GetActiveSelf(); // _active_self
	bool SetActiveSelf(bool _active); //_active_self

private:
	void SyncActivePrev();
	void SetActivePrev(bool activeTotalPrev);
	bool CheckActiveUpdated();
	void ActiveUpdateChain(bool _active_total);

public:
	void SetName(const std::wstring name){_name = name;};
	std::wstring& GetName() { return _name; };

	std::shared_ptr<Scene> GetScene() { assert(!_scene.expired()); return _scene.lock(); };
	void SetScene(const std::shared_ptr<Scene>& scene) { _scene = scene; };

	std::vector<std::shared_ptr<Component>>& GetComponentAll() { return _components; }
	void SetDestroy() override;
	bool IsExecuteAble() override { return  IDelayDestroy::IsExecuteAble() && GetActive(); };


	void Debug();

	static void AddDestroyComponent(const std::shared_ptr<Component>& component);
	static void ExecuteDestroyComponents();

	std::shared_ptr<RendererBase> GetRenderer() { return _renderer; }
	std::shared_ptr<GameObject> GetParent() const {return parent.lock();};
	std::shared_ptr<GameObject> GetRoot() const { return rootParent.lock(); };
	bool IsRoot() const { return rootParent.lock().get() == this; };

	void AddTag(GameObjectTag tag);
	void RemoveTag(GameObjectTag tag);
	GameObjectTag GetTag() const { return  _tag; };
	bool HasTag(const GameObjectTag& tag2) const { return (static_cast<uint64>(_tag) & static_cast<uint64>(tag2)) != 0; };

	void SetType(const GameObjectType& type) { _type = type; };
	GameObjectType GetType() const { return  _type; };

	GameObjectTag _tag = GameObjectTag::NONE;
	GameObjectType _type = GameObjectType::Dynamic;

	std::shared_ptr<Transform> _transform;
	std::shared_ptr<RendererBase> _renderer;

private:
	std::wstring _name = L"none";
	std::weak_ptr<Scene> _scene;
	std::vector<std::shared_ptr<Component>> _components;

	std::weak_ptr<GameObject> parent;
	std::weak_ptr<GameObject> rootParent;
	std::vector<std::weak_ptr<GameObject>> _childs;

	static std::queue<std::shared_ptr<Component>> _componentDestroyQueue;

	friend class Transform;
};

