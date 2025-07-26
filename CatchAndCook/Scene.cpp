#include "pch.h"
#include "Scene.h"
#include "Core.h"
#include "GameObject.h"
#include "RendererBase.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Camera.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "Mesh.h"
#include "InstancingManager.h"
#include "Profiler.h"
#include "ComputeManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "InGameGlobal.h"
#include "LightManager.h"
#include "MeshRenderer.h"
#include "PathFinder.h"
#include "ShadowManager.h"
#include "TerrainManager.h"
#include "Volumetric.h"

bool Scene::_changeScene = false;
SceneType Scene::_changeSceneType = SceneType::Sea01;

void Scene::AddFrontGameObject(const std::shared_ptr<GameObject>& gameObject)
{
    gameObject->SetScene(GetCast<Scene>());

    if (gameObject->GetType() == GameObjectType::Deactivate)
        _gameObjects_deactivate.insert(_gameObjects_deactivate.begin(), gameObject);
    else
        _gameObjects.insert(_gameObjects.begin(),gameObject);
}
void Scene::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	gameObject->SetScene(GetCast<Scene>());

    if (gameObject->GetType() == GameObjectType::Deactivate)
    {
        if (std::ranges::find(_gameObjects_deactivate, gameObject) == _gameObjects_deactivate.end())
			_gameObjects_deactivate.push_back(gameObject);
    }
    else
    {
        if (std::ranges::find(_gameObjects, gameObject) == _gameObjects.end())
			_gameObjects.push_back(gameObject);
    }
}

void Scene::Init()
{
	_finalShader = std::make_shared<Material>();
	_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader"));
    _finalShader->SetPass(RENDER_PASS::Forward);


}

void Scene::Update()
{
   
    Profiler::Set("Logic_Start");
    while (!_changeTypeQueue.empty()) 
    {
        auto& current = _changeTypeQueue.front();
        {
            RemoveGameObject(current.first);
            current.first->SetType(current.second);
            AddGameObject(current.first);
        }
        _changeTypeQueue.pop();
    }

    while (!_startQueue.empty()) 
    {
        auto& current = _startQueue.front();
        current->Start();
        _startQueue.pop();
    }

    Profiler::Fin();

    Profiler::Set("Logic_Update1");
    for (auto& gameObject : _gameObjects)
	    if(gameObject->GetType() == GameObjectType::Dynamic)
            gameObject->Update();
    Profiler::Fin();

    Profiler::Set("Logic_Update2");
    for (auto& gameObject : _gameObjects)
        if(gameObject->GetType() == GameObjectType::Dynamic)
            gameObject->Update2();
    Profiler::Fin();

    Profiler::Set("Logic_CollisionCheck");
    ColliderManager::main->Update();
    Profiler::Fin();
}

void Scene::RenderBegin()
{
    for (auto& ele : _passObjects)
        ele.clear();

    Profiler::Set("Logic_RenderBegin");
    for (auto& gameObject : _gameObjects)
    	gameObject->RenderBegin();
    Profiler::Fin();

    Gizmo::main->RenderBegin();
}

