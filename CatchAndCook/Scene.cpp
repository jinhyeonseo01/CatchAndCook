#include "pch.h"
#include "Scene.h"

#include "Core.h"
#include "GameObject.h"
#include "RendererBase.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Camera.h"
#include "CameraManager.h"

void Scene::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	gameObject->SetScene(GetCast<Scene>());
    if (gameObject->GetType() == GameObjectType::Deactivate)
        _gameObjects_deactivate.push_back(gameObject);
    else
		_gameObjects.push_back(gameObject);
}

void Scene::Init()
{

}

void Scene::Update()
{
    for (auto& gameObject : _gameObjects)
    {
    	gameObject->Start();
	    gameObject->Update();
	}
    for (auto& gameObject : _gameObjects)
        gameObject->Update2();


    for (auto& ele : _passObjects)
    {
        ele.clear();
    }

}

void Scene::RenderBegin()
{
    for (auto& gameObject : _gameObjects)
    {
        gameObject->RenderBegin();
    }
}

void Scene::Rendering()
{
    auto& cmdList = Core::main->GetCmdList();

    CameraManager::main->SetActiveCamera(CameraType::ThirdPersonCamera);
    CameraManager::main->GetActiveCamera()->Update();
    CameraManager::main->GetActiveCamera()->PushData();
    CameraManager::main->GetActiveCamera()->SetData();

    _globalParam.window_size = vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
    _globalParam.Time = Time::main->GetTime();
    auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::GlobalParam)->Alloc(1);
    memcpy(CbufferContainer->ptr, (void*)&_globalParam, sizeof(GlobalParam));
    cmdList->SetGraphicsRootConstantBufferView(0, CbufferContainer->GPUAdress);

    { // Shadow
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

        for (auto& [material, mesh,target] : targets)
        {
            target->Rendering(nullptr, mesh);
        }
    }

    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deffered)];

        for (auto& [material, mesh, target] : targets)
        {
            Core::main->SetPipelineState(material->GetShader().get());
            Core::main->SetPipelineSetting(material);
            target->Rendering(material, mesh);
        }
    }

	{ // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];
        // �̰� �߰��� ������, Sorting�� �ѹ� �����°�
        // ���̴��� Set�Ǹ鼭 PipelineState�� ������ ���µǴ°� ������ ������尡 �� ũ�� ������.
        // ���⼭ Sorting�� �ؼ� ���ε��� ���ŵǴ� Ƚ���� ���̴°� ������ ���ϰ� �� ����.
        ranges::sort(targets, [&](const RenderObjectStrucutre& s1, const RenderObjectStrucutre& s2) {
            if (s1.material->GetShader()->_info._renderOrder != s2.material->GetShader()->_info._renderOrder)
                return (s1.material->GetShader()->_info._renderOrder < s2.material->GetShader()->_info._renderOrder);
            if (s1.material->GetShader().get() != s2.material->GetShader().get())
                return s1.material->GetShader().get() < s2.material->GetShader().get();
            if (s1.mesh != s2.mesh)
                return s1.mesh < s2.mesh;
			return true;
            });

        for (auto& [material, mesh, target] : targets)
        {
            Core::main->SetPipelineState(material->GetShader().get());
            Core::main->SetPipelineSetting(material);
            target->Rendering(material, mesh);
        }
	}

    { // CopyTexture
        auto& cmd = Core::main->GetCmdList();
        //cmd->CopyResource()
    }

    { // Transparent
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Transparent)];
        //���⼱ Mesh���� Camera Z��� Sorting�� �ʿ���.
        ranges::sort(targets, [&](const RenderObjectStrucutre& s1, const RenderObjectStrucutre& s2) {
            if (s1.material->GetShader()->_info._renderOrder != s2.material->GetShader()->_info._renderOrder)
                return (s1.material->GetShader()->_info._renderOrder < s2.material->GetShader()->_info._renderOrder);
            if (s1.material->GetShader().get() != s2.material->GetShader().get())
                return s1.material->GetShader().get() < s2.material->GetShader().get();
            return true;
            });

        for (auto& [material, mesh, target] : targets)
        {
            Core::main->SetPipelineState(material->GetShader().get());
            Core::main->SetPipelineSetting(material);
            target->Rendering(material, mesh);
        }
    }

    {  //UI
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI)];

        for (auto& [material, mesh, target] : targets)
        {
            target->Rendering(nullptr, mesh);
        }
    }
}

