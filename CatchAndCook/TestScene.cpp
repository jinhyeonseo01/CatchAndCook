#include "pch.h"
#include "TestScene.h"
#include "Camera.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "MeshRenderer.h"
#include "testComponent.h"
#include "Transform.h"
#include "Mesh.h"
#include "WaterController.h"
#include "Collider.h"
#include "ComputeManager.h"
#include "PlayerController.h"
#include "testComponent.h"
#include "LightManager.h"
#include "Profiler.h"
#include "Volumetric.h"

void TestScene::Init()
{
	Scene::Init();

	Volumetric::main = make_unique<Volumetric>();
	Volumetric::main->Init();


	_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader_MainField"));
	_finalShader->SetPass(RENDER_PASS::Forward);

	ColliderManager::main->SetCellSize(5);


	std::shared_ptr<Light> light = std::make_shared<Light>();
	light->onOff = 1;
	light->direction = vec3(-0.122f, -0.732f, 0.299f);
	light->position = vec3(0, 1000.0f, 0);
	light->direction.Normalize();

	light->material.ambient = vec3(0.4f, 0.4f, 0.4f);
	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light->material.specular = vec3(0, 0, 0);
	light->material.shininess = 32.0f;
	light->material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	light->strength = vec3(1.0f, 1.0f, 1.0f);
	LightManager::main->PushLight(light);
	LightManager::main->_lightParmas.mainLight = *light.get();

	CameraManager::main->GetCamera(CameraType::DebugCamera)->SetCameraLook(vec3(0.316199, 0.743145, -0.589706));
	CameraManager::main->GetCamera(CameraType::DebugCamera)->SetCameraPos(vec3(245.946, 79.8085, 225.333));



	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/Sky_0.png.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();


		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("_BaseMap", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
		meshRenderer->SetCulling(false);
	}



	random_device urd;
	mt19937 gen(urd());
	uniform_real_distribution<float> ddis(-1000.0f, 1000.0f);
	uniform_real_distribution<float> dis(1, 30.0f);
	uniform_real_distribution<float> rotate(-360.0f, 360.0f);


	shared_ptr<Material> materialO = make_shared<Material>();
	materialO->SetPass(RENDER_PASS::Deferred);
	shared_ptr<Mesh> mesh = GeoMetryHelper::LoadRectangleBox(1.0f);

	ShaderInfo info;
	info.renderTargetCount = 4;
	info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;
	info.RTVForamts[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;

	shared_ptr<Shader> shader = make_shared<Shader>();
	shader->Init(L"DeferredSea.hlsl", StaticProp, ShaderArg{},info);
	shader->SetInjector({ BufferType::SeaDefaultMaterialParam });
	shader->SetPass(RENDER_PASS::Deferred);

	for (int i = 0; i < 1000; ++i)
	{
		{

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
			shared_ptr<GameObject> root = CreateGameObject(L"root_test");

			auto meshRenderer = root->AddComponent<MeshRenderer>();
			auto collider = root->AddComponent<Collider>();
			collider->SetBoundingBox(vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f));

			root->_transform->SetLocalScale(vec3(dis(urd), dis(urd), dis(urd)));
			root->_transform->SetLocalPosition(vec3(ddis(urd), ddis(urd), ddis(urd)));
			root->_transform->SetLocalRotation(vec3(rotate(urd), rotate(urd), rotate(urd)));
			root->SetType(GameObjectType::Static);
			root->AddTag(GameObjectTag::Wall);

			if (i == 0)
			{

				root->AddComponent<testComponent>();
				root->_transform->SetLocalPosition(vec3(0, 0, 0));
				root->_transform->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
				root->_transform->SetLocalRotation(vec3(0, 0, 0));
				root->SetType(GameObjectType::Dynamic);
				root->AddTag(GameObjectTag::Player);
			}


			materialO->SetShader(shader);
			materialO->SetTexture("_BaseMap", texture);

			meshRenderer->AddMaterials({ materialO });
			meshRenderer->AddMesh(mesh);
		}
	};


};

void TestScene::Update()
{
	Scene::Update();

}

void TestScene::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene::Rendering()
{
	GlobalSetting();

	_globalParam.caustics = 0;


	auto& cmdList = Core::main->GetCmdList();
	Core::main->GetRenderTarget()->ClearDepth();


	Profiler::Set("PASS : Deferred", BlockTag::CPU);
	DeferredPass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : FinalPass", BlockTag::CPU);
	FinalRender(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : Forward", BlockTag::CPU);
	ForwardPass(cmdList);
	Profiler::Fin();

	Volumetric::main->Render();

	Profiler::Set("PASS : Transparent", BlockTag::CPU);
	TransparentPass(cmdList); // Position,
	Profiler::Fin();

	Profiler::Set("PASS : UI", BlockTag::CPU);
	UiPass(cmdList);
	Profiler::Fin();
}

void TestScene::DebugRendering()
{
	Scene::DebugRendering();
}

void TestScene::RenderEnd()
{
	Scene::RenderEnd();
}

void TestScene::Finish()
{
	Scene::Finish();

	if (Input::main->GetKeyDown(KeyCode::F6))
	{
		Core::main->FenceCurrentFrame();
		SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::TestScene2), true, true);
	}
}

TestScene::~TestScene()
{
}

void TestScene::UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::UiPass(cmdList);
}

void TestScene::TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::TransparentPass(cmdList);
}

void TestScene::ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::ForwardPass(cmdList);
}

void TestScene::DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::DeferredPass(cmdList);
}

void TestScene::ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::ShadowPass(cmdList);
}

void TestScene::FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::FinalRender(cmdList);
}

void TestScene::SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass)
{
	Scene::SettingPrevData(data, pass);
}

void TestScene::ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ComputeManager::main->DispatchMainField(cmdList);
}