void Scene::Rendering()
{
    GlobalSetting();

    _globalParam.caustics = 0;

#ifdef _DEBUG
    //Gizmo::Width(0.5);
    //Gizmo::Frustum(CameraManager::main->GetCamera(CameraType::ComponentCamera)->_boundingFrsutum);
    //Gizmo::WidthRollBack();
    Light l;
    l.direction = Vector3(1, -1, 1);
    l.direction.Normalize();
    
    //auto a2 = ShadowManager::main->GetFrustums(CameraManager::main->GetCamera(CameraType::ComponentCamera).get(), &l, { 8,30,75,200 });
    //for (auto b2 : a2)
    //{
    //    Gizmo::Width(0.1);
    //    Gizmo::Frustum(b2, Vector4(1, 0, 0, 1));
    //    Gizmo::WidthRollBack();
    //}

    auto light = LightManager::main->GetMainLight();
    auto a = ShadowManager::main->CalculateBounds(CameraManager::main->GetCamera(CameraType::ComponentCamera).get(), light.get(), { 6, 20, 65, 200 });
    for (auto b : a)
    {
        Gizmo::Width(0.5);
        Gizmo::Box(b, Vector4(0,1,0,1));
        Gizmo::WidthRollBack();
    }
#endif

    auto& cmdList = Core::main->GetCmdList();
    Core::main->GetRenderTarget()->ClearDepth();

    Profiler::Set("PASS : Shadow", BlockTag::CPU);
		ShadowPass(cmdList);
    Profiler::Fin();

    Profiler::Set("PASS : Deferred", BlockTag::CPU);
        DeferredPass(cmdList);
    Profiler::Fin();

    Profiler::Set("PASS : FinalPass", BlockTag::CPU);
        FinalRender(cmdList);
    Profiler::Fin();

    Core::main->CopyTexture(Core::main->GetDSReadTexture(), Core::main->GetRenderTarget()->GetDSTexture());
    Core::main->GetDSReadTexture()->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

    ComputeManager::main->DispatchAfterDeferred(cmdList);

    Profiler::Set("PASS : Forward", BlockTag::CPU);
    ForwardPass(cmdList);
    Profiler::Fin();

    Core::main->CopyTexture(Core::main->GetDSReadTexture(), Core::main->GetRenderTarget()->GetDSTexture());
    Core::main->GetDSReadTexture()->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

    Core::main->CopyTexture(Core::main->GetRTReadTexture(), Core::main->GetRenderTarget()->GetRenderTarget());
    Core::main->GetRTReadTexture()->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);


    Profiler::Set("PASS : Transparent", BlockTag::CPU);
        TransparentPass(cmdList); // Position,
    Profiler::Fin();



    Profiler::Set("PASS : Compute", BlockTag::CPU);
        ComputePass(cmdList);
    Profiler::Fin();

    Profiler::Set("PASS : TransparentAfterPP", BlockTag::CPU);
    TransparentAfterPPPass(cmdList); // Position,
    Profiler::Fin();

    Profiler::Set("PASS : UI", BlockTag::CPU);
        UiPass(cmdList);
    Profiler::Fin();

    Profiler::Set("PASS : UI2", BlockTag::CPU);
        Ui2Pass(cmdList);
    Profiler::Fin();

    Profiler::Set("PASS : PariclePass", BlockTag::CPU);
    ParticlePass(cmdList);
    Profiler::Fin();

    ComputeManager::main->ChangeSceneDispatch();



}


void Scene::TransparentPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
    { // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Transparent)];

        std::vector<RenderObjectStrucutre> vec;
        vec.reserve(2048);
        for (auto& [shader, vec2] : targets)
            vec.insert(vec.end(), vec2.begin(), vec2.end());

        Vector3 cameraPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
        Vector3 cameraDir = CameraManager::main->GetActiveCamera()->GetCameraLook();

        auto tangentDistanceSquared = [&](const Vector3& center) -> float {
            Vector3 offset = center - cameraPos;
            float projection = offset.Dot(cameraDir);
            return offset.LengthSquared() - projection * projection;
            };

        std::ranges::sort(vec, [&](const RenderObjectStrucutre& a, const RenderObjectStrucutre& b) {
            return tangentDistanceSquared(a.renderer->_bound.Center) < tangentDistanceSquared(b.renderer->_bound.Center);
            });

        Shader* prevShader = nullptr;
        for (auto& ele : vec)
        {
            Shader* shader = ele.material->GetShader().get();
            if (shader != nullptr && shader != prevShader)
                cmdList->SetPipelineState(shader->_pipelineState.Get());

            g_debug_forward_count++;

            if (ele.renderer->IsCulling() == true)
            {
                if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
                {
                    g_debug_forward_culling_count++;
                    continue;
                }
            }

            SettingPrevData(ele, RENDER_PASS::PASS::Transparent);
            InstancingManager::main->RenderNoInstancing(ele);

            prevShader = shader;
        }
    }

 
}