void Scene::DebugRendering()
{
    { // Shadow
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

        for (auto& [material, mesh, target] : targets)
        {
            target->DebugRendering();
        }
    }

    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deffered)];

        for (auto& [material, mesh, target] : targets)
        {
            target->DebugRendering();
        }
    }

    { // forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

        for (auto& [material, mesh, target] : targets)
        {
            target->DebugRendering();
        }
    }
}

void Scene::RenderEnd()
{
    if (Input::main->GetKeyDown(KeyCode::P))
    {
        auto ptr = Find(L"root_test");

        ptr->SetDestroy();
    }
}

void Scene::Finish()
{
    Scene::ExecuteDestroyGameObjects();
    GameObject::ExecuteDestroyComponents();

}

void Scene::ExecuteDestroyGameObjects()
{
    while (_destroyQueue.empty() == false)
    {
        auto& gameObject = _destroyQueue.front();
        _destroyQueue.pop();

        auto it = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);

        if (it != _gameObjects.end())
        {
            gameObject->Destroy();
            _gameObjects.erase(it);
        }
        else
        {
            it = std::find(_gameObjects_deactivate.begin(), _gameObjects_deactivate.end(), gameObject);
            if (it != _gameObjects_deactivate.end())
            {
                gameObject->Destroy();
                _gameObjects_deactivate.erase(it);
            }
        }
    }
}


std::shared_ptr<GameObject> Scene::CreateGameObject(const std::wstring& name, GameObjectType type)
{
	auto gameObject = std::make_shared<GameObject>(name);
    gameObject->SetType(type);
	AddGameObject(gameObject);
	gameObject->Init();
	return gameObject;
}


bool Scene::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject)
{
    auto iter = std::find(this->_gameObjects.begin(), this->_gameObjects.end(), gameObject);
    if (iter != this->_gameObjects.end()) {
        this->_gameObjects.erase(iter);
        return true;
    }
    return false;
}

bool Scene::RemoveAtGameObject(int index)
{
    if (index >= 0 && index < this->_gameObjects.size()) {
        this->_gameObjects.erase(this->_gameObjects.begin() + index);
        return true;
    }
    return false;
}

void Scene::AddDestroyQueue(const std::shared_ptr<GameObject>& gameObject)
{
	_destroyQueue.push(gameObject);
}




std::shared_ptr<GameObject> Scene::Find(const std::wstring& name, bool includeDestroy)
{
    auto iter = std::find_if(this->_gameObjects.begin(), this->_gameObjects.end(),
        [&](const std::shared_ptr<GameObject>& element)
        {
            if (!includeDestroy && element->IsDestroy())
                return false;
            return element->GetName() == name;
        });
    if (iter != this->_gameObjects.end())
		return *iter;

    iter = std::find_if(this->_gameObjects_deactivate.begin(), this->_gameObjects_deactivate.end(),
        [&](const std::shared_ptr<GameObject>& element)
        {
            if (!includeDestroy && element->IsDestroy())
                return false;
            return element->GetName() == name;
        });
    if (iter != this->_gameObjects_deactivate.end())
        return *iter;

    return nullptr;
}

int Scene::Finds(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy)
{
    int startSize = vec.size();
    for (int i = 0; i < this->_gameObjects.size(); i++)
    {
        auto& current = this->_gameObjects[i];
        if (!includeDestroy && current->IsDestroy())
            continue;
        if (current->GetName() == name)
            vec.push_back(current);
    }
    for (int i = 0; i < this->_gameObjects_deactivate.size(); i++)
    {
        auto& current = this->_gameObjects_deactivate[i];
        if (!includeDestroy && current->IsDestroy())
            continue;
        if (current->GetName() == name)
            vec.push_back(current);
    }
    return vec.size() - startSize;
}

void Scene::AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
    {
        if (RENDER_PASS::HasFlag(material->GetPass(), RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i].emplace_back(material, mesh, renderBase);

        }
    }
}

void Scene::AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
        if (RENDER_PASS::HasFlag(pass, RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i].emplace_back(nullptr, mesh, renderBase);
        }
}

void Scene::Release()
{

}
