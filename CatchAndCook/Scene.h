#pragma once

#include "GameObjectSetting.h"
#include "IGuid.h"
#include "RendererBase.h"
#include "SceneType.h"
class ParticleComponent;
class RendererBase;
class GameObject;

struct GlobalParam
{
	vec2 window_size;
	float Time;
	float SkyBlend = 0.2;
	float caustics = 0;
	float dt, p2, p3 = 0;
};

class Scene : public IGuid
{

public:
	void AddFrontGameObject(const std::shared_ptr<GameObject>& gameObject);
	void AddGameObject(const std::shared_ptr<GameObject>& gameObject);
	bool RemoveGameObject(const std::shared_ptr<GameObject>& gameObject);
	bool RemoveAtGameObject(int index);
	void CameraControl();

	void ExecuteDestroyGameObjects();
	void GlobalSetting();

	GlobalParam& GetGlobalParam() { return _globalParam; }

public:
	virtual void UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	virtual void TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	virtual void ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	virtual void DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	virtual void ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	virtual void FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);
	virtual void ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> & cmdList);

public:
	virtual void SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass);

public:
	void SetName(const std::string& name) { _name = name; };

	virtual void Init();
	virtual void Update();
	virtual void RenderBegin();
	virtual void Rendering();
	virtual void DebugRendering();
	virtual void RenderEnd();
	virtual void Finish();


	std::shared_ptr<GameObject> CreateGameObject(const std::wstring& name,GameObjectType type = GameObjectType::Dynamic);
	std::shared_ptr<GameObject> Find(const std::wstring& name, bool includeDestroy = false);
	int Finds(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy = false);
	int FindsInclude(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy = false);
	void AddChangeTypeQueue(const std::shared_ptr<GameObject>& gmaeObject, GameObjectType type);
	void AddStartQueue(const std::shared_ptr<GameObject>& gameObject);
	void AddDestroyQueue(const std::shared_ptr<GameObject>& gameObject);

	void AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase);
	void AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass);
	SceneType& GetSceneType() { return _type; }

	static bool _changeScene;

	void Release();
	friend class SceneManager;
	friend class SceneLoader;

public:
	std::array<unordered_map<shared_ptr<Shader>,std::vector<RenderObjectStrucutre>>, RENDER_PASS::Count> _passObjects;
	std::vector<std::shared_ptr<GameObject>> _dont_destroy_gameObjects;

public:
	std::vector<std::shared_ptr<GameObject>> _gameObjects; 
	std::vector<std::shared_ptr<GameObject>> _gameObjects_deactivate;

	std::queue<std::shared_ptr<GameObject>> _destroyQueue;
	std::queue<std::shared_ptr<Component>> _destroyComponentQueue;
	std::queue<std::shared_ptr<GameObject>> _startQueue;
	std::queue<std::pair<std::shared_ptr<GameObject>, GameObjectType>> _changeTypeQueue;


	std::string _name;
	SceneType _type;

	GlobalParam _globalParam;
	std::shared_ptr<Material> _finalShader;
};