void Scene::TransparentAfterPPPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    { // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::TransparentAfterPP)];

        std::vector<RenderObjectStrucutre> vec;
        vec.reserve(2048);
        for (auto& [shader, vec2] : targets)
            vec.insert(vec.end(), vec2.begin(), vec2.end());

        Vector3 cameraPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
        Vector3 cameraDir = CameraManager::main->GetActiveCamera()->GetCameraLook();

        auto tangentDistanceSquared = [&](const Vector3& center) -> float {
            Vector3 offset = center - cameraPos;
            float projection = offset.Dot(cameraDir);
            return offset.LengthSquared() - projection * projection;
            };

        std::ranges::sort(vec, [&](const RenderObjectStrucutre& a, const RenderObjectStrucutre& b) {
            if (a.renderer->order == b.renderer->order)
                return (tangentDistanceSquared(a.renderer->GetBound().Center) < tangentDistanceSquared(b.renderer->GetBound().Center));
            return a.renderer->order < b.renderer->order;
            });

        Shader* prevShader = nullptr;
        for (auto& ele : vec)
        {
            Shader* shader = ele.material->GetShader().get();
            if (shader != nullptr && shader != prevShader)
                cmdList->SetPipelineState(shader->_pipelineState.Get());

            g_debug_forward_count++;

            if (ele.renderer->IsCulling() == true)
            {
                if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
                {
                    g_debug_forward_culling_count++;
                    continue;
                }
            }

            SettingPrevData(ele, RENDER_PASS::PASS::Transparent);
            InstancingManager::main->RenderNoInstancing(ele);

            prevShader = shader;
        }
    }
}

void Scene::ForwardPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{

    { // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

        for(auto& [shader,vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());

            for(auto& ele : vec)
            {
                g_debug_forward_count++;

                if(ele.renderer->IsCulling() == true)
                {
                    if(CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound())==false)
                    {
                        g_debug_forward_culling_count++;
                        continue;
                    }
                }

                SettingPrevData(ele, RENDER_PASS::PASS::Forward);

                if (ele.renderer->isInstancing() == false)
                {
                    InstancingManager::main->RenderNoInstancing(ele);
                }
                else
                {
                    InstancingManager::main->AddObject(ele);
                }
            }

            InstancingManager::main->Render();
        }

    }
}

void Scene::DeferredPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
	Core::main->GetGBuffer()->RenderBegin();
    auto camera = CameraManager::main->GetActiveCamera();

    TerrainManager::main->CullingInstancing(camera->GetCameraPos(), camera->GetCameraLook());

    { // Deferred
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deferred)];

        for (auto& [shader, vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());
         
            for (auto& ele : vec)
            {
                g_debug_deferred_count++;

                   if(ele.renderer->IsCulling() == true)
                   {
                       if(CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound())==false)
                       {
                           g_debug_deferred_culling_count++;
                           continue;
                       }
                   }

                   SettingPrevData(ele, RENDER_PASS::PASS::Deferred);

                   if (ele.renderer->isInstancing() == false)
                   {
                       InstancingManager::main->RenderNoInstancing(ele);
                   }
                   else
                   {  
                       InstancingManager::main->AddObject(ele);
                   }           
            }
        
           InstancingManager::main->Render();

        }
    }

    Core::main->GetGBuffer()->RenderEnd();
}

void Scene::ShadowPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
    { // Shadow

        if (!ShadowManager::main->_onOff)
            return;

        auto light = LightManager::main->GetMainLight();
		if (light == nullptr)
			return;
        if (light->intensity <= 0.05)
            return;

        auto boundings = ShadowManager::main->CalculateBounds(CameraManager::main->GetActiveCamera().get(), light.get(), { 6, 20, 65, 200 });

    	auto lastShadowPos = ShadowManager::main->_lightTransform[ShadowManager::main->_lightTransform.size() - 1];

        Profiler::Set("PASS : Shadow Culling", BlockTag::CPU);
    	TerrainManager::main->CullingInstancing(lastShadowPos.first, lastShadowPos.second);
        Profiler::Fin();

        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

		ShadowManager::main->SetData(nullptr);
        ShadowManager::main->RenderBegin();

        int i = 0;

        for (auto& bounding : boundings)
        {
            ShadowCascadeIndexParams shadowCasterParams;

            auto* cbuffer2 = Core::main->GetBufferManager()->GetBufferPool(BufferType::ShadowCascadeIndexParams)->Alloc(1);
            shadowCasterParams.cascadeIndex = i;
            memcpy(cbuffer2->ptr, &shadowCasterParams, sizeof(ShadowCascadeIndexParams));
            Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(7, cbuffer2->GPUAdress);
            Core::main->GetShadowBuffer()->RenderBegin(i);

            for (auto& [shader, vec] : targets)
            {
                cmdList->SetPipelineState(shader->_pipelineState.Get());

                for (auto& renderStructure : vec)
                {

                    if (renderStructure.renderer->IsCulling() == true)
                        if (bounding.Intersects(renderStructure.renderer->GetBound()) == false)
                            continue;

                    SettingPrevData(renderStructure, RENDER_PASS::PASS::Shadow);

                    if (renderStructure.renderer->isInstancing() == false)
                    {
                        g_debug_shadow_draw_call++;
                        InstancingManager::main->RenderNoInstancing(renderStructure);
                    }
                    else
                    {
                        g_debug_shadow_draw_call++;
                        InstancingManager::main->AddObject(renderStructure);
                    }
                }
                InstancingManager::main->Render();
            }
            Core::main->GetShadowBuffer()->RenderEnd();
            i++;
        }

        ShadowManager::main->RenderEnd();
    }

}

