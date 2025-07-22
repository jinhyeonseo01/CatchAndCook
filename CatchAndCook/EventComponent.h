#pragma once
#include "Component.h"

class EventComponent :public Component
{
public:

	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void RenderEnd();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene);
	virtual void SetDestroy() override;
	virtual void Destroy();

public:
	template<typename T>
	void SetSharedState(const string& key, const std::shared_ptr<T>& value)
	{
		_sharedStates[key] = value;
	}

	template<typename T>
	std::shared_ptr<T> GetSharedState(const string& key)
	{
		auto it = _sharedStates.find(key);
		if (it != _sharedStates.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}
		return nullptr;
	}


public:
	void SetOneTimeEventCall(const bool& call) { _oneTimeEventCall = call; }
public:
	void ShowEventMessage(bool show);
	void SetBindTag(GameObjectTag tag) { bindTag = tag; }
	void SetBindMessage(const wstring& message, const vec3& pos , const vec2& size , bool active);
	void BindOnUpdateBlock(const std::function<void(std::shared_ptr<Collider>)>& func) { _OnUpdateBlock.push_back(func); }
	void BindOnUpdateAlways(const std::function<void(std::shared_ptr<Collider>)>& func) { _OnUpdateAlways.push_back(func); }
	void BindOnCollisionBegin(const std::function<void(std::shared_ptr<Collider>)>& func) { _OnCollisionBegins.push_back(func); }
	void BindOnCollisionEnd(const std::function<void(std::shared_ptr<Collider>)>& func) { _OnCollisionEnds.push_back(func); }

private:
	std::unordered_map<string, std::shared_ptr<void>> _sharedStates;

	vector<std::function<void(shared_ptr<Collider>)>> _OnUpdateBlock;
	vector<std::function<void(shared_ptr<Collider>)>> _OnUpdateAlways;
	vector<std::function<void(shared_ptr<Collider>)>> _OnCollisionBegins;
	vector<std::function<void(shared_ptr<Collider>)>> _OnCollisionEnds;

	weak_ptr<GameObject> _messageObject;
	shared_ptr<Collider> _otherCashing;

	GameObjectTag bindTag = GameObjectTag::NONE;

	bool _Oncollision=false;
	bool _oneTimeEventCall = true;
};