void Scene::UiPass(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    {
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI)];

        for (auto& [shader, vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());

            for (auto& renderStructure : vec)
            {
                auto& [material, mesh, target] = renderStructure;
                SettingPrevData(renderStructure, RENDER_PASS::PASS::UI);

                cmdList->OMSetBlendFactor(material->GetBlendFactor().data());

                target->Rendering(material, mesh);
            }
        }
    }
}

void Scene::Ui2Pass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    { // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI2)];

        std::vector<RenderObjectStrucutre> vec;
        vec.reserve(2048);
        for (auto& [shader, vec2] : targets)
            vec.insert(vec.end(), vec2.begin(), vec2.end());

        Vector3 cameraPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
        Vector3 cameraDir = CameraManager::main->GetActiveCamera()->GetCameraLook();

        auto tangentDistanceSquared = [&](const Vector3& center) -> float {
            Vector3 offset = center - cameraPos;
            float projection = offset.Dot(cameraDir);
            return offset.LengthSquared() - projection * projection;
            };

        std::ranges::sort(vec, [&](const RenderObjectStrucutre& a, const RenderObjectStrucutre& b) {
            if (a.renderer->order == b.renderer->order)
                return (tangentDistanceSquared(a.renderer->GetBound().Center) < tangentDistanceSquared(b.renderer->GetBound().Center));
            return a.renderer->order < b.renderer->order;
            });

        Shader* prevShader = nullptr;
        for (auto& ele : vec)
        {
            Shader* shader = ele.material->GetShader().get();
            if (shader != nullptr && shader != prevShader)
                cmdList->SetPipelineState(shader->_pipelineState.Get());

            g_debug_forward_count++;

            if (ele.renderer->IsCulling() == true)
            {
                if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
                {
                    g_debug_forward_culling_count++;
                    continue;
                }
            }

            SettingPrevData(ele, RENDER_PASS::PASS::UI2);
            InstancingManager::main->RenderNoInstancing(ele);

            prevShader = shader;
        }
    }

};

void Scene::FinalRender(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    Core::main->GetRenderTarget()->RenderBegin();

    auto mesh = ResourceManager::main->Get<Mesh>(L"finalMesh");
    auto shader = _finalShader->GetShader();

    cmdList->SetPipelineState(shader->_pipelineState.Get());

    RenderObjectStrucutre ROS = { _finalShader.get(), mesh.get(), nullptr };
    SettingPrevData(ROS, RENDER_PASS::PASS::Deferred);

    _finalShader->SetData();
    mesh->Redner();
}

void Scene::ComputePass(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    ComputeManager::main->Dispatch(cmdList);
}

void Scene::ParticlePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::ParticlePass)];

    for (auto& [shader, vec] : targets)
    {
        for (auto& ele : vec)
        {
            ele.renderer->Rendering(nullptr, nullptr, 1);
        }
    }
}


void Scene::GlobalSetting()
{
    auto& cmdList = Core::main->GetCmdList();


    CameraControl();
    //cout << CameraManager::main->GetActiveCamera()->GetCameraPos().y << endl;

    _globalParam.window_size = vec2(WINDOW_WIDTH,WINDOW_HEIGHT);
    _globalParam.Time = Time::main->GetTime();
    _globalParam.dt = Time::main->GetDeltaTimeNow();

    auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::GlobalParam)->Alloc(1);
    memcpy(CbufferContainer->ptr,(void*)&_globalParam,sizeof(GlobalParam));

    cmdList->SetGraphicsRootConstantBufferView(0,CbufferContainer->GPUAdress);
    cmdList->SetComputeRootConstantBufferView(0, CbufferContainer->GPUAdress);

    auto& table = Core::main->GetBufferManager()->GetTable();
    TableContainer container = Core::main->GetBufferManager()->GetTable()->Alloc(1);
    table->CopyHandle(container.CPUHandle, Core::main->GetDSReadTexture()->GetSRVCpuHandle(), 0);
    cmdList->SetGraphicsRootDescriptorTable(GLOBAL_SRV_DEPTH_INDEX, container.GPUHandle);

    Material::AllocDefualtTextureHandle();
}

void Scene::SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass)
{
    if (data.material != nullptr)
    {
        if (ShadowManager::main->_bakedGIOnOff)
        {
            data.material->SetTexture("_BakedGIMap", ResourceManager::main->_bakedGIFinal1Texture);
            data.material->SetTexture("_BakedGIMap2", ResourceManager::main->_bakedGIFinal2Texture);
        }
        else
        {
            data.material->SetTexture("_BakedGIMap", ResourceManager::main->_noneTexture_Black);
            data.material->SetTexture("_BakedGIMap2", ResourceManager::main->_noneTexture_Black);
        }
    }

    switch (pass)
    {
    case RENDER_PASS::Transparent:
	    {
        if (data.material != nullptr)
			data.material->SetTexture("_ColorTexture", Core::main->GetRTReadTexture());
        break;
	    }
    }
}

void Scene::DebugRendering()
{
    if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::RenderPreview))
    {
        auto pos = CameraManager::main->GetActiveCamera()->GetCameraPos();
        auto lock = CameraManager::main->GetActiveCamera()->GetCameraLook();
        auto right = CameraManager::main->GetActiveCamera()->GetCameraRight();
        auto up = CameraManager::main->GetActiveCamera()->GetCameraUp();
        auto size = Vector3::One * 0.2f;

        Gizmo::Image(Core::main->GetGBuffer()->GetTexture(0),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 0) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetGBuffer()->GetTexture(1),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 1) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetGBuffer()->GetTexture(2),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 2) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetGBuffer()->GetTexture(3),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 3) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        if (Volumetric::main)
        {
            Gizmo::Image(Volumetric::main->GetTexture(),
                pos + lock + right * (-0.8 + (size.x + 0.02) * 4) + up * (0.4 + (size.y + 0.02) * 0),
                -lock, Vector3::Up, size);
        }
        Gizmo::Image(ComputeManager::main->_pongTexture,
            pos + lock + right * (-0.8 + (size.x + 0.02) * 5) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        Gizmo::Image(ComputeManager::main->_ssaoRender->_ssaoTexture,
            pos + lock + right * (-0.8 + (size.x + 0.02) * 6) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        Gizmo::Image(ComputeManager::main->_pingTexture,
            pos + lock + right * (-0.8 + (size.x + 0.02) * 7) + up * (0.4 + (size.y + 0.02) * 0),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetShadowBuffer()->GetDSTexture(0),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 0) + up * (0.4 - (size.y + 0.02) * 1),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetShadowBuffer()->GetDSTexture(1),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 1) + up * (0.4 - (size.y + 0.02) * 1),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetShadowBuffer()->GetDSTexture(2),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 2) + up * (0.4 - (size.y + 0.02) * 1),
            -lock, Vector3::Up, size);
        Gizmo::Image(Core::main->GetShadowBuffer()->GetDSTexture(3),
            pos + lock + right * (-0.8 + (size.x + 0.02) * 3) + up * (0.4 - (size.y + 0.02) * 1),
            -lock, Vector3::Up, size);
    }

    auto& cmdList = Core::main->GetCmdList();

    { // forward
    	auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Debug)];

        for(auto& [shader,vec] : targets)
        {
            for(auto& [material,mesh,target] : vec)
            {
                target->Rendering(material,mesh);
            }
        }
    }

    Gizmo::main->Clear();
}

void Scene::RenderEnd()
{
    Profiler::Set("Logic_RenderEnd");
    for (auto& gameObject : _gameObjects)
        gameObject->RenderEnd();
    Profiler::Fin();
}

void Scene::Finish()
{
    if (Scene::_changeScene)
    {
        for (auto& ele : _gameObjects)
        {
            ele->Reset();
        }
    }

    Scene::ExecuteDestroyGameObjects();
    PathFinder::ClearDebugDraw();
    GameObject::ExecuteDestroyComponents();
    
   
} 

void Scene::ExecuteDestroyGameObjects()
{
    while (_destroyQueue.empty() == false)
    {
        auto& gameObject = _destroyQueue.front();
        _destroyQueue.pop();

        std::vector<std::shared_ptr<GameObject>> vec;
        gameObject->GetChildAll(vec);

        for (auto& ele : vec)
        {
            auto it = std::find(_gameObjects.begin(), _gameObjects.end(), ele);

            if (it != _gameObjects.end())
            {
                ele->Destroy();
                _gameObjects.erase(it);
            }

            else
            {
                it = std::find(_gameObjects_deactivate.begin(), _gameObjects_deactivate.end(), ele);
                if (it != _gameObjects_deactivate.end())
                {
                    ele->Destroy();
                    _gameObjects_deactivate.erase(it);
                }
            }
        }

        {
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
}





std::shared_ptr<GameObject> Scene::CreateGameObject(const std::wstring& name, GameObjectType type)
{
	auto gameObject = std::make_shared<GameObject>(name);
    gameObject->InitGuid();
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

void Scene::CameraControl()
{
    static CameraType type = CameraType::DebugCamera;

    if (Input::main->GetKeyDown(KeyCode::F1))
    {
        type = CameraType::DebugCamera;
        CameraManager::main->Setting(type);
    }

    if (Input::main->GetKeyDown(KeyCode::F2))
    {
        type = CameraType::ComponentCamera;
        CameraManager::main->Setting(type);
    }

    if (Input::main->GetKeyDown(KeyCode::F3))
    {
        type = CameraType::SeaCamera;
        CameraManager::main->Setting(type);
    }

    if (Input::main->GetKeyDown(KeyCode::F4))
    {
        type = CameraType::BoatCamera;
        CameraManager::main->Setting(type);
    }

 
    CameraManager::main->Setting();
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

int Scene::FindsInclude(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy)
{
    int startSize = vec.size();
    for (auto& current : _gameObjects)
    {
        if ((!includeDestroy && current->IsDestroy()) ||
            current->GetName().find(name) == std::wstring::npos)
            continue;
        vec.push_back(current);
    }
    for (auto& current : _gameObjects_deactivate)
    {
        if ((!includeDestroy && current->IsDestroy()) ||
            current->GetName().find(name) == std::wstring::npos)
            continue;
        vec.push_back(current);
    }
    return static_cast<int>(vec.size() - startSize);
}

void Scene::AddChangeTypeQueue(const std::shared_ptr<GameObject>& gameObject, GameObjectType type)
{
    _changeTypeQueue.push(std::make_pair(gameObject, type));
}

void Scene::AddStartQueue(const std::shared_ptr<GameObject>& gameObject)
{
    _startQueue.push(gameObject);
}

void Scene::AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
    {
        if (RENDER_PASS::HasFlag(material->GetPass(), RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i][material->GetShader()].emplace_back(material,mesh,renderBase);

        }
    }
}

void Scene::AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
        if (RENDER_PASS::HasFlag(pass, RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i][nullptr].emplace_back(nullptr,mesh,renderBase);
        }
}

void Scene::Release()
{
    for(auto& obj : _gameObjects)
        obj->SetDestroy();
    for(auto& obj : _gameObjects_deactivate)
        obj->SetDestroy();
    Scene::ExecuteDestroyGameObjects();
    GameObject::ExecuteDestroyComponents();

    Core::main->FenceCurrentFrame();

    for(auto pass : _passObjects)
        pass.clear();
    _dont_destroy_gameObjects.clear();
    _gameObjects.clear();
    _gameObjects_deactivate.clear();
    while(!_destroyQueue.empty()) _destroyQueue.pop();
    while(!_destroyComponentQueue.empty()) _destroyComponentQueue.pop();
    while(!_startQueue.empty()) _startQueue.pop();
    while(!_changeTypeQueue.empty()) _changeTypeQueue.pop();

    ColliderManager::main->Clear();
    LightManager::main->Clear();
}

void Scene::SetDontDestroy(std::shared_ptr<GameObject> obj)
{
    _dont_destroy_gameObjects.push_back(obj);
}
